#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

class LZW {
private:
    std::ifstream file_in;
    std::ofstream file_out;

    void open_files_analysis(const std::string& filename);

    void open_files_decompress(const std::string& filename);

    void close_files();
public:

};
