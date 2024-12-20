#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <map>
#include <ranges>
#include <functional> // for std::bind
#include <set>
#include <regex>

const int8_t A = 0;
const int8_t B = 1;
const int8_t C = 2;

class CPU {
        std::vector<int8_t> instructions;
        std::vector<int8_t>::iterator instruction_head;

        void adv(int operand) {
            long long combo = combo_operand(operand);
            registers.at(A) = registers.at(A) / std::pow(2, combo); 
        }

        void bxl(int operand) {
            registers.at(B) = (registers.at(B) ^ operand) % 8;
        }

        void bst(int operand) {
            long long combo = combo_operand(operand);
            registers.at(B) = combo % 8;
        }

        void jnz(int operand) {
            if (registers.at(A)) instruction_head = instructions.begin() + operand;
        }

        void bxc([[maybe_unused]] int operand) {
            registers.at(B) = registers.at(B) ^ registers.at(C);
        }

        void out(int operand) {
            long long combo = combo_operand(operand);
            output.push_back(combo % 8);
        }

        void bdv(int operand) {
            long long combo = combo_operand(operand);
            registers.at(B) = registers.at(A) / std::pow(2, combo); 
        }

        void cdv(int operand) {
            long long combo = combo_operand(operand);
            registers.at(C) = registers.at(A) / std::pow(2, combo); 
        }

    public:
        std::vector<int> output;
        std::array<long long, 3> registers = {0};

        CPU(std::string input_file) {
            std::ifstream MyReadFile(input_file);
            std::string line;

            std::regex pattern(R"([0-9]+)");

            for (int i = 0; i < 3; i++) {
                std::getline(MyReadFile, line);
                std::smatch match;
                std::regex_search(line, match, pattern);
                registers.at(i) = stoi(match.str(0));
            }

            std::getline(MyReadFile, line);
            std::getline(MyReadFile, line);
            size_t pos = 0;

            line.erase(0, std::string{"Program: "}.size());
            while ((pos = line.find(",")) != std::string::npos) {
                instructions.push_back(stoi(line.substr(0, pos)));
                line.erase(0, pos + 1);
            }
            instructions.push_back(stoi(line));
            instruction_head = instructions.begin();
        }

            long long combo_operand(int8_t operand_inst) {
                    switch (operand_inst) {
                        case 4:
                            return registers.at(A);
                            break;
                        case 5:
                            return registers.at(B);
                            break;
                        case 6:
                            return registers.at(C);
                            break;
                        default:
                            return operand_inst;
                    }
            }

            void execute() {
                while (instruction_head != instructions.end()) {
                    // std::cout << "Processing instruction at " << std::distance(instructions.begin(), instruction_head) << "\n";


                    auto current_instruction = *instruction_head;
                    instruction_head++;

                    auto operand = *instruction_head;
                    instruction_head++;

                    // std::cout << static_cast<int>(current_instruction) << ", " << static_cast<int>(operand_instr) << "\n";


                    switch (current_instruction) {
                        case 0:
                            adv(operand);
                            break;
                        case 1:
                            bxl(operand);
                            break;
                        case 2:
                            bst(operand);
                            break;
                        case 3:
                            jnz(operand);
                            break;
                        case 4:
                            bxc(operand);
                            break;
                        case 5:
                            out(operand);
                            break;
                        case 6:
                            bdv(operand);
                            break;
                        case 7:
                            cdv(operand);
                            break;
                        default:
                            std::cout << "Shouldn't be here";
                            break;
                    }
                }
            }

            void print(bool just_registers = false) {
                std::cout << "Register A: " << registers.at(A) << "\n";
                std::cout << "Register B: " << registers.at(B) << "\n";
                std::cout << "Register C: " << registers.at(C) << "\n";

                if (just_registers) return;

                for (auto inst : instructions) {
                    std::cout << static_cast<int>(inst) << ", ";
                }
                std::cout << "\n";
                std::cout << "Head at " << std::distance(instructions.begin(), instruction_head) << std::endl;
            }

            void print_output() {
                for (int i : output) {
                    std::cout << i << ", ";
                }
                std::cout << "\n";
            }
};

std::vector<int> getLastElements(const std::vector<int>& vec, size_t from) {
    // Ensure we don't try to access out of range
    if (from > vec.size()) {
        from = vec.size();
    }

    return std::vector<int>(vec.begin() + from, vec.end());
}


long long part_two() {
    std::vector<int> expected = {2,4,1,1,7,5,1,5,4,3,0,3,5,5,3,0};
    std::vector<long long> a_options = {0};

    for (int i = expected.size() - 1; i >=0; i--) {
        std::vector<long long> new_a_options;
        for (long long a : a_options) {
            a = a << 3;
            for (int x = 0; x < 8; x++) {
                CPU cpu = CPU("input.txt");
                cpu.registers.at(A) = a + x;
                cpu.execute();
                if (cpu.output == getLastElements(expected, i)) {
                    new_a_options.push_back(a + x);
                }
            }
        }
        a_options = new_a_options;
        }

        // for (long long a: a_options) {
        //         CPU cpu = CPU("example.txt");
        //         cpu.registers.at(A) = a;
        //         cpu.execute();
        //         std::cout << "A: " << a << "\n";
        //         cpu.print_output();
        //         std::cout << "\n";
        // }
    
    return *std::min_element(a_options.begin(), a_options.end());

    }

int main() {
    std::cout << "Part 1: ";
    CPU cpu = CPU("input.txt");
    // cpu.print();
    cpu.execute();
    cpu.print_output();
    // cpu.print();

    long long result = part_two();
    std::cout << "Part 2: " << result;

    return 0;
}