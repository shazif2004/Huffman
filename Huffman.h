#include <string>
#include <queue>
#include <fstream>
#include <unordered_map>
#include "Node.h"
#include "Storage/Storage.h"
#include <map>

#ifndef HUFFMAN_H
#define HUFFMAN_H


/**
 * Huffman compression and decompression for files
 */
class Huffman {
private:
    Node *root; // root of the huffman tree
    std::unordered_map<char, std::string> encodingTable; // maps character to binary string (eg 'A' to "010")
    std::map<char, int> frequencyTable; // maps character to frequencies (eg 'A' to '3')

    /**
     * Builds the frequency table from a given input text
     * @param inputText The text to look at
     */
    void buildFrequencyTable(const std::string& inputText);

    /**
     * Build the Huffman tree using the frequency table
     */
    void buildHuffmanTree();

    /**
     * generates Huffman codes for each character
     * @param currentNode Current node in the tree traversal
     * @param str binary string that needs to be built
     * @param huffmanCode Reference to the map where character codes will be stored
     */
    void generateEncoding(const Node *currentNode, std::string str, std::unordered_map<char, std::string> &huffmanCode);

    /**
     * Frees memory when the tree is deleted
     * @param currentNode Starting node for the recursive memory cleanup
     */
    void freeTree(Node *currentNode); // to prevent memory leaks

    /**
     * Internal compression method that works with input streams
     * @param inputStream  Source stream containing the data to compress
     * @param outputFilePath Path where the compressed file will be saved
     */
    void compress(std::istream &inputStream, const std::string &outputFilePath);

    /**
     * Internal decompression method that works with input streams
     * @param inputStream Source stream containing the compressed data
     * @param outputFilePath Path where the decompressed file will be saved
     */
    void decompress(std::istream &inputStream, const std::string &outputFilePath);


public:

    /**
     * Constructor
     */
    Huffman();

    /**
     * Destructor
     */
    ~Huffman();

    /**
     * Compresses a file using Huffman coding
     * @param inputFilePath Path to the file to be compressed
     * @param outputFilePath Path where the compressed file will be saved
     */
    void compress(const std::string &inputFilePath, const std::string &outputFilePath);

    /**
     * Decompresses a Huffman compressed file
     * @param inputFilePath Path to the compressed file
     * @param outputFilePath Path where the decompressed file will be saved
     */
    void decompress(const std::string &inputFilePath, const std::string &outputFilePath);



};

#endif //HUFFMAN_H
