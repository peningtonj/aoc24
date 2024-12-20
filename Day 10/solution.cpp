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

const char OBSTACLE = '#';
const char UP_GAURD = '^';
const char LEFT_GAURD = '<';
const char RIGHT_GAURD = '>';
const char DOWN_GAURD = 'v';


struct coord {
    int x, y;

    void print() {
        std::cout << "(" << x << ", " << y << ")";
    }

    bool operator==(const coord& other) const {
        return x == other.x && y == other.y;
    }

    bool operator<(const coord& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }
};

coord add_coord_as_vector(coord c, coord v) {
    return coord {c.x + v.x, c.y + v.y};
}

std::vector<coord> DIRECTIONS = {
    coord {0, 1},
    coord {0, -1},
    coord {1, 0},
    coord {-1, 0}
};

class Mountain {
        int height, width;
    public:
        std::vector<std::vector<int> > topographic_map;
        std::vector<std::vector<coord> > paths;

        Mountain(std::string input_file) {
            std::ifstream MyReadFile(input_file);
            std::string line;

            int y = 0;
            while (std::getline(MyReadFile, line)) {
                std::vector<int> heights;            
                int x = 0;
                for (char c : line) {
                    heights.push_back(std::stoi(std::string(1, c)));
                    if (c == '0') {
                        paths.push_back(std::vector<coord> {coord{x, y} }); 
                    }
                    x += 1;
                }
                y += 1;
                topographic_map.push_back(heights);
            }
            height = topographic_map.size();
            width = topographic_map.at(0).size();
        }

        int height_at_coord(coord p) {
            return topographic_map.at(p.y).at(p.x);
        }

        bool insideMap(coord p) {
            return (p.x >= 0) and (p.y >= 0) and (p.x < width) and (p.y < height);
        }

        std::vector<coord> search_around_coord(coord point) {
            std::vector<coord> results;
            int height = height_at_coord(point);
            for (auto dir: DIRECTIONS) {
                coord adj = add_coord_as_vector(point, dir);
                if (!insideMap(adj)) {
                    continue;
                }
                if (height_at_coord(adj) == height + 1) {
                    results.push_back(adj);
                }
            }
            return results;
        }

        void extend_paths(bool keep_distinct = false) {
            bool still_going = true;
            while(still_going) {
                still_going = false;
                std::vector<std::vector<coord> > new_paths;
                for (auto path : paths) {
                    auto last_coord = path.at(path.size() - 1);
                    auto next_steps = search_around_coord(last_coord);
                    if (next_steps.size()) {
                        still_going = true;
                        for (int i = 0; i < next_steps.size(); i++) {
                            std::vector<coord> new_path(path);
                            new_path.push_back(next_steps.at(i));
                            new_paths.push_back(new_path);
                        }
                    }
                }
            if (new_paths.size()){
                paths = new_paths;
            }

            if (!keep_distinct) {
                std::sort(paths.begin(), paths.end(), [](std::vector<coord> a, std::vector<coord> b) {
                    return (a.at(0) < b.at(0)) || ((a.at(0) == b.at(0)) && (a.at(a.size() - 1) < b.at(b.size() -1)));
                });

                auto last = std::unique(paths.begin(), paths.end(), [](std::vector<coord> a, std::vector<coord> b) {
                    return (a.at(0) == b.at(0)) && (a.at(a.size() - 1) == b.at(b.size() - 1));
                });

                paths.erase(last, paths.end());
            }
        }   
    }
};

int part_one(Mountain mnt) {
    std::cout << "Extended paths\n";
    mnt.extend_paths();
    return mnt.paths.size();   
}


int part_two(Mountain mnt) {
    std::cout << "Extended paths\n";
    mnt.extend_paths(true);
    return mnt.paths.size();   
}

int main() {

    Mountain mnt = Mountain("input.txt");
    std::cout << "Part One:\n";
    std::cout << part_one(mnt);
	
    std::cout << "\nPart Two:\n";
    std::cout << part_two(mnt);

    return 0;

}
