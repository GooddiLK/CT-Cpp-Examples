#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

#define EI_NIDENT (16)
#define SYMTAB_SIZE (16) //Размер одной записи в symtab

struct Header {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
};

struct Elf32_Shdr {
    Elf32_Word sh_name;
    Elf32_Word sh_type;
    Elf32_Word sh_flags;
    Elf32_Addr sh_addr;
    Elf32_Off sh_offset;
    Elf32_Word sh_size;
    Elf32_Word sh_link;
    Elf32_Word sh_info;
    Elf32_Word sh_addralign;
    Elf32_Word sh_entsize;
};

struct Elf32_Sym {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
};

void read_header(std::vector<int>& file, Header& header);
Elf32_Half read_half(std::vector<int>& file, int& ptr);
uint32_t read_whole(std::vector<int>& file, int& ptr);
void print_header(Header);

void read_sh(std::vector<int>& file, Elf32_Shdr* sh, Header& header);
void print_sh(Elf32_Shdr* sh, Header& header);

void read_real_names(std::vector<int>& file, std::vector<std::vector<char>>& sh_names, Header& header, Elf32_Shdr* sh);


void parse_text(std::vector<int>& file, Elf32_Shdr* sh, std::vector<std::vector<char>>& sh_names, Header& header, FILE* fp);
bool is_text(std::vector<char>& name);
void final_print_text(std::vector<int>& file, Elf32_Shdr* sh, int indx, FILE* fp, Elf32_Sym* symtab_header,
    int names_start_ptr, int sym_num, std::vector<unsigned int>& Ls);
void print_command(std::vector<int>& file, unsigned int command_adress, int command_start_idx, 
    FILE* fp, Elf32_Sym* symtab_header, int sym_num, std::vector<unsigned int>& Ls, int names_start_ptr);
int find_symtab(std::vector<std::vector<char>>& sh_names, int e_shnum);
int find_meet_Ls(std::vector<unsigned int>& Ls, unsigned int command_adress);
void print_Utype(unsigned int command, int command_adress, FILE* fp, std::string name);
void print_Jtype(unsigned int command, int command_adress, FILE* fp, std::string name,  Elf32_Sym* symtab_header,
    int sym_num, std::vector<unsigned>& Ls, int names_start_ptr, std::vector<int>& file);
void print_Itype(unsigned int command, int command_adress, FILE* fp, std::string name, int type_flag);
void print_Stype(unsigned int command, int command_adress, FILE* fp, std::string name);
void print_Btype(unsigned int command, int command_adress, FILE* fp, std::string name,
            Elf32_Sym* symtab_header, int sym_num, std::vector<unsigned>& Ls, int names_start_ptr, std::vector<int>& file);
void print_Rtype(unsigned int command, int command_adress, FILE* fp, std::string name, int flag_out);
void print_invalid(FILE* fp);
std::string parse_arg_1(unsigned int command);
std::string parse_arg_2(unsigned int command);
std::string parse_arg_3(unsigned int command);
std::string parse_arg_3_mod(unsigned int command);
std::string I_imm(unsigned int command);
std::string S_imm(unsigned int command);
unsigned int B_imm(unsigned int command);
unsigned int U_imm(unsigned int command);
unsigned int J_imm(unsigned int command);
std::string find_command_name(std::vector<int>& file, Elf32_Sym* symtab_header, int sym_num, int command_adress, int names_start_ptr, std::vector<unsigned int>& Ls);
int find_meet(Elf32_Sym* symtab_header, int sym_num, int command_adress);
std::string give_pre_or_suc(unsigned int command, bool is_pre);
unsigned int give_fm(unsigned int command);

void create_labels(std::vector<int>& file, Elf32_Shdr* sh, int indx, 
            Elf32_Sym* symtab_header, int names_start_ptr, int sym_num, std::vector<unsigned int>& Ls);
void selecting_tagging_commands(std::vector<int>& file, unsigned int command_adress, int command_start_idx, Elf32_Sym* symtab_header, 
            int sym_num, std::vector<unsigned int>& Ls, int names_start_ptr);
void create_necessary_label(std::vector<int>& file, unsigned int command, unsigned int command_adress,
    std::vector<unsigned int>& Ls, Elf32_Sym* symtab_header, int sym_num, int names_start_ptr, bool is_B_flag);

void parse_symtab(std::vector<int>& file, Elf32_Shdr* sh, std::vector<std::vector<char>>& sh_names, Header& header, FILE* fp);
int find_strtab(std::vector<std::vector<char>> sh_names, Header header);
bool is_symtab(std::vector<char>& name);
void read_symtab_header(std::vector<int>& file, Elf32_Sym* symtab_header, int offset, int sym_num);
void final_print_symtab(std::vector<int>& file, Elf32_Sym* symtab_header, int sym_num, int names_start_ptr, FILE* fp);
std::string symtab_type(Elf32_Sym* symtab_header, int i);
std::string symtab_bind(Elf32_Sym* symtab_header, int i);
std::string symtab_vis(Elf32_Sym* symtab_header, int i);
std::string symtab_index(Elf32_Sym* symtab_header, int i);
void symtab_name(std::vector<int>& file, int names_start_ptr, Elf32_Sym* symtab_header, int i, std::vector<char>& tmp_name);

void print_section(Elf32_Shdr sh, Header& header, int i);