#include "Header.hpp"

void read_header(std::vector<int>& file, Header& header) {
    int ptr = 0;
    for (int i = 0; i < EI_NIDENT; i++) {
        header.e_ident[i] = file[ptr++];
    }
    header.e_type = read_half(file, ptr);
    header.e_machine = read_half(file, ptr);
    header.e_version = read_whole(file, ptr);
    header.e_entry = read_whole(file, ptr);
    header.e_phoff = read_whole(file, ptr);
    header.e_shoff = read_whole(file, ptr);
    header.e_flags = read_whole(file, ptr);
    header.e_ehsize = read_half(file, ptr);
    header.e_phentsize = read_half(file, ptr);
    header.e_phnum = read_half(file, ptr);
    header.e_shentsize = read_half(file, ptr);
    header.e_shnum = read_half(file, ptr);
    header.e_shstrndx = read_half(file, ptr);
}

Elf32_Half read_half(std::vector<int>& file, int& ptr) {
    Elf32_Half read_el = 0;
    for(int i = 1; i >= 0; i--) {
        read_el *= 16*16;
        read_el += file[ptr + i]; 
    }
    ptr += 2;
    return read_el;
}

uint32_t read_whole(std::vector<int>& file, int& ptr) {
    uint32_t read_el = 0;
    for(int i = 3; i >= 0; i--) {
        read_el *= 16*16;
        read_el += file[ptr + i]; 
    }
    ptr += 4;
    return read_el;
}

void print_header(Header header) {
    for (int i = 0; i < EI_NIDENT; i++) {
        std::cout << header.e_ident[i];
    }
    std::cout << std::endl;
    std::cout << "e_type " << header.e_type << std::endl;
    std::cout << "e_machine " << header.e_machine << std::endl;
    std::cout << "e_version " << header.e_version  << std::endl;
    std::cout << "e_entry " << header.e_entry << std::endl;
    std::cout << "e_phoff " << header.e_phoff << std::endl;
    std::cout << "e_shoff "<< header.e_shoff << std::endl;
    std::cout << "e_flags "<< header.e_flags << std::endl;
    std::cout << "e_ehsize "<< header.e_ehsize << std::endl;
    std::cout << "e_phentsize "<< header.e_phentsize << std::endl;
    std::cout << "e_phnum "<< header.e_phnum << std::endl;
    std::cout << "e_shentsize "<< header.e_shentsize << std::endl;
    std::cout << "e_shnum "<< header.e_shnum << std::endl;
    std::cout << "e_shstrndx "<< header.e_shstrndx << std::endl;
}