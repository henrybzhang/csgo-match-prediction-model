#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include "headers/match_stats.h"
#include "headers/player_stats.h"
//#include "headers/elo_system.h"
#include "headers/input.h"
using namespace std;

const int N_games_needed = 30;

vector<match_stats> total_matches;
vector<player_stats> total_players;

int main() {
	input start;
	start.input_data();
}