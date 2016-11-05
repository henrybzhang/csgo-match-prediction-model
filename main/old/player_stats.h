#ifndef player_stats_H
#define player_stats_H

#include <string>

class player_stats {
	std::string name;
    double rating;
    int number_of_games;
public:
    void reset_player_stats();
    void input_player_data(std::ifstream&);
    std::string get_player_name();
    double get_rating() const;
    void add_N_games();
    int N_played_games();
    void change_rating(double);
    bool operator<(player_stats p1) { return rating > p1.rating; }
};

#endif