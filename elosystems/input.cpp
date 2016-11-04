#include "headers/input.h"
#include "headers/match_stats.h"
#include "headers/player_stats.h"
#include <fstream>
#include <vector>

void input::input_data() {
	extern std::vector<match_stats> total_matches;
	std::ifstream match_input ("../data/data.txt");
	match_input >> total_N_matches;
	match_stats match;
    for(int x=0; x<total_N_matches; x++){
    	match.input_match_data(match_input);
    	total_matches.push_back(match);
    }
    match_input.close();

    extern std::vector<player_stats> total_players;
    std::ifstream player_input ("../data/player_names.txt");
    player_input >> total_N_players;
    player_stats player;
    for(int x=0; x<total_N_players; x++){
    	player.input_player_data(player_input);
    	total_players.push_back(player);
    }
    player_input.close();
}