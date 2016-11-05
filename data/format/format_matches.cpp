#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

int total_N_matches;

struct match_stats
{
    int date, hour;
    int score[2], player_kills[2][5], player_headshots[2][5], player_assists[2][5], player_deaths[2][5];
    string map_name, player_name[2][5], team_name[2], player_team[2][5];
    double player_hltv_ratings[2][5];
};

match_stats* match;

bool sort_by_date(const match_stats &match1, const match_stats &match2)
{
    return match1.date < match2.date;
}

int main()
{
	ifstream fin ("../raw_data.txt");
	fin >> total_N_matches;
	match = new match_stats[total_N_matches];
	bool corrupt_data;
	for(int m=0; m<total_N_matches; m++){
		corrupt_data = false;
		fin >> match[m].date >> match[m].hour >> match[m].map_name;
		match[m].map_name = match[m].map_name.substr(0, match[m].map_name.find('_', 0));
		fin >> match[m].team_name[0] >> match[m].score[0] >> match[m].team_name[1] >> match[m].score[1];
		int team_N = 0, team_N_players[2] = {0, 0};
		for(int x=0; x<10; x++){
			string temp_player_name, temp_team_name;
			fin >> temp_player_name >> temp_team_name;
			if(temp_team_name == match[m].team_name[0]){
				team_N = 0;
				if(team_N_players[team_N] == 5){
					corrupt_data = true;
					team_N = 1; // move the player to the other team before deleting the match
				}
			}
			if(temp_team_name == match[m].team_name[1]){
				team_N = 1;
				if(team_N_players[team_N] == 5){
					corrupt_data = true;
					team_N = 0; // move the player to the other team before deleting the match
				}
			}
			match[m].player_name[team_N][team_N_players[team_N]] = temp_player_name;
            match[m].player_team[team_N][team_N_players[team_N]] = temp_team_name;
            fin >> match[m].player_kills[team_N][team_N_players[team_N]];
            fin >> match[m].player_headshots[team_N][team_N_players[team_N]];
            fin >> match[m].player_assists[team_N][team_N_players[team_N]];
            fin >> match[m].player_deaths[team_N][team_N_players[team_N]];
            fin >> match[m].player_hltv_ratings[team_N][team_N_players[team_N]];
            team_N_players[team_N]++;
		}
		if(match[m].team_name[0] == match[m].team_name[1]){
			corrupt_data = true;
		}
		if(corrupt_data == true){
			m--;
			total_N_matches--;
		}
	}
	sort(match, match + total_N_matches, sort_by_date);

	ofstream fout ("../data.txt");
	for(int m=0; m<total_N_matches; m++){
		if(match[m].map_name == "season" || match[m].map_name == "mill" || match[m].map_name == "dust" || match[m].map_name == "tuscan") continue;
		fout << match[m].date << " " << match[m].hour << " " << match[m].map_name << " ";
		fout << match[m].team_name[0] << " " << match[m].score[0] << " " << match[m].team_name[1] << " " << match[m].score[1] << " ";
		for(int x=0; x<2; x++){
			for(int y=0; y<5; y++){
				fout << match[m].player_name[x][y] << " ";
				fout << match[m].player_kills[x][y] << " ";
	            fout << match[m].player_headshots[x][y] << " ";
	            fout << match[m].player_assists[x][y] << " ";
	            fout << match[m].player_deaths[x][y] << " ";
	            fout << match[m].player_hltv_ratings[x][y] << " ";
			}
		}
		fout << endl;
	}
}