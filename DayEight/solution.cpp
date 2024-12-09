#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <map>
#include <set>
#include <ranges>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <functional> // for std::bind

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

coord get_direction(coord a, coord b) {
    return coord {a.x - b.x, a.y - b.y};
}

coord add_coord_as_vector(coord c, coord v) {
    return coord {c.x + v.x, c.y + v.y};
}


class AntennaMap {
        int width, height;
    public:
        std::map<char, std::vector<coord>> frequency_map;
        std::map<char, std::vector<coord>> antinodes;
        std::set<coord> unique_antinodes;

        AntennaMap(std::string input_file) {
            std::ifstream MyReadFile(input_file);
            std::string line;

            int y = 0;
            while (std::getline(MyReadFile, line)) {
                int x = 0;
                for (char c : line) {
                    if (c != '.') {
                        if (!frequency_map.contains(c)) {
                            frequency_map[c] = std::vector<coord> {};
                            antinodes[c] = std::vector<coord> {};
                        } 
                        frequency_map[c].push_back(coord {x ,y});
                    }
                    x += 1;
                }
                width = x;
                y += 1;
            }
            height = y;

        }

        void processAntennaPair(coord ant_one, coord ant_two, char freq) {
            coord v = get_direction(ant_one, ant_two);
            antinodes[freq].push_back(add_coord_as_vector(ant_one, v));
            v.x = -v.x;
            v.y = -v.y;
            antinodes[freq].push_back(add_coord_as_vector(ant_two, v));
        }

        bool insideMap(coord p) {
            return (p.x >= 0) and (p.y >= 0) and (p.x < width) and (p.y < height);
        }

        void processAntennaPairWithHarmonics(coord ant_one, coord ant_two, char freq) {
            coord v = get_direction(ant_one, ant_two);
            coord current = ant_one;
            while (insideMap(current)) {
                antinodes[freq].push_back(current);
                unique_antinodes.insert(current);
                current = add_coord_as_vector(current, v);

            }
            v.x = -v.x;
            v.y = -v.y;
            current = ant_two;
            while (insideMap(current)) {
                antinodes[freq].push_back(current);
                unique_antinodes.insert(current);
                current = add_coord_as_vector(current, v);
            }
        }

        void calculateAntinodes() {
            for (const auto& [freq, locations] : frequency_map) {
                for (int i = 0; i < locations.size(); i++ ) {
                    for (int j = i + 1; j < locations.size(); j++ ) {
                        processAntennaPair(locations.at(i), locations.at(j), freq);
                    }   
                }
            }

            for (const auto& [freq, a] : antinodes) {
                auto v = a | std::ranges::views::filter([this](const coord& p) { return this->insideMap(p); });
                std::vector<coord> results;
                std::ranges::copy(v, std::back_inserter(results));
                antinodes[freq] = results;
                unique_antinodes.insert(results.begin(), results.end());

            }
        }

        void calculateAntinodesWithHarmonics() {
            for (const auto& [freq, locations] : frequency_map) {
                for (int i = 0; i < locations.size(); i++ ) {
                    for (int j = i + 1; j < locations.size(); j++ ) {
                        processAntennaPairWithHarmonics(locations.at(i), locations.at(j), freq);
                    }   
                }
            }
        }
};

int part_one(AntennaMap a_map) {
    a_map.calculateAntinodes();
    return a_map.unique_antinodes.size();
}

int part_two(AntennaMap a_map) {
    a_map.calculateAntinodesWithHarmonics();
    return a_map.unique_antinodes.size();
}

int main() {

    AntennaMap antenna_map = AntennaMap("input.txt");

    std::cout << "Part One:\n";
    std::cout << part_one(antenna_map);
	
    std::cout << "\nPart Two:\n";
    std::cout << part_two(antenna_map);

    return 0;

}
