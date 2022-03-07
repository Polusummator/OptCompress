#include "rle.h"
#include "utils.h"


PolyHash::PolyHash(const std::basic_string<unsigned char>& str) {
    std::size_t str_size = str.size();

    // Count powers p1 ^ k and p2 ^ k, where k <= str_size
    powers1.resize(str_size + 1);
    powers2.resize(str_size + 1);
    powers1[0] = 1;
    powers2[0] = 1;
    for (int i = 1; i <= str_size; i++) {
        powers1[i] = (powers1[i - 1] * p1) % Mod1;
    }
    for (int i = 1; i <= str_size; i++) {
        powers2[i] = (powers2[i - 1] * p2) % Mod2;
    }

    // Count prefix-hashes
    pref1.resize(str_size);
    pref2.resize(str_size);
    pref1[0] = str[0];
    pref2[0] = str[0];
    for (int i = 1; i < str_size; i++) {
        pref1[i] = (pref1[i - 1] * p1 + str[i]) % Mod1;
        pref2[i] = (pref2[i - 1] * p2 + str[i]) % Mod2;
    }
}

/**
 * Get double hash for the substring
 * @param l Left position
 * @param r Right Postion
 * @return Double hash
 */
inline std::pair<long long, long long> PolyHash::operator() (const int l, const int r) const {
    long long big1 = pref1[r];
    long long big2 = pref2[r];
    long long small1 = 0;
    long long small2 = 0;
    if (l > 0) {
        small1 = (pref1[l - 1] * powers1[r - l + 1]) % Mod1;
        small2 = (pref2[l - 1] * powers2[r - l + 1]) % Mod2;
    }
    big1 = (big1 - small1 + Mod1) % Mod1;
    big2 = (big2 - small2 + Mod2) % Mod2;
    return std::make_pair(big1, big2);
}

void RLE::open_files_analysis(const std::string& filename) {
    file_in.open(filename);
    // Linux:
    file_out.open("/tmp/" + make_filename_out_analysis(filename, 1), std::ios::binary | std::ios::out | std::ios::app);

};

void RLE::open_files_decompress(const std::string& filename) {
    file_in.open(filename);
    file_out.open(make_filename_out_decompress(filename, 1), std::ios::binary | std::ios::out | std::ios::app);
};

void RLE::close_files() {
    file_in.close();
    file_out.close();
};

/**
 * Burrows–Wheeler transform using polynomial hash
 * @param str Source string
 * @return Pair of transformation result and position of source string in the table of shifts
 */
std::pair<std::basic_string<unsigned char>, unsigned int> RLE::bwt_encode_hash(const std::basic_string<unsigned char>& str) {
    std::basic_string<unsigned char> a = str + str;
    std::size_t n = str.size();
    PolyHash hash(a);

    // Make cyclic shifts (cyclic shift = position of the first element in the new string)
    std::vector<int> pos(n);
    for (int i = 0; i < n; i++) {
        pos[i] = i;
    }

    // Sorting by the length of the longest common prefix (using polynomial hash)
    std::stable_sort(pos.begin(), pos.end(), [&](const int p1, const int p2) {
        int l = 0, r = (int)n + 1;
        while (r - l > 1) {
            int m = (l + r) / 2;
            if (m == 0) {
                l = m;
            }
            if (hash(p1, p1 + m - 1) == hash(p2, p2 + m - 1)) {
                l = m;
            }
            else {
                r = m;
            }
        }

        // If there is no common prefix
        if (l == 0) {
            return a[p1] < a[p2];
        }

        // Otherwise, we look at the next character after the common prefix
        return l < n && a[p1 + l] < a[p2 + l];
    });

    // k - position of source string in the table
    unsigned int k = std::find(pos.begin(), pos.end(), 0) - pos.begin();

    // Answer is the last column of the table
    std::basic_string<unsigned char> res;
    for (int p : pos) {
        res += a[p + n - 1];
    }
    return make_pair(res, k);
}

/**
 * Inverse Burrows–Wheeler transform
 * @param s BWT result
 * @param k Position of source string in the table of shifts
 * @return Source string
 */
std::basic_string<unsigned char> RLE::bwt_decode(std::basic_string<unsigned char> s, unsigned int k) {
    std::vector<int> count(256);
    for (unsigned char c : s) {
        count[int(c)]++;
    }
    int sum = 0;
    for (int i = 0; i < 256; i++) {
        sum = sum + count[i];
        count[i] = sum - count[i];
    }
    std::size_t n = s.size();
    std::vector<int> t(n);
    for (int i = 0; i < n; i++) {
        t[count[(int)s[i]]] = i;
        count[(int)s[i]]++;
    }
    int j = t[k];
    std::basic_string<unsigned char> res;
    for (int i = 0; i < n; i++) {
        res += s[j];
        j = t[j];
    }
    return res;
}

/**
 * Structure for representing repeating blocks
 */
struct RLE::Block {
    int start;
    int length;
    unsigned char c;
    Block() = default;
    Block(int start_, int length_, unsigned char c_) {
        start = start_;
        length = length_;
        c = c_;
    }
};

/**
 * Write non-repeated block to file
 * @param pos Position of the beginning of the block
 * @param length_no_repeat Length of the block
 * @param data String
 */
void RLE::write_no_repeat(int pos, int length_no_repeat, const std::basic_string<unsigned char>& data) {
    int z_parts = length_no_repeat / 127;
    auto remaining = (unsigned char)(length_no_repeat % 127);
    for (int part = 0; part < z_parts; part++) {
        file_out.write((char*)&MAX_NO_REPEAT, sizeof(MAX_NO_REPEAT));
        for (int i = 0; i < MAX_NO_REPEAT; i++) {
            file_out.write((char*)&data[pos], sizeof(data[pos]));
            pos++;
        }
    }
    if (remaining > 0) {
        file_out.write((char*)&remaining, sizeof(remaining));
        for (int i = 0; i < remaining; i++) {
            file_out.write((char*)&data[pos], sizeof(data[pos]));
            pos++;
        }
    }
}

