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
#include <unordered_set>

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

namespace std {
    template <>
    struct hash<std::pair<coord, coord>> {
        size_t operator()(const std::pair<coord, coord>& p) const {
            size_t h1 = std::hash<coord>{}(p.first);
            size_t h2 = std::hash<coord>{}(p.second);
            return h1 ^ (h2 * 31); // Again, combine using a prime multiplier
        }
    };
}


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


    // std::map<std::pair<coord, coord>, int> distance_cache;
    int race_distance(const coord& a, const coord& b) {
        // auto key = std::pair{a, b};
        // if (distance_cache.contains(key)) {
            // return distance_cache[key];
        // }
        int dist = abs(a.x - b.x) + abs(a.y - b.y);
        // distance_cache[key] = dist;
        return dist;
    }

    int skip_options(int required_improvement, int cheat_length) {
    std::unordered_set<std::pair<coord, coord>> good_skips;

    int normal_time = path.size();
    for (int i = 0; i < path.size() - required_improvement; i++) {
        coord current = path[i];
        for (int j = i + required_improvement; j < path.size(); j++) {
            coord c = path[j];
            int d = race_distance(current, c);

            if (d > cheat_length) continue;
            int skipped = j - i;
            if ((skipped - d) >= required_improvement) {
                good_skips.emplace(current, c);
            }
        }
    }

    return good_skips.size();
    }

};

int part_one(CPURace race) {
    return race.skip_options(100, 2);
}

int part_two(CPURace race) {
    return race.skip_options(100, 20);
}

int main() {

    CPURace race = CPURace("input.txt");
    std::cout << "\nPart One:\n";
    std::cout << part_one(race);

    std::cout << "\nPart Two:\n";
    std::cout << part_two(race);

    return 0;
}
