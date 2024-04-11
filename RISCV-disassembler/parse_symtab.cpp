#include "Header.hpp"

void parse_symtab(std::vector<int>& file, Elf32_Shdr* sh, std::vector<std::vector<char>>& sh_names, Header& header, FILE* fp) {
    int strtab_idx = find_strtab(sh_names, header);
    if (strtab_idx == -1) {
        return;
    }
    int names_start_ptr = sh[strtab_idx].sh_offset;
    for (int i = 0; i < header.e_shnum; i++) {
        if (is_symtab(sh_names[i])) {
            int sym_num = sh[i].sh_size / SYMTAB_SIZE;
            Elf32_Sym* symtab_header = new Elf32_Sym[sym_num];
            read_symtab_header(file, symtab_header, sh[i].sh_offset, sym_num);
            final_print_symtab(file, symtab_header, sym_num, names_start_ptr, fp);
            delete[] symtab_header;
        }
    }
}

int find_strtab(std::vector<std::vector<char>> sh_names, Header header) {
    for(int i = 0; i < sh_names.size(); i++) {
        if (sh_names[i].size() == 7 && sh_names[i][0] == '.' && sh_names[i][1] == 's' && sh_names[i][2] == 't'
            && sh_names[i][3] == 'r' && sh_names[i][4] == 't' && sh_names[i][5] == 'a' && sh_names[i][6] == 'b') {
            return i;
        }
    }
    return -1;
}

bool is_symtab(std::vector<char>& name) {
    if (name.size() == 7 && name[0] == '.' && name[1] == 's' && name[2] == 'y' && name[3] == 'm' && name[4] == 't'
        && name[5] == 'a' && name[6] == 'b') {
        return true;
    }
    return false;
}

void read_symtab_header(std::vector<int>& file, Elf32_Sym* symtab_header, int offset, int sym_num) {
    int ptr = offset;
    for (int i = 0; i < sym_num; i++) {
        symtab_header[i].st_name = read_whole(file, ptr);
        symtab_header[i].st_value = read_whole(file, ptr);
        symtab_header[i].st_size = read_whole(file, ptr);
        symtab_header[i].st_info = file[ptr++];
        symtab_header[i].st_other = file[ptr++];
        symtab_header[i].st_shndx = read_half(file, ptr);
    }
}

void final_print_symtab(std::vector<int>& file, Elf32_Sym* symtab_header, int sym_num, int names_start_ptr, FILE *fp) {
    fprintf(fp, "\n.symtab\n");
    fprintf(fp, "\nSymbol Value              Size Type     Bind     Vis       Index Name\n");
    for(int i = 0; i < sym_num; i++) {
        std::string type = symtab_type(symtab_header, i); 
        std::string bind = symtab_bind(symtab_header, i);
        std::string vis = symtab_vis(symtab_header, i);
        std::string idx = symtab_index(symtab_header, i);
        std::vector<char> name;
        symtab_name(file, names_start_ptr, symtab_header, i, name);
        std::string out_str (name.begin(), name.end());
        if (out_str.size()) {
            fprintf(fp, "[%4i] 0x%-15X %5i %-8s %-8s %-8s %6s %s\n", i, symtab_header[i].st_value, symtab_header[i].st_size,
                type.c_str(), bind.c_str(), vis.c_str(), idx.c_str(), out_str.c_str());
        } else {
            fprintf(fp, "[%4i] 0x%-15X %5i %-8s %-8s %-8s %6s %s\n", i, symtab_header[i].st_value, symtab_header[i].st_size,
                type.c_str(), bind.c_str(), vis.c_str(), idx.c_str(), "");
        }
    }
}

std::string symtab_type(Elf32_Sym* symtab_header, int i) {
    switch (symtab_header[i].st_info & 0xf) 
    {
    case 0 :
        return "NOTYPE\0";
    case 1 :
        return "OBJECT\0";
    case 2 :
        return "FUNC\0";
    case 3 :
        return "SECTION\0";
    case 4 :
        return "FILE\0";
    case 5 :
        return "COMMON\0";
    case 6 :
        return "TLS\0";
    case 10 :
        return "LOOS\0";
    case 12 :
        return "HIOS\0";
    case 13 :
        return "LOPROC\0";
    case 15 :
        return "HIPROC\0";
    }
    return "\0";
}

std::string symtab_bind(Elf32_Sym* symtab_header, int i) {
    switch (symtab_header[i].st_info >> 4) 
    {
    case 0:
        return "LOCAL\0";
    case 1:
        return "GLOBAL\0";
    case 2:
        return "WEAK\0";
    case 10:
        return "LOOS\0";
    case 12:
        return "HIOS\0";
    case 13:
        return "LOPROC\0";
    case 15:
        return "HIPROC\0";
    }
    return "\0";
}

std::string symtab_vis(Elf32_Sym* symtab_header, int i) {
    switch (symtab_header[i].st_other)
    {
    case 0:
        return "DEFAULT\0";
    case 1:
        return "INTERNAL\0";
    case 2:
        return "HIDDEN\0";
    case 3:
        return "PROTECTED\0";
    case 4:
        return "EXPORTED\0";
    case 5:
        return "SINGLETON\0";
    case 6:
        return "ELIMINATE\0";
    }
    return "\0";
}

std::string symtab_index(Elf32_Sym* symtab_header, int i) {
    switch (symtab_header[i].st_shndx)
    {
    case 0:
        return "UNDEF\0";
    case 0xff00:
        return "LORESERVE\0";
    case 0xff01:
        return "AFTER\0";
    case 0xff02:
        return "AMD64_LCOMMON\0";
    case 0xff1f:
        return "HIPROC\0";
    case 0xff20:
        return "LOOS\0";
    case  0xff3f:
        return "LOSUNW\0"; 
    case  0xfff1:
        return "ABS\0"; 
    case  0xfff2:
        return "COMMON\0"; 
    case  0xffff:
        return "SHN_XINDEX\0"; 
    default:
        int tmp = symtab_header[i].st_shndx;
        return std::to_string(tmp);
    }
}

void symtab_name(std::vector<int>& file, int names_start_ptr, Elf32_Sym* symtab_header, int i, std::vector<char>& tmp_name) {
    if (i < 0 || !symtab_header[i].st_name) {
        return ;
    }
    int ptr = names_start_ptr + symtab_header[i].st_name;
    while(file[ptr++]) {
        tmp_name.push_back(file[ptr - 1]);
    }
}