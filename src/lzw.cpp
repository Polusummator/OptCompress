#include "lzw.h"
#include "utils.h"

Code::Code() = default;

Code::Code(long long value_, int len_) : value(value_), len(len_) {}

bool Code::operator<(const Code& c) const {
    return value < c.value;
}

IOEncode::IOEncode() {
    cur_byte = 0;
    pos_in_byte = 0;
}

void IOEncode::set_last_code(long long last_code_) {
    last_code = last_code_;
}

long long IOEncode::get_last_code() const {
    return last_code;
}

void IOEncode::set_code_len(int len_) {
    code_len = len_;
}

int IOEncode::get_code_len() const {
    return code_len;
}

void IOEncode::write_code(const Code &code, std::ofstream& file_out) {
    int zeroes = code_len - code.len;
    long long val = code.value;

    // Write each bit
    // Write leading zeroes
    for (int i = 0; i < zeroes; i++) {
        cur_byte &= ~(1 << pos_in_byte);
        pos_in_byte++;
        if (pos_in_byte == 8) {
            file_out.write((char*)&cur_byte, sizeof(cur_byte));
            pos_in_byte = 0;
            cur_byte = 0;
        }
    }

    // Write bits of code in this format:
    // 9 8 7 6 5 4 3 2 1 0 ->
    // 2 3 4 5 6 7 8 9 | x x x x x x 0 1
    for (int i = code.len - 1; i >= 0; i--) {
        if ((1 << i) & val) {
            cur_byte |= (1 << pos_in_byte);
        }
        else {
            cur_byte &= ~(1 << pos_in_byte);
        }
        pos_in_byte++;
        if (pos_in_byte == 8) {
            file_out.write((char*)&cur_byte, sizeof(cur_byte));
            pos_in_byte = 0;
            cur_byte = 0;
        }
    }
    if (code.value == 257ll && pos_in_byte != 0) {
        file_out.write((char*)&cur_byte, sizeof(cur_byte));
    }
}

IODecode::IODecode() {
    cur_byte = 0;
    pos_in_byte = 8;
};

void IODecode::set_last_code(long long last_code_) {
    last_code = last_code_;
}

long long IODecode::get_last_code() const {
    return last_code;
}

void IODecode::set_code_len(int len_) {
    code_len = len_;
};

int IODecode::get_code_len() const {
    return code_len;
};

/**
 * The function read codes from file one by one
 * @param file_in input file
 * @param code variable for saving code
 * @return false, if it is the last code in the file
 */
bool IODecode::read_code(std::ifstream& file_in, Code& code) {
    int remained = code_len + (last_code + 1 == (1 << code_len));
    char byte;
    long long result = 0;
    while (remained > 0) {
        if (pos_in_byte == 8) {
            file_in.get(byte);
            cur_byte = (unsigned char)byte;
            pos_in_byte = 0;
        }
        if (cur_byte & (1 << pos_in_byte)) {
            result |= (1 << (remained - 1));
        }
        else {
            result &= ~(1 << (remained - 1));
        }
        remained--;
        pos_in_byte++;
    }
    if (result == 257ll) {
        return false;
    }
    code = Code(result, code_len);
    return true;
};

void IODecode::write_string(std::ofstream& file_out, std::map<Code, std::basic_string<unsigned char>>& dict, const Code& code) {
    const std::basic_string<unsigned char>& to_out = dict[code];
    for (const unsigned char ubyte : to_out) {
        file_out << (char)ubyte;
    }
}

void IODecode::write_string(std::ofstream& file_out, const std::basic_string<unsigned char>& to_out) {
    for (const unsigned char ubyte : to_out) {
        file_out << (char)ubyte;
    }
}

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
    file_out.close();
}

/**
 * Initialize dictionaries (first 257 words)
 */
void LZW::init_dict() {
    dict_code_string.clear();
    dict_string_code.clear();

    for (int i = 0; i <= 255; i++) {
        std::basic_string<unsigned char> s {(unsigned char)i};
        dict_string_code[s] = Code(i, 8);
        dict_code_string[Code(i, 8)] = s;
    }

    io_encode.set_code_len(9);
    io_encode.set_last_code(257ll);
    io_decode.set_code_len(9);
    io_decode.set_last_code(257ll);
}

/**
 * Add pair (code : string) to dictionaries
 * @param s string
 * @param mode 0 if encode, 1 if decode
 */
void LZW::add_string(const std::basic_string<unsigned char>& s, int mode) {
    long long new_code = io_encode.get_last_code() + 1;
    int code_len = io_encode.get_code_len();
    if (new_code == (1 << code_len)) {
        io_encode.set_code_len(code_len + 1);
        io_decode.set_code_len(code_len + 1);
        code_len++;
    }
    dict_code_string[Code(new_code, code_len)] = s;
    dict_string_code[s] = Code(new_code, code_len);
    io_encode.set_last_code(new_code);
    io_decode.set_last_code(new_code);
    if (mode == 0 && new_code + 1 == (1 << MAX_LEN)) {
        io_encode.write_code(Code(256ll, code_len), file_out);
        init_dict();
    }
}

LZW::LZW() {
    io_encode = IOEncode();
    io_decode = IODecode();
};

/**
 * LZW-Encoder
 * @param filename file
 */
void LZW::encode(const std::string &filename) {
    open_files_analysis(filename);
    init_dict();
    io_encode.write_code(Code(256ll, io_encode.get_code_len()), file_out);

    char byte;
    unsigned char ubyte;
    std::basic_string<unsigned char> s;
    while (file_in.get(byte)) {
        ubyte = (unsigned char)byte;
        if (dict_string_code.find(s + ubyte) != dict_string_code.end()) {
            s += ubyte;
        }
        else {
            io_encode.write_code(dict_string_code[s], file_out);
            add_string(s + ubyte, 0);
            s = ubyte;
        }
    }
    if (!s.empty()) {
        io_encode.write_code(dict_string_code[s], file_out);
    }

    // End of information
    io_encode.write_code(Code(257ll, io_encode.get_code_len()), file_out);
    close_files();
}

/**
 * LZW-Decoder
 * @param filename file
 */
void LZW::decode(const std::string &filename) {
    open_files_decompress(filename);
    init_dict();
    Code code;
    Code previous_code;
    while (io_decode.read_code(file_in, code)) {
        if (code.value == 256ll) {
            init_dict();
            std::cout << "Init dict\n";
            if (!io_decode.read_code(file_in, code)) {
                break;
            }
            io_decode.write_string(file_out, dict_code_string, code);
            previous_code = code;
        }
        else {
            if (dict_code_string.find(code) != dict_code_string.end()) {
                io_decode.write_string(file_out, dict_code_string, code);
                std::basic_string<unsigned char> to_add = dict_code_string[previous_code] + dict_code_string[code][0];
                add_string(to_add, 1);
                previous_code = code;
            }
            else {
                std::basic_string<unsigned char> to_out = dict_code_string[previous_code] + dict_code_string[previous_code][0];
                io_decode.write_string(file_out, to_out);
                add_string(to_out, 1);
                previous_code = code;
            }
        }
    }
    close_files();
}
