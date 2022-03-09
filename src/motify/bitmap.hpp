#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <iostream>
#include <vector>

using namespace std;

class BitMap
{
public:
    BitMap(int range);   //初始化，range为BitMap的大小
    void Set(int x);     //设置
    void Reset(int x);   //重设
    bool Test(int x);    //测试
    int Getlen();        //
    

private:
    vector<int> _bits;
    long long len;
};



#endif
