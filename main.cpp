#include "utils.h"
#include "rle.h"

int main() {

    RLE rle;
    std::string file = "RLECompress_cbpY_rle.opt_rle";
    rle.decode(file);
    return 0;
}