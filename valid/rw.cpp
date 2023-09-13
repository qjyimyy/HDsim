#pragma warning(disable:4996);
#include<iostream>
#include<vector>
#include<algorithm>
#include<climits>
#include<float.h>
#include<string>
#include<string.h>
#include<algorithm>
using namespace std;
int main(int argc, char* argv[]) {
	FILE  *inputFile;
	vector<float>realTrace;
	vector<float>simutionTrace;
	vector<pair<int, string>>ret;
	FILE  *wucha;
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
	vector<float>simutionLatency;
	vector<float>writeLatency;
	vector<float>readLatency;

	vector<float>realLatency;
	inputFile = fopen(argv[1], "r");
	int flags = 0;
	while (5 == fscanf(inputFile, "%f ,opid %d, blkno %d, bcount %d %d\n", &latency, &device, &offset, &length,&flags)) {
		simutionLatency.push_back(latency);
		if (flags) {
			readLatency.push_back(latency);
		}
		else {
			writeLatency.push_back(latency);
		}
	}

	std::fclose(inputFile);
	sort(simutionLatency.begin(), simutionLatency.end());
	sort(realLatency.begin(), realLatency.end());
	float suma = 0;
	float sumwrite = 0;
	float sumread = 0;
	for (int i = 0; i < readLatency.size(); i++) {
		sumread += readLatency[i];
	}
	for (int i = 0; i < writeLatency.size(); i++) {
		sumwrite += writeLatency[i];
	}

	suma = suma / simutionLatency.size();
	sumwrite = sumwrite / writeLatency.size();
	sumread = sumread / readLatency.size();
	cout<<"avarge  :"<< suma <<endl;
	cout << "avarge write :" << sumwrite << endl;
	cout << "avarge read :" << sumread << endl;
	return 0;
}
