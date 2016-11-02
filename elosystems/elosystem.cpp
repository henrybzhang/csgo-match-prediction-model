#include "headers/elosystem.h"

bool player_stats::find_player_index(int m)
{
    enough_games = true;
    for(int x=0; x<2; x++){
        for(int y=0; y<5; y++){
            for(int p=0; p<total_N_players; p++){
                if(player[p].name == match[m].player_name[x][y]){
                    player_index[x][y] = p;
                    if(player[p].number_of_games < N_games_needed){
                        enough_games = false;
                    }
                    break;
                }
                if(p == total_N_players - 1){ // if player's name is not found in player name array
                    cout << match[m].player_name[x][y] << endl;
                    cout << "Error: could not find player '" << match[m].player_name[x][y] << "'" << endl;
                    cout << "Match Number: " << m + 1 << endl;
                    return false;
                }
            }
        }
    }
    return true;
}