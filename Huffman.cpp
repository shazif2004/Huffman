#include "Huffman.h"
#include "Node.h"
#include <queue>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <map>
#include "Storage/Storage.h"

Huffman::Huffman() : root(nullptr) {}

// we use freeTree function to delete the tree as this will prevent memory leaks
void Huffman::freeTree(Node *currentNode) {
    if (currentNode == nullptr) { // check if its empty
        return;
    }
    freeTree(currentNode->zero);
    freeTree(currentNode->one);
    delete currentNode;
}

Huffman::~Huffman() {
    freeTree(root); // cleans up memory by destorying all tree nodes
}


void Huffman::buildFrequencyTable(const std::string &inputText) {
    frequencyTable.clear();
    for (std::size_t i = 0; i < inputText.length(); ++i) {
        char letter = inputText[i];
        frequencyTable[letter]++;
    }
}

// Builds huffman tree and decode given input text
void Huffman::buildHuffmanTree() {
    std::priority_queue<Node*, std::deque<Node*>, compareWeights> priorityQueue;

    /**
     * This will get the first item in the map and get the last item in the map.
     * It counts how many times a letter appears and its going to store it in a map.
     */
    std::map<char, int>::iterator it; // lets call iterator "it"
    for (it = frequencyTable.begin(); it != frequencyTable.end(); ++it) {
        Node *node = new Node(it->first, it->second, nullptr, nullptr);
        priorityQueue.push(node);
    }


    while (priorityQueue.size() > 1) {
        Node *leftNode = priorityQueue.top(); priorityQueue.pop();
        Node *rightNode = priorityQueue.top(); priorityQueue.pop();
        int combinedWeights = leftNode->weight + rightNode->weight;
        Node *mergeNode = new Node('\0', combinedWeights, leftNode, rightNode);
        priorityQueue.push(mergeNode);
    }

    if(!priorityQueue.empty()) {
        root = priorityQueue.top();
    }


}

void Huffman::generateEncoding(const Node *currentNode, std::string str,
                               std::unordered_map<char, std::string> &huffmanCode) {
    if (currentNode == nullptr) {
        return;
    }

    if (currentNode->zero == nullptr && currentNode->one == nullptr) {
        huffmanCode[currentNode->letter] = str;
        return;
    }
    generateEncoding(currentNode->zero, str + "0", huffmanCode);
    generateEncoding(currentNode->one, str + "1", huffmanCode);
}


void Huffman::compress(const std::string &inputFilePath, const std::string &outputFilePath) {
    std::ifstream inputFile(inputFilePath.c_str());
    if (!inputFile.is_open()) {
        std::cerr << "Error could not open teh file!" << std::endl;
        return;
    }
    compress(inputFile, outputFilePath);
    inputFile.close();
}

void Huffman::compress(std::istream &inputStream, const std::string &outputFilePath) {
    std::string inputText;
    char character;
    while (inputStream.get(character)) {
        inputText += character;
    }

    buildFrequencyTable(inputText);
    buildHuffmanTree();

    encodingTable.clear();
    generateEncoding(root, "", encodingTable);

    std::ofstream outputFile(outputFilePath.c_str(), std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Error could not open the output file" << std::endl;
        return;
    }

    /**
     * save how many different characters we have
     */
    std::size_t tableSize = frequencyTable.size();
    outputFile << tableSize << '\n';

    /**
     * Save each character and then how many times it should appear
     */
    std::map<char, int>::iterator it;
    for (it = frequencyTable.begin(); it != frequencyTable.end(); ++it) {
        outputFile.put(it->first);
        outputFile << it->second << '\n';
    }

    /**
     * then we convert our text into a sequence of bits
     */
    std::string encodedString;
    std::size_t i;
    for (i = 0; i < inputText.length(); ++i) {
        encodedString += encodingTable[inputText[i]];
    }

    /**
     * save how many bits encoded string has
     */
    std::size_t bitLength = encodedString.length();
    outputFile << bitLength << '\n';

    /**
     * Then we pack our bits into bytes (which is 8 bites each) and then save them
     */
    char byte = 0;
    int bitCount = 0;
    for (i = 0; i < encodedString.length(); ++i) {
        // shift left by 1 and add new bit
        byte = (byte << 1) | (encodedString[i] - '0');
        bitCount++;

        // when bits = 8, we write the byte to the file
        if (bitCount == 8) {
            outputFile.put(byte);
            byte = 0;
            bitCount = 0;
        }
    }

    /**
     * if we have leftover bits that don't make a full byte, add padding
     */
    if (bitCount > 0) {
        byte = byte << (8 - bitCount); // push bit to left and then fill with zeros
        outputFile.put(byte);
    }

    outputFile.close();
    std::cout << "compression is done" << std::endl;
    freeTree(root);
}




void Huffman::decompress(const std::string &inputFilePath, const std::string &outputFilePath) {
    std::ifstream inputFile(inputFilePath.c_str(), std::ios::binary);  // open in binary mode
    if (!inputFile.is_open()) {
        std::cerr << "Error opening the compressed file!" << std::endl;
        return;
    }

    decompress(inputFile, outputFilePath);
    inputFile.close();
}


void Huffman::decompress(std::istream &inputStream, const std::string &outputFilePath) {
    std::ofstream outputFile(outputFilePath.c_str());
    if (!outputFile.is_open()) {
        std::cerr << "Couldn't open the output file!" << std::endl;
        return;
    }

    /**
     * first we will read how many different characters we stored
     */
    std::size_t tableSize;
    inputStream >> tableSize;
    inputStream.get(); // skip newline

    /**
     * then each character and its count
     */
    frequencyTable.clear();
    for (std::size_t i = 0; i < tableSize; ++i) {
        char ch;
        int freq;
        inputStream.get(ch);
        inputStream >> freq;
        inputStream.get(); // skip newline
        frequencyTable[ch] = freq;
    }

    /**
     * read how many bits are in our encoded data
     */
    std::size_t bitLength;
    inputStream >> bitLength;
    inputStream.get();


    buildHuffmanTree(); // rebuild our Huffman tree using the same character counts


    if (root == nullptr) {
        std::cerr << "Error: Huffman tree is empty. Cannot decompress." << std::endl;
        return;
    }

    /**
     * Read the compressed data and decode it
     */
    Node *traverseNode = root; // we're basically following paths through the tree
    char byte;
    std::size_t bitsRead = 0;

    /**
     * Read each byte from the file
     */
    while (inputStream.get(byte)) {

        // Process each bit in the byte from left to right
        for (int bit = 7; bit >= 0; --bit) {
            if (bitsRead >= bitLength) { // stop if we read all bits
                break;
            }

            bool bitValue = (byte >> bit) & 1;
            bitsRead++;

            /**
             * Use the bit to navigate the tree - left for 0, right for 1
             */
            if (bitValue == 0) {
                traverseNode = traverseNode->zero; // Go left in the tree
            } else {
                traverseNode = traverseNode->one; // Go right in the tree
            }

            /**
             * Just to check
             */
            if (traverseNode == nullptr) {
                std::cerr << "Error tree traversal failed." << std::endl;
                return;
            }

            /**
             * When we reach a leaf node we've found a character
             */
            if (traverseNode->zero == nullptr && traverseNode->one == nullptr) {
                outputFile << traverseNode->letter; // Write the character and go back to the root for the next one
                traverseNode = root;
            }
        }
    }

    outputFile.close();
    std::cout << "Decompression is complete" << std::endl;
    freeTree(root);
}