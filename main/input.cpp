#include "headers/input.h"
#include "headers/match_stats.h"
#include <fstream>
#include <vector>

extern std::vector<match_stats> match_list;
extern std::vector<std::string> player_name_list;

void input::input_data() {
	std::ifstream match_input ("../data/data.txt");
	match_stats match;
    while(match.input_match_data(match_input) == true) match_list.push_back(match);
    match_input.close();

    std::ifstream player_input ("../data/player_names.txt");
    std::string player;
    do{
    	player_input >> player;
    	player_name_list.push_back(player);
    }
    while(player_input.eof() == false);
    player_input.close();
}