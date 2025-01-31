CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3
TARGET = extract_logs
SRC = src/extract_logs.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
	rm -rf output/

.PHONY: all clean
