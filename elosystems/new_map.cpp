//basic skeleton of elo system
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
using namespace std;

// # of games a player must play to be tested in the prediction model
const int N_games_needed = 30;
const int N_games_needed_for_map = 4;

int total_N_matches, total_N_players;

int player_index[2][5], map_index;
bool enough_games, could_not_find_player;

double team_average[2], team_map_average[2];

int game_counter, N_correct_predictions;
double mean_squared_error;

struct match_stats
{
    int date, score[2];
    string map_name, player_name[2][5], team_name[2];
};
struct player_stats
{
    string name;
    double overall_rating, map_rating[8];
    int total_number_of_games, map_number_of_games[8];
};
match_stats* match;
player_stats* player;

void input_data()
{
    ifstream fin ("../data_files/old_sorted_total_data.txt"); //input match data
    fin >> total_N_matches;
    match = new match_stats[total_N_matches];
    for(int m=0; m<total_N_matches; m++){
        fin >> match[m].date >> match[m].map_name;

        fin >> match[m].team_name[0] >> match[m].score[0];
        for(int y=0; y<5; y++) fin >> match[m].player_name[0][y];

        fin >> match[m].team_name[1] >> match[m].score[1];
        for(int y=0; y<5; y++) fin >> match[m].player_name[1][y];
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
        player[x].overall_rating = 0.5;
        player[x].total_number_of_games = 0;
        for(int y=0; y<8; y++){
            player[x].map_rating = 0.5;
            player[x].map_number_of_games = 0;
        }
    }
    mean_squared_error = game_counter = N_correct_predictions = 0;
}

void which_map(int match_number)
{
    if(match[match_number].map_name == "dust2") map_index = 0;
    if(match[match_number].map_name == "mirage") map_index = 1;
    if(match[match_number].map_name == "inferno") map_index = 2;
    if(match[match_number].map_name == "cache") map_index = 3;
    if(match[match_number].map_name == "overpass") map_index = 4;
    if(match[match_number].map_name == "cobblestone") map_index = 5;
    if(match[match_number].map_name == "train") map_index = 6;
    if(match[match_number].map_name == "nuke") map_index = 7;
}

