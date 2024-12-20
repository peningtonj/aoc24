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

struct coord {
    int x, y;

    void print() {
        std::cout << x << ", " << y;
    }

    bool operator==(const coord& other) const {
        return x == other.x && y == other.y;
    }

    bool operator<(const coord& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }
};

struct robot {
    coord location;
    coord velocity;
};

template <typename T>
bool contains(const std::vector<T>& elements, const T& target) {
    return std::find(elements.begin(), elements.end(), target) != elements.end();
}

class RobotMap {
    public:
        std::vector<robot> robots;
        std::vector<coord> locations;
        int width, height;

    RobotMap(std::string input_file) {
        std::ifstream MyReadFile(input_file);
        std::string line;
        std::regex pattern(R"(p=([0-9]*),([0-9]*) v=(-?[0-9]*),(-?[0-9]*))");

        while (std::getline(MyReadFile, line)) {


            // Variable to hold the match result
            std::smatch match;

            // Apply the regex pattern to the input string
            if (std::regex_match(line, match, pattern)) {
                // Extract the captured groups
                coord p = coord{std::stoi(match[1].str()), std::stoi(match[2].str())};
                coord v = coord{std::stoi(match[3].str()), std::stoi(match[4].str())};
                robots.push_back(robot {p, v});
            }
        }

    width = 101;
    height = 103;

    }

    std::vector<robot> move_robots(int steps) {
        std::vector<robot> new_positions;
        std::vector<coord> new_locations;
        for (auto r : robots ) {
            // r.location.print(); std::cout << " - "; r.velocity.print(); std::cout << "\n";
            r.location.x += steps * r.velocity.x;
            r.location.y += steps * r.velocity.y;

            r.location.x %= width;
            r.location.y %= height;

            r.location.x = r.location.x < 0 ? width + r.location.x: r.location.x;
            r.location.y = r.location.y < 0 ? height + r.location.y: r.location.y;

            new_positions.push_back(r);
            new_locations.push_back(r.location);
        }

        locations = new_locations;
        return new_positions;
    }

    bool check_for_tree() {
        for (int x = 0; x < height; x++ ) {
            auto filtered_view = robots | std::views::filter([x](robot r) {
                return r.location.x == x; 
            });

            std::vector<robot> filteredVec(filtered_view.begin(), filtered_view.end());
            std::ranges::sort(filteredVec, [](robot a, robot b) {
                return a.location < b.location;
            });
            int progress = 0;
            for (int i = 1; i < filteredVec.size(); i++) {
                progress = filteredVec[i].location.y == (filteredVec[i - 1].location.y + 1) ? progress + 1 : 0;
                if (progress > 10) {
                    return true;
                }
            }

        }

        return false;
    }

    long long look_for_tree() {
        long long result = 0;
        while (!check_for_tree()) {
            robots = move_robots(1);
            result += 1;
        }

        print_map();
        return result;
    }

    void print_map() {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                std::string c = contains(locations, coord {x, y}) ? "*" : ".";
                std::cout << c;
            }
            std::cout << "\n";
        }
    }
};


int part_one(RobotMap rmap) {
    int result = 1;
    // rmap.move_robots(1);
    // rmap.move_robots(2);
    // rmap.move_robots(3);
    // rmap.move_robots(4);
    // rmap.move_robots(5);
    std::map<std::pair<int, int>, int> quadrants = {
        {std:: pair{-1, 1} , 0},
        {std:: pair{-1, -1} , 0},
        {std:: pair{1, -1} , 0},
        {std:: pair{1, 1} , 0}
    };


    for (auto r : rmap.move_robots(100)) {
            // r.location.print(); std::cout << "\n"; 

        if (!((((rmap.width - 1) / 2) == r.location.x) or (((rmap.height - 1) / 2) == r.location.y))) {
            int xsector = (r.location.x - (rmap.width / 2)) > 0 ? 1 : -1;
            int ysector = (r.location.y - (rmap.height / 2)) > 0 ? 1 : -1;
            quadrants[{xsector, ysector}] += 1;
        }
    }
    for (const auto& [q, count] : quadrants) {
        std::cout << count << "\n";
        result *= count;
    }
    
    return result;
}

long long part_two(RobotMap rmap) {
    return rmap.look_for_tree();
}

int main() {

    RobotMap rmap = RobotMap("input.txt");

    std::cout << "\nPart One:\n";
    std::cout << part_one(rmap);

    std::cout << "\nPart Two:\n";
    std::cout << part_two(rmap);

    return 0;
}
