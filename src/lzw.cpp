#include "lzw.h"
#include "utils.h"

void LZW::open_files_analysis(const std::string &filename) {
    file_in.open(filename);
    // Linux:
    file_out.open("/tmp/" + make_filename_out_analysis(filename, 0), std::ios::binary | std::ios::out | std::ios::app);
}

void LZW::open_files_decompress(const std::string &filename) {
    file_in.open(filename);
    file_out.open(make_filename_out_decompress(filename, 0), std::ios::binary | std::ios::out | std::ios::app);
}

void LZW::close_files() {
    file_in.close();
    file_out.close()
}
