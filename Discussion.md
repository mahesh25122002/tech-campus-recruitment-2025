# Discussion.md

## Solutions Considered

1. *Sequential Read Approach*
   - Simple implementation using ifstream
   - Low memory usage but O(n) time complexity
   - Not suitable for 1TB files

2. *Chunked Reading*
   - Read file in chunks
   - Better memory management
   - Still O(n) complexity

3. *Memory Mapping with Binary Search (Chosen)*
   - O(log n) time complexity
   - Efficient memory utilization
   - Leverages OS virtual memory
   - Optimal for large files
   - Fast random access

## Final Solution Summary

The chosen solution uses memory mapping with binary search because:
1. Memory mapping allows efficient handling of 1TB+ files
2. Binary search provides logarithmic time complexity
3. Chunk-based reading aligns with system page size
4. RAII ensures proper resource cleanup
5. Error handling covers all edge cases

## Steps to Run

1. Clone repository:
bash
git clone <repository-url>
cd <repository-name>


2. Build:
bash
make


3. Run:
bash
./extract_logs YYYY-MM-DD


4. Output located in:

output/output_YYYY-MM-DD.txt
