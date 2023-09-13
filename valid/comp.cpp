#pragma warning(disable : 4996);
#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <float.h>
#include <string>
#include <string.h>
#include <algorithm>
using namespace std;
int main(int argc, char *argv[]) {
    FILE *inputFile;
    vector<float> realTrace;
    vector<float> simutionTrace;
    vector<pair<int, string>> ret;
    FILE *outputFile;
    FILE *wucha;
    char writebuffer[1024];
    float latency = 0.0;
    int device = 0;
    int offset = 0;
    int length = 0;
    int rw = 0;
    int time = 0;
    int per;
    string minPian = "min piancha";
    float minVlaue = 1000.0;
    vector<float> simutionLatency;
    vector<float> realLatency;
    inputFile = fopen(argv[1], "r");
    outputFile = fopen(argv[2], "r");
    int flags = 0;
    while (5 == fscanf(outputFile, "%f ,opid %d, blkno %d, bcount %d %d\n", &latency, &device, &offset, &length, &flags)) {
        simutionLatency.push_back(latency);
    }

    while (5 == fscanf(inputFile, "%d, %f, %d, %d, %d\n", &time, &latency, &rw, &length, &per)) {
        realLatency.push_back(latency / 1000000);
    }
    std::fclose(outputFile);
    std::fclose(inputFile);
    sort(simutionLatency.begin(), simutionLatency.end());
    sort(realLatency.begin(), realLatency.end());
    float suma = 0;
    float sumb = 0;

    for (int i = 0; i < simutionLatency.size(); i++) {
        suma += simutionLatency[i];
    }
    suma = suma / simutionLatency.size();

    for (int i = 0; i < realLatency.size(); i++) {
        sumb += realLatency[i];
    }
    sumb = sumb / realLatency.size();
    printf("real latency : %f ,simulation  latency :%f   , piancha: %f \n", sumb, suma, abs(suma - sumb) / sumb);
    return 0;
}
