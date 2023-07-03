#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <map>

std::string trim(const std::string &line) {
    std::string filtered_read = std::regex_replace(line, std::regex("^ +| +$|( ) +"), "$1");
    return filtered_read;
}

std::string return_reg_value(const std::string &reg) {
    std::map<std::string, std::string> registers = {
            {"r0", "000"},
            {"r1", "001"},
            {"r2", "010"},
            {"r3", "011"},
            {"r4", "100"},
            {"r5", "101"},
            {"r6", "110"},
            {"r7", "111"}
    };
    auto location = registers.find(reg);
    return location->second;
}

std::string return_opcode_value(const std::string &opcode) {
    std::map<std::string, std::string> opcodes = {
            {"ADD",   "0000"},
            {"SUB",   "0001"},
            {"AND",   "0010"},
            {"OR",    "0011"},
            {"JMP",   "0100"},
            {"JGT",   "0101"},
            {"JLT",   "0110"},
            {"JEQ",   "0111"},
            {"INC",   "1001"},
            {"DEC",   "1010"},
            {"NOT",   "1011"},
            {"LOAD",  "1100"},
            {"STORE", "1101"}
    };
    auto location = opcodes.find(opcode);
    return location->second;
}

bool check_syntax(const std::string &operation, const std::string &filtered_read) {
    //Regular expressionstest
    std::regex add(
            "(ADD [rR][0-7], (#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))|(ADD [rR][0-7],(#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))");
    std::regex andd(
            "(AND [rR][0-7], (#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))|(AND [rR][0-7],(#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))");
    std::regex sub(
            "(SUB [rR][0-7], (#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))|(SUB [rR][0-7],(#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))");
    std::regex oor(
            "(OR [rR][0-7], (#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))|(OR [rR][0-7],(#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))");
    std::regex jmp("(JMP (#[0-9]+|[a-z]+|[0-9]+))");
    std::regex jgt(
            "(JGT [rR][0-7], (#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))|(JGT [rR][0-7],(#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))");
    std::regex jlt(
            "(JLT [rR][0-7], (#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))|(JLT [rR][0-7],(#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))");
    std::regex jeq(
            "(JEQ [rR][0-7], (#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))|(JEQ [rR][0-7],(#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))");
    std::regex inc("(INC [rR][0-7]+)");
    std::regex dec("(DEC [rR][0-7]+)");
    std::regex nnot("(NOT [rR][0-7]+)");
    std::regex load(
            "(LOAD [rR][0-7], (#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))|(LOAD [rR][0-7],(#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))");
    std::regex store(
            "(STORE [rR][0-7], (#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))|(STORE [rR][0-7],(#[0-9]+|[a-z]+|[0-9]+|[rR][0-7]))");

    std::map<std::string, std::regex> syntax{
            {"ADD",   add},
            {"SUB",   sub},
            {"AND",   andd},
            {"OR",    oor},
            {"JMP",   jmp},
            {"JGT",   jgt},
            {"JLT",   jlt},
            {"JEQ",   jeq},
            {"INC",   inc},
            {"DEC",   dec},
            {"NOT",   nnot},
            {"LOAD",  load},
            {"STORE", store}
    };
    if (syntax.find(operation) != syntax.end()) {
        auto index = syntax.find(operation);
        if (std::regex_match(filtered_read, index->second)) {
            return true;
        }
    }
    return false;
}

bool check_loop_variable(const std::string &loop) {
    std::regex syntax("([a-z]+):");
    if (std::regex_match(loop, syntax))
        return true;
    return false;
}

std::string binary_converter(int number) {
    bool flag = (number > 63);
    const int bits = ((flag) ? 21 : 5);
    std::string binaryValue{};
    for (int i{bits}; i >= 0; i--) {
        int value = number >> i;
        if (value & 1)
            binaryValue.push_back('1');
        else
            binaryValue.push_back('0');
    }
    return binaryValue;
}

bool check_variable_names(const std::map<std::string, std::string> &variable_names, const std::string &operand) {
    auto iterator = variable_names.find(operand);
    if (iterator != variable_names.end())
        return true;
    return false;
}

bool check_loop_names(const std::map<std::string, std::string> &loop_names, const std::string &operand) {
    auto iterator = loop_names.find(operand);
    if (iterator != loop_names.end())
        return true;
    return false;
}

