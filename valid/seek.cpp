#pragma warning(disable:4996);
#include<iostream>
#include<vector>
#include<algorithm>
#include<climits>
#include<float.h>
#include<string>
#include<string.h>
using namespace std;
int main(int argc, char* argv[]) {
	FILE  *inputFile;
	FILE  *outputFile;
	char writebuffer[1024];
        inputFile = fopen(argv[1], "r");
	bool first = true;
	outputFile = fopen(argv[2], "w+");
	int number=0;
	float latency=0.0;

	fgets(writebuffer, 1024, inputFile);
	fprintf(outputFile, "%s", writebuffer);
  
	while (2== fscanf(inputFile, "%d,      %f\n", &number, &latency)) {
		fprintf(outputFile, "%d,      %f\n",number,latency/stof(argv[3]));
	}
	fclose(inputFile);
	fclose(outputFile);
	return 0;
}
