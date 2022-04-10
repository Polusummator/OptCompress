#include "utils.h"
#include "rle.h"
#include "huffman.h"
#include "lzw.h"


int main() {

    std::string file1 = "tom.doc";
    std::string file2 = "/tmp/tom_docY_lzw.opt_lzw";
    LZW lzw;
    lzw.encode(file1);
    lzw.decode(file2);

    return 0;
}
