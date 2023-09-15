/**
 * 该程序作用是计算 disksim 格式的 trace 实际占用的硬盘空间
 * 这和 trace 表示的数据量大小不同。
 * 前者会包含重复的请求，故不会重复计算，且 trace 实际占用的空间
 * footprint 只计算写请求。
 * 
 * 执行方式：g++ trace_foot_size.cpp -o trace_foot_size
 *         ./trace_foot_size [tracefile]
 */
#include <float.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <algorithm>
#include <climits>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define ll size_t
// bitmap:位图，用于计算Footprint
class BitMap {
public:
    BitMap(ll range) {
        // 此时多开辟一个空间
        _bits.resize(range / 32 + 1);
    }
    void Set(ll x) {
        int index = x / 32;           // 确定哪个数据（区间）
        int temp = x % 32;            // 确定哪个Bit位
        _bits[index] |= (1 << temp);  // 位操作即可
    }
    void Reset(ll x) {
        int index = x / 32;
        int temp = x % 32;
        _bits[index] &= ~(1 << temp);  // 置0
    }
    bool Test(ll x) {
        int index = x / 32;
        int temp = x % 32;
        if (_bits[index] & (1 << temp))
            return 1;
        else
            return 0;
    }

private:
    vector<int> _bits;
};

BitMap bitmapwrite(INT_MAX);
BitMap bitmapread(INT_MAX);
pair<string, string> getFootprint(FILE *traceFile);
vector<string> split(char *str, const char *delimiter);
void format(pair<string, string> *p);

// 主函数
int main(int args, char **argv) {
    string traceFile = argv[1];
    FILE *inputFile = fopen(traceFile.c_str(), "r");
    pair<string, string> p;
    p = getFootprint(inputFile);
    format(&p);
    cout << "tracefile = " << argv[1] << endl;
    cout << "footprint = " << p.first << endl;
    cout << "totalsize = " << p.second << endl;
    return 0;
}

pair<string, string> getFootprint(FILE *traceFile) {
    ll volume = 0, footprint = 0;
    char writebuffer[1024];
    ll offset, length = 0;  // 偏移和 length 均是以块(512B)为单位
    ll maxAddress = 0;
    ll minAddress = INT_MAX;
    string rw = "";  // 读写标记

    while (fgets(writebuffer, 1024, traceFile) != NULL) {
        ll real = 0;
        // printf("%s\n", writebuffer);
        // 存储每一行 trace 记录的六个参数
        vector<string> arg = split(writebuffer, " ");
        rw = arg[0];
        offset = stol(arg[2]);
        length = stol(arg[3]);

        maxAddress = max(maxAddress, offset);
        minAddress = min(minAddress, offset);

        if (rw == "W") {
            real = 0;
            ll blkno = offset;
            ll len = length;
            for (ll i = blkno; i < blkno + len; i++) {
                if (bitmapwrite.Test(i) == 0) {
                    real++;
                }
                bitmapwrite.Set(i);
            }
            footprint += real;
        } else {
            real = 0;
            ll blkno = offset;
            ll len = length;
            for (ll i = blkno; i < blkno + len; i++) {
                if (bitmapread.Test(i) == 0) {
                    real++;
                }
                bitmapread.Set(i);
            }
        }
        volume += length;
    }
    return make_pair(to_string(footprint), to_string(volume));
}

/**
 * 根据指定字符切分字符串
 */
vector<string> split(char *str, const char *delimiter) {
    vector<string> res;
    char *tmp = NULL;
    tmp = strtok(str, delimiter);
    while (tmp != NULL) {
        string str = string(tmp);
        res.push_back(str);
        tmp = strtok(NULL, " ");
    }
    return res;
}

/**
 * 格式化输出，将大小按照 KB, MB, GB, TB, PB 格式进行输出
 */
void format(pair<string, string> *p) {
    // 因为以扇区为单位，因此大小不可能以 B 作为单位，四舍五入至少为 1KB
    const char *suffix[6] = {"KB", "MB", "GB", "TB", "PB"};
    double footprint = stod(p->first);
    double volume = stod(p->second);
    short cnt = 0;
    // 单位为块(512B)
    footprint /= 2;
    volume /= 2;

    // FIX: 冗余代码，可以再进行函数抽取
    while (1) {
        if (footprint < 512) {
            // 保留两位小数
            ostringstream oss;
            oss << fixed << setw(7) << left << setprecision(2) << footprint;
            p->first = oss.str() + suffix[cnt];
            break;
        }
        footprint /= 1024;
        cnt++;
    }
    cnt = 0;
    while (1) {
        if (volume < 512) {
            // 保留两位小数
            ostringstream oss;
            oss << fixed << setw(7) << left << setprecision(2) << volume;
            p->second = oss.str() + suffix[cnt];
            break;
        }
        volume /= 1024;
        cnt++;
    }
}