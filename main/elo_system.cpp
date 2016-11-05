#include "headers/elo_system.h"
#include "headers/player_stats.h"
#include "headers/match_stats.h"
#include "headers/input.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <string>

extern std::vector<player_stats> total_players;
extern std::vector<match_stats> total_matches;
extern input base;

std::string* player_names;

struct player_stats_new {
    std::string name;
    double rating;
    int number_of_games;
};

bool sort_by_rating (const player_stats &p1, const player_stats &p2)
{
    return p1.get_rating() > p2.get_rating();
}

void elo_system::find_player_index() {
    enough_games = true;
    bool found_player;
    for(int x=0; x<2; x++) {
        for(int y=0; y<5; y++) {
            found_player = false;
            std::string match_player_name = total_matches[match_index].match_player_name(x, y);
            int start = 0;
            int upto = total_players.size();
            while(start < upto) {
                int mid = (start + upto) / 2;
                if(match_player_name < player_names[mid]) {
                    upto = mid;
                }
                else if(match_player_name > player_names[mid]) {
                    start = mid + 1;
                }
                else{
                    player_index[x][y] = mid;
                    if(total_players[mid].N_played_games() < N_games_needed) {
                        enough_games = false;
                    }
                    found_player = true;
                    break;
                }
            }
            if(found_player == false) { // if player's name is not found in player name array
                std::cout << match_player_name << '\n';
                std::cout << "Error: could not find player '" << match_player_name << "'" << '\n';
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
    for(int x=0; x<total_players.size(); x++) {
        total_players[x].reset_player_stats();
    }
    game_counter = N_correct_predictions = mean_squared_error = 0;
}

double elo_system::get_mean(int N, double score_array[]) {
    double total = 0;
    for(int x=0; x<N; x++) total += score_array[x];
    total /= N;
    return total;
}

double elo_system::get_standard_deviation(int N, double mean, double score_array[]) {
    double total = 0;
    for(int x=0; x<N; x++) total += (mean - score_array[x]) * (mean - score_array[x]);
    total /= (N - 1);
    total = sqrt(total);
    return total;
}

double elo_system::cdf(double value) {
   return 0.5 * erfc(-value * 1 / sqrt(2));
}

void elo_system::swap(double &n1, double &n2) {
     double temp = n1;
     n1 = n2;
     n2 = temp;
}

void elo_system::test_program() {
    double standard_deviation[2], mean[2];
    for(int x=0; x<2; x++){
        double team_player_ratings[5];
        for(int y=0; y<5; y++){
            int p = player_index[x][y];
            team_player_ratings[y] = total_players[p].get_rating();
        }
        mean[x] = get_mean(5, team_player_ratings);
        standard_deviation[x] = get_standard_deviation(5, mean[x], team_player_ratings);
    }

    std::cout << total_matches[match_index].team0_wins << '\n';
    if(team_average[0] > team_average[1] && total_matches[match_index].team0_wins == true) N_correct_predictions++;
    else if(team_average[0] < team_average[1] && total_matches[match_index].team0_wins == false) N_correct_predictions++;
    game_counter++;

    //make mean[0] the winning team's mean rating
    if(total_matches[match_index].team0_wins == false){
        swap(mean[0], mean[1]);
        swap(standard_deviation[0], standard_deviation[1]);
    }

    double zscore = (mean[0] - mean[1]) / sqrt((standard_deviation[0] * standard_deviation[0]) + (standard_deviation[1] * standard_deviation[1]));
    double probability = cdf(zscore);
    mean_squared_error += (1 - probability) * (1 - probability);
}

void elo_system::output_tests() {
    std::ofstream output_tests ("../results/main_predictions.txt");
    output_tests << "Elo Constant: " << elo_constant << '\n';
    output_tests << "Predicted " << N_correct_predictions << " out of " << game_counter << " games correctly." << '\n';
    output_tests << "Percentage: " << (double) N_correct_predictions / game_counter  * 100 << '%' << '\n';
    output_tests << "Mean Squared Error: " << mean_squared_error / game_counter << '\n' << '\n';
}

void elo_system::main_program(int constant1) {
    elo_constant = (double) constant1 / 10;
    player_names = new std::string[total_players.size()];
    for(int x=0; x<total_players.size(); x++) {
        player_names[x] = total_players[x].get_player_name();
    }
    reset_program();
    for(match_index=0; match_index<total_matches.size(); match_index++) {
        find_player_index();
        find_team_average();
        if(enough_games == true) test_program();
        update_player_ratings();
    }
    output_ratings();
    output_tests();
}