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

void parse_input(std::string input_file, std::vector<long long>& secrets) {
    std::ifstream MyReadFile(input_file);
    std::string line;

    while (std::getline(MyReadFile, line)) {
        secrets.push_back(stoi(line));
    }
}

void mix(long long value, long long& secret) {
    secret = value ^ secret;
}

void prune(long long& secret) {
    secret = secret % 16777216;
}

void new_secret(long long& secret) {
    mix(secret * 64, secret);
    prune(secret);

    mix(secret / 32, secret);
    prune(secret);

    mix(secret * 2048, secret);
    prune(secret);
}

long long part_one(std::vector<long long> secrets) {
    long long result = 0;
    for (auto secret : secrets) {
        for (int i = 0; i < 2000; i++) {
            new_secret(secret);
        }
        result += secret;
    }
    return result;
}

int part_two(std::vector<long long> secrets) {
    std::map<std::vector<int>, std::map<int, int>> sequences;

    for (int s = 0; s < secrets.size(); s++) {
        long long secret = secrets.at(s);
        long long initial_secret = secrets.at(s);
        std::vector<int> secrets;
        std::vector<int> secret_prices;
        std::vector<int> secret_price_changes;
        secret_prices.push_back(secret % 10);
        secrets.push_back(secret);
        secret_price_changes.push_back(100);

        for (int i = 1; i < 2000; i++) {
            new_secret(secret);
            secrets.push_back(secret);
            secret_prices.push_back(secret % 10);
            secret_price_changes.push_back(secret_prices.at(i) - secret_prices.at(i -1));
            if (i > 4) {
                std::vector<int> seq = std::vector<int>(secret_price_changes.begin() + i - 4, secret_price_changes.begin() + i);
                if (!sequences[seq].contains(initial_secret)) {
                    sequences[seq][initial_secret] = secret_prices.at(i - 1);
                }
            }
        }

        // if (initial_secret == )
    }

    std::pair<std::vector<int>, int> best = {{}, 0};


    for (auto [seq, values] : sequences) {
        int banana_total = 0;
        for (auto [secret, bannans]: values) {
            banana_total += bannans;
        }
        if (banana_total > best.second) {
            best = {seq, banana_total};
        }
    }

    for (int i : best.first) {
        std::cout << i << ", ";
    }
    std::cout << "\n" << best.second << "\n";
    return 0;
}

int main() {
    std::vector<long long> secrets;
    parse_input("input.txt", secrets);

    std::cout << "Part One: \n" << part_one(secrets) << "\n";
    std::cout << "Part Two: \n" << part_two(secrets) << "\n";

    return 0;
}