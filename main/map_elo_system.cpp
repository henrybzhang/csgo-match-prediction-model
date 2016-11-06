#include "headers/map_elo_system.h"
#include "headers/match_stats.h"
#include "headers/input.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <string>

extern std::vector<match_stats> match_list;
extern std::vector<std::string> player_name_list;
extern input base;

struct player_stats {
    std::string name;
    double rating, map_rating[8];
    int number_of_games, map_number_of_games[8];
};

std::vector<player_stats> player_stats_list;

bool sort_by_rating(const player_stats &p1, const player_stats &p2)
{
    return p1.rating > p2.rating;
}

void map_elo_system::find_player_index() {
    enough_games = true;
    bool found_player;
    for(int x=0; x<2; x++) {
        for(int y=0; y<5; y++) {
            found_player = false;
            std::string match_player_name = match_list[match_index].match_player_name(x, y);
            int start = 0;
            int upto = player_stats_list.size();
            while(start < upto) {
                int mid = (start + upto) / 2;
                if(match_player_name < player_stats_list[mid].name) {
                    upto = mid;
                }
                else if(match_player_name > player_stats_list[mid].name) {
                    start = mid + 1;
                }
                else{
                    player_index[x][y] = mid;
                    if(player_stats_list[mid].number_of_games < N_games_needed) {
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

bool map_elo_system::find_map_index() {
    for(map_index=0; map_index<8; map_index++) {
        if(match_list[match_index].get_map() == map_list[map_index]) break;
    }
    if(map_index == 8) {
        std::cout << "Error: Could not find map '" << match_list[match_index].get_map()  << "'" << '\n';
        std::cout << "Match Number: " << match_index + 1 << '\n';
        return false;
    }
    return true;
}

void map_elo_system::find_team_average() {
    for(int x=0; x<2; x++) {
        team_average[x] = 0;
        for(int y=0; y<5; y++) team_average[x] += player_stats_list[player_index[x][y]].rating;
        team_average[x] /= 5;
    }
}

void map_elo_system::find_team_map_average()
{
    for(int x=0; x<2; x++){
        team_map_average[x] = 0;
        for(int y=0; y<5; y++){
            int p = player_index[x][y];
            team_map_average[x] += (player_stats_list[p].map_rating[map_index] * map_constant + player_stats_list[p].rating * (1 - map_constant));
        }
        team_map_average[x] /= 5;
    }
}

void map_elo_system::update_player_ratings() {
    for(int x=0; x<2; x++) {
        int counter = 1 - x;

        double percentage_rounds_won = match_list[match_index].get_score(x) / (double)(match_list[match_index].get_score(0) + match_list[match_index].get_score(1));
        double base_score = percentage_rounds_won;

        // percentage * other team's rating average
        double weighted_score = team_average[counter] * base_score;
        double map_weighted_score = team_map_average[counter] * base_score;

        // update player ratings
        for(int y=0; y<5; y++) {
            int p = player_index[x][y];

            player_stats_list[p].number_of_games++;
            //std::cout << player_stats_list[p].number_of_games << '\n';
            player_stats_list[p].map_number_of_games[map_index]++;

            double expected_score = 1 / (1 + exp(team_average[counter] - player_stats_list[p].rating));
            player_stats_list[p].rating += elo_constant * (base_score - expected_score);
            
            double map_expected_score = 1 / (1 + exp(team_map_average[counter] - (player_stats_list[p].map_rating[map_index] * map_constant + player_stats_list[p].rating * (1 - map_constant))));
            player_stats_list[p].map_rating[map_index] += elo_constant * (base_score - expected_score);
        }
    }
}

void map_elo_system::output_ratings() {
    std::ofstream output_ratings ("../results/main_player_stats.txt");
    // get rid of players with less than the necessary number of games
    for(int x=0; x<player_stats_list.size(); x++) {
        if(player_stats_list[x].number_of_games < N_games_needed) {
            //std::cout << player_stats_list[x].number_of_games << '\n';
            player_stats_list.erase(player_stats_list.begin() + x);
            x--;
        }
    }
    std::sort(player_stats_list.begin(), player_stats_list.end(), sort_by_rating);

    output_ratings << player_stats_list.size() << '\n';
    for(int p=0; p<player_stats_list.size(); p++) output_ratings << player_stats_list[p].name << " " << player_stats_list[p].rating << '\n';
    output_ratings.close();
}

double map_elo_system::get_mean(int N, double score_array[]) {
    double total = 0;
    for(int x=0; x<N; x++) total += score_array[x];
    total /= N;
    return total;
}

double map_elo_system::get_standard_deviation(int N, double mean, double score_array[]) {
    double total = 0;
    for(int x=0; x<N; x++) total += (mean - score_array[x]) * (mean - score_array[x]);
    total /= (N - 1);
    total = sqrt(total);
    return total;
}

double map_elo_system::cdf(double value) {
   return 0.5 * erfc(-value * 1 / sqrt(2));
}

void map_elo_system::swap(double &n1, double &n2) {
     double temp = n1;
     n1 = n2;
     n2 = temp;
}

void map_elo_system::test_program() {
    double standard_deviation[2], mean[2];
    for(int x=0; x<2; x++){
        double team_player_ratings[5];
        for(int y=0; y<5; y++){
            int p = player_index[x][y];
            team_player_ratings[y] = player_stats_list[p].map_rating[map_index] * map_constant + player_stats_list[p].rating * (1 - map_constant);
        }
        mean[x] = get_mean(5, team_player_ratings);
        standard_deviation[x] = get_standard_deviation(5, mean[x], team_player_ratings);
    }
    if((mean[0] > mean[1] && match_list[match_index].team0_wins == true) || mean[0] < mean[1] && match_list[match_index].team0_wins == false){
        N_correct_predictions[map_index]++;
    }
    game_counter[map_index]++;

    //make mean[0] the winning team's mean rating
    if(match_list[match_index].team0_wins == false){
        swap(mean[0], mean[1]);
        swap(standard_deviation[0], standard_deviation[1]);
    }

    double zscore = (mean[0] - mean[1]) / sqrt((standard_deviation[0] * standard_deviation[0]) + (standard_deviation[1] * standard_deviation[1]));
    double probability = cdf(zscore);
    mean_squared_error[map_index] += (1 - probability) * (1 - probability);
}

void map_elo_system::output_tests() {
    std::ofstream output_tests ("../results/main_predictions.txt", std::ofstream::app);
    output_tests << "Elo Constant: " << elo_constant << '\n';
    output_tests << "Map Constant: " << map_constant << '\n' << '\n';
    for(int x=0; x<8; x++){
        output_tests << map_list[x] << '\n';
        output_tests << "Predicted " << N_correct_predictions[x] << " out of " << game_counter[x] << " games correctly." << '\n';
        output_tests << "Percentage: " << (double) N_correct_predictions[x] / game_counter[x]  * 100 << '%' << '\n';
        output_tests << "Mean Squared Error: " << mean_squared_error[x] / game_counter[x] << '\n' << '\n';
    }
}

void map_elo_system::main_program() {
    for(match_index=0; match_index<match_list.size(); match_index++) {
        find_player_index();
        find_team_average();
        find_team_map_average();
        if(find_map_index() == false) continue;
        if(enough_games == true) test_program();
        update_player_ratings();
    }
    output_ratings();
    output_tests();
}

map_elo_system::map_elo_system(int constant1, int constant2) {
    elo_constant = (double) constant1 / 10;
    map_constant = (double) constant2 / 10;

    player_stats_list.clear();
    player_stats player;
    for(int x=0; x<player_name_list.size(); x++) {
        player.name = player_name_list[x];
        player.rating = 0.5;
        player.number_of_games = 0;
        for(int y=0; y<8; y++){
            player.map_rating[y] = 0.5;
            player.map_number_of_games[y] = 0;
        }
        player_stats_list.push_back(player);
    }

    for(int x=0; x<8; x++) {
        game_counter[x] = N_correct_predictions[x] = 0;
        mean_squared_error[x] = 0;
    }

    map_list[0] = "dust2";
    map_list[1] = "mirage";
    map_list[2] = "inferno";
    map_list[3] = "cache";
    map_list[4] = "overpass";
    map_list[5] = "cobblestone";
    map_list[6] = "train";
    map_list[7] = "nuke";
}