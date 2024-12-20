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

struct node {
    coord location;
    coord direction;
   
    bool operator==(const node& other) const {
        return location == other.location && direction == other.direction;
    }

    bool operator<(const node& other) const {
        if (location.x != other.location.x) return location.x < other.location.x;
        if (location.y != other.location.y) return location.y < other.location.y;
        if (direction.x != other.direction.x) return direction.x < other.direction.x;
        return direction.y < other.direction.y;
}

};

const node NULL_NODE = node {coord{-1, -1}, coord{0,0}};


struct edge {
    node origin;
    node dest;
    int weight;
};

const std::vector<coord> DIRECTIONS = {
    coord{1,0},
    coord{-1,0},
    coord{0,-1},
    coord{0,1},
};

coord rot(coord c, int direction) {
    if (direction == 1) {
        int temp = c.x;
        c.x = c.y;
        c.y = -temp;
    } else {
        int temp = c.x;
        c.x = -c.y;
        c.y = temp;
    }

    return c;
}

namespace std {
    template <>
    struct hash<coord> {
        size_t operator()(const coord& c) const {
            // Combine the hash of x and y
            return std::hash<int>{}(c.x) ^ (std::hash<int>{}(c.y) << 1);
        }
    };
}

namespace std {
    template <>
    struct hash<node> {
        size_t operator()(const node& n) const {
            // Combine the hashes of location and direction
            size_t locHash = std::hash<coord>{}(n.location);
            size_t dirHash = std::hash<coord>{}(n.direction);
            return locHash ^ (dirHash << 1);  // Combine the two hashes
        }
    };
}

class ReindeerMaze {
        int width, height;
        std::set<coord> path_coords;
        coord start_coord; 
        coord end_coord;
    public:
        std::vector<node> path_nodes; 
        std::unordered_map<node, std::vector<std::pair<node, int>>> adjList;
        std::set<coord> shortest_paths_coords;
        std::set<node> shortest_paths;    
        node start;
        node end;
        std::map<node, int> dist;       // Distance from start node
        std::map<node, std::vector<node>> prev;      // Predecessors for path reconstruction
        std::set<node> visited;         // Set to track visited nodes

        void read_file(std::string input_file) {
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
            }

        void create_graph() {
            start = node{start_coord, DIRECTIONS.at(0)};
            end = node{end_coord, coord{0, 0}};
            adjList[end] = {}; 

            for (auto c : path_coords) {
                std::vector<node> direction_nodes;
                for (auto d : DIRECTIONS) {
                    adjList[node{c, d}] = {}; 
                    for (auto r : {rot(d, 1), rot(d, -1)}) {
                        adjList.at(node{c, d}).push_back({node{c, r}, 1000});
                    }
                
                    if (path_coords.find(c + d) != path_coords.end()) {
                        adjList.at(node{c, d}).push_back({node{c + d, d}, 1});
                    } 
                }        
            }

            for (auto d : DIRECTIONS) {
                adjList.at(node{end_coord, d}).push_back({end, 0});
                adjList.at(end).push_back({node{end_coord, d}, 0});
            }
        }
        
        ReindeerMaze(std::string input_file) {
            read_file(input_file);
            create_graph();
        }

    void dj() {

        std::cout << "Calculating paths from S";
        // Initialize all distances to "infinity"
        for (const auto& n : adjList) {
            dist[n.first] = INT_MAX;
            prev[n.first] = {NULL_NODE};
        }
        
        // Lambda for priority queue comparison (min-heap)
        auto compare = [this](const node& a, const node& b) { return dist.at(a) > dist.at(b); };
        std::priority_queue<node, std::vector<node>, decltype(compare)> pq(compare);

        // Start with the source node
        dist[start] = 0;
        pq.push(start);

        // Dijkstra's main loop
        while (!pq.empty()) {
            node u = pq.top();
            pq.pop();

            if (visited.find(u) != visited.end()) continue; // Skip if already visited
            visited.insert(u);

            // Check neighbors (edges originating from u)
            for (const auto& n : adjList[u]) {

                node v = n.first;
                int weight = n.second;
                int alt = dist[u] + weight;
                if (alt < dist[v]) { // Relaxation
                    dist[v] = alt;
                    prev[v] = {u};
                    pq.push(v);
                } else if (alt == dist[v]) {
                    prev[v].push_back(u);
                    pq.push(v);
                }
            }
        }
    }


    int find_shortest_paths() {
        // Path reconstruction
        std::queue<node> to_check;
        to_check.push(end);
        while (!to_check.empty()) {
            auto current = to_check.front();
            to_check.pop();
            if (shortest_paths.contains(current)) continue;
            shortest_paths.insert(current);
            for (auto p : prev[current]) {
                // p.location.print();
                to_check.push(p);
            }
        }
        shortest_paths.insert(start);
        for (auto n: shortest_paths) {
            shortest_paths_coords.insert(n.location);
        }

        return shortest_paths_coords.size() - 1;
    }

    void print_map() {
        std::cout << "\n";
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                std::string c = path_coords.contains(coord {x, y}) ? "." : "#";
                c = shortest_paths_coords.contains(coord {x, y}) ? "O" : c;
                std::cout << c;
            }
            std::cout << "\n";
        }
    }
};

int part_one(ReindeerMaze maze) {
    return maze.dist.at(maze.end);;
}

int part_two(ReindeerMaze maze) {
    return maze.find_shortest_paths();;
}

int main() {

    ReindeerMaze maze = ReindeerMaze("input.txt");
    maze.dj();
    std::cout << "\nPart One:\n";
    std::cout << part_one(maze);

    std::cout << "\nPart Two:\n";
    std::cout << part_two(maze);

    return 0;
}
