#include "Header.hpp"

void read_sh(std::vector<int>& file, Elf32_Shdr* sh, Header& header) {
    for (int i = 0; i < header.e_shnum; i++) {
        int ptr = header.e_shoff + i * header.e_shentsize;
        Elf32_Shdr tmp;
        tmp.sh_name = read_whole(file, ptr);
        tmp.sh_type = read_whole(file, ptr);
        tmp.sh_flags = read_whole(file, ptr);
        tmp.sh_addr = read_whole(file, ptr);
        tmp.sh_offset = read_whole(file, ptr);
        tmp.sh_size = read_whole(file, ptr);
        tmp.sh_link = read_whole(file, ptr);
        tmp.sh_info = read_whole(file, ptr);
        tmp.sh_addralign = read_whole(file, ptr);
        tmp.sh_entsize = read_whole(file, ptr);
        sh[i] = (tmp);
    }
}

void print_sh(Elf32_Shdr* sh, Header& header) {
    for(int i = 0; i < header.e_shnum; i++) {
        std::cout << sh[i].sh_name << std::endl;
    }
}
//--------------------------------------------------------------------
void read_real_names(std::vector<int>& file, std::vector<std::vector<char>>& sh_names, Header& header, Elf32_Shdr* sh) {
    int strtab_idx = sh[header.e_shstrndx].sh_offset;
    for(int i = 0; i < header.e_shnum; i++) {
        std::vector<char> tek_name;
        int j = 0;
        while(file[strtab_idx + sh[i].sh_name + j]) {
            tek_name.push_back(file[strtab_idx + sh[i].sh_name + j]);
            j++;
        }
        sh_names[i] = tek_name;
        //ar_copy(sh_names, tek_name, i);
    }
}
