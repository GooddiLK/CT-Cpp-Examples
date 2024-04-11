#include "Header.hpp"

void parse_text(std::vector<int>& file, Elf32_Shdr* sh, std::vector<std::vector<char>>& sh_names, Header& header, FILE* fp) {
    int symtab_idx = find_symtab(sh_names, header.e_shnum);
    int names_start_ptr = sh[find_strtab(sh_names, header)].sh_offset;
    int sym_num = sh[symtab_idx].sh_size / SYMTAB_SIZE;

    Elf32_Sym* symtab_header = new Elf32_Sym[sym_num];
    read_symtab_header(file, symtab_header, sh[symtab_idx].sh_offset, sym_num);

    std::vector<unsigned int> Ls;

    for (int i = 0; i < header.e_shnum; i++) {
        if (is_text(sh_names[i])) {
            create_labels(file, sh, i, symtab_header, names_start_ptr, sym_num, Ls);
        }
    }

    for (int i = 0; i < header.e_shnum; i++) {
        if (is_text(sh_names[i])) {
            final_print_text(file, sh, i, fp, symtab_header, names_start_ptr, sym_num, Ls);
        }
    }

    delete[] symtab_header;
}

void final_print_text(std::vector<int>& file, Elf32_Shdr* sh, int indx, FILE* fp, 
            Elf32_Sym* symtab_header, int names_start_ptr, int sym_num, std::vector<unsigned int>& Ls) {
    fprintf(fp, ".text\n");

    for(int i = 0; i < sh[indx].sh_size / 4; i++) {
        print_command(file, sh[indx].sh_addr + i * 4, sh[indx].sh_offset + i * 4, fp, symtab_header, sym_num, Ls, names_start_ptr);
    }
    fprintf(fp, "\n");
}

