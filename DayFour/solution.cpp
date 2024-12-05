#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <map>
#include <ranges>
#include <functional> // for std::bind

std::string old_code = "XMAS";
std::string real_code = "MAS";

struct coord_letter {
    int x;
    int y;
    char c;

    bool operator==(const coord_letter& other) const {
        return x == other.x && y == other.y && c == other.c;
    }

};

void parse_input(std::vector<std::string>& wordSearch, std::string input_file) {
    std::string myText;
    std::ifstream MyReadFile(input_file);

    std::string line;
    // read each pair of numbers into the two vectors
    while (std::getline(MyReadFile, line)) {
        wordSearch.push_back(line);
    }


    // Close the file
    MyReadFile.close();

}

std::vector<coord_letter> find_letter(std::vector<coord_letter> search_letters, char letter) {
    std::vector<coord_letter> matches;


    size_t index = 0; // Manual index tracking
    for (coord_letter ch : search_letters) {
        if (ch.c == letter) {
            matches.push_back(coord_letter{ch});
        }
        ++index;
    }

    return matches;
}


std::vector<std::vector<coord_letter> > spiral_out(std::vector<std::string>& wordSearch, coord_letter start, std::vector<int> directions = {-1, 0 ,1}, std::string code = old_code, int length = 4) {
    int width = wordSearch.at(0).size();
    int height = wordSearch.size();

    auto in_width = [width](int i) { return ((i >= 0) and (i < width)); };
    auto in_height = [height](int i) { return ((i >= 0) and (i < height)); };

    std::vector<std::vector<coord_letter>> results;

    for (int x_dir : directions) {
        for (int y_dir : directions) {
            std::vector<coord_letter> str;
            for (int i = 0; i < length; i++ ) {
                int x = start.x + (i * x_dir);
                int y = start.y + (i * y_dir);
                if (in_width(x) and in_height(y)) {
                    str.push_back({x, y, wordSearch.at(y).at(x)});
                }
            }
            results.push_back(str);
        }
    }

    return results;

}

int part_one(std::vector<std::string>& wordSearch) {
    
    std::vector<std::vector<coord_letter> > to_search;

    int result = 0;
    for (int y = 0; y < wordSearch.size(); y++) {
        std::vector<coord_letter> line_to_search;
        for (int x = 0; x < wordSearch.at(y).size(); x++) {
            line_to_search.push_back(coord_letter {x, y, wordSearch.at(y).at(x)});
        }

        to_search.push_back(line_to_search);
    }

    auto coord_letters_match  = [](std::vector<coord_letter> letters) {
        std::string str = "";
        for (auto x: letters) {
            str += x.c;
        }
        return (str == old_code);
    };

    for (auto line : to_search) {
        std::vector<coord_letter> matches = find_letter(line, 'X');
        for (auto match: matches) {
            std::vector<std::vector<coord_letter> > spirals = spiral_out(wordSearch, match);
            auto results_view = spirals | std::views::filter(coord_letters_match);
            std::vector<std::vector<coord_letter> > results;
            std::ranges::copy(results_view, std::back_inserter(results));

            result += results.size();
        }
    }
    return result;

}

bool contains(const std::vector<coord_letter>& coords, const coord_letter& target) {
    return std::find(coords.begin(), coords.end(), target) != coords.end();
}


void print_letter_with_coord(coord_letter x) {
    std::cout << x.x << ", " << x.y << ", " << x.c << "\n";
}

void print_str_from_coord_letters(std::vector<coord_letter> s) {
    for (auto x: s) {
        std::cout << x.c;
    }
    std::cout << "\n";
}

int part_two(std::vector<std::string>& wordSearch) {
    std::vector<std::vector<coord_letter> > to_search;

    int result = 0;

    std::vector<coord_letter> potential_crosses; 

    for (int y = 0; y < wordSearch.size(); y++) {
        std::vector<coord_letter> line_to_search;
        for (int x = 0; x < wordSearch.at(y).size(); x++) {
            line_to_search.push_back(coord_letter {x, y, wordSearch.at(y).at(x)});
        }

        to_search.push_back(line_to_search);
    }

    auto coord_letters_match  = [](std::vector<coord_letter> letters) {
        std::string str = "";
        for (auto x: letters) {
            str += x.c;
        }
        return (str == real_code);
    };

    for (auto line : to_search) {
        std::vector<coord_letter> matches = find_letter(line, 'M');
        for (auto match: matches) {
            std::vector<std::vector<coord_letter> > spirals = spiral_out(wordSearch, match, {-1, 1}, real_code, 3);
            auto results_view = spirals | std::views::filter(coord_letters_match);
            for (auto r : results_view) {
                potential_crosses.push_back(r.at(1));
            }
        }
    }

    std::vector<coord_letter> seen_crosses; 

    for (auto x: potential_crosses) {
        // std::cout << x.x << ", " << x.y << ", " << x.c << "\n";
        if (contains(seen_crosses, x)) {
            result += 1;
        }
        seen_crosses.push_back(x);
    }

    return result;
}

int main() {

    std::vector<std::string> wordSearch;
    parse_input(wordSearch, "input.txt");

    std::cout << "Part One:\n";
    std::cout << part_one(wordSearch);
	
    std::cout << "\nPart Two:\n";
    std::cout << part_two(wordSearch);

    return 0;

}
