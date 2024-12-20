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


int width = 5;
int height = 3;
int initial_value = 0;



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

    std::vector<coord> get_adj() {
        return {
            coord{x, y + 1},
            coord{x, y - 1},
            coord{x + 1, y},
            coord{x - 1, y},
        };
    }
};

namespace std {
    template <>
    struct hash<coord> {
        size_t operator()(const coord& c) const {
            // Combine the hash of x and y
            return std::hash<int>{}(c.x) ^ (std::hash<int>{}(c.y) << 1);
        }
    };
}

coord NULL_COORD = coord{-1, -1};

template <typename T>
bool contains(const std::vector<T>& elements, const T& target) {
    return std::find(elements.begin(), elements.end(), target) != elements.end();
}

class MemoryMap {
    int width, height;
    std::unordered_map<coord, std::vector<std::pair<coord, int>>> adjList;
    std::map<coord, coord> prev;      // Predecessors for path reconstruction
    std::vector<std::vector<coord> > paths;
    
    bool point_in_maze(coord point) {
        return (point.x >= 0) and (point.x < width) and (point.y >= 0) and (point.y < height);
    }

    public:
        std::map<coord, std::vector<coord>> all_prev;      // Predecessors for path reconstruction
        coord end;
        std::map<coord, int> dist;       // Distance from start node

        std::vector<coord> falling;
        MemoryMap(int width, int height, int lapsed, std::string input_file) {
            this->width = width;
            this->height = height;
            end = coord{width - 1, height - 1};

            std::string myText;
            std::ifstream MyReadFile(input_file);

            int x, y;
            char c;

            while ((MyReadFile >> x >> c >> y) && lapsed)
            {
                falling.push_back(coord{x, y});
                lapsed -= 1;
            }
            MyReadFile.close();

            for (int x = 0; x < width; x++) {
                for (int y = 0; y < width; y++ ) {
                    if(!contains(falling, coord{x, y})) {
                        // coord{x, y}.print();
                        for (auto adj: coord{x, y}.get_adj()) {
                            if (!contains(falling, adj) && point_in_maze(adj)) 
                                adjList[coord{x, y}].push_back({adj, 1});
                        }
                    }
                }
            }
        }

        void dj(coord start) {
            std::set<coord> visited;         // Set to track visited nodes

            // std::cout << "Calculating paths from ";
            // Initialize all distances to "infinity"
            for (const auto& n : adjList) {
                dist[n.first] = INT_MAX;
                prev[n.first] = NULL_COORD;
            }
            
            // Lambda for priority queue comparison (min-heap)
            auto compare = [this](const coord& a, const coord& b) { return dist.at(a) > dist.at(b); };
            std::priority_queue<coord, std::vector<coord>, decltype(compare)> pq(compare);

            // Start with the source node
            dist[start] = 0;
            pq.push(start);

            // Dijkstra's main loop
            while (!pq.empty()) {
                coord u = pq.top();
                pq.pop();

                if (visited.find(u) != visited.end()) continue; // Skip if already visited
                visited.insert(u);

                // std::cout << "#######\n";
                // u.print();
                // Check neighbors (edges originating from u)
                for (const auto& n : adjList[u]) {
                    coord v = n.first;
                    // v.print();
                    int weight = n.second;
                    int alt = dist[u] + weight;
                    if (alt < dist[v]) { // Relaxation
                        dist[v] = alt;
                        prev[v] = u;
                        pq.push(v);
                    }
                }
            }
    }

    std::vector<coord> reconstruct_path() {
            // Path reconstruction
        std::vector<coord> path;
        coord current = end;
        while (current != coord{0, 0} && current != NULL_COORD) {
            path.push_back(current);
            current = prev[current];
        }
        if (current == NULL_COORD) return path;
        path.push_back(coord{0, 0});
        std::reverse(path.begin(), path.end());
        return path;
    }

        coord check_paths(int lapsed, std::string input_file) {
            std::string myText;
            std::ifstream MyReadFile(input_file);

            int x, y;
            char c;

            while (lapsed)
            {
                MyReadFile >> x >> c >> y;
                lapsed -= 1;
            }
            
            dj(coord{0, 0});
            std::vector path = reconstruct_path();

            int limit = 10;
            while ((path.at(0) == coord{0,0}) && limit) {
                MyReadFile >> x >> c >> y;
                    for (auto& [key, adj] : adjList) { 
                        for (int i = 0; i < adj.size(); i++ ) {
                            if (adj.at(i).first == coord{x, y}) {
                                adj.erase(adj.begin() + i);
                            }
                        }
                    }
                if (contains(path, coord{x, y})) {
                    dj(coord{0, 0});
                    path = reconstruct_path();
                }
            } 
        return coord{x, y};
        }


};

int part_one(MemoryMap mem) {
    mem.dj(coord{0, 0});
    return mem.dist.at(coord{mem.end});
}

coord part_two(MemoryMap mem) {
    return mem.check_paths(1024, "input.txt");
}

int main() {

    MemoryMap mem = MemoryMap(71, 71, 1024, "input.txt");

    std::cout << "Part One:\n";
    std::cout << part_one(mem);
	
    std::cout << "\nPart Two:\n";
    part_two(mem).print();

    return 0;

}
