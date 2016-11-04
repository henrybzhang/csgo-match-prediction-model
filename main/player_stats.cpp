#include "headers/player_stats.h"
#include <fstream>
#include <iostream>
#include <string>

void player_stats::reset_player_stats() {
    rating = 0.5;
    number_of_games = 0;
}

void player_stats::input_player_data(std::ifstream& player_input) {
	player_input >> name;
}

std::string player_stats::get_player_name() {
	return name;
}

double player_stats::get_rating() const{
	return rating;
}

void player_stats::add_N_games() {
	number_of_games++;
}

int player_stats::N_played_games() {
	return number_of_games;
}

void player_stats::change_rating(double change) {
	rating += change;
	//std::cout << rating << '\n';
}