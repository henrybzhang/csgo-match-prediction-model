#include "headers/match_stats.h"
#include <fstream>  
#include <iostream>

bool match_stats::input_match_data(std::ifstream& match_input) {
    if(match_input.eof()){
        return false;
    }
    match_input >> date >> hour >> map_name;

    for(int x=0; x<2; x++){
        match_input >> team_name[x] >> score[x];
    }
    team0_wins = true;
    if(score[0] < score[1]) {
        team0_wins = false;
        std::swap(team_name[0], team_name[1]);
        std::swap(score[0], score[1]);
    }
    if(team0_wins == true){
        for(int x=0; x<2; x++){
            for(int y=0; y<5; y++){
                match_input >> player_name[x][y];
                match_input >> player_kills[x][y];
                match_input >> player_headshots[x][y];
                match_input >> player_assists[x][y];
                match_input >> player_deaths[x][y];
                match_input >> player_hltv_ratings[x][y];
            }
        }
    }
    else{
        for(int x=1; x>=0; x--){
            for(int y=0; y<5; y++){
                match_input >> player_name[x][y];
                match_input >> player_kills[x][y];
                match_input >> player_headshots[x][y];
                match_input >> player_assists[x][y];
                match_input >> player_deaths[x][y];
                match_input >> player_hltv_ratings[x][y];
            }
        }
    }
    return true;
}

std::string match_stats::match_player_name(int x, int y) {
    return player_name[x][y];
}

double match_stats::get_score(int team) {
    return score[team];
}

void match_stats::output_data() {
    std::cout << date << " " << hour << " " << map_name << " ";

    for(int x=0; x<2; x++){
        std::cout << team_name[x] << " " << score[x] << " ";
    }
    if(score[0] > score[1]) team0_wins = true;
    for(int x=0; x<2; x++){
        for(int y=0; y<5; y++){
            std::cout << player_name[x][y] << " ";
            std::cout << player_kills[x][y] << " ";
            std::cout << player_headshots[x][y] << " ";
            std::cout << player_assists[x][y] << " ";
            std::cout << player_deaths[x][y] << " ";
            std::cout << player_hltv_ratings[x][y] << " ";
        }
    }
    std::cout << '\n';
}

std::string match_stats::get_map() {
    return map_name;
}