#include <fstream>
#include <iostream>
using namespace std;

int main()
{
    ifstream fin ("monbrey_stats.txt");
    double total_error = 0;
    int N = 0;
    while(!fin.eof()){
        N++;
        for(int x=0; x<3; x++){
            int win;
            double percentA, percentB;
            fin >> win >> percentA >> percentB;
            if(win == 1) total_error += (1 - percentA) * (1 - percentA);
            if(win == 0) total_error += (1 - percentB) * (1 - percentB);
        }
    }
    total_error /= N;
    cout << total_error << endl;
}
