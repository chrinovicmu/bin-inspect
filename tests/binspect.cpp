#include <iostream>
#include <stdint.h>
#include <string> 
#include <vector> 
#include <filesystem>
#include "loader.h"

int main (int argc, char *argv[]) {

    size_t x; 
    Binary bin; 
    Section *sec; 
    Symbol *sym; 
    std::string fname; 

    if(argc < 2){
        std::cerr << "Usage: " << std::filesystem::path(argv[0]).filename()
            << " <binary>\n"; 

        return EXIT_FAILURE; 
    }

    fname = argv[1];

    if(load_binary(fname, &bin, Binary::BIN_TYPE_AUTO) < 0){
        std::cerr << "failed to load binary: " << fname << "\n"; 
        return EXIT_FAILURE; 
    }

    bin.print_bin_info(); 
    bin.print_bin_sections();
    bin.print_bin_symbols();
    return 0; 



}
