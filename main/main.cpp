#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <chrono>
#include "headers/match_stats.h"
#include "headers/player_stats.h"
#include "headers/elo_system.h"
#include "headers/input.h"
using namespace std;

int N_games_needed = 30;

vector<match_stats> total_matches;
vector<player_stats> total_players;
input base;
elo_system rank_system;

int main() {
	base.input_data();
	
	for(int constant1=1; constant1<=1; constant1++) {
		rank_system.main_program(constant1);
	}
}
/*
std::chrono::time_point<std::chrono::system_clock> start, end;
start = std::chrono::system_clock::now();
end = std::chrono::system_clock::now();
std::chrono::duration<double> elapsed_seconds = end - start;
std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
*/