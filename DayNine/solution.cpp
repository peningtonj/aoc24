#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <map>
#include <set>
#include <ranges>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <functional> // for std::bind

int GAP = -1;

class FileSystem {
    public:
        std::vector<int> chunks;
        std::vector<int> file_ids;
        int end_space;

    FileSystem(std::string input_file) {
        std::ifstream MyReadFile(input_file);

        std::string line;
        std::getline(MyReadFile, line);

        bool is_file = true;
        int file_id = 0;
        for (char c: line) {
            chunks.push_back(std::stoi(std::string(1, c)));
            if (is_file) {
                file_ids.push_back(file_id);
                file_id += 1;
            } else {
                file_ids.push_back(GAP);
            }
            is_file = !is_file;
        }
        if (!is_file) {
        chunks.push_back(0);
        file_ids.push_back(GAP);
        }
        // clear_print();
        // Close the file
        MyReadFile.close();
        }

    void clear_print() {
        for (int i = 0; i < file_ids.size(); i++) {
            for (int j = 1; j <= chunks.at(i); j++) {
                std::string p;
                if (file_ids.at(i) == -1) {
                    p = ".";
                } else {
                    p = std::to_string(file_ids.at(i));
                }
                std::cout << p;
            }
        }
        std::cout << "\n";
    }


    void process_filesystem() {
        auto next_gap = std::find(file_ids.begin(), file_ids.end(), GAP);

        while ((next_gap != file_ids.end() - 1) and (next_gap != file_ids.end())) {
            move_chunk();
            next_gap = std::find(file_ids.begin(), file_ids.end(), GAP);
            // clear_print();
        }
    }

    void move_chunk() {
        // Get the file size at -2
        chunks.pop_back();
        int file_size = chunks.at(chunks.size() - 1);
        chunks.pop_back();

        // Get the file id at -2
        file_ids.pop_back();
        int last_id = file_ids.at(file_ids.size() - 1);
        // std::cout << "I'm going to move the file at the end " << last_id << " with size " << file_size << "\n";
        file_ids.pop_back();


        auto next_gap = file_ids.begin();
        int gap_size;

        while (file_size) {
            next_gap = std::find(file_ids.begin(), file_ids.end(), GAP);
            auto gap_index = std::distance(file_ids.begin(), next_gap); // Calculate the index
            if (next_gap == file_ids.end()) {
                chunks.at(chunks.size() - 1) += file_size;
                break;
            }
            gap_size = chunks.at(gap_index);
            // std::cout << "The next gap is at index " << gap_index << " in the fs with size " << gap_size << "\n";

            if (gap_size == file_size) {
                file_ids.at(gap_index) = last_id;
                file_size -= gap_size;
            } else if (gap_size < file_size) {
                file_ids.at(gap_index) = last_id;
                file_size -= gap_size;
            } else {
                file_ids.at(gap_index) = last_id;
                gap_size -= file_size;

                chunks.at(gap_index) = file_size;
                chunks.insert(chunks.begin() + gap_index + 1, gap_size); // Use the correct index
                file_ids.at(gap_index) = last_id;                     // Adjust file_ids accordingly
                file_ids.insert(next_gap + 1, GAP); 
                
                file_size = 0;
                // Adjust file_ids accordingly
            }
        }
    }

    void move_chunk_together(int file_id_idx) {
        int file_size = chunks.at(file_id_idx);
        int file_id = file_ids.at(file_id_idx);
        auto next_gap = file_ids.begin();
        next_gap = std::find(next_gap + 1, file_ids.end(), GAP);

        while (next_gap < (file_ids.begin() + file_id_idx)) {
            auto gap_index = std::distance(file_ids.begin(), next_gap);
            int gap_size = chunks.at(gap_index);
            if (gap_size >= file_size) {
                file_ids.at(file_id_idx) = GAP;
                file_ids.at(gap_index) = file_id;
                chunks.at(gap_index) = file_size;
                file_ids.insert(next_gap + 1, GAP); 
                chunks.insert(chunks.begin() + gap_index + 1, gap_size - file_size);
                break;
            }
            
            next_gap = std::find(next_gap + 1, file_ids.end(), GAP);

        }
    }

    void combine_gaps() {
        for (int i = 0; i < file_ids.size() - 1; i++) {
            if ((file_ids.at(i) == GAP) and (file_ids.at(i + 1) == GAP)) {
                chunks.at(i) += chunks.at(i + 1);
                file_ids.erase(file_ids.begin() + i + 1);
                chunks.erase(chunks.begin() + i + 1);
            }
        }
    }

    void process_filesystem_chunked() {
        for (int i = file_ids.size() - 1; i > 0; i--) {
            // clear_print();

            if (file_ids.at(i) != GAP) {
                // std::cout << "Moving file " << file_ids.at(i) << "\n";
                move_chunk_together(i);
                combine_gaps();
            }
        }
        
    }

    long long checksum() {
        long long result = 0;
        int idx = 0;
        for (int i = 0; i < file_ids.size(); i++) {
            if (file_ids.at(i) == GAP) {
                idx += chunks.at(i);
                continue;
            }
            for (int j = 0; j <= chunks.at(i) - 1; j++) {
                result += file_ids.at(i) * idx;
                idx += 1;
            }
        }

        return result;
    }
};

long long part_one(FileSystem fs) {
    fs.process_filesystem();
    return fs.checksum();
}

long long part_two(FileSystem fs) {
    fs.process_filesystem_chunked();
    return fs.checksum();
}

int main() {

    FileSystem fs = FileSystem("input.txt");

    std::cout << "Part One:\n";
    std::cout << part_one(fs);
	
    std::cout << "\nPart Two:\n";
    std::cout << part_two(fs);

    return 0;

}
