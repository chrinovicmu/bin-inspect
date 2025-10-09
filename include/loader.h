#ifndef LOADER_H 
#define LOADER_H

#include <stdint.h> 
#include <string>
#include <vector> 
#include <iomanip> 

class Binary; 
class Section; 
class Symbol; 

class Symbol{
public:

    enum SymbolType{
        SYM_TYPE_UNKNOWN = 0, 
        SYM_TYPE_FUNC   = 1,
        SYM_TYPE_OBJECT = 3, 
        SYM_TYPE_LABEL  = 4 
    }; 

    Symbol() : type(SYM_TYPE_UNKNOWN), name(), addr(0) {}
    SymbolType  type;
    std::string name; 
    uint64_t    addr; 
}; 

class Section {
public:

    enum SectionType{
        SEC_TYPE_NONE = 0, 
        SEC_TYPE_CODE = 1, 
        SEC_TYPE_DATA = 2
    };

    Section() : binary(NULL), type(SEC_TYPE_NONE), 
                vma(0), size(0), bytes(NULL) {}

    bool contains(uint64_t addr){
        return (addr >= vma) && (addr-vma < size); 
    }

    Binary          *binary; 
    std::string     name;
    SectionType     type; 
    uint64_t        vma;
    uint64_t        size; 
    uint8_t         *bytes; 
}; 

class Binary{
public:

    enum BinaryType {
        BIN_TYPE_AUTO = 0, 
        BIN_TYPE_ELF =  1,
        BIN_TYPE_COFF = 2,
        BIN_TYPE_PE = 3, 
        BIN_TYPE_MACHO
    };

    enum BinaryArch {
        ARCH_NONE, 
        ARCH_X86, 
        ARCH_ARM,
        ARCH_RISCV 
    };

    Binary() : type(BIN_TYPE_AUTO), arch(ARCH_NONE), bits(0), entry(0) {}

    Section *get_text_section(){
        for(auto &s: sections)
            if(s.name == ".text")
                return &s; 
        return NULL; 
    }

    void print_bin_info() const {
        std::cout << "Filename: " << filename << "\n"; 
        std::cout << "Type: " << type_str << "\n";
        std::cout << "Arch: " << arch_str << "(" << bits << "-bits)" << "\n";
        std::cout << "Entry: " << std::hex << entry << std::dec << "\n"; 
        std::cout << "Sections: " << sections.size() << "\n"; 
        std::cout << "Symbols: " << symbols.size() << "\n\n"; 
    }

    void print_bin_sections() const {

        std::cout << "\nSections:\n";
        std::cout <<  std::left 
            << std::setw(20) << "Name" 
            << std::setw(12) << "VMA" 
            << std::setw(12) << "Size" 
            << std::setw(12) << "Type" << "\n"; 
        std::cout << std::string(56, '-') << "\n"; 

        for(const auto& s : sections){
            std::cout << std::left
                << std::setw(20) << s.name
                << std::setw(12) << std::hex << s.vma
                << std::setw(12) << s.size 
                << std::setw(12) << (s.type == Section::SEC_TYPE_CODE ? "CODE" :
                                     s.type == Section::SEC_TYPE_DATA ? "DATA" : "NONE")
                << std::dec << "\n"; 
        }

    }

     void print_bin_symbols() const {

        std::cout << "\nSymbols:\n";
        std::cout << std::left
                  << std::setw(30) << "Name"
                  << std::setw(12) << "Addr" << "\n";
        std::cout << std::string(54, '-') << "\n";

        for (const auto& sym : symbols) {
            std::cout << std::left
                      << std::setw(30) << sym.name
                      << std::setw(12) << std::hex << sym.addr
                      << std::dec << "\n";
        }
    }

    std::string     filename; 
    BinaryType      type; 
    std::string     type_str; 
    BinaryArch      arch; 
    std::string     arch_str; 
    uint64_t        entry;
    uint64_t        bits; 
    std::vector<Section> sections; 
    std::vector<Symbol> symbols; 
}; 

int load_binary(std::string &fname, Binary *bin, Binary::BinaryType type); 
void unload_binary(Binary *bin); 

#endif 
