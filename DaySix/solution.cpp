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
        std::cout << x << ", " << y;
    }

    bool operator==(const coord& other) const {
        return x == other.x && y == other.y;
    }

    bool operator<(const coord& other) const {
        return (x < other.x) || (x == other.x && y < other.y);
    }

    void turn_left() {
        int temp = x;
        x = y;
        y = -temp;
    }

    void turn_right() {
        int temp = x;
        x = -y;
        y = temp;
    }
};

coord add_coord_as_vector(coord c, coord v) {
    return coord {c.x + v.x, c.y + v.y};
}


struct guard_step {
    coord location;
    coord direction; 
    int step;

    void print() {
        location.print();
        std::cout << " - ";
        direction.print();
    }

    bool operator==(const guard_step& other) const {
        return (location == other.location) and (direction == other.direction);
    }
};

const std::vector<guard_step> LOOP = {guard_step {coord {0, 0}, coord {0, 0}, -1}};

template <typename T>
bool contains(const std::vector<T>& elements, const T& target) {
    return std::find(elements.begin(), elements.end(), target) != elements.end();
}

class Maze {
        int height, width;
    public:
        std::vector<coord> obstacles; 
        guard_step starting_point;

        Maze(std::string input_file) {
            std::cout << "Reading Maze";
            std::ifstream MyReadFile(input_file);
            std::string line;

            int y = 0;
            while (std::getline(MyReadFile, line)) {
                int x = 0;
                for (char c : line) {
                    switch (c)
                    {
                        case OBSTACLE:
                            obstacles.push_back(coord {x, y});
                            break;
                        case UP_GAURD:
                            starting_point = guard_step{ coord {x, y}, coord {0, -1}, 0 };
                            break;
                        case LEFT_GAURD:
                            starting_point = guard_step{ coord {x, y}, coord {-1, 0}, 0 };
                            break;
                        case RIGHT_GAURD:
                            starting_point = guard_step{ coord {x, y}, coord {1, 0}, 0 };
                            break;
                        case DOWN_GAURD:
                            starting_point = guard_step{ coord {x, y}, coord {0, 1}, 0 };
                            break;
                        
                        default:
                            break;
                    }

                    x += 1;
                }
                y += 1;
                width = x;

            }

            height = y;
        }

    bool point_in_maze(coord point) {
        return (point.x > 0) and (point.x < width) and (point.y > 0) and (point.y < height);
    }
};

coord normalise_vector(coord c) {
    c.x = c.x == 0 ? 0 : c.x / abs(c.x);
    c.y = c.y == 0 ? 0 : c.y / abs(c.y);

    return c;
}

coord get_direction(coord a, coord b) {
    return coord {a.x - b.x, a.y - b.y};
}

int get_distance(coord a, coord b) {
    return (abs(a.x - b.x) + abs(a.y - b.y));
}

bool in_the_way(coord obstacle, coord start, coord direction) {
    return normalise_vector(get_direction(obstacle, start)) == direction;
}


void add_path_from_points(std::vector<guard_step>& path, guard_step start, coord end, bool include_end = false) {
    coord path_direction = normalise_vector(get_direction(end, start.location));
    coord current = start.location;
    int step_count = start.step + 1;
    while (!(current == end)) {
        path.push_back(guard_step {current, path_direction, step_count});
        current.x += path_direction.x;
        current.y += path_direction.y;
        step_count += 1;
    }
}

void add_path_to_exit(Maze maze, std::vector<guard_step>& path, guard_step start) {
    coord current = start.location;
    int step_count = start.step + 1;

    while (maze.point_in_maze(current)) {
        path.push_back(guard_step {current, start.direction, step_count});
        current.x += start.direction.x;
        current.y += start.direction.y;
        step_count += 1;

    }

}

std::set<coord> unique_steps_by_location(const std::vector<guard_step>& guard_steps) {
    std::set<coord> unique_locations;

    for (const auto& g : guard_steps) {
        unique_locations.insert(g.location);
    }

    return unique_locations;
}

