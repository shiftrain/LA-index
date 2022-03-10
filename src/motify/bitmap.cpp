#include <iostream>
#include <vector>
#include "bitmap.hpp"

using namespace std;

BitMap::BitMap(int range):_bits(range >> 5 + 1, 0), len(0){}

void BitMap::Set(int x) {
    int index = x >> 5;//确定哪个数据（区间）
    int temp = x & 31;//确定哪个Bit位
    _bits[index] |= (1 << temp);//位操作即可
    len++;
}
void BitMap::Reset(int x) {
    int index = x >> 5;
    int temp = x & 31;
    _bits[index] &= ~(1 << temp);//取反
    len--;
}
bool BitMap::Test(int x) {
    int index = x >> 5;
    int temp = x & 31;
    if (_bits[index] & (1 << temp))
        return 1;
    else
        return 0;
}
int BitMap::Getlen() {
    return len;
}
typedef long long int LL;
typedef struct NODE
{
    LL skey;
    LL nvalue;
};

int main() {        
    BitMap BM(64);
    BM.Set(1);
    cout<<BM.Test(1)<<endl;
    BM.Reset(1);
    cout<<BM.Test(1)<<endl;
    BM.Set(1);
    cout<<BM.Test(1)<<endl;
    BM.Reset(1);
    cout<<BM.Test(1)<<endl;
    // cout<<sizeof(int)<<endl;
}