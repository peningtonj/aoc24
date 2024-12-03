#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <regex>

std::string parse_input(std::string input_file) {
    std::ifstream MyReadFile(input_file);

    std::string memory((std::istreambuf_iterator<char>(MyReadFile)),
                             std::istreambuf_iterator<char>());

    // Close the file
    MyReadFile.close();
    return memory;
}


int part_one(std::string memory) {

    int result = 0;
    // Regular expression to match "mul(number1,number2)" and capture the numbers
    std::regex pattern(R"(mul\((\d+),(\d+)\))");

    // Create a regex iterator
    std::sregex_iterator matches_begin(memory.begin(), memory.end(), pattern);
    std::sregex_iterator matches_end;

    // Iterate through all matches
    for (std::sregex_iterator it = matches_begin; it != matches_end; ++it) {
        std::smatch match = *it; // Get the current match
        result += stoi(match.str(1)) * stoi(match.str(2));
    }

    return result;

}

int part_two(std::string memory) {

    int result = 0;

    // Get all sections of the memory that are between a do() and don't(). 
    // Also get from the start to the first do() and the last do() to the end.
    std::regex meta_pattern(R"((^|do\(\))([\s\S]*?)(don't\(\)|$))");

    // Create a regex iterator
    std::sregex_iterator matches_begin(memory.begin(), memory.end(), meta_pattern);
    std::sregex_iterator matches_end;

    // Iterate through all matches
    for (std::sregex_iterator it = matches_begin; it != matches_end; ++it) {
        std::smatch match = *it; // Get the current match
        result += part_one(match.str(0));
    }

    return result;

}

int main() {

    std::string memory = parse_input("input.txt");

    std::cout << "Part One:\n";
    std::cout << part_one(memory);
	
    std::cout << "\nPart Two:\n";
    std::cout << part_two(memory);

    return 0;

}
