#include "headers/elo_system.h"

void elo_system::find_player_index() {
    enough_games = true;
    for(int x=0; x<2; x++){
        for(int y=0; y<5; y++){
            for(int p=0; p<total_N_players; p++){
                if(player[p].get_player_name(p) == match[m].get_player_name(x, y)){
                    player_index[x][y] = p;
                    if(player[p].number_of_games < N_games_needed){
                        enough_games = false;
                    }
                    break;
                }
                if(p == total_N_players - 1){ // if player's name is not found in player name array
                    cout << match[match_index].get_player_name(x, y) << endl;
                    cout << "Error: could not find player '" << match[match_index].get_player_name(x, y) << "'" << endl;
                    cout << "Match Number: " << match_index + 1 << endl;
                    return false;
                }
            }
        }
    }
    return true;
}

void elosystem::find_team_average() {
    for(int x=0; x<2; x++){
        team_average[x] = 0;
        for(int y=0; y<5; y++) team_average[x] += player[player_index[x][y]].get_rating();
        team_average[x] /= 5;
    }
}

void elosystem::update_player_ratings() {
    for(int x=0; x<2; x++){
        int counter = 1 - x;

        double percentage_rounds_won = match[match_index].get_score(x) / (double)(match[match_index].get_score(0) + match[match_index].get_score(1));
        double base_score = percentage_rounds_won;

        // percentage * other team's rating average
        double weighted_score = team_average[counter] * base_score;

        // update player ratings
        for(int y=0; y<5; y++){
            int p = player_index[x][y];
            player[p].add_N_games();
            double expected_score = 1 / (1 + exp(team_average[counter] - player[p].get_rating()));
            player[p].change_rating(elo_constant * (base_score - expected_score));
        }
    }
}