#include <iostream>

using namespace std;

int binary_search(int a[], int target)
{
    //改：目标不是等于，而是最小值
    int left = 0;
    int right = 9;
    int min = 0x7f7f7f7f;
    int middle;
    int res;
    while (left <= right)
    {
        
        middle = left + ((right - left) / 2);
        // cout<<middle<<endl;
        // cout<<hash_arrayfor2[pos1][middle]->mKey<<endl;
        // cout<<left<<" "<<right<<endl;
        if (a[middle] > target)
        {
            right = middle - 1;
        }
        else if (a[middle] < target)
        {
            left = middle + 1;
        }
        else
        {
            return middle, res = 1;
        }
    }

    return middle, res = -1;
    
}


int main()
{
    int a[10] = {0,10,20,30,40,50,60,70,80,90};
    int x,y = binary_search(a,33);
    cout << x << " " << y <<endl;
}