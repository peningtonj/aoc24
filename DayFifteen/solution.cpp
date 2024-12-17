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
struct robot {
    coord location;
    std::string plan;

    char pop_front() {
        char c = '\0';    
        if (!plan.empty()) {
            c = plan.at(0);
            plan.erase(0, 1);
        }
        return c;
    }
};


const char WALL = '#';
const char BOX = 'O';
const char ROBOT = '@';

template <typename T>
bool contains(const std::vector<T>& elements, const T& target) {
    return std::find(elements.begin(), elements.end(), target) != elements.end();
}

class RobotMaze {
        int height, width;
    public:
        std::vector<coord> boxes; 
        std::vector<coord> walls; 
        robot guard;

        RobotMaze(std::string input_file) {
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
                        case WALL:
                            walls.push_back(coord {x, y});
                            break;
                        case BOX:
                            boxes.push_back(coord {x, y});
                            break;
                        case ROBOT:
                            guard.location = coord {x, y};                        
                        default:
                            break;
                    }

                    x += 1;
                }
                y += 1;
                width = x;

            }
            height = y;
        

        std::string instructions = "";
        while (std::getline(MyReadFile, line)) {
            instructions += line;
        }

        guard.plan = instructions;

    }   

    void update_box(coord old_box, coord new_location) {
        for (int i = 0; i < boxes.size(); i++) {
            if (boxes.at(i) == old_box) {
                boxes.at(i) = new_location;
            }
        }
    }

    bool move_box(coord location, coord direction) {
        coord new_location = location + direction;
        if (contains(walls, new_location)) {
            return false;
        } else if (contains(boxes, new_location)) {
            if (move_box(new_location, direction)) {
                update_box(location, new_location);
                return true;
            } else {
                return false;
            }
        } else {
            update_box(location, new_location);
            return true;
        }
    }

    void try_step(coord location, coord direction) {
        if (contains(walls, location + direction)) {
            return;
        } else if (contains(boxes, location + direction)) {
            if (move_box(location + direction, direction)) {
                guard.location = location + direction;
            };
        } else {
            guard.location = location + direction;
        }
        return;
    }

    void execute() {
        char next_step = guard.pop_front();
        while (next_step) {
            coord direction;
            switch (next_step)
                {
                case '<':
                    direction = coord {-1, 0};
                    break;
                case '^':
                    direction = coord {0, -1};
                    break;
                case 'v':
                    direction = coord {0, 1};
                    break;
                case '>':
                    direction = coord {1, 0};
                    break;
                default:
                    break;
        }
        try_step(guard.location, direction);
        next_step = guard.pop_front();
    }
    }
    void print_map() {
        std::cout << "\n";
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            std::string c = contains(walls, coord {x, y}) ? "#" : ".";
            c = contains(boxes, coord {x, y}) ? "O" : c;
            c = (guard.location == coord {x, y}) ? "@" : c;
            std::cout << c;
        }
        std::cout << "\n";
    }
}

    long long gps() {
        int result = 0;
        for (auto b : boxes) {
            result += (b.y*100) + b.x;
        }

        return result;
    }

};


class BigRobotMaze {
    int height, width;

public:
    std::vector<std::shared_ptr<coord>> boxes; 
    std::vector<coord> walls; 
    robot guard;

    BigRobotMaze(std::string input_file) {
        std::ifstream MyReadFile(input_file);
        std::string line;

        int y = 0;
        while (std::getline(MyReadFile, line)) {
            if (line.empty()) break;

            int x = 0;
            for (char c : line) {
                switch (c) {
                    case '#': // Wall
                        walls.push_back(coord{x, y});
                        walls.push_back(coord{x + 1, y});
                        break;
                    case 'O': // Box
                        boxes.push_back(std::make_shared<coord>(coord{x, y}));
                        break;
                    case '@': // Robot
                        guard.location = coord{x, y};
                        break;
                }
                x += 2;
            }
            y += 1;
            width = x;
        }
        height = y;

        std::string instructions = "";
        while (std::getline(MyReadFile, line)) {
            instructions += line;
        }
        guard.plan = instructions;
    }

