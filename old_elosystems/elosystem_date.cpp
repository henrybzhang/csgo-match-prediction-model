// implement HGA from http://www.matterofstats.com/mafl-stats-journal/2013/10/13/building-your-own-team-rating-system.html
#include <fstream>
#include <algorithm> // sort
#include <cmath> // math functions
#include <iostream>
#include <utility> // pairs
using namespace std;

ofstream fout ("test.txt");

int total_matches, total_players;
int pn[2][5]; //store position of player in player array
double team_average[2]; // average rating team 1 and 2
int N_of_recents = 30; // # of recent ratings used to predict game outcome

bool overNgames, could_not_find_player;

int game_counter, upset_counter, correct;
double mean_squared_error, weighted_mean_squared_error, exp_weighted_mean_squared_error;

struct match1
{
    int date, score[2];
    string mapp, mplayer[2][5], team[2];
};
match1 match[10000];

struct player1
{
    string name;
    double rating, weighted_ratings[1000], actual_ratings[1000];
    int game_date[1000], gnumb;
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
        player[x].rating = 1;
        player[x].gnumb = 0;
    }
    weighted_mean_squared_error = 0;
    mean_squared_error = 0;
    game_counter = 0;
    correct = 0;
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
                    fout << "Match Number: " << match_number << endl;
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

