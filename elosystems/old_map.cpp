#include <fstream>
#include <algorithm>
#include <cmath>
#include <iostream>
using namespace std;

ofstream fout ("test.txt");

int total_matches, total_players;
int pn[2][5]; //store position of player in player array
double team_average[2], team_average_map[2]; // average rating team 1 and 2
int N_of_recents = 30; // # of recent ratings used to predict game outcome
int N_of_mapr = 10; // # of recent ratings used to predict map rating for each player

bool overNgames, could_not_find_player;

int game_counter, correct, map_number;
double mean_squared_error, act_exp_error;

struct match1
{
    int date, score[2];
    string mapp, mplayer[2][5], team[2];
};
match1 match[10000];

struct player1
{
    string name;
    double rating, weighted_ratings[8][500], actual_ratings[8][500], total_weighted_ratings[1000];
    int game_date[1000], gnumb, mapn[8];
    bool overN_of_mapr[8];
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
        for(int y=0; y<8; y++){
            player[x].mapn[y] = 0;
            player[x].overN_of_mapr[y] = false;
        }
    }
    mean_squared_error = 0;
    act_exp_error = 0;
    game_counter = 0;
    correct = 0;
}

double get_mean(int N_of_scores, double score_array[])
{
    double total = 0;
    if(N_of_scores == 0) return 0;
    for(int x=0; x<N_of_scores; x++){
        //fout << score_array[x] << endl;
        total += score_array[x];
    }
    total /= N_of_scores;
    //fout << endl;
    return total;
}

double get_standard_deviation(int N_of_scores, double mean, double score_array[])
{
    double total = 0;
    for(int x=0; x<N_of_scores; x++) total += (mean - score_array[x]) * (mean - score_array[x]);
    total /= (N_of_scores - 1);
    total = sqrt(total);
    return total;
}

double cdf(double value)
{
   return 0.5 * erfc(-value * 1 / sqrt(2));
}

bool sort_by_rating(const player1 &p1, const player1 &p2)
{
    return p1.rating>p2.rating;
}

// old way to test error
double test_program(int match_number)
{
    
    double standard_deviation[2], mean[2], scores[N_of_recents * 5], scores_date[N_of_recents * 5];
    for(int x=0; x<2; x++){
        int rating_counter = 0;
        for(int y=0; y<5; y++){
            for(int z=0; z<N_of_recents; z++){
                // find the N_of_recents for each player of the team
                scores[rating_counter] = player[pn[x][y]].total_weighted_ratings[player[pn[x][y]].gnumb - (z + 1)];
                scores_date[rating_counter] = player[pn[x][y]].game_date[player[pn[x][y]].gnumb - (z + 1)];
                rating_counter++;
            }
        }

        mean[x] = get_mean(rating_counter, scores);
        standard_deviation[x] = get_standard_deviation(rating_counter, mean[x], scores);
    }
    double zscore = (mean[0] - mean[1]) / sqrt((standard_deviation[0] * standard_deviation[0]) + (standard_deviation[1] * standard_deviation[1]));
    double probability = cdf(zscore);
    mean_squared_error += (1 - probability) * (1 - probability);
    if(probability > 0.5) correct++;

    game_counter++;
}

// new way to test error
void test_program2(double actual, double expected)
{
    act_exp_error += ((actual - expected) * (actual - expected));
    game_counter++;
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
    // overall average
    for(int x=0; x<2; x++){
        team_average[x] = 0;
        for(int y=0; y<5; y++){
            team_average[x] += player[pn[x][y]].rating;
        }
        team_average[x] /= 5;
    }
}