void print_command(std::vector<int>& file, unsigned int command_adress, int command_start_idx, FILE* fp,
            Elf32_Sym* symtab_header, int sym_num, std::vector<unsigned int>& Ls, int names_start_ptr) {
    unsigned int command = read_whole(file, command_start_idx);
    std::string command_name = find_command_name(file, symtab_header, sym_num, command_adress, names_start_ptr, Ls);
    if (command_name.length()) {
         fprintf(fp, "\n%08x \t<%s>:\n", command_adress, command_name.c_str());
    }

    switch(command & ((1U << 7) - 1)) 
    {
    case 0b0110111 :
        print_Utype(command, command_adress, fp, "lui");
        break;
    case 0b0010111 :
        print_Utype(command, command_adress, fp, "auipc");
        break;
    case 0b1101111 :
        print_Jtype(command, command_adress, fp, "jal", symtab_header, sym_num, Ls, names_start_ptr, file);
        break;
    case 0b1100111 :
        print_Itype(command, command_adress, fp, "jalr", 2);
        break;
    case 0b1100011 :
        switch((command & (((1U << 15) - 1) - ((1U << 12) - 1))) >> 12)
        {
        case 0b000 :
            print_Btype(command, command_adress, fp, "beq", symtab_header, sym_num, Ls, names_start_ptr, file);
            break;
        case 0b001 :
            print_Btype(command, command_adress, fp, "bne", symtab_header, sym_num, Ls, names_start_ptr, file);
            break;
        case 0b100 :
            print_Btype(command, command_adress, fp, "blt", symtab_header, sym_num, Ls, names_start_ptr, file);
            break;
        case 0b101 :
            print_Btype(command, command_adress, fp, "bge", symtab_header, sym_num, Ls, names_start_ptr, file);
            break;
        case 0b110 :
            print_Btype(command, command_adress, fp, "bltu", symtab_header, sym_num, Ls, names_start_ptr, file);
            break;
        case 0b111 :
            print_Btype(command, command_adress, fp, "bgeu", symtab_header, sym_num, Ls, names_start_ptr, file);
            break;
        default :
            print_invalid(fp);
        }
        break;
    case 0b0000011 :
        switch((command & (((1U << 15) - 1) - ((1U << 12) - 1))) >> 12)
        {
        case 0b000 :
            print_Itype(command, command_adress, fp, "lb", 2);
            break;
        case 0b001 :
            print_Itype(command, command_adress, fp, "lh", 2);
            break;
        case 0b010 :
            print_Itype(command, command_adress, fp, "lw", 2);
            break;
        case 0b100 :
            print_Itype(command, command_adress, fp, "lbu", 2);
            break;
        case 0b101 :
            print_Itype(command, command_adress, fp, "lhu", 2);
            break;
        default :
            print_invalid(fp);
        }
        break;
    case 0b0100011 :
        switch((command & (((1U << 15) - 1) - ((1U << 12) - 1))) >> 12)
        {
        case 0b000 :
            print_Stype(command, command_adress, fp, "sb");
            break;
        case 0b001 :
            print_Stype(command, command_adress, fp, "sh");
            break;
        case 0b010 :
            print_Stype(command, command_adress, fp, "sw");
            break;
        default :
            print_invalid(fp);
        }
        break;
    case 0b0010011 :
        switch((command & (((1U << 15) - 1) - ((1U << 12) - 1))) >> 12)
        {
        case 0b000 :
            print_Itype(command, command_adress, fp, "addi", 1);
            break;
        case 0b010 :
            print_Itype(command, command_adress, fp, "slti", 1);
            break;
        case 0b011 :
            print_Itype(command, command_adress, fp, "sltiu", 1);
            break;
        case 0b100 :
            print_Itype(command, command_adress, fp, "xori", 1);
            break;
        case 0b110 :
            print_Itype(command, command_adress, fp, "ori", 1);
            break;
        case 0b111 :
            print_Itype(command, command_adress, fp, "andi", 1);
            break;
        case 0b001 :
            print_Rtype(command, command_adress, fp, "slli", 2);
            break;
        case 0b101 :
            if (!(command & (1U << 30))) {
                print_Rtype(command, command_adress, fp, "srli", 2);
            } else {
                print_Rtype(command, command_adress, fp, "srai", 2);
            }
            break;
        default :
            print_invalid(fp);
        }
        break;
    case 0b0110011 :
        if (((command & (((1U << 7) - 1) << 25)) >> 25) == (0b0000001)) {
        //RV32M
            switch((command & (((1U << 15) - 1) - ((1U << 12) - 1))) >> 12)
            {
            case 0b000 :
                print_Rtype(command, command_adress, fp, "mul", 1);
                break;
            case 0b001 :
                print_Rtype(command, command_adress, fp, "mulh", 1);
                break;
            case 0b010 :
                print_Rtype(command, command_adress, fp, "mulhsu", 1);
                break;
            case 0b011 :
                print_Rtype(command, command_adress, fp, "mulhu", 1);
                break;
            case 0b100 :
                print_Rtype(command, command_adress, fp, "div", 1);
                break;
            case 0b101 :
                print_Rtype(command, command_adress, fp, "divu", 1);
                break;
            case 0b110 :
                print_Rtype(command, command_adress, fp, "rem", 1);
                break;
            case 0b111 :
                print_Rtype(command, command_adress, fp, "remu", 1);
                break;
            default :
                print_invalid(fp);
            }
        } else {
            switch((command & (((1U << 15) - 1) - ((1U << 12) - 1))) >> 12)
            {
            case 0b000 :
                if(!(command & (1U << 30))) {
                    print_Rtype(command, command_adress, fp, "add", 1);
                } else {
                    print_Rtype(command, command_adress, fp, "sub", 1); 
                }
                break;
            case 0b001 :
                print_Rtype(command, command_adress, fp, "sll", 1);
                break;
            case 0b010 :
                print_Rtype(command, command_adress, fp, "slt", 1);
                break;
            case 0b011 :
                print_Rtype(command, command_adress, fp, "sltu", 1);
                break;
            case 0b100 :
                print_Rtype(command, command_adress, fp, "xor", 1);
                break;
            case 0b101 :
                if(!(command & (1U << 30))) {
                    print_Rtype(command, command_adress, fp, "srl", 1);
                } else {
                    print_Rtype(command, command_adress, fp, "sra", 1);
                }
                break;
            case 0b110 :
                print_Rtype(command, command_adress, fp, "or", 1);
                break;
            case 0b111 :
                print_Rtype(command, command_adress, fp, "and", 1);
                break;
            default :
                print_invalid(fp);
            }
        }
        break;
    case 0b0001111 :
        switch ((command & (((1U << 15) - 1) - ((1U << 12) - 1))) >> 12)
        {
        case 0b000:
            print_Itype(command, command_adress, fp, "fence", 3); 
            break;
        case 0b001:
            // Zifencei
            print_Itype(command, command_adress, fp, "fence.i", 1); 
            break;
        default :
            print_invalid(fp);
        }
        break;
    case 0b1110011 :
        if(!(command & (1U << 19))) {
            print_Itype(command, command_adress, fp, "ecall", 4); 
        } else {
            print_Itype(command, command_adress, fp, "ebreak", 4);
        }
        break;
    case 0b0101111 :
        //RV32A
        switch (((command & (((1U << 5) - 1) << 27)) >> 27)) 
        {
        case 0b00010 :
            print_Rtype(command, command_adress, fp, "lr.w", 3);
            break;
        case 0b00011 :
            print_Rtype(command, command_adress, fp, "sc.w", 3);
            break;
        case 0b00001 :
            print_Rtype(command, command_adress, fp, "amoswap.w", 3);
            break;
        case 0b00000 :
            print_Rtype(command, command_adress, fp, "amoadd.w", 3);
            break;
        case 0b00100 :
            print_Rtype(command, command_adress, fp, "amoxor.w", 3);
            break;
        case 0b01100 :
            print_Rtype(command, command_adress, fp, "amoand.w", 3);
            break;
        case 0b01000 :
            print_Rtype(command, command_adress, fp, "amoor.w", 3);
            break;
        case 0b10000 :
            print_Rtype(command, command_adress, fp, "amomin.w", 3);
            break;
        case 0b10100 :
            print_Rtype(command, command_adress, fp, "amomax.w", 3);
            break;
        case 0b11000 :
            print_Rtype(command, command_adress, fp, "amominu.w", 3);
            break;
        case 0b11100 :
            print_Rtype(command, command_adress, fp, "amomaxu.w", 3);
            break;
        default :
            print_invalid(fp);
        }
        break;
    default :
        print_invalid(fp);
    }
}

