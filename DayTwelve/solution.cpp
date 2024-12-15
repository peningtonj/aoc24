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

    std::vector<coord> get_adj() {
        return std::vector {
            coord {x, y + 1},
            coord {x + 1, y},
            coord {x, y - 1},
            coord {x - 1, y },
        };
    }
};

struct fence {
    coord a, b;

    std::vector<fence> get_adj() {
        if (a.y == b.y) {
            return std::vector<fence> {
                fence {
                    coord{a.x , a.y + 1}, coord {b.x, b.y + 1},
                },
                fence {
                    coord{a.x , a.y - 1}, coord {b.x, b.y - 1}
                }
            };
        } else {
            return std::vector<fence> {
                fence {
                    coord{a.x + 1 , a.y}, coord {b.x + 1, b.y},
                },
                fence {
                    coord{a.x - 1, a.y}, coord {b.x - 1, b.y}
                }
            };
        }
    }

    bool operator==(const fence& other) const {
        return a == other.a && b == other.b;
    }
};

struct CoordHash {
    std::size_t operator()(const coord& c) const {
        std::size_t h1 = std::hash<int>{}(c.x);
        std::size_t h2 = std::hash<int>{}(c.y);
        return h1 ^ (h2 << 1); // Combine the two hashes
    }
};

struct FenceHash {
    std::size_t operator()(const fence& f) const {
        std::size_t h1 = CoordHash{}(f.a);
        std::size_t h2 = CoordHash{}(f.b);
        return h1 ^ (h2 << 1); // Combine the two hashes
    }
};

#include <unordered_set>

bool has_intersection(const std::vector<coord>& vec1, const std::vector<coord>& vec2) {
    std::unordered_set<coord, CoordHash> set1(vec1.begin(), vec1.end());

    for (coord val : vec2) {
        if (set1.find(val) != set1.end()) {
            return true;
        }
    }
    return false;
}

bool has_intersection(const std::vector<fence>& vec1, const std::vector<fence>& vec2) {
    std::unordered_set<fence, FenceHash> set1(vec1.begin(), vec1.end());

    for (fence val : vec2) {
        if (set1.find(val) != set1.end()) {
            return true;
        }
    }
    return false;
}

template <typename T>
bool contains(const std::vector<T>& elements, const T& target) {
    return std::find(elements.begin(), elements.end(), target) != elements.end();
}

class GardenSeg {
    public:
        char food;
        std::vector<coord> locations;
        std::vector<fence> fences;

    bool operator==(const GardenSeg& other) const {
        return (food == other.food) and (locations.at(0) == other.locations.at(0));
    }

    GardenSeg(char f, coord start) {
        food = f;
        locations.push_back(start);
    }

    int price() {
        int edges = 0;
        for (auto c : locations) {
            for (auto d : c.get_adj()) {
                if (!contains(locations, d)) {
                    edges += 1;
                }
            }
        }
    return edges * locations.size();
    }

    void fence_segment() {
        for (auto c : locations) {
            for (auto d : c.get_adj()) {
                if (!contains(locations, d)) {
                    fences.push_back(fence{c, d});
                }
            }
        }
    }

    int bulk_price() {
        std::vector<std::vector<fence> > long_fences;
        for (auto f : fences) {
            auto adj = f.get_adj();

            auto filtered_view = long_fences | std::views::filter([f, adj](std::vector<fence> x) {
                return has_intersection(adj, x); 
            });
            auto it = filtered_view.begin();
            auto end = filtered_view.end();
            if (it != end) {
                auto kept_segment = it;
                kept_segment->push_back(f);
                
                ++it;
                std::vector<std::vector<fence> > to_remove; // To store segments to remove
                
                for (; it != end; ++it) {
                    kept_segment->insert(kept_segment->end(), it->begin(), it->end());
                    to_remove.push_back(*it); // Add segment to removal list
                }

                for (const auto& seg : to_remove) {
                    long_fences.erase(std::remove(long_fences.begin(), long_fences.end(), seg), long_fences.end());
                }
            } else {
                long_fences.push_back(std::vector{f});
            }
        }

        return locations.size() * long_fences.size();
    }
};


void parse_input(std::vector<GardenSeg>& segments, std::string input_file) {
    std::ifstream MyReadFile(input_file);
    std::string line;

    int y = 0;
    while (std::getline(MyReadFile, line)) {
        int x = 0;
        for (char c : line) {
            // std::cout << "Processing " << c << " at (" << x << ", " << y << ")\n";

            std::vector<coord> adj = coord{x, y}.get_adj();
            auto filtered_view = segments | std::views::filter([c, adj](GardenSeg x) {
                return (x.food == c) and has_intersection(adj, x.locations); 
                });
            auto it = filtered_view.begin();
            auto end = filtered_view.end();
            if (it != end) {
                // std::cout << "Found a segment to add it to\n";
                auto kept_segment = it;
                kept_segment->locations.push_back(coord{x, y});
                
                ++it;
                std::vector<GardenSeg> to_remove; // To store segments to remove
                
                for (; it != end; ++it) {
                    kept_segment->locations.insert(kept_segment->locations.end(), it->locations.begin(), it->locations.end());
                    to_remove.push_back(*it); // Add segment to removal list
                }

                // Remove the segments after the loop ends to avoid invalidating iterators
                for (const auto& seg : to_remove) {
                    segments.erase(std::remove(segments.begin(), segments.end(), seg), segments.end());
                }
            } else {
                segments.push_back(GardenSeg {c, coord{x,y}});
            }
            x += 1;
        }
        y += 1;
    }
    MyReadFile.close();
}


int part_one(std::vector<GardenSeg>& segments) {
    int result = 0;
    for (auto s : segments) {
        result += s.price();
    }
    return result;
}

int part_two(std::vector<GardenSeg>& segments) {
    int result = 0;
    for (auto s : segments) {
        s.fence_segment();
        result += s.bulk_price();
    }
    return result;
}

int main() {

    std::vector<GardenSeg> segments;
    parse_input(segments, "input.txt");

    std::cout << "\nPart One:\n";
    std::cout << part_one(segments);

    std::cout << "\nPart Two:\n";
    std::cout << part_two(segments);

    return 0;
}
