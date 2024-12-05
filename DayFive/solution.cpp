#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <map>
#include <ranges>
#include <functional> // for std::bind
#include <sstream>
#include <algorithm>

void parse_input(std::vector<std::pair<int, int> >& rules, std::vector<std::vector<int> >& updates, std::string input_file) {
    std::string myText;
    std::ifstream MyReadFile(input_file);

    std::string line;
    int a, b;
    char sep;
    // read each pair of numbers into the two vectors
    while (std::getline(MyReadFile, line)) {
        std::istringstream iss(line);
        if (iss >> a >> sep >> b) {
            rules.push_back({a, b});
        } else {
            break;
        }
    }

    size_t pos = 0;

    while (std::getline(MyReadFile, line)) {
        std::vector<int> update;
        while ((pos = line.find(",")) != std::string::npos) {
            update.push_back(stoi(line.substr(0, pos)));
            line.erase(0, pos + 1);
        }
        update.push_back(stoi(line));
        updates.push_back(update);

    }

    // Close the file
    MyReadFile.close();

}


int get_index(const std::vector<int>& vec, int value, int not_found = -1) {
    auto it = std::find(vec.begin(), vec.end(), value);
    if (it != vec.end()) {
        return std::distance(vec.begin(), it);
    }
    return not_found; 
}


bool check_update(std::vector<std::pair<int, int> > rules, std::vector<int> update) {

    for (auto rule: rules) {
        auto first_idx = get_index(update, rule.first, -1);
        auto second_idx = get_index(update, rule.second, update.size() + 1);


        if (first_idx > second_idx) {
            return false;
        }
    }
    return true;

}

int part_one(std::vector<std::pair<int, int> > rules, std::vector<std::vector<int> > updates) {
    int result = 0;
    for (auto update: updates) {
        // std::cout << update.at(update.size() / 2);

        if (check_update(rules, update)) {
            result += update.at(update.size() / 2);
        }
    }
    return result;
}

void move_element(std::vector<int>& vec, size_t from, size_t to) {
    if (from == to) return; // No need to move if indices are the same

    int temp = vec[from];
    vec.erase(vec.begin() + from);
    vec.insert(vec.begin() + to, temp);
}

std::vector<int> fix_update(std::vector<std::pair<int, int> > rules, std::vector<int> update) {

    for (int i = 0; i < rules.size(); i++) {
        auto rule = rules.at(i);
        auto first_idx = get_index(update, rule.first, -1);
        auto second_idx = get_index(update, rule.second, update.size() + 1);


        if (first_idx > second_idx) {
            move_element(update, first_idx, second_idx);
            i = 0;
        }
    }

    return update;
}

int part_two(std::vector<std::pair<int, int> > rules, std::vector<std::vector<int> > updates) {
    int result = 0;
    for (auto update: updates) {
        // std::cout << update.at(update.size() / 2);

        if (!check_update(rules, update)) {
            update = fix_update(rules, update);
            result += update.at(update.size() / 2);
        }
    }
    return result;
}

int main() {

    std::vector<std::pair<int, int> > rules;
    std::vector<std::vector<int> > updates;
    parse_input(rules, updates, "example.txt");

    std::cout << "Part One:\n";
    std::cout << part_one(rules, updates);
	
    std::cout << "\nPart Two:\n";
    std::cout << part_two(rules, updates);

    return 0;

}
