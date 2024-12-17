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

class ReindeerMaze {
        int width, height;
        std::vector<coord> path_coords;
        coord start_coord; 
        coord end_coord;
    public:
        std::vector<node> path_nodes; 
        std::vector<edge> edges;
        std::set<coord> shortest_paths;    
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
                                path_coords.push_back(coord{x,y});
                                break;
                            case END:
                                end_coord = coord{x, y};
                                path_coords.push_back(coord{x,y});
                                break;
                            case PATH:
                                path_coords.push_back(coord{x,y});
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
            for (auto c : path_coords) {
                
                std::vector<node> direction_nodes;
                for (auto d : DIRECTIONS) {
                    direction_nodes.push_back(
                        node{c, d}
                    );
                }

                for (auto& x : direction_nodes) {
                    for (auto& y : direction_nodes) {
                        if ((x.direction.x != y.direction.x * -1) && (x.direction.y != y.direction.y * -1)) {
                            edges.push_back(edge {x, y, 1000});
                        }
                    }
                }
        
                path_nodes.insert(path_nodes.end(), direction_nodes.begin(), direction_nodes.end());
            }

            end.location = end_coord;
            end.direction = coord {0, 0};

            for (auto& x : path_nodes) {
                if ((x.location == start_coord) and (x.direction == DIRECTIONS.at(0))) {
                    start = x;
                }

                if (x.location == end_coord) {
                    edges.push_back(edge {x, end, 0});
                }

                for (auto& y : path_nodes) {
                    if ((x.location + x.direction) == y.location && x.direction == y.direction) {
                        edges.push_back(edge {x, y, 1});
                    }
                }
            }

        path_nodes.push_back(end);

        std::cout << "# Coords: " << path_coords.size() ;
        std::cout << " # Nodes: " << path_nodes.size() << " # Edges: " << edges.size();

        }
        
        ReindeerMaze(std::string input_file) {
            read_file(input_file);
            create_graph();
        }

    void dj() {

        // Initialize all distances to "infinity"
        for (const auto& n : path_nodes) {
            dist[n] = INT_MAX;
            prev[n] = {NULL_NODE};
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
            for (const auto& e : edges | std::views::filter([&u](const edge& ed) { return ed.origin == u; })) {

                node v = e.dest;
                int weight = e.weight;
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

    int shortest_path_value() {
        // Path reconstruction
        std::vector<node> path;
        node current = end;
        while (current != start && current != NULL_NODE) {
            path.push_back(current);
            current = prev[current].at(0);
        }
        path.push_back(start);
        std::reverse(path.begin(), path.end());

        // Calculate the total weight of the shortest path
        int result = 0;
        std::cout << "\n";

        for (size_t i = 0; i < path.size() - 1; ++i) {
            auto it = std::find_if(edges.begin(), edges.end(),
                                [&path, i](const edge& e) { return e.origin == path[i] && e.dest == path[i + 1]; });
            
            if (it != edges.end()) {
                // it->origin.location.print(); std::cout << " - "; it->origin.direction.print();
                // std::cout << " -- " << it->weight << " -->";
                // std::cout << "\n";
                result += it->weight;
            }
        }

        return result;
    }

    int find_shortest_paths() {
        // Path reconstruction
        std::queue<node> to_check;
        to_check.push(end);
        while (!to_check.empty()) {
            auto current = to_check.front();
            // current.location.print(); std::cout << "\n";
            to_check.pop();
            // if (shortest_paths.find(current.location) != shortest_paths.end()) continue;
            shortest_paths.insert(current.location);
            for (auto p : prev[current]) {
                to_check.push(p);
            }
        }
        shortest_paths.insert(start.location);

        return shortest_paths.size();
    }

    void print_map() {
        std::cout << "\n";
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                std::string c = contains(path_coords, coord {x, y}) ? "." : "#";
                c = shortest_paths.contains(coord {x, y}) ? "O" : c;
                std::cout << c;
            }
            std::cout << "\n";
        }
    }
};

int part_one(ReindeerMaze maze) {
    return maze.shortest_path_value();
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