void print_Itype(unsigned int command, int command_adress, FILE* fp, std::string name, int type_flag) {
    if (type_flag == 1) {
        fprintf(fp, "   %05x:\t%08x\t%7s\t%s, %s, %s\n", 
            command_adress, command, name.c_str(), parse_arg_1(command).c_str(), parse_arg_2(command).c_str(), I_imm(command).c_str());
    } else if (type_flag == 2) {
        fprintf(fp, "   %05x:\t%08x\t%7s\t%s, %s(%s)\n", 
            command_adress, command, name.c_str(), parse_arg_1(command).c_str(), I_imm(command).c_str(), parse_arg_2(command).c_str());
    } else if (type_flag == 3) {
        //fence
        if (give_pre_or_suc(command, 0) == "" && give_pre_or_suc(command, 1) == "w" && give_fm(command) == 0) {
            //Zihintpause
            fprintf(fp, "   %05x:\t%08x\t%7s\n", command_adress, command, name.c_str());
            return;
        }
        fprintf(fp, "   %05x:\t%08x\t%7s\t%s, %s\n", command_adress, command, name.c_str(), give_pre_or_suc(command, 0).c_str(), give_pre_or_suc(command, 1).c_str());
    } else if (type_flag == 4) {
        fprintf(fp, "   %05x:\t%08x\t%7s\n", command_adress, command, name.c_str());
    } 
}

void print_Jtype(unsigned int command, int command_adress, FILE* fp, std::string name, 
            Elf32_Sym* symtab_header, int sym_num, std::vector<unsigned>& Ls, int names_start_ptr, std::vector<int>& file) {
    fprintf(fp, "   %05x:\t%08x\t%7s\t%s, 0x%x <%s>\n", 
        command_adress, command, name.c_str(), parse_arg_1(command).c_str(), command_adress + J_imm(command), find_command_name(file, symtab_header, sym_num, command_adress + J_imm(command), names_start_ptr, Ls).c_str());
}

