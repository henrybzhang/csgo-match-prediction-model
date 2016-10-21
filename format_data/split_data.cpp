// implement HGA from http://www.matterofstats.com/mafl-stats-journal/2013/10/13/building-your-own-team-rating-system.html
#include <fstream>
#include <algorithm> // sort
#include <cmath> // math functions
#include <iostream>
using namespace std;

ofstream fout ("test.txt");

/*
0 dust2
1 mirage
2 inferno
3 cache
4 overpass
5 cobblestone
6 train
7 nuke
*/

int total_matches, total_players;
int pn[2][5]; //store position of player in player array
double team_average[2]; // average rating team 1 and 2
int N_of_recents = 30; // # of recent ratings used to predict game outcome

bool overNgames, could_not_find_player;

int game_counter, map_number;

struct match1
{
    int date, score[2];
    string mapp, mplayer[2][5], team[2];
};
match1 match[10000];

struct player1
{
    string name;
    double rating, weighted_ratings[8][100], actual_ratings[1000];
    int game_date[1000], gnumb, mapn[8];
};
player1 player[5000];

void input_data()
{
    ifstream fin ("sorted_total_data.txt"); //input match data
    fin >> total_matches;
    for(int m=0; m<total_matches; m++){
        fin >> match[m].date >> match[m].mapp;

        fin >> match[m].team[0] >> match[m].score[0];
        for(int y=0; y<5; y++) fin >> match[m].mplayer[0][y];

        fin >> match[m].team[1] >> match[m].score[1];
        for(int y=0; y<5; y++) fin >> match[m].mplayer[1][y];
    }
    fin.close();

    ifstream pin ("player_names.txt"); //input player names
    pin >> total_players;
    for(int m=0; m<total_players; m++) pin >> player[m].name;
    pin.close();
}

void reset_program()
{
    for(int x=0; x<total_players; x++){
        player[x].rating = 0.5;
        player[x].gnumb = 0;
    }
    game_counter = 0;
}

void find_player_position(int match_number)
{
    overNgames = true;
    could_not_find_player = false;
    for(int x=0; x<2; x++){
        for(int y=0; y<5; y++){
            for(int p=0; p<total_players; p++){
                if(player[p].name == match[match_number].mplayer[x][y]){
                    pn[x][y] = p;
                    if(player[p].gnumb < N_of_recents) overNgames = false;
                    break;
                }
                if(p == total_players - 1){ // if player's name is not found in player name array
                    fout << match[match_number].mplayer[x][y] << endl;
                    could_not_find_player = true;
                    fout << "Error: could not find player (" << match[match_number].mplayer[x][y] << ")" << endl;
                    fout << "Match Number: " << match_number + 1 << endl;
                    return;
                }
            }
        }
    }
}

void find_team_average()
{
    for(int x=0; x<2; x++){
        team_average[x] = 0;
        for(int y=0; y<5; y++){
            team_average[x] += player[pn[x][y]].rating;
        }
        team_average[x] /= 5;
    }
}

void which_map(int match_number)
{
    if(match[match_number].mapp == "dust2") map_number = 0;
    if(match[match_number].mapp == "mirage") map_number = 1;
    if(match[match_number].mapp == "inferno") map_number = 2;
    if(match[match_number].mapp == "cache") map_number = 3;
    if(match[match_number].mapp == "overpass") map_number = 4;
    if(match[match_number].mapp == "cobblestone") map_number = 5;
    if(match[match_number].mapp == "train") map_number = 6;
    if(match[match_number].mapp == "nuke") map_number = 7;
}

void update_player_ratings(int match_number, double k)
{
    for(int x=0; x<2; x++){
        int counter = 1 - x;

        // percentage of rounds won + base of winning or losing
        double percentage_won = match[match_number].score[x] / (double)(match[match_number].score[0] + match[match_number].score[1]);
        double actual_score = percentage_won;
        
        // percentage x enemy team average rating
        double weighted_score = team_average[counter] * actual_score;

        // update each player's rating in both teams
        for(int y=0; y<5; y++){
            player[pn[x][y]].weighted_ratings[map_number][player[pn[x][y]].mapn[map_number]] = weighted_score;
            player[pn[x][y]].mapn[map_number]++;
            player[pn[x][y]].actual_ratings[player[pn[x][y]].gnumb] = actual_score;
            player[pn[x][y]].game_date[player[pn[x][y]].gnumb] = match[match_number].date;
            player[pn[x][y]].gnumb++;
            double expected_score = 1 / (1 + exp(team_average[counter] - player[pn[x][y]].rating));
            player[pn[x][y]].rating += k * (actual_score - expected_score);
        }
    }
}

bool sort_by_rating(const player1 &p1, const player1 &p2)
{
    return p1.rating>p2.rating;
}

void output_data()
{
    ofstream output_results ("player_stats.txt");
    int subtract = 0;
    for(int x=0; x<total_players; x++){ // get rid of all players who have less games needed to predict outcomes
        if(player[x].gnumb < N_of_recents){
            player[x].rating = 0;
            subtract++;
        }
    }
    sort(player, player + total_players, sort_by_rating);
    total_players -= subtract;

    //fout << total_players << endl;
    for(int p=0; p<1; p++){
        //fout << player[p].name << endl;
        p = 1;
        fout << "map score" << endl;
        for(int x=0; x<8; x++){
            for(int y=player[p].mapn[x] - 1; y>player[p].mapn[x] - 10; y--){
                if(player[p].weighted_ratings[x][y] == 0) continue;
                fout << x << " " << player[p].weighted_ratings[x][y] << endl;
            }
        }
    }
}

int main()
{
    double constant = 0.1;
    input_data();
    //for(double constant=0.1; constant<=1; constant += 0.1){
        reset_program();
        for(int m=0; m<total_matches; m++){
            find_player_position(m);
            if(could_not_find_player == true) return 0;
            which_map(m);
            find_team_average();
            update_player_ratings(m, constant);
        }
        output_data();
    //}
}
