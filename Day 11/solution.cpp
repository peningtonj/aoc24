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


void parse_input(std::vector<long long>& stones, std::string input_file) {
    std::ifstream MyReadFile(input_file);

    size_t pos = 0;
    std::string line;
    std::getline(MyReadFile, line);
    while ((pos = line.find(" ")) != std::string::npos) {
        stones.push_back(stoll(line.substr(0, pos)));
        line.erase(0, pos + 1);
    }
    stones.push_back(stoll(line));


    // Close the file
    MyReadFile.close();
}

std::vector<long long> transform_stone(long long stone) {
    // std::cout << "^" << stone << "^";
    std::string stone_as_string = std::to_string(stone);
    if (stone == 0) {
        return std::vector<long long> {1};
    } else if (stone_as_string.size() % 2 == 0) {
        // std::cout << "#" <<  stone_as_string.substr(0, stone_as_string.size() / 2) << "#" << stone_as_string.substr((stone_as_string.size() / 2), stone_as_string.size());
        return std::vector<long long> {
            stoll(stone_as_string.substr(0, stone_as_string.size() / 2)),
            stoll(stone_as_string.substr((stone_as_string.size() / 2), stone_as_string.size()))
        };
    } else {
        return std::vector<long long> {stone * 2024};
    }
}

long long part_one(std::vector<long long> stones, int steps) {
    for (long long i = 0; i < steps; i ++) {
        std::vector<long long> new_stones;
        for (auto s : stones) {
            std::vector<long long> transformed_stones = transform_stone(s);
            for (auto ts : transformed_stones) {
                new_stones.push_back(ts);
            }
        }
        stones = new_stones;
    }
    return stones.size();
}


template <typename T>
bool contains(const std::vector<T>& elements, const T& target) {
    return std::find(elements.begin(), elements.end(), target) != elements.end();
}

std::map<long long, long long> process_stones(std::map<long long, long long> stones_map) {
    std::map<long long, long long> result;

    for (const auto& [stone, count] : stones_map) {
        std::string stone_as_string = std::to_string(stone);

        if (stone == 0) {
            result[1] = count;
        } else if (stone_as_string.size() % 2 == 0) {
            result[stoll(stone_as_string.substr(0, stone_as_string.size() / 2))] += count;
            result[stoll(stone_as_string.substr((stone_as_string.size() / 2), stone_as_string.size()))] += count;
        } else {
            result[stone * 2024] += count;
        }
    }
    
    return result;

}

long long part_two(std::vector<long long> stones, int steps) {
    std::map<long long, long long> stones_map;
    for (auto s: stones) {
        if(stones_map.contains(s)) {
            stones_map[s] += 1;
        } else {
            stones_map[s] = 1;
        }
    }

    while (steps) {
        stones_map = process_stones(stones_map);
        steps -= 1;
    }

    long long result = 0;
    for (const auto& [stone, count] : stones_map) {
        // std::cout << stone << ": " << count << "\n";
        result += count;
    }
    
    return result;
}

int main() {

    std::vector<long long> stones;
    parse_input(stones, "input.txt");
    for (auto s : stones) {
        std::cout << s << ", ";
    }
    std::cout << "\n";

    std::cout << "\nPart One:\n";
    std::cout << part_two(stones, 25);;

    std::cout << "\nPart Two:\n";
    std::cout << part_two(stones, 75);;
    return 0;

}
