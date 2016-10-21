#include <fstream>
#include <algorithm>
#include <string>
#include <iostream>
using namespace std;

string junk;
string* player;
int counter, matchc, total_players;

bool sortw(const string &players, const string &players1)
{
    return players>players1;
}

int main()
{
    ifstream fin ("sorted_total_data.txt");
    fin >> matchc;
    player = new string[matchc*10]; // # of matches * players in each match

    for(int x=0; x<matchc; x++){
        fin >> junk >> junk >> junk >> junk;
        for(int y=0; y<5; y++){
            fin >> player[counter];
            counter++;
        }
        fin >> junk >> junk;
        for(int y=0; y<5; y++){
            fin >> player[counter];
            counter++;
        }
    }
    fin.close();

    sort(player, player + counter); // sort names to find same
    total_players = counter;
    for(int x=0; x<counter-1; x++){
        if(player[x] == player[x + 1]){ // find same name immediately after
            for(int y=x+1; y<counter; y++){ // find all names after
                if(player[x] == player[y]){
                    player[y].clear();
                    total_players--;
                }
                else{
                    x = y - 1;
                    break;
                }
            }
        }
    }

    ofstream fout ("player_names.txt");
    fout << total_players + 1 << endl;
    sort(player, player+counter, sortw);
    for(int x=0; x<total_players + 1; x++)
        fout << player[x] << endl;
}