void print_Utype(unsigned int command, int command_adress, FILE* fp, std::string name) {
    unsigned int tmp = U_imm(command);
    if (tmp & (1U << 19)) {
        tmp += ((1U << 12) - 1) << 20;
    }
    fprintf(fp, "   %05x:\t%08x\t%7s\t%s, 0x%x\n", 
        command_adress, command, name.c_str(), parse_arg_1(command).c_str(), tmp);
}

void print_Btype(unsigned int command, int command_adress, FILE* fp, std::string name,
            Elf32_Sym* symtab_header, int sym_num, std::vector<unsigned>& Ls, int names_start_ptr, std::vector<int>& file) {
    fprintf(fp, "   %05x:\t%08x\t%7s\t%s, %s, 0x%x, <%s>\n", 
        command_adress, command, name.c_str(), parse_arg_2(command).c_str(), parse_arg_3(command).c_str(), command_adress + B_imm(command), find_command_name(file, symtab_header, sym_num, command_adress + B_imm(command), names_start_ptr, Ls).c_str());
}

void print_Stype(unsigned int command, int command_adress, FILE* fp, std::string name) {
    fprintf(fp, "   %05x:\t%08x\t%7s\t%s, %d(%s)\n", 
        command_adress, command, name.c_str(), parse_arg_3(command).c_str(), atoi(S_imm(command).c_str()), parse_arg_2(command).c_str());
}

void print_Rtype(unsigned int command, int command_adress, FILE* fp, std::string name, int flag_out) {
    if (flag_out == 1) {
        fprintf(fp, "   %05x:\t%08x\t%7s\t%s, %s, %s\n", 
            command_adress, command, name.c_str(), parse_arg_1(command).c_str(), parse_arg_2(command).c_str(), parse_arg_3(command).c_str());
    } else if (flag_out == 2) {
        fprintf(fp, "   %05x:\t%08x\t%7s\t%s, %s, %s\n", 
            command_adress, command, name.c_str(), parse_arg_1(command).c_str(), parse_arg_2(command).c_str(), parse_arg_3_mod(command).c_str());
    } else if (flag_out == 3) {
        //RV32A
        if ((command & (1U << 26))) {
            name += ".aq";
        }
        if ((command & (1U << 25))) {
            name += "rl";
        }
        fprintf(fp, "   %05x:\t%08x\t%7s\t%s, %s, %s\n", 
            command_adress, command, name.c_str(), parse_arg_1(command).c_str(), parse_arg_2(command).c_str(), parse_arg_3(command).c_str());
    }
}

void print_invalid(FILE* fp) {
    fprintf(fp, "invalid_instruction\n");
}

std::string I_imm(unsigned int command) {
    unsigned int ans = 0;
    for(int i = 31; i > 10; i--) {
        if ((command & (1U << 31))) {
            ans += 1U << i;
        }
    }
    for(int i = 10; i > 4; i--) {
        if ((command & (1U << (20 + i)))) {
            ans += 1U << i;
        }
    }
    for(int i = 4; i > 0; i--) {
        if ((command & (1U << (20 + i)))) {
            ans += 1U << i;
        }
    }
    ans += (command & (1U << 20)) >> 20;
    return std::to_string(int (ans));
}

std::string S_imm(unsigned int command) {
    unsigned int ans = 0;
    for (int i = 31; i > 10; i--) {
        if ((command & (1U << 31))) {
            ans += 1U << i;
        }
    }
    for (int i = 10; i > 4; i--) {
        if ((command & (1U << (20 + i)))) {
            ans += 1U << i;
        }
    }
    for (int i = 4; i > 0; i--) {
        if ((command & (1U << (7 + i)))) {
            ans += 1U << i;
        }
    }
    ans += (command & (1U << 7)) >> 7;
    return std::to_string(int (ans));
}

