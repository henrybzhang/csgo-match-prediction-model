#include "headers/match_stats.h"
#include <fstream>  

void match_stats::input_data() {
    fin >> date >> hour >> map_name;
    for(int x=0; x<2; x++){
        fin >> team_name[x] >> score[x];
    }
    if(score[0] > score[1]) team0_wins = true;
    for(int x=0; x<2; x++){
        for(int y=0; y<5; y++){
            fin >> player_name[x][y];
            fin >> player_kills[x][y];
            fin >> player_headshots[x][y];
            fin >> player_assists[x][y];
            fin >> player_deaths[x][y];
            fin >> player_hltv_ratings[x][y];
        }
    }
    fin.close();
}