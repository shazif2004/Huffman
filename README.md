Huffman coding is a compression technique that basically assigns shorter binary code to characters that appears the most which reduces the file size. The program uses data structures such as std::map, std::unordered_map, and priority queueu to build a huffman tree from character frequencies. Internally, the compression process involves reading a text files, generating frequency table, builds the Huffman tree and then creating a binary encoding for each character. For decompression, it reads the header, reconstructs huffman tree and then decodes bitsream back into the original text. To use the tool, all you have to do is simply call the compress and decompress function, and its up to you which file you want to compress and then decompress it back. When the file is compressed, the following output would be:

At first, it will you give you number of unique characters in the existing file.
Then the number of times that each individual character appears.
Then the total encoded bit length.
Then its going to print strange looking symbol which is basically a part of compressed binary data thats written to the output file after Huffman encoding.
In one of the test, the phrase "go go go gophers!", was sucessfully compressed and decompress back to its original text, which shows accurate data recovery. On a larger scale, compressing the full text of Adventures_of_Sherlock_Holmes.txt (~6.34 MB) resulted in compressed file of 3.61 MB - a reduction of half the size.

Overall, this tool is really useful for reducing the storage space.
