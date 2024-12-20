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
const char WALL = '#';
const char END = 'E';
const char START = 'S';
const char PATH = '.';

template <typename T>
bool contains(const std::vector<T>& elements, const T& target) {
    return std::find(elements.begin(), elements.end(), target) != elements.end();
}


const std::vector<coord> DIRECTIONS = {
    coord{1,0},
    coord{-1,0},
    coord{0,-1},
    coord{0,1},
};


class CPURace {
        int width, height;
        std::set<coord> path_coords;
        coord start_coord; 
        coord end_coord;
    public:
        std::vector<coord> path; 

        CPURace(std::string input_file) {
                std::cout << "Reading Maze";
                std::ifstream MyReadFile(input_file);
                std::string line;

                int y = 0;
                while (std::getline(MyReadFile, line)) {
                    if (!line.size()) {
                        break;
                    }
                    int x = 0;
                    for (char c : line) {
                        switch (c)
                        {
                            case START:
                                start_coord = coord{x, y};
                                path_coords.insert(coord{x,y});
                                break;
                            case END:
                                end_coord = coord{x, y};
                                path_coords.insert(coord{x,y});
                                break;
                            case PATH:
                                path_coords.insert(coord{x,y});
                            default:
                                break;
                        }

                        x += 1;
                    }
                    width = x;
                    y += 1;
                }
                height = y;
                create_path();
            }


    void create_path() {
        coord current = start_coord;
        path.push_back(start_coord);
        while (current != end_coord) {
            for (auto d : DIRECTIONS) {
                if (!contains(path, current + d) && path_coords.contains(current + d)) {
                    path.push_back(current + d);
                    current = current + d;
                    break;
                }
            }
        }
    }

    int race_distance(coord a, coord b) {
        return abs(a.x - b.x) + abs(a.y - b.y);
    }

    int skip_options(int required_improvement) {
        int good_skips = 0;
        int normal_time = path.size();
        for (int i = 0; i < path.size() - required_improvement; i++) {
            std::vector<coord> remaining_path(path.begin() + i + required_improvement, path.end());
            coord current = path.at(i);
            auto options = remaining_path | std::views::filter([this, current](coord c){
                return race_distance(current, c) < 3;
            });
            for (auto o : options) {
                auto skip_to = std::find(path.begin() + i, path.end(), o);
                int skipped = std::distance(path.begin() + i, skip_to);
                if ((skipped - race_distance(current, o)) >= required_improvement) good_skips++;
            }
        }

        return good_skips;
    }

    int new_rules(int required_improvement) {
        std::set<std::pair<coord, coord> > good_skips;

        int normal_time = path.size();
        for (int i = 0; i < path.size() - required_improvement; i++) {
            std::vector<coord> remaining_path(path.begin() + i + required_improvement, path.end());
            coord current = path.at(i);
            auto options = remaining_path | std::views::filter([this, current](coord c){
                return race_distance(current, c) < 21;
            });
            for (auto o : options) {
                auto skip_to = std::find(path.begin() + i, path.end(), o);
                int skipped = std::distance(path.begin() + i, skip_to);
                if ((skipped - race_distance(current, o)) >= required_improvement) {
                    good_skips.insert({current, o});
                }
            }
        }

        return good_skips.size();
    }

    void print_map() {
        std::cout << "\n";
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                std::string c = path_coords.contains(coord {x, y}) ? "." : "#";
                std::cout << c;
            }
            std::cout << "\n";
        }
    }
};

int part_one(CPURace race) {
    return race.skip_options(100);
}

int part_two(CPURace race) {
    return race.new_rules(100);
}

int main() {

    CPURace race = CPURace("input.txt");
    std::cout << "\nPart One:\n";
    std::cout << part_one(race);

    std::cout << "\nPart Two:\n";
    std::cout << part_two(race);

    return 0;
}
