#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include "utils.h"

struct HNode {
    bool contains;
    unsigned char symbol;
    HNode* l;
    HNode* r;
    unsigned int priority;
    HNode();
    HNode(unsigned char ubyte, unsigned int freq);
    HNode(HNode* nl, HNode* nr);
};

class Huffman {
private:
    std::ifstream file_in;
    std::ofstream file_out;
    unsigned int freq_table[256];
    std::pair<int, int> codes[256];
    HNode* tree_root;

    void delete_tree(HNode* v);

    void open_files_analysis(const std::string& filename);

    void open_files_decompress(const std::string& filename);

    void close_files();

    void make_freq_table();

    HNode* make_tree();

    void make_codes(HNode* node, int code, int length);

    static void go_tree(HNode*& node, bool move);

public:
    Huffman();

    void encode(const std::string& filename);

    void decode(const std::string& filename);
};