unsigned int B_imm(unsigned int command) {
    unsigned int ans = 0;
    
    for (int i = 31; i > 11; i--) {
        if ((command & (1U << 31))) {
            ans += 1U << i;
        }
    }

    ans += (command & (1U << 7)) << 4;
    for (int i = 10; i > 4; i--) {
        if ((command & (1U << (20 + i)))) {
            ans += 1U << i;
        }
    }
    for (int i = 4; i > 0; i--) {
        if ((command & (1U << (7 + i)))) {
            ans += 1U << i;
        }
    }
    return ans;
}

unsigned int U_imm(unsigned int command) {
    unsigned int ans = 0;
    for (int i = 31; i > 11; i--) {
        ans += (command & (1U << i));
    }
    return ans >> 12;
}

unsigned int J_imm(unsigned int command) {
    unsigned int ans = 0;

    for (int i = 31; i > 19; i--) {
        if ((command & (1U << 31))) {
            ans += 1U << i;
        }
    }

    for (int i = 19; i > 11; i--) {
        ans += (command & (1U << i));
    }

    ans += (command & (1U << 20)) >> 9;

    for (int i = 10; i > 4; i--) {
        if ((command & (1U << (20 + i)))) {
            ans += 1U << i;
        }
    }

    for (int i = 4; i > 0; i--) {
        if ((command & (1U << (20 + i)))) {
            ans += 1U << i;
        }
    }

    return ans;
}
std::string parse_arg_1(unsigned int command) {
    return parse_arg_2(command << 8);
}
std::string parse_arg_2(unsigned int command) {

    unsigned int arg = ((((1U << 20) - 1) - ((1U << 15) - 1)) & command) >> 15;

    switch(arg)
    {
    case 0:
        return "zero";   
    case 1:
        return "ra";  
    case 2:
        return "sp";  
    case 3:
        return "gp";   
    case 4:
        return "tp";   
    case 5:
        return "t0";
    case 6:
        return "t1";   
    case 7:
        return "t2";
    case 8:
        return "s0";   
    case 9:
        return "s1";    
    case 10:
        return "a0";
    case 11:
        return "a1";
    default:
        if (arg < 18) {
            std::string s(1, '0' + arg - 10);
            return "a" + s;
        }
        if (arg < 26) {
            std::string s(1, '0' + arg - 16);
            return "s" + s;
        }
        if (arg == 26) {
            return "s10";
        }
        if (arg == 27) {
            return "s11";
        }
        std::string s(1, '0' + arg - 25);
        return "t" + s;
    }         
}

std::string parse_arg_3(unsigned int command) {
    return(parse_arg_2(command >> 5));
}

std::string parse_arg_3_mod(unsigned int command) {
    command >>= 5;
    unsigned int arg = ((((1U << 20) - 1) - ((1U << 15) - 1)) & command) >> 15;
    return std::to_string(arg);
}

int find_meet_Ls(std::vector<unsigned int>& Ls, unsigned int command_adress) {
    for(int i = 0; i < Ls.size(); i++) {
        if (Ls[i] == command_adress) {
            return i;
        }
    }
    return -1;
}

int find_symtab(std::vector<std::vector<char>>& sh_names, int e_shnum) {
    for (int i = 0; i < e_shnum; i++) {
        if(is_symtab(sh_names[i])) {
            return i;
        }
    }
    return -1;
}

bool is_text(std::vector<char>& name) {
    if (name.size() == 5 && name[0] == '.' && name[1] == 't' && name[2] == 'e' && name[3] == 'x' && name[4] == 't') {
        return true;
    }
    return false;
    /*
    if (name[0] == 0 || name[0] != '.') {
        return false;
    }
    if (name[1] == 0 || name[1] != 't') {
        return false;
    }
    if (name[2] == 0 || name[2] != 'e') {
        return false;
    }
    if (name[3] == 0 || name[3] != 'x') {
        return false;
    }
    if (name[4] == 0 || name[4] != 't') {
        return false;
    }
    if (name[5] == 0 || name[5] == 127) {
        return true;
    }
    return false;
    */
}

