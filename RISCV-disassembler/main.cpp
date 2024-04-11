#include "Header.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uncorrect argument number of command line" << std::endl;
        return 0;
    }
    std::ifstream in(argv[1], std::ios::binary|std::ios::in);
    std::vector<int> file;
    while(true) {
        file.push_back(in.get());
        if (in.eof()) {
            break;
        }
    }
    FILE *fp;
    if(!(fp = fopen(argv[2], "w"))) {
        std::cerr << "Can`t open output file" << std::endl;
        return 0;
    }
    
    Header header;
    read_header(file, header);

    Elf32_Shdr* section_header = new Elf32_Shdr[header.e_shnum];
    read_sh(file, section_header, header);

    std::vector<std::vector<char>> sh_names;
    for (int i = 0 ; i < header.e_shnum; i++) {
        std::vector<char> tmp;
        sh_names.push_back(tmp);
    }
    read_real_names(file, sh_names, header, section_header);

    parse_text(file, section_header, sh_names, header, fp);
    parse_symtab(file, section_header, sh_names, header, fp);

    fclose(fp);
    return 0;
}