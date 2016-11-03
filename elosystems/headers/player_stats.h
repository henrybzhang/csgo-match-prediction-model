#ifndef player_stats_H
#define player_stats_H

#include <string>

class player_stats {
	std::string name;
    double rating;
    int number_of_games;
public:
    void reset_program();
    void input_data();
    std::string get_player_name();
    double get_rating(int);
    void add_N_games();
    void change_rating(double);
};

#endif