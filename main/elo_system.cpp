#include "headers/elo_system.h"
#include "headers/player_stats.h"
#include "headers/match_stats.h"
#include "headers/input.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

extern int N_games_needed;
extern std::vector<player_stats> total_players;
extern std::vector<match_stats> total_matches;
extern input base;

bool sort_by_rating (const player_stats &p1, const player_stats &p2)
{
    return p1.get_rating() > p2.get_rating();
}

void elo_system::find_player_index() {
    enough_games = true;
    int total_N_players = base.get_N_players();
    bool found_player;
    for(int x=0; x<2; x++) {
        for(int y=0; y<5; y++) {
            found_player = false;
            for(int p=0; p<total_N_players; p++) {
                if(total_players[p].get_player_name() == total_matches[match_index].match_player_name(x, y)) {
                    player_index[x][y] = p;
                    if(total_players[p].N_played_games() < N_games_needed){
                        enough_games = false;
                    }
                    found_player = true;
                    break;
                }
            }
            if(found_player == false) { // if player's name is not found in player name array
                std::cout << total_matches[match_index].match_player_name(x, y) << '\n';
                std::cout << "Error: could not find player '" << total_matches[match_index].match_player_name(x, y) << "'" << '\n';
                std::cout << "Match Number: " << match_index + 1 << '\n';
            }
        }
    }
}

void elo_system::find_team_average() {
    for(int x=0; x<2; x++) {
        team_average[x] = 0;
        for(int y=0; y<5; y++) team_average[x] += total_players[player_index[x][y]].get_rating();
        team_average[x] /= 5;
    }
}

void elo_system::update_player_ratings() {
    for(int x=0; x<2; x++) {
        int counter = 1 - x;

        double percentage_rounds_won = total_matches[match_index].get_score(x) / (double)(total_matches[match_index].get_score(0) + total_matches[match_index].get_score(1));
        double base_score = percentage_rounds_won;

        // percentage * other team's rating average
        double weighted_score = team_average[counter] * base_score;

        // update player ratings
        for(int y=0; y<5; y++) {
            int p = player_index[x][y];
            total_players[p].add_N_games();
            double expected_score = 1 / (1 + exp(team_average[counter] - total_players[p].get_rating()));
            total_players[p].change_rating(elo_constant * (base_score - expected_score));
        }
    }
}

void elo_system::main_program(int constant1) {
    elo_constant = (double) constant1 / 10;
    reset_program();
    for(match_index=0; match_index<base.get_N_matches(); match_index++) {
        find_player_index();
        find_team_average();
        update_player_ratings();
    }
    output_ratings();
}

void elo_system::output_ratings() {
    std::ofstream output_ratings ("../results/main_player_stats.txt");
    // get rid of players with less than the necessary number of games
    for(int x=0; x<total_players.size(); x++) {
        if(total_players[x].N_played_games() < N_games_needed) {
            total_players.erase(total_players.begin() + x);
            x--;
        }
    }
    std::sort(total_players.begin(), total_players.end());

    output_ratings << total_players.size() << '\n';
    for(int p=0; p<total_players.size(); p++) output_ratings << total_players[p].get_player_name() << " " << total_players[p].get_rating() << '\n';
}

void elo_system::reset_program() {
    for(int x=0; x<base.get_N_players(); x++){
        total_players[x].reset_player_stats();
    }
}