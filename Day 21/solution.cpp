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
#include <numeric> // For std::accumulate
#include <algorithm> // For std::min

struct coord {
    int x, y;

    void print() {
        std::cout << x << ", " << y << "\n";
    }

    bool operator==(const coord& other) const {
        return x == other.x && y == other.y;
    }

    coord operator+(const coord& other) const {
        return coord{x + other.x, y + other.y};
    }

    bool operator<(const coord& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }
};

namespace std {
    template <>
    struct hash<coord> {
        size_t operator()(const coord& c) const {
            size_t h1 = std::hash<int>{}(c.x);
            size_t h2 = std::hash<int>{}(c.y);
            return h1 ^ (h2 * 31); // Multiply by a prime number to reduce collisions
        }
    };
}


const std::unordered_map<char, coord> MAIN_BUTTONS = {
    {'X', {0, 0}},
    {'0', {1, 0}},
    {'1', {0, 1}},
    {'2', {1, 1}},
    {'3', {2, 1}},
    {'4', {0, 2}},
    {'5', {1, 2}},
    {'6', {2, 2}},
    {'7', {0, 3}},
    {'8', {1, 3}},
    {'9', {2, 3}},
    {'A', {2, 0}},
};

coord PRESS = coord{-1, -1};
std::unordered_map<char, coord> DIRECTION_PAD = {
    {'X', {0, 1}},
    {'<', {0, 0}},
    {'v', {1, 0}},
    {'>', {2, 0}},
    {'^', {1, 1}},
    {'A', {2, 1}},
};

std::unordered_map<coord, char> DIRECTIONS = {
    { coord {0, 1}, '^'},
    { coord {0, -1}, 'v'},
    { coord {1, 0}, '>'},
    { coord {-1, 0}, '<'},
    { PRESS, 'A'},
};

std::unordered_map<char, coord> REVERSE_DIRECTIONS = {
    {'^', coord {0, 1}},
    {'v', coord {0, -1}},
    {'>', coord {1, 0}},
    {'<', coord {-1, 0}},
    {'A', coord {0, 0}},
};


struct button {
    char value;
    coord location;

    bool operator<(const button& other) const {
        return (location < other.location);
    }

    bool operator==(const button& other) const {
        return location == other.location && value == other.value;
    }

};

namespace std {
    template <>
    struct hash<button> {
        size_t operator()(const button& b) const {
            size_t h1 = std::hash<coord>{}(b.location);
            return h1;
        }
    };
}

void parse_input(std::string input_file, std::vector<std::vector<button> >& codes) {
    std::ifstream MyReadFile(input_file);
    std::string line;

    while (std::getline(MyReadFile, line)) {
        std::vector<button> code;
        for (char c : line) {
            code.push_back({c, MAIN_BUTTONS.at(c)});
        }
        codes.push_back(code);
    }
}

void print_buttons(std::vector<button> buttons) {
    for (auto b : buttons) {
        std::cout << b.value;
    }
    std::cout << "\n";
}

void print_chunked(std::vector<std::vector<button>> buttons) {
    for (auto chunk : buttons) {
        for ( auto b : chunk ) {
            std::cout << b.value;
        }
    }
    std::cout << "\n";
}

std::vector<coord> required_path_from_chunk(const std::vector<button>& chunk, bool main_pad) {
    std::vector<coord> result;
    // Every pad starts at A
    if (main_pad) result.push_back(MAIN_BUTTONS.at('A'));
    else result.push_back(DIRECTION_PAD.at('A'));
    // Add the rest of the path
    for (auto x : chunk) {
        result.push_back(x.location);
    }
    return result;
}

bool check_for_hole(coord c, bool main_pad) {
    if (main_pad) return c == MAIN_BUTTONS.at('X');
    else return c == DIRECTION_PAD.at('X');
}

void add_x_moves(coord start, coord end, std::vector<button>& path) {
    int step = (end.x > start.x) ? 1 : -1;
    auto direction = DIRECTIONS.at({step, 0});
    auto direction_location = DIRECTION_PAD.at(direction);
    for (int i = start.x + step; i != end.x + step; i += step) {
        path.push_back(button{direction, direction_location});
    }
}

void add_y_moves(coord start, coord end, std::vector<button>& path) {
    int step = (end.y > start.y) ? 1 : -1;
    auto direction = DIRECTIONS.at({0, step});
    auto direction_location = DIRECTION_PAD.at(direction);
    for (int i = start.y + step; i != end.y + step; i += step) {
        path.push_back(button{direction, direction_location});
    }
}

void get_movements(coord current, coord next, std::vector<button>& path, bool y_first) {
    if (y_first) {
        add_y_moves(current, next, path);
        add_x_moves(current, next, path);
    } else {
        add_x_moves(current, next, path);
        add_y_moves(current, next, path);
    }
    path.push_back({DIRECTIONS.at(PRESS), DIRECTION_PAD.at('A')});
}


#include <unordered_map>

