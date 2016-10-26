#include <fstream>
#include <iostream>
using namespace std;

int main()
{
    ifstream fin ("../data_files/monbrey_stats.txt");
    double error = 0;
    int predicted_correct = 0;
    int N = 0;
    while(!fin.eof()){
        int win;
        double percentA, percentB;
        N++;
        fin >> win >> percentA >> percentB;
        percentA *= 0.01;
        percentB *= 0.01;
        if(win == 1){
            error += (1 - percentA) * (1 - percentA);
            if(percentA > 0.5) predicted_correct++;
        }
        if(win == 0){
            error += (1 - percentB) * (1 - percentB);
            if(percentB > 0.5) predicted_correct++;
        }
    }
    error /= N;
    cout << "CSGOLounge Error: " << error << endl;
    cout << "Percentage Correct: " << (double) predicted_correct / N << endl;
}
