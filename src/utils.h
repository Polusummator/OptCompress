#pragma once
#include <string>
#include <climits>
#include <fstream>

std::pair<std::string, std::string> split_filename(const std::string& filename, char delimiter);

std::string make_filename_out_analysis(const std::string& filename, short mode);

std::string make_filename_out_decompress(const std::string& filename, short mode);

std::string cut_path(const std::string& filename);

//std::pair<bool, unsigned int> read_index(std::ifstream& file_in, unsigned int cnt_words);

unsigned int chars_to_int(const unsigned char* a);

void int_to_chars(unsigned char* k_chars, unsigned int a);

