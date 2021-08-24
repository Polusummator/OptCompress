#include "utils.h"
#include "rle.h"
#include "huffman.h"


int main() {

//    RLE rle;
//    std::string file = "Compress.cbp";
//    rle.encode(file);

    std::string file = "lol.bmp";
    Huffman huf;
    huf.encode(file);
    return 0;
}
