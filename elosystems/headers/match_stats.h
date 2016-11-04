#ifndef match_stats_H
#define match_stats_H

#include <string>

class match_stats {
	int date, hour;
    int score[2], player_kills[2][5], player_headshots[2][5], player_assists[2][5], player_deaths[2][5];
    std::string map_name, player_name[2][5], team_name[2];
    double player_hltv_ratings[2][5];
public:
    bool team0_wins;
    void input_match_data(std::ifstream&);
    std::string get_player_name(int, int);
    double get_score(int);
    void output_data();
};

#endif