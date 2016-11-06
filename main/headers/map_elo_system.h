#ifndef map_elo_system_H
#define map_elo_system_H

#include <string>
#include <vector>

class map_elo_system {
	double elo_constant, map_constant;
	int N_games_needed = 30, N_games_needed_for_map = 4;
	bool enough_games;
	int player_index[2][5];	
	double team_average[2], team_map_average[2];
	int match_index, map_index;
	int game_counter[8], N_correct_predictions[8];
	double mean_squared_error[8];
	std::string map_list[8];

	void find_player_index();
	bool find_map_index();
	void find_team_average();
	void find_team_map_average();
	void update_player_ratings();
	void output_ratings();

	double get_mean(int, double[]);
	double get_standard_deviation(int, double, double[]);
	double cdf(double);
	void swap(double&, double&);
	void test_program();
	void output_tests();
public:
	void main_program();
	map_elo_system(int, int);
};

#endif