#ifndef player_stats_H
#define player_stats_H

class player_stats {
public:
    std::string name;
    double rating;
    int number_of_games;
    void reset_program();
};

#endif