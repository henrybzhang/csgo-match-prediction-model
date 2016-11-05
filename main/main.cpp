#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <chrono>
#include "headers/match_stats.h"
#include "headers/map_elo_system.h"
#include "headers/input.h"
using namespace std;

vector<match_stats> match_list;
vector<string> player_name_list;

input base;

int main() {
	base.input_data();
	
	chrono::time_point<std::chrono::system_clock> start, end;
	start = chrono::system_clock::now();

	//empty files
	ofstream output_tests ("../results/main_predictions.txt");
	output_tests.close();

	for(int constant1=1; constant1<=10; constant1++){
		for(int constant2=1; constant2<=10; constant2++) {
			map_elo_system rank_system(constant1, constant2);
			rank_system.main_program();
		}
	}

	end = chrono::system_clock::now();
	chrono::duration<double> elapsed_seconds = end - start;
	cout << "Elapsed Time: " << elapsed_seconds.count() << "s\n";
}
/*
std::chrono::time_point<std::chrono::system_clock> start, end;
start = std::chrono::system_clock::now();
end = std::chrono::system_clock::now();
std::chrono::duration<double> elapsed_seconds = end - start;
std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
*/