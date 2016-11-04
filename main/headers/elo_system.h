#ifndef elo_system_H
#define elo_system_H

#include "player_stats.h"

class elo_system {
	bool enough_games;
	int player_index[2][5];	
	double team_average[2];
	double elo_constant;
	int match_index;
	bool sort_by_rating(const player_stats, const player_stats);
public:
	void find_player_index();
	void find_team_average();
	void update_player_ratings();
	void main_program(int);
	void output_ratings();
	void reset_program();
};

#endif