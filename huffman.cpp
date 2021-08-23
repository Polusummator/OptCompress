#include "huffman.h"


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
        ubyte = (unsigned char)byte;
        freq_table[ubyte]++;
    }
}

struct Huffman::HNode {
    bool contains;
    unsigned char symbol;
    HNode* l;
    HNode* r;
    int priority;
    HNode() {
        contains = false;
        l = nullptr;
        r = nullptr;
        priority = 0;
    }
    HNode(unsigned char ubyte, int freq) {
        contains = true;
        symbol = ubyte;
        l = nullptr;
        r = nullptr;
        priority = freq;
    }
    HNode(HNode* nl, HNode* nr) {
        contains = false;
        l = nl;
        r = nr;
        priority = l->priority + r->priority;
    }
};

class Huffman::PriorityQueue {
    std::vector<HNode*> queue;
    int size;

public:
    PriorityQueue() {
        size = 0;
    }

    int get_size() const {
        return size;
    }

    void insert(HNode* tree) {
        if (size == 0) {
            queue.push_back(tree);
        }
        else {
            for (int i = 0; i < size; i++) {
                if (queue[i]->priority > tree->priority) {
                    queue.insert(queue.begin() + i, tree);
                    break;
                }
                if (i == size - 1) {
                    queue.push_back(tree);
                }
            }
        }
        size++;
    }

    HNode* remove_first() {
        HNode* first = queue[0];
        queue.erase(queue.begin());
        size--;
        return first;
    }
};

Huffman::HNode* Huffman::make_tree() {
    PriorityQueue queue;
    for (int i = 0; i < 256; i++) {
        auto new_node = new HNode((unsigned char)i, freq_table[i]);
        queue.insert(new_node);
    }
    while (queue.get_size() > 1) {
        HNode* t1 = queue.remove_first();
        HNode* t2 = queue.remove_first();
        auto merged = new HNode(t1, t2);
        queue.insert(merged);
    }
    HNode* root = queue.remove_first();
    return root;
}

void Huffman::make_codes(Huffman::HNode* node, short code, short length) {
    if (node->contains) {
        codes[node->symbol] = {length, code};
        return;
    }
    if (node->l != nullptr) {
        make_codes(node->l, code, length + 1);
    }
    if (node->r != nullptr) {
        make_codes(node->r, code | (1 << length), length + 1);
    }
}

Huffman::Huffman() = default;

void Huffman::encode(const std::string& filename) {
    open_files_analysis(filename);
    make_freq_table();
    HNode* tree_root = make_tree();
    std::fill(codes, codes + 256, std::make_pair(0, 0));
    make_codes(tree_root, 0, 0);

    for (int i = 0; i < 256; i++) {
        int length = codes[i].first;
        int code = codes[i].second;
        std::cout << freq_table[i] << " ";
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
