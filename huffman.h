#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include "utils.h"

class Huffman {
private:
    std::ifstream file_in;
    std::ofstream file_out;
    int freq_table[256];
    std::pair<short, short> codes[256];

    void open_files_analysis(const std::string& filename);

    void open_files_decompress(const std::string& filename);

    void close_files();

    void make_freq_table();

    struct HNode;

    class PriorityQueue;

    HNode* make_tree();

    void make_codes(HNode* node, short code, short length);

public:
    Huffman();

    void encode(const std::string& filename);
};
