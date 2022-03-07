#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

/**
* Structure for counting double polynomial hash of the string
*/
struct PolyHash {

    const long long p1 = 179;
    const long long p2 = 139;
    const long long Mod1 = 556556107;
    const long long Mod2 = 2e9 + 11;

    std::vector<long long> powers1;
    std::vector<long long> powers2;

    std::vector<long long> pref1;
    std::vector<long long> pref2;


    explicit PolyHash(const std::basic_string<unsigned char>& str);

    /**
     * Get double hash for the substring
     * @param l Left position
     * @param r Right Postion
     * @return Double hash
     */
    inline std::pair<long long, long long> operator() (int l, int r) const;
};

class RLE {
private:
    const unsigned char MAX_REPEAT = 255;
    const unsigned char MAX_NO_REPEAT = 127;
    std::ifstream file_in;
    std::ofstream file_out;

    void open_files_analysis(const std::string& filename);

    void open_files_decompress(const std::string& filename);

    void close_files();

    /**
     * Burrows–Wheeler transform using polynomial hash
     * @param str Source string
     * @return Pair of transformation result and position of source string in the table of shifts
     */
    static std::pair<std::basic_string<unsigned char>, unsigned int> bwt_encode_hash(const std::basic_string<unsigned char>& str);

    /**
     * Inverse Burrows–Wheeler transform
     * @param s BWT result
     * @param k Position of source string in the table of shifts
     * @return Source string
     */
    static std::basic_string<unsigned char> bwt_decode(std::basic_string<unsigned char> s, unsigned int k);

    /**
     * Structure for representing repeating blocks
     */
    struct Block;

    /**
     * Write non-repeated block to file
     * @param pos Position of the beginning of the block
     * @param length_no_repeat Length of the block
     * @param data String
     */
    void write_no_repeat(int pos, int length_no_repeat, const std::basic_string<unsigned char>& data);

    /**
     * Write repeated block to file
     * @param length_repeat Length of the block
     * @param c Repeated symbol
     */
    void write_repeat(int length_repeat, unsigned char c);

public:
    RLE();

    /**
     * Run-length encoding using Burrows–Wheeler transform for the file
     * @param filename Name of the file
     */
    void encode(const std::string& filename);

    /**
     * Decoding run-length encoding using Burrows–Wheeler transform for the file
     * @param filename Name of the file
     */
    void decode(const std::string& filename);
};