void find_player_positions(int match_number)
{
    enough_games = true;
    could_not_find_player = false;
    for(int x=0; x<2; x++){
        for(int y=0; y<5; y++){
            for(int p=0; p<total_N_players; p++){
                if(player[p].name == match[match_number].player_name[x][y]){
                    player_index[x][y] = p;
                    if(player[p].total_number_of_games < N_games_needed || player[p].map_number_of_games[map_index] < N_games_needed_for_map){
                        enough_games = false;
                        return;  
                    } 
                    break;
                }
                if(p == total_N_players - 1){ // if player's name is not found in player name array
                    cout << match[match_number].player_name[x][y] << endl;
                    could_not_find_player = true;
                    cout << "Error: could not find player '" << match[match_number].player_name[x][y] << "'" << endl;
                    cout << "Match Number: " << match_number + 1 << endl;
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
        for(int y=0; y<5; y++) team_average[x] += player[player_index[x][y]].overall_rating;
        team_average[x] /= 5;
    }
}

double find_team_map_average()
{
    for(int x=0; x<2; x++){
        team_map_average[x] = 0;
        double overall_average = 0;
        int N_ratings = 0;
        for(int y=0; y<5; y++){
            // get overall average for all maps in the team
            if(player[pn[x][y]].mapn[map_number] == 0 && player[pn[x][y]].overN_of_mapr[map_number] == false) return 0; // not enough information to predict map advantage
            if(player[pn[x][y]].overN_of_mapr[map_number] == true){
                N_ratings += N_of_mapr;
                for(int z=0; z<N_of_mapr; z++) overall_average += player[pn[x][y]].weighted_ratings[map_number][z];
            }
            else{
                N_ratings += player[pn[x][y]].mapn[map_number];
                for(int z=0; z<player[pn[x][y]].mapn[map_number]; z++) overall_average += player[pn[x][y]].weighted_ratings[map_number][z];
            }
            
            // get average for the map being played on
            if(player[pn[x][y]].overN_of_mapr[map_number] == true) team_map_average[x] += get_mean(N_of_mapr, player[pn[x][y]].weighted_ratings[map_number]);
            else team_map_average[x] += get_mean(player[pn[x][y]].mapn[map_number], player[pn[x][y]].weighted_ratings[map_number]);
        }
        overall_average /= N_ratings;
        team_map_average[x] /= 5;
        return (overall_average - team_average[x]) / overall_average;
    }
}

void update_player_ratings(int match_number, double k)
{
    for(int x=0; x<2; x++){
        int counter = 1 - x;

        double percentage_rounds_won = match[match_number].score[x] / (double)(match[match_number].score[0] + match[match_number].score[1]);
        double base_score = percentage_rounds_won;

        // percentage * other team's rating average
        double weighted_score = team_average[counter] * base_score;

        // update player ratings
        for(int y=0; y<5; y++){
            int p = player_index[x][y];
            int game_number = player[p].total_number_of_games;
            player[p].weighted_ratings[game_number] = weighted_score;
            player[p].actual_ratings[game_number] = base_score;
            player[p].total_weighted_ratings[game_number] = weighted_score;
            player[p].total_number_of_games++;

            double expected_score = 1 / (1 + exp(team_average[counter] - player[p].overall_rating));
            player[p].overall_rating += k * (base_score - expected_score);
        }
    }
}

bool sort_by_rating(const player_stats &p1, const player_stats &p2)
{
    return p1.overall_rating>p2.overall_rating;
}

void output_data()
{
    ofstream output_results ("../results/base_player_stats.txt");

    // get rid of players with less than the necessary number of games
    int subtract = 0;
    for(int x=0; x<total_N_players; x++){
        if(player[x].total_number_of_games < N_games_needed){
            player[x].overall_rating = 0;
            subtract++;
        }
    }
    sort(player, player + total_N_players, sort_by_rating);
    total_N_players -= subtract;

    output_results << total_N_players << endl;
    for(int p=0; p<total_N_players; p++) output_results << player[p].name << " " << player[p].overall_rating << endl;

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

double test_program(int match_number)
{
    double standard_deviation[2], mean[2];
    for(int x=0; x<2; x++){
        double team_player_ratings[5];
        for(int y=0; y<5; y++) team_player_ratings[y] = player[player_index[x][y]].overall_rating;
        mean[x] = get_mean(5, team_player_ratings);
        standard_deviation[x] = get_standard_deviation(5, mean[x], team_player_ratings);
    }

    // the winning team is always the first team
    if(team_average[0] > team_average[1]) N_correct_predictions++;
    game_counter++;

    double zscore = (mean[0] - mean[1]) / sqrt((standard_deviation[0] * standard_deviation[0]) + (standard_deviation[1] * standard_deviation[1]));
    double probability = cdf(zscore);
    mean_squared_error += (1 - probability) * (1 - probability);
}


ofstream fout ("../results/base_predictions.txt");
void output_tests(double k)
{
    fout << "k: " << k << endl;
    fout << "Predicted " << N_correct_predictions << " out of " << game_counter << " games correctly." << endl;
    fout << "Percentage: " << (double) N_correct_predictions / game_counter  * 100 << '%' << endl;
    fout << "Mean Squared Error: " << mean_squared_error / game_counter << endl << endl;
}


int main()
{
    input_data();

    // try different constants to see which works best
    for(int constant1=1; constant1<=10; constant1++){
        for(int constant2=1; constant2<=10; constant2++){
            reset_program();
            double K = (double) constant1 / 10;
            double map_constant = (double) constant2 / 10;
            for(int m=0; m<total_N_matches; m++){
                find_player_positions(m);
                if(could_not_find_player == true) continue;
                find_team_average();

                //test the program here
                if(enough_games == true) test_program(m);
                update_player_ratings(m, K);
            }
            output_tests(constant);
            //output_data();
        }
    }
}