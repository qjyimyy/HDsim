#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    FILE *outputFile = fopen("ssd-cache-4M.outv", "r");
    char writebuffer[1024];
    vector<string> v;
    while (fgets(writebuffer, 1024, outputFile) != NULL) {
        string temp = string(writebuffer);
        char *res;
        if (string::npos != temp.find("IOdriver Response time average")) {
            res = strtok(writebuffer, ":");
            while (res != NULL) {
                string str = string(res);
                v.push_back(str);
                printf("%s\n", res);
                res = strtok(NULL, " ");
            }
        }
    }
    
    cout << stod(v[1]) << endl;
    
    return 0;
}