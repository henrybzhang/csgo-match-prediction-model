#include "headers/player_stats.h"

void player_stats::reset_program() {
    rating = 0.5;
    number_of_games = 0;
}

void player_stats::input_data() {
	pin >> name;
}