#ifndef elo_system_H
#define elo_system_H

#include "player_stats.h"

class elo_system {
	const int N_games_needed = 30, N_games_needed_for_map = 4;
	bool enough_games;
	int player_index[2][5];	
	double team_average[2], team_map_average[2];
	double elo_constant;
	int match_index, map_index;
	std::string* player_names;
	int game_counter, N_correct_predictions;
	double mean_squared_error;

	bool sort_by_rating(const player_stats, const player_stats);
	void find_player_index();
	void find_team_average();
	void update_player_ratings();
	void output_ratings();
	void reset_program();

	double get_mean(int, double[]);
	double get_standard_deviation(int, double, double[]);
	double cdf(double);
	void swap(double&, double&);
	void test_program();
	void output_tests();
public:
	void main_program(int);
};

#endif