void update_player_ratings(int match_number, double k)
{
    for(int x=0; x<2; x++){
        int counter = 1 - x;

        // percentage of rounds won
        double actual_score = match[match_number].score[x] / (double)(match[match_number].score[0] + match[match_number].score[1]);
        
        // percentage x enemy team average rating
        double weighted_score = team_average[counter] * actual_score;

        // update each player's rating in both teams
        for(int y=0; y<5; y++){
            player[pn[x][y]].weighted_ratings[player[pn[x][y]].gnumb] = weighted_score;
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
    output_results << total_players << endl;
    for(int p=0; p<total_players; p++){
        output_results << player[p].name << endl;
        for(int x=0; x<N_of_recents; x++) output_results << player[p].weighted_ratings[player[p].gnumb - (x + 1)] << " " << player[p].game_date[player[p].gnumb - (x + 1)] << endl;
        output_results << endl;
    }
}
// END OF MAIN PROGRAM!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


bool sort_scores(const pair <int, double> &i, const pair <int, double> &j)
{
    return i.first > j.first;
}

double get_mean(int N_of_scores, double score_array[])
{
    double total = 0;
    for(int x=0; x<N_of_scores; x++) total += score_array[x];
    total /= N_of_scores;
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

double get_weighted_mean(int match_number, int N_of_scores, const pair <int, double> score_data[])
{
    double total = 0;
    double date_total = 0;
    double date_difference[N_of_scores];
    for(int x=0; x<N_of_scores; x++){
        date_difference[N_of_scores - 1 - x] = (match[match_number].date - score_data[x].first);
        date_total += date_difference[N_of_scores - 1 - x];
    }
    for(int x=0; x<N_of_scores; x++) total += (date_difference[x] / date_total) * score_data[x].second;
    return total;
}

double get_weighted_standard_deviation(int match_number, int N_of_scores, double mean, const pair <int, double> score_data[])
{
    double total = 0;
    double date_total = 0;
    double date_difference[N_of_scores];
    for(int x=0; x<N_of_scores; x++){
        date_difference[N_of_scores - 1 - x] = (match[match_number].date - score_data[x].first);
        date_total += date_difference[N_of_scores - 1 - x];
    }
    for(int x=0; x<N_of_scores; x++) total += (date_difference[x] / date_total) * (mean - score_data[x].second) * (mean - score_data[x].second);
    total = sqrt(total);
    return total;
}

double get_weighted_mean_exp(int match_number, int N_of_scores, const pair <int, double> score_data[])
{
    double total = 0;
    double date_total = 0;
    for(int x=0; x<N_of_scores; x++) date_total += score_data[x].first;
    for(int x=0; x<N_of_scores; x++) total += (score_data[x].first / date_total) * score_data[x].second;
    return total;
}

double get_weighted_standard_deviation_exp(int match_number, int N_of_scores, double mean, const pair <int, double> score_data[])
{
    double total = 0;
    double date_total = 0;
    for(int x=0; x<N_of_scores; x++) date_total += score_data[x].first;
    for(int x=0; x<N_of_scores; x++) total += (score_data[x].first / date_total) * (mean - score_data[x].second) * (mean - score_data[x].second);
    total = sqrt(total);
    return total;
}

double cdf(double value)
{
   return 0.5 * erfc(-value * 1 / sqrt(2));
}

double test_program(int match_number)
{
    pair <int, double> score_data[N_of_recents * 5];
    double standard_deviation[2], mean[2], scores[N_of_recents * 5], scores_date[N_of_recents * 5];
    double weighted_standard_deviation[2], weighted_mean[2];
    double exp_weighted_standard_deviation[2], exp_weighted_mean[2];
    for(int x=0; x<2; x++){
        int rating_counter = 0;
        for(int y=0; y<5; y++){
            for(int z=0; z<N_of_recents; z++){
                // find the N_of_recents for each player of the team
                scores[rating_counter] = player[pn[x][y]].weighted_ratings[player[pn[x][y]].gnumb - (z + 1)];
                scores_date[rating_counter] = player[pn[x][y]].game_date[player[pn[x][y]].gnumb - (z + 1)];

                // confusing
                score_data[rating_counter].first = player[pn[x][y]].game_date[z];
                score_data[rating_counter].second = player[pn[x][y]].weighted_ratings[z];
                rating_counter++;
            }
        }
        sort(score_data, score_data + rating_counter, sort_scores);

        mean[x] = get_mean(rating_counter, scores);
        standard_deviation[x] = get_standard_deviation(rating_counter, mean[x], scores);

        weighted_mean[x] = get_weighted_mean(match_number, rating_counter, score_data);
        weighted_standard_deviation[x] = get_weighted_standard_deviation(match_number, rating_counter, mean[x], score_data);

        exp_weighted_mean[x] = get_weighted_mean_exp(match_number, rating_counter, score_data);
        exp_weighted_standard_deviation[x] = get_weighted_standard_deviation_exp(match_number, rating_counter, mean[x], score_data);
    }
    double zscore = (mean[0] - mean[1]) / sqrt((standard_deviation[0] * standard_deviation[0]) + (standard_deviation[1] * standard_deviation[1]));
    double probability = cdf(zscore);
    mean_squared_error += (1 - probability) * (1 - probability);
    if(probability > 0.5 ) correct++;

    double weighted_zscore = (weighted_mean[0] - weighted_mean[1]) / sqrt(weighted_standard_deviation[0] * weighted_standard_deviation[0] + weighted_standard_deviation[1] * weighted_standard_deviation[1]);
    double weighted_probability = cdf(weighted_zscore);
    weighted_mean_squared_error += (1 - weighted_probability) * (1 - weighted_probability);

    double exp_weighted_zscore = (exp_weighted_mean[0] - exp_weighted_mean[1]) / sqrt(exp_weighted_standard_deviation[0] * exp_weighted_standard_deviation[0] + exp_weighted_standard_deviation[1] * exp_weighted_standard_deviation[1]);
    double exp_weighted_probability = cdf(exp_weighted_zscore);
    exp_weighted_mean_squared_error += (1 - exp_weighted_probability) * (1 - exp_weighted_probability);

    game_counter++;
}


void output_tests(double k)
{
    fout << "k: " << k << endl;
    fout << "# of Games: " << game_counter << endl;
    fout << "# Predicted Correctly: " << correct << endl;
    fout << "Correct Percentage " << (double) correct / game_counter << endl;
    fout << "Normal: " << mean_squared_error / game_counter << " " << game_counter << endl;
    fout << "Weighted: " << weighted_mean_squared_error / game_counter << " " << game_counter << endl;
    fout << "Experimental_weighted: " << exp_weighted_mean_squared_error / game_counter << " " << game_counter << endl << endl;

    //sort(player, player + total_players, sort_by_rating);
    //for(int x=0; x<10; x++) fout << player[x].name << " " << player[x].rating << endl;
}


int main()
{
    double constant = 1;
    input_data();
    //for(double constant=0.1; constant<2; constant += 0.1){
        reset_program();
        for(int m=0; m<total_matches; m++){
            find_player_position(m);
            if(could_not_find_player == true) return 0;
            find_team_average();
            if(overNgames == true) test_program(m);
            update_player_ratings(m, constant);
        }
        output_tests(constant);
        //output_data();
    //}
}