double find_team_average_map()
{
    // find the average for the map being played
    for(int x=0; x<2; x++){
        team_average_map[x] = 0;
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
            if(player[pn[x][y]].overN_of_mapr[map_number] == true) team_average_map[x] += get_mean(N_of_mapr, player[pn[x][y]].weighted_ratings[map_number]);
            else team_average_map[x] += get_mean(player[pn[x][y]].mapn[map_number], player[pn[x][y]].weighted_ratings[map_number]);
        }
        overall_average /= N_ratings;
        team_average_map[x] /= 5;
        return (overall_average - team_average[x]) / overall_average;
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


// MAIN PROGRAM!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void update_player_ratings(int match_number, double k)
{
    bool skip_map_ratings = false;
    for(int x=0; x<2; x++){
        int counter = 1 - x;

        // percentage of rounds won + base of winning or losing
        double percentage_won = match[match_number].score[x] / (double)(match[match_number].score[0] + match[match_number].score[1]);
        double actual_score = percentage_won;

        // percentage x enemy team average rating
        double weighted_score = team_average[counter] * actual_score;

        // update each player's rating in both teams
        for(int y=0; y<5; y++){
            // find team rating difference for the map being played
            double map_dif;
            map_dif = find_team_average_map();
            if(map_dif == 0) skip_map_ratings = true; // not enough information
            if(skip_map_ratings == true) map_dif = 0;
            //map_dif = team_average_map[counter] - team_average_map[x];
            //fout << map_dif << endl;

            player[pn[x][y]].weighted_ratings[map_number][player[pn[x][y]].mapn[map_number]] = weighted_score;
            player[pn[x][y]].actual_ratings[map_number][player[pn[x][y]].mapn[map_number]] = actual_score;
            player[pn[x][y]].mapn[map_number]++;
            if(player[pn[x][y]].mapn[map_number] == N_of_mapr){
                player[pn[x][y]].overN_of_mapr[map_number] = true;
                player[pn[x][y]].mapn[map_number] = 0;
            }

            player[pn[x][y]].total_weighted_ratings[player[pn[x][y]].gnumb] = weighted_score;
            player[pn[x][y]].game_date[player[pn[x][y]].gnumb] = match[match_number].date;
            player[pn[x][y]].gnumb++;
            double expected_score = 1 / (1 + exp(team_average[counter] - player[pn[x][y]].rating));
            player[pn[x][y]].rating += k * (actual_score - expected_score);
            if(match_number > 5000 && overNgames == true) test_program2(actual_score, expected_score);
        }
    }
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

    /*output_results << total_players << endl;
    for(int p=0; p<total_players; p++){
        output_results << player[p].name << endl;
        for(int x=0; x<N_of_recents; x++) output_results << player[p].weighted_ratings[player[p].gnumb - (x + 1)] << " " << player[p].game_date[player[p].gnumb - (x + 1)] << endl;
        output_results << endl;
    }*/
    for(int p=0; p<2; p++){
        fout << player[p].rating << endl;
        for(int x=0; x<8; x++){
            if(player[p].overN_of_mapr[0] == true){
                fout << get_mean(10, player[p].weighted_ratings[x]) << endl;
            }
            else{
                fout << get_mean(player[p].mapn[x], player[p].weighted_ratings[x]) << endl;
            }
        }      
        fout << endl;
    }
}

void output_tests(double k)
{
    fout << "k: " << k << endl;
    fout << "# of Games: " << game_counter << endl;
    //fout << "# Predicted Correctly: " << correct << endl;
    //fout << "Correct Percentage " << (double) correct / game_counter << endl;
    //fout << "MSE: " << mean_squared_error / game_counter << endl << endl;
    fout << "Expected - Actual Error: " << sqrt(act_exp_error / (game_counter / 10)) << endl << endl;

    //sort(player, player + total_players, sort_by_rating);
    //for(int x=0; x<10; x++) fout << player[x].name << " " << player[x].rating << endl;
}

int main()
{
    double constant = 0.2;
    input_data();
    //fout << player[2916].name << endl;
    for(double constant=0.1; constant<0.61; constant += 0.1){
        reset_program();
        for(int m=0; m<total_matches; m++){
            find_player_position(m);
            if(could_not_find_player == true) return 0;
            which_map(m);
            find_team_average();
            //if(overNgames == true && m > 5000) test_program(m);
            update_player_ratings(m, constant);
        }
        output_tests(constant);
        //output_data();
    }
}