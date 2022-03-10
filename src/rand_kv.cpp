#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fstream>

using namespace std;

int main()
{
    fstream out("./data.txt");//打开文件
    // fstream out2("./data_for_B.txt");
    srand((unsigned)time(NULL));//随机种子 
    long long int MAX = 10000;//min max的值可以自己随便设置
    long long int MIN = 0;
    bool flag[10010];
    for(long long int i=0; i<10000; i++)
    {
        long long int a = (int) (((double)rand() / RAND_MAX) * 10000) % MAX;
        long long int b = (int) (((double)rand() / RAND_MAX) * 10000) % MAX;
        if (flag[a] == 1)
            continue;
        flag[a] = 1;
        out << a << " " << b << endl;
        // out2 << "INSERT " << a << endl;
    }

    out.close();//关闭文件
    // out2.close();
    

}