/**
 * Write repeated block to file
 * @param length_repeat Length of the block
 * @param c Repeated symbol
 */
void RLE::write_repeat(int length_repeat, unsigned char c) {
    int z_parts = length_repeat / 127;
    auto remaining = (unsigned char)(length_repeat % 127);
    for (int part = 0; part < z_parts; part++) {
        file_out.write((char*)&MAX_REPEAT, sizeof(MAX_REPEAT));
        file_out.write((char*)&c, sizeof(c));
    }
    if (remaining > 0) {
        remaining = (unsigned char)(remaining | 128);
        file_out.write((char*)&remaining, sizeof(remaining));
        file_out.write((char*)&c, sizeof(c));
    }
}

RLE::RLE() = default;

/**
 * Run-length encoding using Burrows–Wheeler transform for the file
 * @param filename Name of the file
 */
void RLE::encode(const std::string& filename) {
    // Read file
    open_files_analysis(filename);
    std::streampos start = file_in.tellg();
    file_in.seekg(0, std::ios::end);
    std::streampos end = file_in.tellg();
    file_in.seekg(0, std::ios::beg);
    std::vector<char> data;
    data.resize(static_cast<std::size_t>(end - start));
    file_in.read(&data[0], (long)data.size());

    // Convert to unsigned char
    std::basic_string<unsigned char> udata;
    for (char c : data) {
        udata += (unsigned char)c;
    }

    // BWT
    std::pair<std::basic_string<unsigned char>, int> bwt = bwt_encode_hash(udata);
    std::basic_string<unsigned char> bwt_udata = bwt.first;
    unsigned int k = bwt.second;

    // Write k to file (split k into 4 unsigned chars)
    unsigned char k_chars[4];
    int_to_chars(k_chars, k);
    for (unsigned char& k_char : k_chars) {
        file_out.write((char*)&k_char, sizeof(k_char));
    }

    // Making repeating blocks
    // The first bit of the number is a type (0 - non-repeated, 1 - repeated), other bits are the number of symbols
    std::size_t size = bwt_udata.size();
    std::vector<Block> blocks;
    int cur_length = 1;
    int cur_start = 0;
    for (int i = 1; i < size; i++) {
        if (bwt_udata[i] == bwt_udata[i - 1]) {
            cur_length++;
        }
        else {
            if (cur_length > 1) {
                blocks.emplace_back(cur_start, cur_length, bwt_udata[i - 1]);
            }
            cur_length = 1;
            cur_start = i;
        }
    }
    if (cur_length > 1) {
        blocks.emplace_back(cur_start, cur_length, bwt_udata.back());
    }
    std::size_t blocks_cnt = blocks.size();

    // Write blocks
    if (blocks_cnt > 0 && blocks[0].start > 0) {
        write_no_repeat(0, blocks[0].start, bwt_udata);
    }
    for (int bl = 0; bl < blocks_cnt; bl++) {
        RLE::Block cur_block = blocks[bl];
        write_repeat(cur_block.length, cur_block.c);
        if (bl + 1 != blocks_cnt) {
            RLE::Block next_block = blocks[bl + 1];
            int length_no_repeat = next_block.start - (cur_block.start + cur_block.length);
            if (length_no_repeat > 0) {
                write_no_repeat(cur_block.start + cur_block.length, length_no_repeat, bwt_udata);
            }
        }
    }
    if (blocks_cnt > 0 && blocks.back().start + blocks.back().length != size) {
        int length_no_repeat = (int)size - (blocks.back().start + blocks.back().length);
        write_no_repeat(blocks.back().start + blocks.back().length, length_no_repeat, bwt_udata);
    }
    if (blocks_cnt == 0) {
        write_no_repeat(0, (int)size, bwt_udata);
    }
    close_files();
}

/**
 * Decoding run-length encoding using Burrows–Wheeler transform for the file
 * @param filename Name of the file
 */
void RLE::decode(const std::string& filename) {
    open_files_decompress(filename);
    char byte;
    unsigned char ubyte;
    int cnt_bytes = 0;
    unsigned char cnt_not_repeated = 0;
    unsigned char to_repeat = 0;
    bool repeat = false;
    bool is_num = true;
    std::basic_string<unsigned char> res;
    unsigned char k_chars[4];
    while (file_in.get(byte)) {
        ubyte = (unsigned char)byte;
        if (cnt_bytes < 4) {
            k_chars[cnt_bytes] = ubyte;
            cnt_bytes++;
            continue;
        }
        if (is_num) {
            repeat = false;
            is_num = false;
            if (128 & ubyte) {
                to_repeat = ubyte - 128;
                repeat = true;
            }
            else {
                cnt_not_repeated = ubyte;
            }
        }
        else {
            if (repeat) {
                for (unsigned char i = 0; i < to_repeat; i++) {
                    res += ubyte;
                }
                is_num = true;
            }
            else {
                res += ubyte;
                cnt_not_repeated--;
                if (cnt_not_repeated == 0) {
                    is_num = true;
                }
            }
        }
        cnt_bytes++;
    }
    unsigned int k = chars_to_int(k_chars);
    res = bwt_decode(res, k);

    std::string res_char;
    for (unsigned char c : res) {
        res_char += (char)c;
    }
    file_out << res_char;
    close_files();
}
