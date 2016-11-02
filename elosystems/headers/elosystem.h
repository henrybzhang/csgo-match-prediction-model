#ifndef elo_system_H
#define elo_system_H

class elo_system {
	bool enough_games;
	int player_index[2][5];
	double team_average[2];
public:
	void find_player_index(int);
	void find_team_average();
	void update_player_ratings();
};

#endif