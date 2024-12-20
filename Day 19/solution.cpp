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


class TowelDesign {
    std::vector<std::string> towels;
    std::vector<std::string> designs;

        void find_combos(std::string towel, std::string remaining, std::vector<std::string> combo) {

            // std::cout << "Trying to make design " << towel << " and remaining is " << remaining << "\n";

            if (remaining == "") {
                bool bad_combo = false;
                std::string last_towel = combo.at(combo.size() - 1);
                for (auto t : towels) {
                    if ((last_towel != t) && (last_towel == t.substr(0, last_towel.size()))) {
                        // std::cout << "Issue with " << last_towel << " due to " << t << "\n";
                        bad_combo = true;
                        break;
                    }
                }

                if (!bad_combo && (combo != std::vector{towel})) composite_towels.at(towel).push_back(combo);
                return;
            }

            for (auto next_towel : towels) {
                if (next_towel == remaining.substr(0, next_towel.size())) {
                        // std::cout << "Making profress with " << next_towel << "\n";
                        std::vector<std::string> running_combo = combo;
                        running_combo.push_back(next_towel);
                        find_combos(towel, remaining.substr(next_towel.size(), remaining.size()), running_combo);
                    }
                }
            return;
        } 

    void make_composite_towels() {
        for (auto towel : towels) {
            composite_towels[towel] = {{towel}};
            find_combos(towel, towel, {});
        }
    }

    public:
        std::map<std::string, std::vector<std::vector <std::string> > >composite_towels;
        std::map<std::string, bool> skippable;
        std::map<std::string, long long> cache;

        TowelDesign(std::string input_file) {
            std::string myText;
            std::ifstream MyReadFile(input_file);

            std::string line;
            std::getline(MyReadFile, line);

            size_t pos = 0;
            while ((pos = line.find(",")) != std::string::npos) {
                towels.push_back(line.substr(0, pos));
                line.erase(0, pos + 2);
            }
            towels.push_back(line);
            std::getline(MyReadFile, line); // Flush empty line

            while (std::getline(MyReadFile, line)) {
                designs.push_back(line);
            }

            make_composite_towels();

            // for (auto [key, value]: composite_towels) {
            //     std::cout << key << ": \n";
            //     for (auto v : value) {
            //         for (auto t : v ) {
            //             std::cout << t << ", ";
            //         }
            //         std::cout << "\n";
            //     }
            // }

            std::sort(towels.begin(), towels.end(), [](const std::string& a, const std::string& b){
                return a.size() > b.size();
            });            
        }



        bool make_design(std::string design) {
            
            // std::cout << "Trying to make design " << design << "\n";

            auto it = std::find(towels.begin(), towels.end(), design);
            if (it != towels.end()) return true;

            for (auto towel : towels) {
                // std::cout << towel << "-" << towel.size() <<  " - " << design.substr(0, towel.size()) << "\n";
                if (towel == design.substr(0, towel.size())) {
                    if (make_design(design.substr(towel.size(), design.size()))) {
                        return true;
                    }
                }
            }

            return false;
        } 

        int makable_designs() {
            int result = 0;
            for (auto d : designs) {
                result += make_design(d);
            }
            return result;
        }

        long long count_design(std::string design) {

            if (design == "") return 1;
            if (cache.contains(design)) return cache[design];

            long long result = 0;
            for (auto towel : towels) {
                if (towel == design.substr(0, towel.size())) {
                        result += count_design(design.substr(towel.size(), design.size()));
                    }
                }

            cache[design] = result;
            return result;

        }

        long long count_designs() {
            long long result = 0;
            for (auto d : designs) {
                    result += count_design(d);
            }

            return result;
        }


};

int part_one(TowelDesign td) {
    return td.makable_designs();
}

long long part_two(TowelDesign td) {
    
    return td.count_designs();
    // return 0;
}

int main() {

    TowelDesign td = TowelDesign("input.txt");

    std::cout << "Part One:\n";
    std::cout << part_one(td);
	
    std::cout << "\nPart Two:\n";
    std::cout << part_two(td);
    

    return 0;

}
