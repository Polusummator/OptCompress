#include "utils.h"

const std::string modes[3] {
    "_lzw.opt_lzw",
    "_rle.opt_rle",
    "_huf.opt_huf"
};

std::pair<std::string, std::string> split_filename(const std::string& filename, char delimiter) {
    if (filename.find(delimiter) != std::string::npos) {
        int sz = (int)filename.size();
        std::string ext;
        std::string filename_without_ext;
        bool was_delimiter = false;
        for (int i = sz - 1; i >= 0; i--) {
            if (was_delimiter) {
                filename_without_ext = filename[i] + filename_without_ext;
            }
            else {
                if (filename[i] == delimiter) {
                    was_delimiter = true;
                }
                else {
                    ext = filename[i] + ext;
                }
            }
        }
        return std::make_pair(filename_without_ext, ext);
    }
    return std::make_pair(filename, "");
}

// Y - extension exists
// N - extension doesn't exist
std::string make_filename_out_analysis(const std::string& filename, short mode) {
    std::pair<std::string, std::string> p = split_filename(filename, '.');
    if (!p.second.empty())
        return p.first + '_' + p.second + "Y" + modes[mode];
    return p.first + "N" + modes[mode];
}

std::string make_filename_out_decompress(const std::string& filename, short mode) {
    std::string nfilename = filename;
    auto msize = (short)modes[mode].size();
    nfilename = nfilename.substr(0, nfilename.size() - msize);
    char contains_ext = nfilename.back();
    nfilename.pop_back();
    if (contains_ext == 'N') {
        return nfilename;
    }
    std::pair<std::string, std::string> p = split_filename(nfilename, '_');
    return p.first + "." + p.second;
}

std::string cut_path(const std::string& filename) {
    char delimiter;
    if (filename.find('/') != std::string::npos) {
        delimiter = '/';
    }
    else if (filename.find('\\') != std::string::npos) {
        delimiter = '\\';
    }
    else {
        return filename;
    }
    int sz = (int)filename.size();
    std::string filename_without_path;
    for (int i = sz - 1; i >= 0; i--) {
        if (filename[i] == delimiter) {
            break;
        }
        filename_without_path = filename[i] + filename_without_path;
    }
    return filename_without_path;
}

// Bullshit
//std::pair<bool, unsigned int> read_index(std::ifstream& file_in, unsigned int cnt_words) {
//    if (file_in.eof()) {
//        return std::make_pair(0, 0);
//    }
//    unsigned int res = 0;
//    unsigned int mult = 1;
//    char byte;
//    unsigned short num_bytes;
//    if (cnt_words <= USHRT_MAX) {
//        num_bytes = 2;
//    }
//    else if (cnt_words <= UINT_MAX) {
//        num_bytes = 4;
//    }
//    else {
//        num_bytes = 8;
//    }
//    // Это, скорее всего, неверно
//    for (int i = 0; i < num_bytes; i++) {
//        file_in.get(byte);
//        res += (unsigned int)(unsigned char)byte * mult;
//        mult *= 256;
//    }
//    return {1, res};
//}

unsigned int chars_to_int(const unsigned char* a) {
    unsigned int res = 0;
    res += a[0];
    res *= 256;
    res += a[1];
    res *= 256;
    res += a[2];
    res *= 256;
    res += a[3];
    return res;
}

void int_to_chars(unsigned char* k_chars, unsigned int a) {
    k_chars[3] = a % 256;
    a /= 256;
    k_chars[2] = a % 256;
    a /= 256;
    k_chars[1] = a % 256;
    a /= 256;
    k_chars[0] = a % 256;
}
