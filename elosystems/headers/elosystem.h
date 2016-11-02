#ifndef Elo_system_H
#define Elo_system_H

class Elo_system {
	int total_N_matches, total_N_players;
	int player_index[2][5];
	double team_average[2];
public:
	void find_player_index();
	void find_team_average();
	void update_player_ratings();
};

#endif