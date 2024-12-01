#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <map>

typedef std::map<int,unsigned int> CounterMap;


void parse_input(std::vector<int>& locations_one, std::vector<int>& locations_two, std::string input_file) {
    std::string myText;
    std::ifstream MyReadFile(input_file);

    int a, b;
    // read each pair of numbers into the two vectors
    while (MyReadFile >> a >> b)
    {
        locations_one.push_back(a);
        locations_two.push_back(b);
    }


    // Close the file
    MyReadFile.close();

}

int part_one(std::vector<int>& locations_one, std::vector<int>& locations_two) {
    // sort the vectors as directed
    std::sort(locations_one.begin(),locations_one.end());
    std::sort(locations_two.begin(),locations_two.end());

    int distance = 0;
    
    // sum differences
    for(int i=0; i<locations_one.size(); i++)
	{
		distance += abs(locations_one.at(i) - locations_two.at(i));
	}

    return distance;
}

CounterMap count_elements(std::vector<int>& locations) {
    // Order of the vectors doesn't matter so let's just count the number of times each number occurs
    CounterMap counts;

    for (int val :locations) {
        if (counts.find(val) != counts.end()) {
            counts[val] += 1;
        } else {
            counts[val] = 1;
        }
    }

    return counts;
}


int part_two(std::vector<int>& locations_one, std::vector<int>& locations_two) {

    // get counts of elements for each locations list 
    CounterMap locations_one_counts = count_elements(locations_one);
    CounterMap locations_two_counts = count_elements(locations_two);

    int similarity = 0;

    // the contribution of each element to the similarity score simplifies to
    // value * occurances in list one * occurances in list two
    for (const auto& [key, value] : locations_one_counts) {
        // check if the number is in list two
        if (locations_two_counts.find(key) != locations_two_counts.end()) {
            similarity += key * value * locations_two_counts[key];
        }
    }

    return similarity;
}

int main() {

    std::vector<int> locations_one;
    std::vector<int> locations_two;

    parse_input(locations_one, locations_two, "input.txt");

    std::cout << "Part One:\n";
    std::cout << part_one(locations_one, locations_two);
	
    std::cout << "\nPart Two:\n";
    std::cout << part_two(locations_one, locations_two);

    return 0;

}
