/*
This sorts the data by date and reformats the data based on stats wanted
*/
#include <fstream>
#include <algorithm>
using namespace std;

int total_matches;

struct match_stats
{
    int date, hour, home_score, away_score;
    string map_name, match_player[2][5], team[2][5], home_team, away_team;
    int kills[2][5], headshots[2][5], assists[2][5], deaths[2][5];
    double hltv_rating[2][5];
};
match_stats* match;

string team;

bool sort_by_date(const match_stats &player1, const match_stats &player2)
{
    return player1.date<player2.date;
}

int main()
{
    ifstream fin ("data.txt");
    fin >> total_matches;
    match = new match_stats[total_matches];

    for(int m=0; m<total_matches; m++){

        fin >> match[m].date >> match[m].hour >> match[m].map_name;

        //place winning team as home_team
        fin >> team[0] >> score[0] >> team[1] >> score[1];
        if(score[0] > score[1]){
            match[m].home_team = team[0];
            match[m].home_score = score[0];

            match[m].away_team = team[1];
            match[m].away_score = score[1];
        }
        else{
            match[m].home_team = team[1];
            match[m].home_score = score[1];

            match[m].away_team = team[0];
            match[m].away_score = score[0];
        }

        for(int x=0; x<2; x++){
            int other_x;
            if(x == 0) other_x = 1;
            else other_x = 0;
            for(int y=0; y<5; y++){
                if(match[m].home_team == team[0]){
                    fin >> match[m].match_player[x][y];
                    fin >> match[m].team[x][y];
                    fin >> match[m].kills[x][y];
                    fin >> match[m].headshots[x][y]
                    fin >> match[m].assists[x][y];
                    fin >> match[m].deaths[x][y]
                    fin >> match[m].hltv_rating[x][y]
                }
                else{
                    fin >> match[m].match_player[other_x][y];
                    fin >> match[m].team[other_x][y];
                    fin >> match[m].kills[other_x][y];
                    fin >> match[m].headshots[other_x][y]
                    fin >> match[m].assists[other_x][y];
                    fin >> match[m].deaths[other_x][y]
                    fin >> match[m].hltv_rating[other_x][y]
                }
            }
        }

        // get rid of nuke maps and teams that have the same name and players less than 5
        if(match[m].home_team == match[m].away_team || match[m].map_name == "nuke" || counter1 <= 4 || counter2 <= 4 || checkp == true){
            m--;
            total_matches--;
        }
    }

    sort(match, match + total_matches, sort_by_date);

    ofstream fout ("/data_files/sorted_total_data.txt");
    fout << total_matches << endl;
    for(int m=0; m<total_matches; m++){
        fout << match[m].date << " ";
        fout << match[m].map_name << " ";

        fout << match[m].home_team << " " << match[m].home_score << " ";
        for(int x=0; x<5; x++){
            fout << match[m].match_player[0][x] << " ";
            fout << match[m].hltv_rating[0][x] << " ";
        }

        fout << match[m].away_team << " " << match[m].away_score << " ";
        for(int x=0; x<5; x++){
            fout << match[m].match_player[1][x] << " ";
            fout << match[m].hltv_rating[1][x] << " ";
        }
        fout << endl;
    }
}
