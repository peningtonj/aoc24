#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <map>
#include <ranges>
#include <sstream>
#include <algorithm>
#include <numeric>

class Equation {
    public:
        long long target;
        std::vector<long long> inputs;

        Equation(std::string line) {
            long long colon_pos = line.find(":");
            target = stoll(line.substr(0, colon_pos));

            std::string inputs_line = line.substr(colon_pos + 2, line.size());
            size_t pos = 0;

            while ((pos = inputs_line.find(" ")) != std::string::npos) {
                inputs.push_back(stoll(inputs_line.substr(0, pos)));
                inputs_line.erase(0, pos + 1);
            }
            inputs.push_back(stoll(inputs_line));
        }
};

long long add(long long a, long long b) { return a + b; }
long long multiply(long long a, long long b) { return a * b; }
long long concat(long long a, long long b) { return stoll(std::to_string(a) + std::to_string(b)); }

void parse_input(std::vector<Equation>& equations, std::string input_file) {
    std::ifstream MyReadFile(input_file);
    std::string line;
    while (std::getline(MyReadFile, line)) {
        equations.push_back(Equation(line));
    }
}


void generate_combinations(const std::vector<long long(*)(long long, long long)>& options, long long length, 
                           std::vector<long long(*)(long long, long long)>& current, 
                           std::vector<std::vector<long long(*)(long long, long long)>>& result) {
    if (current.size() == length) {
        result.push_back(current); // Add the combination to the result
        return;
    }

    for (const auto& option : options) {
        current.push_back(option);       // Choose an option
        generate_combinations(options, length, current, result); // Recurse
        current.pop_back();              // Backtrack
    }
}



long long solve(std::vector<Equation> equations, std::vector<long long(*)(long long, long long)> operations) {
    long long result = 0; 
    
    for (auto equation : equations) {
        // std::cout << "Solving equation with target: " << equation.target << "\n and inputs: ";
        // for (long long i: equation.inputs) {
        //     std::cout << i << ", ";
        // }

        std::vector<std::vector<long long(*)(long long, long long)>> combinations;
        std::vector<long long(*)(long long, long long)> current;

        generate_combinations(operations, equation.inputs.size() - 1, current, combinations);
        for (auto comb: combinations) {
            long long running_total = comb.at(0)(equation.inputs.at(0), equation.inputs.at(1));
            for (long long i = 1; i < comb.size(); i++) {
                running_total = comb.at(i)(running_total, equation.inputs.at(i + 1));
            }
            if (running_total == equation.target) {
                result += equation.target;
                break;
            }
        }
    }

    return result;
}

long long part_one(std::vector<Equation> equations) {
    std::vector<long long(*)(long long, long long)> operations = {add, multiply};
    return solve(equations, operations);

}
long long part_two(std::vector<Equation> equations) {
    std::vector<long long(*)(long long, long long)> operations = {add, multiply, concat};
    return solve(equations, operations);
}

int main() {

    std::vector<Equation> equations;
    parse_input(equations, "input.txt");

    std::cout << "Part One:\n";
    std::cout << part_one(equations);
	
    std::cout << "\nPart Two:\n";
    std::cout << part_two(equations);

    return 0;

}