    bool contains(const std::vector<coord>& list, const coord& target) const {
        return std::find(list.begin(), list.end(), target) != list.end();
    }

    std::shared_ptr<coord> contains_box(const coord& c) {
        for (auto& box : boxes) {
            if (*box == c || *box == c + coord{-1, 0}) {
                return box;
            }
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<coord>> get_box_chain(const coord& start, const coord& direction) {
        std::vector<std::shared_ptr<coord>> chain;
        std::queue<std::shared_ptr<coord>> boxes_to_check;

        auto box = contains_box(start);
        if (box) boxes_to_check.push(box);

        while (!boxes_to_check.empty()) {
            auto b = boxes_to_check.front();
            boxes_to_check.pop();

            if (std::find(chain.begin(), chain.end(), b) == chain.end()) {
                chain.push_back(b);

                for (auto& c : { *b, *b + coord{1, 0} }) {
                    auto next_box = contains_box(c + direction);
                    if (next_box && std::find(chain.begin(), chain.end(), next_box) == chain.end()) {
                        boxes_to_check.push(next_box);
                    }
                }
            }
        }
        return chain;
    }

    bool move_box(const coord& box_location, const coord& direction) {
        // std::cout << "Trying to move the box at "; box_location.print(); std::cout << " in direction "; direction.print(); std::cout << "\n";

        auto box_chain = get_box_chain(box_location, direction);
        for (auto& box_ptr : box_chain) {
            for (auto c : { *box_ptr, *box_ptr + coord(1, 0) }) {
                if (contains(walls, c + direction)) {
                    return false; // Blocked
                }
            }
        }

        // Move boxes
        for (auto& box_ptr : box_chain) {
            *box_ptr = *box_ptr + direction;
        }
        return true;
    }

    void try_step(const coord& location, const coord& direction) {
        auto box_ptr = contains_box(location + direction);
        if (contains(walls, location + direction)) {
            return;
        } else if (box_ptr) {
            if (move_box(*box_ptr, direction)) {
                guard.location = guard.location + direction;
            }
        } else {
            guard.location = guard.location + direction;
        }
    }

    void execute() {
        char next_step = guard.pop_front();

        while (next_step) {
            coord direction;

            switch (next_step) {
                case '<': direction = coord{-1, 0}; break;
                case '^': direction = coord{0, -1}; break;
                case 'v': direction = coord{0, 1}; break;
                case '>': direction = coord{1, 0}; break;
                default: break;
            }

            try_step(guard.location, direction);
            next_step = guard.pop_front();
        }
    }

    void print_map() {
        std::cout << "\n";
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                std::string c = ".";
                if (contains(walls, coord{x, y})) {
                    c = "#";
                }
                for (auto& box : boxes) {
                    if (*box == coord{x, y}) {
                        c = "[]";
                        x += 1;
                    }
                }
                if (guard.location == coord{x, y}) c = "@";
                std::cout << c;
            }
            std::cout << "\n";
        }
    }

    long long gps() {
        int result = 0;
        for (auto b : boxes) {
            result += (b->y*100) + b->x;
        }

        return result;
    }

};


int part_one(RobotMaze maze) {
    maze.execute();
    maze.print_map();
    return maze.gps();
}

long long part_two(BigRobotMaze maze) {
    maze.execute();
    maze.print_map();
    return maze.gps();
}

int main() {

    RobotMaze maze = RobotMaze("input.txt");

    std::cout << "\nPart One:\n";
    std::cout << part_one(maze);

    BigRobotMaze big_maze = BigRobotMaze("input.txt");

    std::cout << "\nPart Two:\n";
    std::cout << part_two(big_maze);

    return 0;
}
