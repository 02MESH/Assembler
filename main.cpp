#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <string>
#include <regex>
#include <map>

//Removing leading and trailing whitespaces
std::string trim(const std::string &line) {
    std::string filtered_read = std::regex_replace(line, std::regex("^ +| +$|( ) +"), "$1");
    return filtered_read;
}

//Returns the binary equivalent of the supplied register.
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

//Returns the binary equivalent of the opcode e.g. ADD = 0000
std::string return_opcode_value(const std::string &opcode) {
    std::map<std::string, std::string> opcodes = {
            {"ADD",   "0000"},
            {"AND",   "0001"},
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

//Pass in string to check whether if it is a word. Operand check
bool check_string(const std::string &word) {
    std::regex letter_check("[a-z]+");
    if (std::regex_match(word, letter_check))
        return true;
    return false;
}

//Checks the syntax for the given line
bool check_syntax(const std::string &operation, const std::string &filtered_read) {
    bool validity = false;
    //Regular expressionstest
    std::regex add("(ADD r[0-7], (#[0-9]+|[a-z]+|[0-9]+|r[0-7]))|(ADD r[0-7],(#[0-9]+|[a-z]+|[0-9]+|r[0-7]))");
    std::regex sub("(SUB r[0-7], (#[0-9]+|[a-z]+|[0-9]+|r[0-7]))|(SUB r[0-7],(#[0-9]+|[a-z]+|[0-9]+|r[0-7]))");
    std::regex oor("(OR r[0-7], (#[0-9]+|[a-z]+|[0-9]+|r[0-7]))|(OR r[0-7],(#[0-9]+|[a-z]+|[0-9]+|r[0-7]))");
    std::regex jmp("(JMP (#[0-9]+|[a-z]+|[0-9]+))");
    std::regex jgt("(JGT r[0-7], (#[0-9]+|[a-z]+|[0-9]+|r[0-7]))|(JGT r[0-7],(#[0-9]+|[a-z]+|[0-9]+|r[0-7]))");
    std::regex jlt("(JLT r[0-7], (#[0-9]+|[a-z]+|[0-9]+|r[0-7]))|(JLT r[0-7],(#[0-9]+|[a-z]+|[0-9]+|r[0-7]))");
    std::regex jeq("(JEQ r[0-7], (#[0-9]+|[a-z]+|[0-9]+|r[0-7]))|(JEQ r[0-7],(#[0-9]+|[a-z]+|[0-9]+|r[0-7]))");
    std::regex inc("(INC r[0-7]+)");
    std::regex dec("(DEC r[0-7]+)");
    std::regex nnot("(NOT r[0-7]+)");
    std::regex load("(LOAD r[0-7], (#[0-9]+|[a-z]+|[0-9]+|r[0-7]))|(LOAD r[0-7],(#[0-9]+|[a-z]+|[0-9]+|r[0-7]))");
    std::regex store("(STORE r[0-7], (#[0-9]+|[a-z]+|[0-9]+|r[0-7]))|(STORE r[0-7],(#[0-9]+|[a-z]+|[0-9]+|r[0-7]))");

    std::map<std::string, std::regex> syntax{
            {"ADD",   add},
            {"AND",   sub},
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

//Checks whether if the current line has a loop variable
bool check_loop_variable(const std::string &loop) {
    std::regex syntax("([a-z]+):");
    if (std::regex_match(loop, syntax))
        return true;
    return false;
}

//Converts decimal numbers into its binary equivalent, returns 6 bit binary value if the decimal value is lower than 64
//Returns a 22 bit binary value otherwise
std::string binary_converter(int number) {
    bool flag = (number > 64);
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

//Checks whether if the variable names exists in the variable names hashmap which was processed during the .data part
bool check_variable_names(const std::map<std::string, std::string> &variable_names, const std::string &operand) {
    auto iterator = variable_names.find(operand);
    if (iterator != variable_names.end()) {
        return true;
    }
    return false;
}

//Checks whether if the current loop condition exists in the loop names hashmap
bool check_loop_names(const std::map<std::string, std::string> &loop_names, const std::string &operand) {
    auto iterator = loop_names.find(operand);
    if (iterator != loop_names.end())
        return true;
    return false;
}

int main(int argc, char *argv[]) {
    std::vector<std::string> instructions{};
    std::map<std::string, std::string> variable_names{};
    std::map<std::string, std::string> loop_names{};

    //Writing the .sal file
    std::string reader_file_name{};

    std::cout << "Please enter the name of the file." << std::endl;
    std::cin >> reader_file_name;

    std::ifstream reader{"../" + reader_file_name};

    //Making the .bin file
    std::ofstream writing{"../write.bin"};

    //Opening the files
    if (!reader) {
        std::cerr << "Cannot open file" << std::endl;
        return 1;
    }
    if (!writing) {
        std::cerr << "Cannot open the write file" << std::endl;
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

        if (input == ".data") {
            dataflag = true;
        }

        //Processes and assigns the variable name along with its binary address into the hashmap.
        if (dataflag && mainStartdata) {
            if (input != ".code") {
                std::string filtered_read = trim(read);
                std::cout << filtered_read << std::endl;
                variable_names.insert(std::make_pair(filtered_read, binary_converter(variable_value)));
                variable_value++;
            }
        }

        //Once the code reaces this block, it means that all the variables have been processed.
        if (!start) {
            if (input == ".code") {
                start = true;
                dataflag = false;
            }
        }

        //Part that process the .code part i.e. the actual code
        if (start && mainStart) {

            //Filter the string by removing the leading/trailing spaces via trim method
            std::string filtered_read = trim(read);
            std::string filtered_comments{};
            int comment_position = filtered_read.find("//");
            if (comment_position != std::string::npos) {
                filtered_comments = filtered_read.substr(0, comment_position);
                filtered_read = trim(filtered_comments);
            }

            //Count the number of words in the string
            int count = 1;
            for (int i = 0; filtered_read[i] != '\0'; i++) {
                if (filtered_read[i] == ' ')
                    count++;
            }

            //Check whether if the syntax matches for the operation i.e. OPCODE
            bool syntax_check = false;
            std::string operation = filtered_read.substr(0, filtered_read.find(' '));
            if (check_syntax(operation, filtered_read)) {
                //std::cout<<operation<<std::endl;
                syntax_check = true;
            }

            //Whenever it encounters a loop variable, the line number gets recorded and stored in the hashmap.
            if (check_loop_variable(filtered_read)) {
                int word_len = filtered_read.size();
                std::string loop_var = filtered_read.substr(0, word_len - 1);
                loop_names.insert(std::make_pair(loop_var, binary_converter(line_counter)));
                //std::cout << "This is where the loop is" << std::endl;
            }

            std::string final_instruction{};
            //Put the stuff from the current string line into a vector of strings
            if (syntax_check) {
                instructions.push_back(filtered_read);
                //std::cout<<filtered_read<<std::endl;
            }

            //Putting the stuff from the current string into the vector
            std::stringstream ss(filtered_read);
            std::istream_iterator<std::string> begin(ss);
            std::istream_iterator<std::string> end;

            std::vector<std::string> instruction(begin, end);
            std::copy(instruction.begin(), instruction.end(), std::ostream_iterator<std::string>(std::cout, " "));
            std::cout << std::endl;


            for (auto i{0}; instruction.size() > i; i++, line_counter++) {
                if (instruction.size() == 3) {
                    if (i == 0) {
                        final_instruction += return_opcode_value(instruction.at(i)) + ":";
                    } else if (i == 1) {
                        std::string reg = instruction[1];
                        std::string reg_value = reg.substr(0, 2);
                        std::string reg_instruction = return_reg_value(reg_value);

                        final_instruction += reg_instruction + ":";

                    } else if (i == 2) {
                        //get the register value after the r
                        std::string operand = instruction[2];
                        int end = operand.size();

                        //Check for #value
                        if (operand.at(0) == '#') {
                            std::string chopped = operand.substr(1, end - 1);
                            int decimal_value = std::stoi(chopped);
                            std::string binary_value = binary_converter(decimal_value);
                            final_instruction += ((decimal_value > 64) ? "1:" : "0:");
                            final_instruction += "01:" + binary_value;
                        } else if (check_variable_names(variable_names, operand)) {
                            auto iterator = variable_names.find(instruction.at(2));
                            std::string binary_value = iterator->second;
                            final_instruction += "0:10:" + binary_value;
                        } else if (operand.at(0) == 'r') {
                            std::string chopped = operand.substr(1, 2);
                            int decimal_value = std::stoi(chopped);
                            final_instruction += "0:00:" + binary_converter(decimal_value);
                        } else if (check_loop_names(loop_names, operand)) {
                            final_instruction += "0:10:";
                            auto iterator = loop_names.find(instruction.at(2));
                            if (iterator != loop_names.end()) {
                                std::string binary_value = iterator->second;
                                final_instruction += binary_value;
                            }
                        } else {
                            int decimal_value = std::stoi(operand);
                            final_instruction += "0:10:" + binary_converter(decimal_value);
                        }
                    }

                } else if (instruction.size() == 2) {
                    std::string jmp = "JMP";
                    if (operation == jmp) {
                        if (i == 0) {
                            final_instruction += return_opcode_value(instruction.at(i)) + ":";
                        } else {
                            std::string operand = instruction[1];
                            final_instruction += "000:0:10:";
                            int end = operand.size();
                            if (operand.at(0) == '#') {
                                std::string chopped = operand.substr(1, end - 1);
                                int decimal_value = std::stoi(chopped);
                                std::string binary_value = binary_converter(decimal_value);
                                final_instruction += binary_value;
                            } else {
                                auto iterator = loop_names.find(instruction.at(1));
                                if (iterator != loop_names.end()) {
                                    std::string binary_value = iterator->second;
                                    final_instruction += binary_value;
                                }
                            }
                        }

                    } else {
                        if (instruction.at(1).find(',') != std::string::npos) {
                            int comma_position = instruction.at(1).find(',');
                            std::string register_value = instruction.at(1).substr(0, comma_position);
                            //std::cout<<register_value<<std::endl;
                            std::cout << "This 2 word sentence is in the wrong syntax" << std::endl;
                        } else {
                            if (i == 0) {
                                final_instruction += return_opcode_value(instruction.at(i)) + ":";
                                //std::cout<<return_opcode_value(instruction.at(i))<<std::endl;
                            } else if (i == 1) {
                                std::string reg = instruction[1];
                                std::string reg_value = reg.substr(0, 2);
                                std::string reg_instruction = return_reg_value(reg_value);

                                final_instruction += reg_instruction + ":0:11:000000";

                            }
                        }
                    }
                }
            }


            std::cout << final_instruction << std::endl;

            line_counter++;
        }

        if (start) {
            mainStart = true;
        }
        if (dataflag) {
            mainStartdata = true;
        }
    }

    return 0;
}