void create_labels(std::vector<int>& file, Elf32_Shdr* sh, int indx, Elf32_Sym* symtab_header, 
        int names_start_ptr, int sym_num, std::vector<unsigned int>& Ls) {
    for(int i = 0; i < sh[indx].sh_size / 4; i++) {
        selecting_tagging_commands(file, sh[indx].sh_addr + i * 4, sh[indx].sh_offset + i * 4, symtab_header, sym_num, Ls, names_start_ptr);
    }
}

void selecting_tagging_commands(std::vector<int>& file, unsigned int command_adress, int command_start_idx, Elf32_Sym* symtab_header, 
            int sym_num, std::vector<unsigned int>& Ls, int names_start_ptr) {
    unsigned int command = read_whole(file, command_start_idx);
    switch(command & ((1U << 7) - 1)) 
    {
        case 0b1100011 :
        switch((command & (((1U << 15) - 1) - ((1U << 12) - 1))) >> 12)
        {
        case 0b000 :
            create_necessary_label(file, command, command_adress, Ls, symtab_header, sym_num, names_start_ptr, true);
            break;
        case 0b001 :
            create_necessary_label(file, command, command_adress, Ls, symtab_header, sym_num, names_start_ptr, true);
            break;
        case 0b100 :
            create_necessary_label(file, command, command_adress, Ls, symtab_header, sym_num, names_start_ptr, true);
            break;
        case 0b101 :
            create_necessary_label(file, command, command_adress, Ls, symtab_header, sym_num, names_start_ptr, true);
            break;
        case 0b110 :
            create_necessary_label(file, command, command_adress, Ls, symtab_header, sym_num, names_start_ptr, true);
            break;
        case 0b111 :
            create_necessary_label(file, command, command_adress, Ls, symtab_header, sym_num, names_start_ptr, true);
            break;
        }
        break;
        case 0b1101111 :
            create_necessary_label(file, command, command_adress, Ls, symtab_header, sym_num, names_start_ptr, false);
            break;
    }
}

void create_necessary_label(std::vector<int>& file, unsigned int command, unsigned int command_adress, std::vector<unsigned int>& Ls, Elf32_Sym* symtab_header, int sym_num, int names_start_ptr, bool is_B_flag) {
    unsigned int target = command_adress;
    if (is_B_flag) {
        target += B_imm(command);
        std::string command_name = find_command_name(file, symtab_header, sym_num, target, names_start_ptr, Ls);
        if (!command_name.length()) {
            Ls.push_back(target);
        }
        return;
    } else {
        target += J_imm(command);
        std::string command_name = find_command_name(file, symtab_header, sym_num, target, names_start_ptr, Ls);
        if (!command_name.length()) {
            Ls.push_back(target);
        }
        return;
    }
}

std::string find_command_name(std::vector<int>& file, Elf32_Sym* symtab_header, int sym_num, int command_adress, int names_start_ptr, std::vector<unsigned int>& Ls) {
    int tmp = find_meet(symtab_header, sym_num, command_adress);

    if (tmp != -1) {
        std::vector<char> command_name;
        symtab_name(file, names_start_ptr, symtab_header, tmp, command_name);
        if (!command_name.empty()) {
            std::string str(command_name.begin(), command_name.end());
            return str;
        }
    } else {
        tmp = find_meet_Ls(Ls, command_adress);
        if (tmp != -1) {
            return "L" + std::to_string(tmp);
        }
    }
    return "";
}

int find_meet(Elf32_Sym* symtab_header, int sym_num, int command_adress) {
    int res = -1;
    for (int i = 0; i < sym_num; i++) {
        if (symtab_header[i].st_value == command_adress) {
            res = i;
        }
    }
    return res;
}

std::string give_pre_or_suc(unsigned int command, bool is_pre) {
    std::string res = "";
    unsigned int ptr = 1U << 23;
    if (is_pre) {
        ptr <<= 4;
    }
    if ((command & ptr)) {
        res += "i";
    }
    if ((command & (ptr >> 1))) {
        res += "o";
    }
    if ((command & (ptr >> 2))) {
        res += "r";
    }
    if ((command & (ptr >> 3))) {
        res += "w";
    }
    return res;
}

unsigned int give_fm(unsigned int command) {
    return (command & (((1U << 4) - 1)) << 28);
}