#include <iostream>
#include <fstream>
#include <string>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <filesystem>
#include <cstring>
#include <chrono>

class Timer {
    std::chrono::high_resolution_clock::time_point start_time;
public:
    Timer() : start_time(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end_time - start_time).count();
    }
};

class LogExtractor {
private:
    const char* filename;
    char* mapped_data;
    size_t file_size;
    int fd;

    bool mapFile() {
        fd = open(filename, O_RDONLY);
        if (fd == -1) {
            std::cerr << "Error opening file" << std::endl;
            return false;
        }

        struct stat sb;
        if (fstat(fd, &sb) == -1) {
            std::cerr << "Error getting file size" << std::endl;
            close(fd);
            return false;
        }

        file_size = sb.st_size;
        mapped_data = static_cast<char*>(mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0));
        
        if (mapped_data == MAP_FAILED) {
            std::cerr << "Error mapping file" << std::endl;
            close(fd);
            return false;
        }
        return true;
    }

    void unmapFile() {
        if (mapped_data != MAP_FAILED) {
            munmap(mapped_data, file_size);
        }
        if (fd != -1) {
            close(fd);
        }
    }

    size_t findLineStart(size_t pos) {
        while (pos > 0 && mapped_data[pos - 1] != '\n') {
            pos--;
        }
        return pos;
    }

    std::string getDateFromLine(size_t pos) {
        return std::string(mapped_data + pos, 10);
    }

    size_t binarySearch(const std::string& target_date) {
        size_t left = 0;
        size_t right = file_size;
        const size_t chunk_size = 4096;

        while (left < right) {
            size_t mid = left + (right - left) / 2;
            mid = findLineStart(mid);
            
            if (mid >= file_size) {
                right = mid;
                continue;
            }

            std::string current_date = getDateFromLine(mid);
            
            if (current_date == target_date) {
                while (mid > 0) {
                    size_t prev = findLineStart(mid - 1);
                    if (getDateFromLine(prev) != target_date) {
                        break;
                    }
                    mid = prev;
                }
                return mid;
            }
            
            if (current_date < target_date) {
                left = mid + chunk_size;
            } else {
                right = mid;
            }
        }
        
        return file_size;
    }

public:
    LogExtractor(const char* fname) : filename(fname), mapped_data(nullptr), file_size(0), fd(-1) {}

    ~LogExtractor() {
        unmapFile();
    }

    bool extractLogs(const std::string& target_date) {
        Timer timer;

        if (!mapFile()) {
            return false;
        }

        std::filesystem::create_directories("output");
        std::string output_path = "output/output_" + target_date + ".txt";
        std::ofstream output_file(output_path);

        if (!output_file) {
            std::cerr << "Error creating output file" << std::endl;
            return false;
        }

        size_t pos = binarySearch(target_date);
        if (pos >= file_size) {
            std::cout << "No logs found for date " << target_date << std::endl;
            return true;
        }

        while (pos < file_size) {
            size_t end = pos;
            while (end < file_size && mapped_data[end] != '\n') {
                end++;
            }
            
            std::string line(mapped_data + pos, end - pos);
            if (line.substr(0, 10) != target_date) {
                break;
            }
            
            output_file << line << '\n';
            pos = end + 1;
        }

        std::cout << "Logs extracted to " << output_path << std::endl;
        std::cout << "Total execution time: " << timer.elapsed() << " seconds" << std::endl;
        return true;
    }
};

bool validateDate(const std::string& date) {
    if (date.length() != 10) return false;
    if (date[4] != '-' || date[7] != '-') return false;
    
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(date[i])) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " YYYY-MM-DD" << std::endl;
        return 1;
    }

    std::string target_date = argv[1];
    if (!validateDate(target_date)) {
        std::cerr << "Invalid date format. Use YYYY-MM-DD" << std::endl;
        return 1;
    }

    LogExtractor extractor("test_logs.log");
    if (!extractor.extractLogs(target_date)) {
        return 1;
    }

    return 0;
}
