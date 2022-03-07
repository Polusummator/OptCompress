#include "utils.h"
#include "rle.h"
#include "huffman.h"


int main() {

//    RLE rle;
//    std::string file = "Compress.cbp";
//    rle.encode(file);

    std::string file1 = "/tmp/war_peace_txtY_huf.opt_huf";
    std::string file2 = "war_peace.txt";

    Huffman huf;
    huf.encode(file2);
    huf.decode(file1);
    return 0;
}
