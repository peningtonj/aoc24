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
#include <algorithm> // For std::min

void parse_input(std::string input_file, std::vector<std::pair<std::string, std::string>>& connections) {
    std::ifstream MyReadFile(input_file);
    std::string line;

    while (std::getline(MyReadFile, line)) {
        connections.push_back({line.substr(0, 2), line.substr(3,5)});
    }
}
class Graph {
    public:
    std::vector<std::pair<std::string, std::string>> edges;
    std::set<std::string> nodes;
    std::unordered_map<std::string, std::set<std::string>> adjList;
    Graph(std::vector<std::pair<std::string, std::string>>& connections) {
        edges = connections;
        for (auto e : connections) {
            nodes.insert(e.first);
            nodes.insert(e.second);

            if (adjList.contains(e.first)) adjList[e.first].insert(e.second);
            else adjList[e.first] = {e.second};

            if (adjList.contains(e.second)) adjList[e.second].insert(e.first);
            else adjList[e.second] = {e.first};
        }
    }

    // std::set<std::set<std::string>> 
    void find_cycles(int depth = 3) {
        std::map<int, std::vector<std::vector<std::string>>> paths;
        paths[0] = {};

        for (auto n : nodes) {
            paths[0].push_back(std::vector<std::string>{n});
        }

        for (int i = 1 ; i <= depth; i++) {
            paths[i] = {};
            for (auto p : paths[i - 1]) {
                auto end = *(p.end() - 1);
                for (auto next : adjList[end]) {
                    auto new_path = p;
                    new_path.push_back(next);
                    paths[i].push_back(new_path);
                }
            }
        }

        auto cycles = paths[3] | std::views::filter([](std::vector<std::string> path) {
            return *path.begin() == *(path.end() - 1);
        }) | std::views::filter([](std::vector<std::string> path) {
            for (auto n : path) {
                if (n.starts_with('t')) return true;
            }
            return false;;
            }
        );

        std::set<std::set<std::string>> unique_networks;

        for (auto c : cycles) {
            std::set<std::string> network;
            for (auto n : c) {
                network.insert(n);
            }
            unique_networks.insert(network);
        }

        std::cout << "\n" << unique_networks.size() << "\n";
    }

    std::set<std::string> intersectSets(const std::vector<std::set<std::string>>& sets) {
        if (sets.empty()) {
            return {};
        }

        // Sort sets by size to minimize intersection cost
        auto sortedSets = sets;
        std::sort(sortedSets.begin(), sortedSets.end(), [](const auto& a, const auto& b) {
            return a.size() < b.size();
        });

        // Start with the smallest set
        std::set<std::string> result = sortedSets[0];

        for (size_t i = 1; i < sortedSets.size(); ++i) {
            std::set<std::string> temp;
            std::set_intersection(result.begin(), result.end(),
                                sortedSets[i].begin(), sortedSets[i].end(),
                                std::inserter(temp, temp.begin()));
            result = std::move(temp);

            // Early exit if intersection becomes empty
            if (result.empty()) {
                break;
            }
        }

        return result;
    }


    bool all_connect(std::vector<std::string> n1, std::vector<std::string> n2) {
        for ( auto c1 : n1 ) {
            for (auto c2 : n2) {
                if (!adjList[c1].contains(c2)) return false;
                }
            }
        return true;
    }

    void find_lan_networks() {
        std::vector<std::vector<std::string> > networks;
        std::set<std::set<std::string> > final_networks;

        for (auto n : nodes) {
            networks.push_back(std::vector<std::string>{n});
        }


        int x = 0;

        while (true) {
            std::cout << x << " - " << networks.size() << "\n";
                for (auto c : networks.at(0)) {
                    std::cout << c << ", ";
                }
            std::cout << "\n";
            x++;
            std::vector<std::vector<std::string> > new_networks;
            for (int i = 0; i < networks.size(); i++) {
                auto n1 = networks.at(i);
                std::vector<std::string> new_network;
                for (int j = i + 1; j < networks.size(); j++) {
                    auto n2 = networks.at(j);
                    if (all_connect(n1, n2)) {
                        std::set_union(n1.cbegin(), n1.cend(),
                                        n2.cbegin(), n2.cend(),
                                        std::back_inserter(new_network));
                        std::sort(new_network.begin(), new_network.end());
                        auto it = unique(new_network.begin(), new_network.end());
                        new_network.erase(it, new_network.end());
                        n1 = new_network;
                    }
                }

                if (new_network.size()) {
                    new_networks.push_back(new_network);
                }
            }

            if (new_networks.size()) {
                networks = new_networks;
            } else {
                break;
            }
        }

        auto largestSetIt = std::max_element(
            networks.begin(), 
            networks.end(), 
            [](const std::vector<std::string>& a, const std::vector<std::string>& b) {
                return a.size() < b.size(); // Compare by size
            }
        );

        if (largestSetIt != networks.end()) {
            std::cout << "Largest set size: " << largestSetIt->size() << "\n";
            std::cout << "Elements: ";
            auto party = *largestSetIt;
            std::sort(party.begin(), party.end());
            for (const auto& elem : party) {
                std::cout << elem << ",";
            }
            std::cout << "\n";
        } else {
            std::cout << "The vector is empty.\n";
        }
    }
};



int part_one(std::vector<std::pair<std::string, std::string>>& connections) {
    Graph g = Graph(connections);
    
    g.find_cycles();
    return 0;
}

int part_two(std::vector<std::pair<std::string, std::string>>& connections) {
    Graph g = Graph(connections);
    g.find_lan_networks();
    return 0;
}

int main() {
    std::vector<std::pair<std::string, std::string>> connections;
    parse_input("input.txt", connections);

    // std::cout << "Part One: \n" << part_one(connections) << "\n";
    std::cout << "Part Two: \n" << part_two(connections) << "\n";

    return 0;
}