coord get_next_obstacle_from_step(Maze maze, guard_step step) {
        auto in_the_way_of = [step](coord obstacle) { return in_the_way(obstacle, step.location, step.direction);};
        auto in_path_view = maze.obstacles 
                       | std::views::filter(in_the_way_of);

        std::vector<coord> in_path{in_path_view.begin(), in_path_view.end()};

        if (in_path.size()) {
            std::ranges::sort(in_path, [step](coord a, coord b) {
                return get_distance(step.location, a) < get_distance(step.location, b);
            });

            return in_path.at(0);
        } else {
            return coord {-1, -1};
        }
}

std::vector<guard_step> get_maze_path(Maze maze) {
    std::vector<guard_step> path;
    guard_step last_spot = maze.starting_point;
    do {
        coord next_obstacle = get_next_obstacle_from_step(maze, last_spot);
        if (next_obstacle.x != -1 ) {
            add_path_from_points(path, last_spot, next_obstacle);
            last_spot = path.at(path.size() - 1);
            last_spot.direction.turn_right();
            
            for (int i = 0; i < path.size() - 1; i++) {
                    if ((last_spot.direction == path.at(i).direction) and (last_spot.location == path.at(i).location)) {
                    return LOOP;
                }
            }
        } else {
            add_path_to_exit(maze, path, last_spot);
            break;
        }
    } while (true);

    return path;
}

int part_one(Maze maze) {
    return unique_steps_by_location(get_maze_path(maze)).size();
}

coord spot_before(coord spot, coord direction){
    direction.x = -direction.x;
    direction.y = -direction.y;
    return add_coord_as_vector(spot, direction);
}

bool steps_into_loop(Maze maze, guard_step step) {
    std::vector<coord> four_corners;

    guard_step last_spot = step;

    for (int i = 0; i < 5; i++){
        coord obstacle = get_next_obstacle_from_step(maze, last_spot);
        if (obstacle.x != -1 ) {
            four_corners.push_back(obstacle);
            last_spot.location = spot_before(obstacle, last_spot.direction);
            last_spot.direction.turn_right();
        } else {
            return false;
        }
    }


    return four_corners.at(0) == four_corners.at(four_corners.size() - 1); 
}

// Place an obstacle in front of the guard_step and see if it creates a loop
bool check_if_creates_loop_easy(Maze maze, guard_step step, std::vector<guard_step> path) {
    return false;
    coord search_direction = step.direction;
    search_direction.turn_right();

    coord new_obstacle_location = add_coord_as_vector(step.location, step.direction);

    if (contains(maze.obstacles, new_obstacle_location)) {
        return false;
    }

    coord next_obstacle = get_next_obstacle_from_step(maze, guard_step {step.location, search_direction, 0});

    if (next_obstacle.x != -1){
        coord needed_walking_drection = search_direction;
        needed_walking_drection.turn_right();

        for (int i = 0; i < step.step; i++)
            {
                if ((path.at(i).location == spot_before(next_obstacle, search_direction)) 
                and path.at(i).direction == needed_walking_drection) {
                    return true;
                }
            }

        coord start_position = spot_before(new_obstacle_location, step.direction);

        guard_step new_step = guard_step {start_position, search_direction, -1};
        return steps_into_loop(maze, new_step);
        }

    return false;
}

bool check_full_path(Maze maze, coord new_obstacle, guard_step new_starting_point) {
    Maze new_maze = maze;
    new_maze.obstacles.push_back(new_obstacle);
    new_maze.starting_point = new_starting_point;
    std::vector path = get_maze_path(new_maze);
    if (path.size()) {
        return path.at(0).direction == coord {0 ,0};
    }
    return false;
}

int part_two(Maze maze) {
    std::vector<coord> tried_obstacles;
    int result = 0;
    std::vector path = get_maze_path(maze);
    for (int i = 0; i < path.size(); i++) {
        guard_step step = path.at(i);
        coord new_obstacle_location = add_coord_as_vector(step.location, step.direction);
        if (contains(tried_obstacles, new_obstacle_location)) {
            continue;
        }
        if (check_full_path(maze, new_obstacle_location, path.at(i))) {
            result += 1;
        }
        tried_obstacles.push_back(new_obstacle_location);
    }
    return result;
}

int main() {

    Maze maze = Maze("input.txt");
    // Maze maze = Maze("example.txt");

    std::cout << "Part One:\n";
    std::cout << part_one(maze);
	
    // Maze new_maze = Maze("example.txt");

    std::cout << "\nPart Two:\n";
    std::cout << part_two(maze);

    return 0;

}
