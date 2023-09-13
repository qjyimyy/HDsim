#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>

using namespace std;

int main() {
    double ELAT = 100.0;
    double LAT = 0.0;
    double a = 0.125;
    double sum = 0.0;
    int cnt = 0;
    // cin >> LAT;
    // ELAT = LAT;
    while (cin >> LAT) {
        ELAT = (1 - a) * ELAT + a * LAT;
        cout << "ELAT = " << ELAT << endl;
        cnt++;
        sum += LAT;
        cout << "AVG = " << sum / cnt << endl;
    }
    return 0;
}