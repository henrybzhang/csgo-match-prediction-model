//basic skeleton of elo system
//winning team not always first
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

// # of games a player must play to be tested in the prediction model
const int N_games_needed = 30;

int total_N_matches, total_N_players;

int player_index[2][5];
double team_average[2];

bool enough_games;

int game_counter, N_correct_predictions;
double mean_squared_error;

struct match_stats
{
    int date, hour;
    int score[2], player_kills[2][5], player_headshots[2][5], player_assists[2][5], player_deaths[2][5];
    string map_name, player_name[2][5], team_name[2];
    double player_hltv_ratings[2][5];
    bool team0_wins;
};
struct player_stats
{
    string name;
    double rating;
    int number_of_games;
};
match_stats* match;
player_stats* player;

void input_data()
{
    ifstream fin ("../data_files/data.txt"); //input match data
    fin >> total_N_matches;
    match = new match_stats[total_N_matches];
    for(int m=0; m<total_N_matches; m++){
        fin >> match[m].date >> match[m].hour >> match[m].map_name;

        for(int x=0; x<2; x++){
            fin >> match[m].team_name[x] >> match[m].score[x];
        }
        if(match[m].score[0] > match[m].score[1]) match[m].team0_wins = true;
        for(int x=0; x<2; x++){
            for(int y=0; y<5; y++){
                fin >> match[m].player_name[x][y];
                fin >> match[m].player_kills[x][y];
                fin >> match[m].player_headshots[x][y];
                fin >> match[m].player_assists[x][y];
                fin >> match[m].player_deaths[x][y];
                fin >> match[m].player_hltv_ratings[x][y];
            }
        }
    }
    fin.close();

    ifstream pin ("../data_files/player_names.txt"); //input player names
    pin >> total_N_players;
    player = new player_stats[total_N_players];
    for(int m=0; m<total_N_players; m++) pin >> player[m].name;
    pin.close();
}

void reset_program()
{
    for(int x=0; x<total_N_players; x++){
        player[x].rating = 0.5;
        player[x].number_of_games = 0;
    }
    mean_squared_error = game_counter = N_correct_predictions = 0;
}

bool find_player_index(int m)
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

void find_team_average()
{
    for(int x=0; x<2; x++){
        team_average[x] = 0;
        for(int y=0; y<5; y++) team_average[x] += player[player_index[x][y]].rating;
        team_average[x] /= 5;
    }
}

void update_player_ratings(int m, double k)
{
    for(int x=0; x<2; x++){
        int counter = 1 - x;

        double percentage_rounds_won = match[m].score[x] / (double)(match[m].score[0] + match[m].score[1]);
        double base_score = percentage_rounds_won;

        // percentage * other team's rating average
        double weighted_score = team_average[counter] * base_score;

        // update player ratings
        for(int y=0; y<5; y++){
            int p = player_index[x][y];
            player[p].number_of_games++;
            double expected_score = 1 / (1 + exp(team_average[counter] - player[p].rating));
            player[p].rating += k * (base_score - expected_score);
        }
    }
}

bool sort_by_rating(const player_stats &p1, const player_stats &p2)
{
    return p1.rating>p2.rating;
}

void output_data()
{
    ofstream output_results ("../results/base_player_stats.txt");

    // get rid of players with less than the necessary number of games
    int subtract = 0;
    for(int x=0; x<total_N_players; x++){
        if(player[x].number_of_games < N_games_needed){
            player[x].rating = 0;
            subtract++;
        }
    }
    sort(player, player + total_N_players, sort_by_rating);
    total_N_players -= subtract;

    output_results << total_N_players << endl;
    for(int p=0; p<total_N_players; p++) output_results << player[p].name << " " << player[p].rating << endl;

    output_results.close();
}


double get_mean(int N, double score_array[])
{
    double total = 0;
    for(int x=0; x<N; x++) total += score_array[x];
    total /= N;
    return total;
}

double get_standard_deviation(int N, double mean, double score_array[])
{
    double total = 0;
    for(int x=0; x<N; x++) total += (mean - score_array[x]) * (mean - score_array[x]);
    total /= (N - 1);
    total = sqrt(total);
    return total;
}

double cdf(double value)
{
   return 0.5 * erfc(-value * 1 / sqrt(2));
}

void swap(double &n1, double &n2)
{
     double temp = n1;
     n1 = n2;
     n2 = temp;
}

double test_program(int m)
{
    double standard_deviation[2], mean[2];
    for(int x=0; x<2; x++){
        double team_player_ratings[5];
        for(int y=0; y<5; y++){
            int p = player_index[x][y];
            team_player_ratings[y] = player[p].rating;
        }
        mean[x] = get_mean(5, team_player_ratings);
        standard_deviation[x] = get_standard_deviation(5, mean[x], team_player_ratings);
    }

    if(team_average[0] > team_average[1] && match[m].team0_wins == true) N_correct_predictions++;
    else if(team_average[0] < team_average[1] && match[m].team0_wins == false) N_correct_predictions++;
    game_counter++;

    //make mean[0] the winning team's mean rating
    if(match[m].team0_wins == false){
        swap(mean[0], mean[1]);
        swap(standard_deviation[0], standard_deviation[1]);
    }

    double zscore = (mean[0] - mean[1]) / sqrt((standard_deviation[0] * standard_deviation[0]) + (standard_deviation[1] * standard_deviation[1]));
    double probability = cdf(zscore);
    mean_squared_error += (1 - probability) * (1 - probability);
}

ofstream fout ("../results/base_predictions.txt");
void output_tests(double elo_constant)
{
    fout << "Elo Constant: " << elo_constant << endl;
    fout << "Predicted " << N_correct_predictions << " out of " << game_counter << " games correctly." << endl;
    fout << "Percentage: " << (double) N_correct_predictions / game_counter  * 100 << '%' << endl;
    fout << "Mean Squared Error: " << mean_squared_error / game_counter << endl << endl;
}

int main()
{
    input_data();

    // try different constants to see which works best
    for(int constant_1=1; constant_1<=10; constant_1++){
        cout << "Constant_1: " << constant_1 << endl;
        reset_program();
        double elo_constant = (double) constant_1 / 10;
        for(int m=0; m<total_N_matches; m++){
            if(find_player_index(m) == false) continue;
            find_team_average();

            //test the program here
            if(enough_games == true) test_program(m);

            update_player_ratings(m, elo_constant);
        }
        output_tests(elo_constant);
        //output_data();
    }
}