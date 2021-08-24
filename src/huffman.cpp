#include "huffman.h"

HNode::HNode() {
    contains = false;
    l = nullptr;
    r = nullptr;
    priority = 0;
}

HNode::HNode(unsigned char ubyte, unsigned int freq) {
    contains = true;
    symbol = ubyte;
    l = nullptr;
    r = nullptr;
    priority = freq;
}

HNode::HNode(HNode* nl, HNode* nr) {
    contains = false;
    l = nl;
    r = nr;
    priority = nl->priority + nr->priority;
}


void Huffman::open_files_analysis(const std::string& filename) {
    file_in.open(filename);
    // Linux:
    file_out.open("/tmp/" + make_filename_out_analysis(filename, 2), std::ios::binary | std::ios::out | std::ios::app);

};

void Huffman::open_files_decompress(const std::string& filename) {
    file_in.open(filename);
    file_out.open(make_filename_out_decompress(filename, 2), std::ios::binary | std::ios::out | std::ios::app);
};

void Huffman::close_files() {
    file_in.close();
    file_out.close();
};

void Huffman::make_freq_table() {
    std::fill(freq_table, freq_table + 256, 0);
    char byte;
    unsigned char ubyte;
    while (file_in.get(byte)) {
        ubyte = (unsigned char) byte;
        freq_table[ubyte]++;
    }
    file_in.clear();
    file_in.seekg(0);
}

HNode* Huffman::make_tree() {
    auto comparator = [](HNode* a, HNode* b) {return a->priority > b->priority;};
    std::priority_queue<HNode*, std::vector<HNode*>, decltype(comparator)> queue(comparator);

    for (int i = 0; i < 256; i++) {
        if (freq_table[i] > 0) {
            auto new_node = new HNode((unsigned char)i, freq_table[i]);
            queue.push(new_node);
        }
    }
    while (queue.size() > 1) {
        HNode* t1 = queue.top();
        queue.pop();
        HNode* t2 = queue.top();
        queue.pop();
        auto merged = new HNode(t1, t2);
        queue.push(merged);
    }

    HNode* root = queue.top();
    return root;
}

void Huffman::make_codes(HNode* node, int code, int length) {
    if (node->contains) {
        codes[node->symbol] = {length, code};
        return;
    }
    make_codes(node->l, code, length + 1);
    make_codes(node->r, code | (1 << length), length + 1);
}

Huffman::Huffman() = default;

void Huffman::encode(const std::string& filename) {
    open_files_analysis(filename);
    make_freq_table();
    HNode* tree_root = make_tree();
    std::fill(codes, codes + 256, std::make_pair(0, 0));
    make_codes(tree_root, 0, 0);

    char byte;
    unsigned char ubyte;
    unsigned char write_byte = 0;
    char pos = 0;
    unsigned int cnt_bytes = 0;

    // first element of encoded is number of bytes (only code-bytes)
    // second element of encoded is length of the last byte
    std::vector<unsigned char> encoded;
    while(file_in.get(byte)) {
        ubyte = (unsigned char)byte;
        int code = codes[ubyte].second;
        int length = codes[ubyte].first;
        for (int j = 0; j < length; j++) {
            if (code & (1 << j)) {
                write_byte = write_byte | (1 << pos);
            }
            pos++;
            if (pos == 8) {
                encoded.push_back(write_byte);
                pos = 0;
                write_byte = 0;
                cnt_bytes++;
            }
        }
    }
    cnt_bytes++;
    encoded.insert(encoded.begin(), pos);
    encoded.push_back(write_byte);

    std::cout << cnt_bytes << ' ' << encoded.size() << std::endl;

    // begin of the file - frequencies of bytes (unsigned int for each byte)
    unsigned char k_chars[4];
    for (unsigned int i : freq_table) {
        int_to_chars(k_chars, i);
        for (unsigned char c : k_chars) {
            file_out.write((char*)&c, sizeof(c));
        }
    }

    // Number of bytes in compressed file (there can be a zero byte in the end of the file)
    int_to_chars(k_chars, cnt_bytes);
    for (unsigned char c : k_chars) {
        file_out.write((char*)&c, sizeof(c));
    }

    for (unsigned char c : encoded) {
        file_out.write((char*)&c, sizeof(c));
    }

    // printing codes (for testing)
    for (int i = 0; i < 256; i++) {
        int length = codes[i].first;
        int code = codes[i].second;
        std::cout << freq_table[i] << ' ' << length <<  " ";
        for (int j = 0; j < length; j++) {
            if (code & (1 << j)) {
                std::cout << "1";
            }
            else {
                std::cout << "0";
            }
        }
        std::cout << std::endl;
    }

    close_files();
}
