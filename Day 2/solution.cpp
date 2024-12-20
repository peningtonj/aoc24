#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <map>
#include <functional> 

typedef std::map<int,unsigned int> CounterMap;

std::vector<int> split(std::string str,   char* delimiter)
{
    std::vector<int> v;
     char *token = strtok(const_cast<char*>(str.c_str()), delimiter);
    while (token != nullptr)
    {
        v.push_back(int(std::stoi(token)));
        token = strtok(nullptr, delimiter);
    }
  
  return v;
}


void parse_input(std::vector<std::vector<int> >& reports, std::string input_file) {
    std::ifstream MyReadFile(input_file);

    std::string line;
    while (std::getline(MyReadFile, line))
    {
        reports.push_back(split(line, " "));
    }


    // Close the file
    MyReadFile.close();

}

void print_report(std::vector<int> report) {
    // std::cout << "\n***";
    for (int value : report) {
        std::cout << value << ",";
    }
    // std::cout << "***\n";

}

bool check_levels(int x, int y) {
    return ((x != y) and (abs(x - y) <= 3));
}

int part_one(std::vector<std::vector<int> >& reports) {

    int results = 0;

    for (std::vector<int> report : reports) {
        auto operation = (report.at(0) < report.at(1))
            ? std::function<bool(int, int)>([](int a, int b) { return a <= b; })
            : std::function<bool(int, int)>([](int a, int b) { return a >= b; });

        int result = 1;

        for (int i = 0; i < report.size() - 1; i++) {
            if (!check_levels(report.at(i), report.at(i + 1)) or (!operation(report.at(i), report.at(i+1)))) {
                result = 0;
                break;
            }
        }
        results += result;
    }
    return results;

}

bool check_report_with_tolerance(std::vector<int> report, bool tolerance = true, int removed_index = 0) {
        std::vector<int> safe_report = report;

        if (!tolerance) {
            safe_report.erase(safe_report.begin() + removed_index);
        } 

        auto operation = (safe_report.at(0) < safe_report.at(1))
            ? std::function<bool(int, int)>([](int a, int b) { return a < b; })
            : std::function<bool(int, int)>([](int a, int b) { return a > b; });

        for (int i = 0; i < safe_report.size() - 1; i++) {
            if (!check_levels(safe_report.at(i), safe_report.at(i + 1))) {
                if (tolerance) {
                    return check_report_with_tolerance(safe_report, false, i) or check_report_with_tolerance(safe_report, false, i+1);
                } else {
                    return 0;
                }
            } else if (!operation(safe_report.at(i), safe_report.at(i+1))) {
                if (tolerance) {
                    return check_report_with_tolerance(safe_report, false, 0) or 
                    check_report_with_tolerance(safe_report, false, 1) or
                    check_report_with_tolerance(safe_report, false, i) or
                    check_report_with_tolerance(safe_report, false, i + 1);
            } else {
                    return 0;
                }
            }
        }

        return 1;
}

int part_two(std::vector<std::vector<int> >& reports) {

    int results = 0;

    for (int i = 0; i < reports.size(); i++) {
        int result = check_report_with_tolerance(reports.at(i), true);
        results += result;
    }

    return results;
}

int main() {

    std::vector<std::vector<int> > reports;

    parse_input(reports, "input.txt");

    std::cout << "Part One:\n";
    std::cout << part_one(reports);
	
    std::cout << "\nPart Two:\n";
    std::cout << part_two(reports);

    return 0;

}
