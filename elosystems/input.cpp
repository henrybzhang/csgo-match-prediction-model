#include "headers/input.h"
#include <fstream>

void input::input_data() {
	std::ifstream fin ("../data/data.txt");
	fin >> total_N_matches;
    match = new match_stats[total_N_matches];
    for(int x=0; x<total_N_matches; x++){
    	match[x].input_data(fin);
    }
}