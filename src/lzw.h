#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>

// Maximal len of code
const int MAX_LEN = 24;

/**
 * Structure for code (includes value and len of the code)
 */
struct Code {
    long long value;
    int len;

    Code();
    Code(long long value_, int len_);

    bool operator<(const Code& c) const;
};

/**
 * Class for IO in encoder
 */
class IOEncode {
private:
    unsigned char cur_byte;
    int pos_in_byte;
    int code_len;
    long long last_code;

public:

    IOEncode();

    void set_last_code(long long last_code_);

    long long get_last_code() const;

    void set_code_len(int len_);

    int get_code_len() const;

    void write_code(const Code& code, std::ofstream& file_out);
};

/**
 * Class for IO in decoder
 */
class IODecode {
private:
    unsigned char cur_byte;
    int pos_in_byte;
    int code_len;
    long long last_code;

public:

    IODecode();

    void set_last_code(long long last_code_);

    long long get_last_code() const;

    void set_code_len(int len_);

    int get_code_len() const;

    /**
     * The function read codes from file one by one
     * @param file_in input file
     * @param code variable for saving code
     * @return false, if it is the last code in the file
     */
    bool read_code(std::ifstream& file_in, Code& code);

    static void write_string(std::ofstream& file_out, std::map<Code, std::basic_string<unsigned char>>& dict, const Code& code);

    static void write_string(std::ofstream& file_out, const std::basic_string<unsigned char>& to_out);
};

class LZW {
private:
    std::ifstream file_in;
    std::ofstream file_out;
    IOEncode io_encode;
    IODecode io_decode;
    std::map<std::basic_string<unsigned char>, Code> dict_string_code;
    std::map<Code, std::basic_string<unsigned char>> dict_code_string;

    void open_files_analysis(const std::string& filename);

    void open_files_decompress(const std::string& filename);

    void close_files();

    /**
     * Initialize dictionaries (first 257 words)
     */
    void init_dict();

    /**
     * Add pair (code : string) to dictionaries
     * @param s string
     * @param mode 0 if encode, 1 if decode
     */
    void add_string(const std::basic_string<unsigned char>& s, int mode); // 0 - encode, 1 - decode
public:
    LZW();

    /**
     * LZW-Encoder
     * @param filename file
     */
    void encode(const std::string& filename);

    /**
     * LZW-Decoder
     * @param filename file
     */
    void decode(const std::string& filename);
};