namespace std {
    template <>
    struct hash<std::vector<button>> {
        size_t operator()(const std::vector<button>& vec) const {
            size_t seed = vec.size();
            for (const auto& btn : vec) {
                seed ^= std::hash<button>{}(btn) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    template <>
    struct hash<std::pair<std::vector<button>, int>> {
        size_t operator()(const std::pair<std::vector<button>, int>& p) const {
            size_t hash1 = std::hash<std::vector<button>>{}(p.first);
            size_t hash2 = std::hash<int>{}(p.second);
            return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
        }
    };
}

button A_BUTTON = button{'A', DIRECTION_PAD['A']};


std::vector<std::vector<button>> splitVector(const std::vector<button>& input, button delimiter) {
    std::vector<std::vector<button>> result;
    std::vector<button> current;

    for (button value : input) {
        current.push_back(value);
        if (value == delimiter) {
            result.push_back(current);
            current.clear();
        }
    }

    return result;
}

int TOP_LAYER = 25;

size_t best_directions_for_chunk(const std::vector<button> bs, int layer) {
    // std::cout << "I'm trying to work out the best way to input ";
    // print_buttons(bs);
    // std::cout << " from " << layer << " layers away\n";

    // Check if it is in the cache
    std::pair<std::vector<button>, int> parameters = {bs, layer};
    static std::unordered_map<std::pair<std::vector<button>, int>, size_t> cache;

    if (cache.contains(parameters)) {
        return cache[parameters];
    } 

    size_t output = 0;

    auto to_do = required_path_from_chunk(bs, layer==TOP_LAYER);
    for (int i = 0; i < to_do.size() - 1; i++) {
        size_t shortest_inputs = SIZE_T_MAX;

        for (bool y_first : {true, false}) {
            if (layer == TOP_LAYER) {
                if (y_first && to_do.at(i).x == 0 && to_do.at(i + 1).y == 0 ) continue; // Goes through bad region
                if (!y_first && to_do.at(i).y == 0 && to_do.at(i + 1).x == 0 ) continue; // Goes through bad region
                if (y_first && to_do.at(i).y == to_do.at(i + 1).y ) continue; // Duplicates x first because no y movement
                if (y_first && to_do.at(i).x == to_do.at(i + 1).x ) continue; // Duplicates x first because no x movement
                if (!y_first // x first
                    && (to_do.at(i + 1).x > to_do.at(i).x) // going left
                    && !(to_do.at(i).x == 0 && to_do.at(i + 1).y == 0) // Not inaccessible
                    && !(to_do.at(i).y == to_do.at(i + 1).y) // not skipping the y first
                    ) continue;

            } else {
                if (y_first && to_do.at(i).x == 0) continue; // Goes through bad region
                if (!y_first && to_do.at(i).y == 1 && to_do.at(i + 1).x == 0 ) continue; // Goes through bad region
                if (y_first && to_do.at(i).y == to_do.at(i + 1).y ) continue; // Duplicates x first because no y movement
                if (y_first && to_do.at(i).x == to_do.at(i + 1).x ) continue; // Duplicates x first because no x movement

                if (!y_first // x first
                    && (to_do.at(i + 1).x > to_do.at(i).x) // going left
                    && !(to_do.at(i).x == 0) // Not inaccessible
                    && !(to_do.at(i).y == to_do.at(i + 1).y) // not skipping the y first
                    ) continue;

                if (y_first // y first
                    && (to_do.at(i + 1).x < to_do.at(i).x) // going down
                    && !(to_do.at(i).y == 1 && to_do.at(i + 1).x == 0) // Not inaccessible
                    && !(to_do.at(i).y == to_do.at(i + 1).y) // not skipping the y first
                    ) continue;
            }

            std::vector<button> inputs;
            get_movements(to_do.at(i), to_do.at(i + 1), inputs, y_first);
                if (layer) {
                    size_t new_inputs = 0;
                    for (auto chunk : splitVector(inputs, A_BUTTON)) {
                        new_inputs += best_directions_for_chunk(chunk, layer - 1);
                    }
                    shortest_inputs = std::min(shortest_inputs, new_inputs);
                } else {
                    // print_buttons(inputs);
                    shortest_inputs = std::min(shortest_inputs, inputs.size());
                    // std::cout << shortest_inputs << "\n";
            }
        }
        output += shortest_inputs;
    }

    // Cache the result
    cache[parameters] = output;
    // std::cout << "And I think it's to do: ";
    // print_buttons(output);
    return output;
}

int code_value(std::vector<button> buttons) {
    std::string s;
    for (int i = 0; i < buttons.size() - 1; i++) {
        s += buttons.at(i).value;
    }
    return stoi(s);
}

int main() {
    std::vector<std::vector<button> > codes;
    parse_input("input.txt", codes);

    auto test_code = codes.at(0);
    // auto result = best_directions_for_chunk(test_code, TOP_LAYER);
    // std::cout << result << "\n";
    long long complexity = 0;
    for (auto code : codes) {
        print_buttons(code);
        auto result = best_directions_for_chunk(code, TOP_LAYER);
        std::cout << code_value(code) << " * " << result << " \n";
        complexity += result * code_value(code);
        }
    std::cout << complexity;
    return 0;
}
