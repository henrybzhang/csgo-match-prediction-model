#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <chrono>
#include <thread>
#include "headers/match_stats.h"
#include "headers/map_elo_system.h"
#include "headers/input.h"
using namespace std;

vector<match_stats> match_list;
vector<string> player_name_list;
vector<map_elo_system> rank_system_list;

const int N_threads = 8;

input base;

void create_rank_system(int constant1, int constant2, int index) {
	map_elo_system rank_system(constant1, constant2);
	rank_system_list.push_back(rank_system);
	//rank_system.main_program();
	rank_system_list.erase(rank_system_list.begin() + index);
}

int main() {
	base.input_data();
	
	chrono::time_point<std::chrono::system_clock> start, end;
	start = chrono::system_clock::now();

	//empty files
	ofstream output_tests ("../results/main_predictions.txt");
	output_tests.close();

	thread threads[N_threads];
	int thread_index = 0;

	for(int constant1=1; constant1<=1; constant1++) {
		for(int constant2=1; constant2<=7; constant2++) {
			if(thread_index < 8) {
				threads[thread_index] = thread(create_rank_system, constant1, constant2, thread_index);
				thread_index++;
				continue;
			}
			for(int x=0; x<8; x++) threads[x].join();
			thread_index = 0;
		}
	}
	for(int x=0; x<thread_index; x++) threads[x].join();

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