int main(int argc, char **argv) {
    std::vector<std::string> instructions{};
    std::map<std::string, std::string> variable_names{};
    std::map<std::string, std::string> loop_names{};
    //Writing the .sal file through argv
    std::string reader_file_name(argv[1]);
    if (!std::regex_match(reader_file_name, std::regex("([a-z]*)([0-9]*).sal|([0-9]*)([a-z]*).sal"))) {
        std::cerr << "Error: Wrong file format" << std::endl;
        return 1;
    }
    std::ifstream reader{reader_file_name};
    //Making the .bin file
    int sal_position = reader_file_name.find(".sal");
    std::string writer_file_name = reader_file_name.substr(0, sal_position) + ".bin";
    std::ofstream writing{writer_file_name};
    //Opening the files
    if (!reader) {
        std::cerr << "Error: Could not open file. Make sure that you selected the proper file!" << std::endl;
        return 1;
    }
    if (!writing) {
        std::cerr << "Error: Could not open file!" << std::endl;
        return 1;
    }
    std::string check_drop{".drop"};
    std::string read;
    bool start = false;
    bool mainStart = false;
    bool dataflag = false;
    bool mainStartdata = false;
    int variable_value = 0;
    int line_counter = 0;
    while (std::getline(reader, read)) {
        std::string input = read.substr(0, read.find(' '));
        if (input == ".data")
            dataflag = true;
        if (dataflag && mainStartdata) {
            if (input != ".code") {
                std::string filtered_read = trim(read);
                //std::cout << filtered_read << std::endl;
                variable_names.insert(std::make_pair(filtered_read, binary_converter(variable_value)));
                variable_value++;
            }
        }
        if (!start) {
            if (input == ".code") {
                start = true;
                dataflag = false;
            }
        }
        //Part that process the .code part
        if (start && mainStart) {
            //Filter the string by removing the leading/trailing spaces via trim method
            std::string filtered_read = trim(read);
            std::string filtered_comments{};
            int comment_position = filtered_read.find("//");
            if (comment_position != std::string::npos) {
                filtered_comments = filtered_read.substr(0, comment_position);
                filtered_comments = filtered_comments.erase(comment_position, filtered_comments.size());
                filtered_read = trim(filtered_comments);
            }
            if (filtered_read.size() == 0)
                continue;
            //Count the number of words in the string
            int count = 1;
            for (int i = 0; filtered_read[i] != '\0'; i++) {
                if (filtered_read[i] == ' ')
                    count++;
            }
            if (check_loop_variable(filtered_read)) {
                int word_len = filtered_read.size();
                std::string loop_var = filtered_read.substr(0, word_len - 1);
                loop_names.insert(std::make_pair(loop_var, binary_converter(line_counter)));
                line_counter++;
                continue;
            }
            //Check whether if the syntax matches
            //std::cout<<filtered_read
            bool syntax_check = false;
            std::string operation = filtered_read.substr(0, filtered_read.find(' '));
            if (check_syntax(operation, filtered_read))
                syntax_check = true;
            //Put the stuff from the current string line into a vector of strings
            if (syntax_check) {
                instructions.push_back(filtered_read);
                line_counter++;
            } else {
                std::cerr << "Syntax Error at line " << line_counter << std::endl;
                return 1;
            }
        }
        if (start) {
            mainStart = true;
        }
        if (dataflag) {
            mainStartdata = true;
        }
    }
    for (const std::string &instruction: instructions) {
        int opcode_space = instruction.find(' ');
        std::string opcode = instruction.substr(0, opcode_space);
        std::string final_instruction{};
        //Print the opcode first
        final_instruction += return_opcode_value(opcode);
        if (instruction.find(',') != std::string::npos || instruction.find(", ") != std::string::npos) {
            //Process the register value
            std::string reg_value = instruction.substr(opcode_space + 1, 2);
            std::transform(reg_value.begin(), reg_value.end(), reg_value.begin(),
                           [](char c) { return std::tolower(c); });
            final_instruction += return_reg_value(reg_value);
            //Process the operand now
            std::string operand = instruction.substr(opcode_space + 4, instruction.size());
            std::string space_check = instruction.substr(opcode_space + 4, 1);

            if (space_check == " ") {
                operand.erase(remove(operand.begin(), operand.end(), ' '), operand.end());
            }
            //Process the operand now
            if (operand.at(0) == '#') {
                std::string number_operand = operand.substr(1, operand.size());
                int decimal_value = std::stoi(number_operand);
                final_instruction += ((decimal_value > 63) ? "1" : "0");
                final_instruction += "01" + binary_converter(decimal_value);
            } else if (check_variable_names(variable_names, operand))
                final_instruction += "010" + variable_names.find(operand)->second;
            else if (check_loop_names(loop_names, operand))
                final_instruction += "010" + loop_names.find(operand)->second;
            else if (operand.at(0) == 'r' || operand.at(0) == 'R') {
                std::string reg_value_operand = operand.substr(1, 2);
                int reg_value_number = std::stoi(reg_value_operand);
                final_instruction += "000" + binary_converter(reg_value_number);
            } else {
                int haha = stoi(operand);
                std::string test = binary_converter(haha);
                final_instruction += ((haha > 63) ? "1" : "0");
                final_instruction += "10" + test;
            }
        } else {
            int instruction_size = instruction.size();
            std::string operand = instruction.substr(opcode_space + 1, instruction_size);
            int operand_size = operand.size();

            if (opcode == "JMP") {
                final_instruction += +"000010";
                //Check what type of instruction it is i.e. register, operand or #operand
                if (operand.at(0) == '#') {
                    std::string chopped = operand.substr(1, operand_size - 1);
                    int decimal_value = std::stoi(chopped);
                    final_instruction += binary_converter(decimal_value);
                } else {
                    auto iterator = loop_names.find(operand);
                    if (iterator != loop_names.end())
                        final_instruction += iterator->second;
                }
            } else {
                if (operand.at(0) == 'r' || 'R') {
                    std::string reg_value = operand.substr(0, operand_size);
                    std::transform(reg_value.begin(), reg_value.end(), reg_value.begin(),
                                   [](char c) { return std::tolower(c); });
                    final_instruction += return_reg_value(reg_value);
                    final_instruction += "011000000";
                }
            }
        }
        if (final_instruction.size() == 32) {
            std::string final_instruction_1 = final_instruction.substr(0, 16) + "\n";
            std::string final_instruction_2 = final_instruction.substr(16, 16) + "\n";
            writing << final_instruction_1 << final_instruction_2;
        } else
            writing << final_instruction << "\n";
    }
    return 0;
}