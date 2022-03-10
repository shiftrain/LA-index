#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <map>

using namespace std;

#define N1 1000
#define N2 100
#define HASH_TABLE_MAX_SIZE 1000
// #define line N1 / N2
#define line N1 / N2

typedef struct Hash3Node_Struct Hash3Node;
struct Hash3Node_Struct
{
    int Key = 0;
    int Value = 0;
    Hash3Node *p3Next = NULL;
}; //哈希表3层数据结构

typedef struct HashNode_Struct HashNode;
struct HashNode_Struct
{
    int mKey = 0;
    HashNode *pNext = NULL;
    Hash3Node *p3Head = NULL;
}; //哈希表2层数据结构

typedef struct HashBucket_Struct HashBucket;
struct HashBucket_Struct
{
    int mKey = 0; //其实不需要，直接hash索引就行了
    HashNode *pHead = NULL;
}; //哈希表1层数据结构

typedef struct Arrayfor2_Struct Arrayfor2;
struct Arrayfor2_Struct
{
    int mKey = 0;
    HashNode *pLink = NULL;
}; //哈希表2层数据结构

HashNode *hashTable[HASH_TABLE_MAX_SIZE];
int hash_table_size[HASH_TABLE_MAX_SIZE]; //哈希表中键值对的数量
bool hash_if_init[HASH_TABLE_MAX_SIZE];
Arrayfor2 *hash_arrayfor2[HASH_TABLE_MAX_SIZE][N1 / N2];

int hash_arrayfor2_size[HASH_TABLE_MAX_SIZE];

//初始化哈希表
void hash_table_init()
{
    // cout << "test_table_init" <<endl;
    memset(hashTable, 0, sizeof(HashNode) * HASH_TABLE_MAX_SIZE);
    // memset(hash_arrayfor2, 0, sizeof(Arrayfor2) * HASH_TABLE_MAX_SIZE * N1 / N2);
    // memset(void *s,int c,size_t n);
    //将s中后n个字节换成c所代表的内容
    //该函数是对较大结构体或数组进行清零操作的一种最快的方法
}

//第二层数组初始化
void hash_2_init(int pos1)
{
    hash_if_init[pos1] = true;
    HashNode *phead = hashTable[pos1];
    int i = 0;
    while (phead != NULL)
    {
        // cout<<i<<endl;
        Arrayfor2 *NewNode = (Arrayfor2 *)malloc(sizeof(Arrayfor2));
        memset(NewNode, 0, sizeof(Arrayfor2));
        NewNode->mKey = phead->mKey;
        NewNode->pLink = phead;
        hash_arrayfor2[pos1][i] = NewNode;
        i++;
        phead = phead->pNext;
    }
    hash_arrayfor2_size[pos1] = i;
}

//第二层二分查找
int binary_search(int pos1, int target, int &res)
{
    //改：目标不是等于，而是最小值
    int left = 0;
    int right = hash_arrayfor2_size[pos1] - 1;
    int middle;

    // cout<<hash_arrayfor2[pos1][left]->mKey<<" "<<target<<" "<<hash_arrayfor2[pos1][right]->mKey<<endl;

    while (left <= right)
    {

        middle = left + ((right - left) / 2);
        // cout<<middle<<endl;
        // cout<<middle<<endl;
        // cout<<hash_arrayfor2[pos1][middle]->mKey<<endl;
        // cout<<left<<" "<<right<<endl;
        if (hash_arrayfor2[pos1][middle]->mKey > target)
        {
            right = middle - 1;
        }
        else if (hash_arrayfor2[pos1][middle]->mKey < target)
        {
            left = middle + 1;
        }
        else
        {
            res = 1;
            return middle;
        }
    }
    res = -1;
    return middle;
}

//第二层二分查找:用于范围查询scan
int binary_search_for_scan(int pos1, int target)
{
    int left = 0;
    int right = hash_arrayfor2_size[pos1] - 1;
    int middle;
    while (left <= right)
    {
        middle = left + ((right - left) / 2);
        if (hash_arrayfor2[pos1][middle]->mKey > target)
        {
            right = middle - 1;
        }
        else if (hash_arrayfor2[pos1][middle]->mKey < target)
        {
            left = middle + 1;
        }
        else
        {
            return middle;
        }
    }
    if (hash_arrayfor2[pos1][middle]->mKey == 0)
    {
        return middle;
    }
    else if (hash_arrayfor2[pos1][middle]->mKey == hash_arrayfor2_size[pos1] - 1)
    {
        return middle - 1;
    }
    else if (hash_arrayfor2[pos1][middle]->mKey < target)
    {
        return middle;
    }
    else if (hash_arrayfor2[pos1][middle]->mKey > target)
    {
        return middle - 1;
    }
    return -1;
}

//插入第一阶段
void hash_table_insert_first(int skey, int nvalue)
{
    // cout<<1<<endl;
    // cout << "test_insert_first_1" <<endl;
    int pos1 = skey / N1;
    int pos2 = skey % N1 / N2;
    int tar_m = pos2 * N2;
    // cout<<pos1<<" "<<pos2<<endl;
    HashNode *pHead = hashTable[pos1];

    Hash3Node *New3Node = (Hash3Node *)malloc(sizeof(Hash3Node));
    memset(New3Node, 0, sizeof(Hash3Node));
    New3Node->Key = skey;
    New3Node->Value = nvalue;
    // cout<<skey<<" "<<nvalue<<endl;
    // cout<<New3Node->Key<<" "<<New3Node->Value<<endl;
    //第二层为空

    if (pHead == NULL)
    {

        // cout << "test_insert_first_2" <<endl;
        HashNode *NewNode = (HashNode *)malloc(sizeof(HashNode));
        memset(NewNode, 0, sizeof(HashNode));
        NewNode->mKey = tar_m;

        NewNode->p3Head = New3Node;
        NewNode->pNext = NULL;
        hashTable[pos1] = NewNode;
        return;
    }
    //第二层的第一个大于目标
    else if (pHead->mKey > tar_m)
    {

        // cout << "test_insert_first_3" <<endl;
        HashNode *NewNode = (HashNode *)malloc(sizeof(HashNode));
        memset(NewNode, 0, sizeof(HashNode));
        NewNode->mKey = tar_m;

        NewNode->p3Head = New3Node;
        NewNode->pNext = pHead;
        hashTable[pos1] = NewNode;
        return;
    }
    //第二层的第一个等于目标
    else if (pHead->mKey == tar_m)
    {

        // cout << "test_insert_first_4" <<endl;
        New3Node->p3Next = pHead->p3Head;
        pHead->p3Head = New3Node;
        // cout<<New3Node->Key<<" "<<New3Node->Value<<endl;
        // cout<<pHead->p3Head->Key<<" "<<pHead->p3Head->Value<<endl<<endl;
        return;
    }
    //第二层的第一个小于目标
    else if (pHead->mKey < tar_m)
    {

        // cout << "test_insert_first_5" <<endl;
        while (pHead != NULL)
        {
            //如果到最后都没有，就在结尾插入新节点
            if (pHead->pNext == NULL)
            {
                // cout<<1<<endl;

                HashNode *NewNode = (HashNode *)malloc(sizeof(HashNode));
                memset(NewNode, 0, sizeof(HashNode));

                NewNode->mKey = tar_m;
                // cout<<NewNode->mKey<<endl;
                NewNode->p3Head = New3Node;
                // cout<<10<<endl;
                NewNode->pNext = NULL;
                // cout<<20<<endl;
                pHead->pNext = NewNode;
                // cout<<30<<endl;
                return;
            }
            //找到了目标，不用新建
            else if (pHead->pNext->mKey == tar_m)
            {
                // cout<<2<<endl;

                New3Node->p3Next = pHead->pNext->p3Head;
                pHead->pNext->p3Head = New3Node;
                return;
            }
            //该出现的位置没有出现，插入新节点
            else if (pHead->pNext->mKey > tar_m)
            {
                // cout<<3<<endl;

                HashNode *NewNode = (HashNode *)malloc(sizeof(HashNode));
                memset(NewNode, 0, sizeof(HashNode));
                NewNode->mKey = tar_m;

                NewNode->p3Head = New3Node;
                NewNode->pNext = pHead->pNext;
                pHead->pNext = NewNode;
                return;
            }
            //下一个
            else if (pHead->pNext->mKey < tar_m)
            {
                // cout<<4<<endl;

                pHead = pHead->pNext;
            }
        }
    }
}

//插入第二阶段
void hash_table_insert_second(int skey, int nvalue)
{
    // cout<<2<<endl;
    int pos1 = skey / N1;
    int pos2 = skey % N1 / N2;
    int posin = pos2 * N2;
    HashNode *tar = hashTable[pos1];
    Hash3Node *New3Node = (Hash3Node *)malloc(sizeof(Hash3Node));
    memset(New3Node, 0, sizeof(Hash3Node));
    New3Node->Key = skey;
    New3Node->Value = nvalue;
    // cout<<New3Node->Key<<" xxxxxx "<<New3Node->Value<<endl;
    // cout<<posin<<endl;
    int res;
    int target = binary_search(pos1, pos2 * N2, res);
    // cout<<target<<" "<<res<<endl;
    tar = hash_arrayfor2[pos1][target]->pLink;
    if (res == 1)
    {
        New3Node->p3Next = tar->p3Head;
        tar->p3Head = New3Node;
    }
    else if (res == -1)
    { //*-
        if (tar->mKey < posin)
        {
            // cout<<"IN1"<<endl;
            if (target == hash_arrayfor2_size[pos1] - 1)
            {
                HashNode *NewNode = (HashNode *)malloc(sizeof(HashNode));
                memset(NewNode, 0, sizeof(HashNode));
                NewNode->mKey = posin;
                NewNode->pNext = NULL;
                NewNode->p3Head = New3Node;
                tar->pNext = NewNode;
                hash_arrayfor2[pos1][target]->mKey = posin;
                hash_arrayfor2[pos1][target]->pLink = NewNode;
                return;
            }
            else
            {
                while (tar != hash_arrayfor2[pos1][target + 1]->pLink)
                {
                    if (tar->pNext->mKey > posin)
                    {
                        HashNode *NewNode = (HashNode *)malloc(sizeof(HashNode));
                        memset(NewNode, 0, sizeof(HashNode));
                        NewNode->mKey = posin;
                        NewNode->p3Head = New3Node;
                        NewNode->pNext = tar->pNext;
                        tar->pNext = NewNode;
                        return;
                    }
                    else if (tar->pNext->mKey == posin)
                    {
                        New3Node->p3Next = tar->pNext->p3Head;
                        tar->pNext->p3Head = New3Node;
                        return;
                    }
                    else
                    {
                        tar = tar->pNext;
                    }
                }
            }
        }
        // -*
        else if (posin < tar->mKey)
        {
            if (target == 0)
            {
                HashNode *NewNode = (HashNode *)malloc(sizeof(HashNode));
                memset(NewNode, 0, sizeof(HashNode));
                NewNode->mKey = posin;
                NewNode->pNext = tar;
                NewNode->p3Head = New3Node;
                hash_arrayfor2[pos1][target]->mKey = posin;
                hash_arrayfor2[pos1][target]->pLink = NewNode;
                return;
            }
            else
            {
                tar = hash_arrayfor2[pos1][target - 1]->pLink;
                while (tar != hash_arrayfor2[pos1][target]->pLink)
                {
                    if (tar->pNext->mKey > posin)
                    {
                        HashNode *NewNode = (HashNode *)malloc(sizeof(HashNode));
                        memset(NewNode, 0, sizeof(HashNode));
                        NewNode->mKey = posin;
                        NewNode->p3Head = New3Node;
                        NewNode->pNext = tar->pNext;
                        tar->pNext = NewNode;
                        return;
                    }
                    else if (tar->pNext->mKey == posin)
                    {
                        New3Node->p3Next = tar->pNext->p3Head;
                        tar->pNext->p3Head = New3Node;
                        return;
                    }
                    else
                    {
                        tar = tar->pNext;
                    }
                }
            }
        }
    }
}

//插入
void hash_tabel_insert(int skey, int nvalue)
{
    int pos1 = skey / N1;
    hash_table_size[pos1]++;
    int num = hash_table_size[pos1];
    // cout<<num<<" "<<line<<endl;
    // if (skey == 392)
    // {
    //     cout << num << " " << line <<endl;
    // }
    // hash_table_insert_first(skey, nvalue);
    // return ;
    if (num < line)
    {
        hash_table_insert_first(skey, nvalue);
    }
    else if (num == line)
    {
        hash_table_insert_first(skey, nvalue);
        hash_2_init(pos1);
    }
    else
    {
        hash_table_insert_second(skey, nvalue);
    }
}

//重哈希
void ReHash()
{
}

//垃圾回收
void GC()
{
}

Hash3Node *hash_table_lookup_test(int skey)
{
    int pos1 = skey / N1;
    int pos2 = skey % N1 / N2;
    for (size_t i = 0; i < HASH_TABLE_MAX_SIZE; i++)
    {
        HashNode *pHead = hashTable[pos1];
        while (pHead)
        {
            if (pHead->mKey == pos2 * N2)
            {
                Hash3Node *pFind = pHead->p3Head;
                while (pFind)
                {
                    if (pFind->Key == skey)
                    {
                        return pFind;
                    }
                    pFind = pFind->p3Next;
                }
            }
            pHead = pHead->pNext;
        }
    }
    return NULL;
}

//查找
Hash3Node *hash_table_lookup(int skey)
{
    // cout<<"in lookup"<<endl;
    int pos1 = skey / N1;
    int pos2 = skey % N1 / N2;

    int target_mkey = pos2 * N2;
    // cout<<"skey:"<<skey<<" pos2:"<<pos2<<" target_mkey:"<<target_mkey<<endl;

    HashNode *pHead = hashTable[pos1];
    while (pHead)
    {
        // cout<<1<<endl;
        if (pHead->mKey == target_mkey)
        {
            Hash3Node *p3Head = pHead->p3Head;
            while (p3Head)
            {
                if (p3Head->Key == skey)
                {
                    return p3Head;
                }
                else
                {
                    p3Head = p3Head->p3Next;
                }
            }
            return NULL;
        }
        else if (pHead->mKey > target_mkey)
        {
            return NULL;
        }
        else
        {
            pHead = pHead->pNext;
        }
    }
    return NULL;
}

//范围查找
void hash_table_scan(int start, int end, int &sum)
{
    int pos1_start = start / N1;
    int pos1_end = end / N1;
    int pos2_start = start % N1 / N2;
    int pos2_end = end % N1 / N2;

    int target = binary_search_for_scan(pos1_start, pos2_start * N2);
    //纠正pos1_start 和 pos1_end
    if (target == -1)
    {
        // todo:当下一个bucket为空时；
        pos1_start += 1;
        pos2_start = 0;
    }
    else
    {
        pos2_start = target;
    }
    // cout << "pos1_start:" << pos1_start << endl;
    // cout << "pos1_end:" << pos1_end << endl;
    // cout << "pos2_start:" << pos2_start << endl;
    // cout << "pos2_end:" << pos2_end << endl;

    HashNode *pHead = hash_arrayfor2[pos1_start][pos2_start]->pLink;

    while (pos1_start != pos1_end)
    {
        bool flag[N2 + 1] = {false};
        Hash3Node *pScan = pHead->p3Head;
        Hash3Node *pScan_pre = pHead->p3Head;

        int f = 0;
        while (pScan != NULL)
        {
            if (f == 1)
            {
                pScan_pre = pScan_pre->p3Next;
            }
            // cout<<1<<endl;
            if (pScan->Key >= start && pScan->Key <= end && flag[(pScan->Key) % N2] == false)
            {
                // printf ("FIND: KEY->%d, VALUE->%d\n",pScan->Key, pScan->Value);
                flag[(pScan->Key) % N2] = true;
                sum++;
            }
            else if (flag[(pScan->Key) % N2] == true)
            {
                pScan_pre->p3Next = pScan->p3Next;
            }
            pScan = pScan->p3Next;

            // free(???)

            f = 1;
        }
        pHead = pHead->pNext;

        if (pHead == NULL)
        {
            // cout<<"NEXT"<<endl;
            pos1_start++;
            pHead = hash_arrayfor2[pos1_start][0]->pLink;
        }
        // free(flag);
    }
    // cout<<"LAST"<<endl;
    // cout<<pos1_end<<" "<<pos2_start<<endl;
    pos2_start = 0;
    pHead = hash_arrayfor2[pos1_end][pos2_start]->pLink;
    while (true)
    {
        Hash3Node *pScan = pHead->p3Head;
        bool flag[N2 + 1] = {false};
        // cout << pScan->Key << endl;
        while (pScan != NULL)
        {
            // cout<<1<<endl;
            if (pScan->Key >= start && pScan->Key <= end && flag[(pScan->Key) % N2] == false)
            {
                // printf ("FIND: KEY->%d, VALUE->%d\n",pScan->Key, pScan->Value);
                flag[(pScan->Key) % N2] = true;
                sum++;
            }
            else if (pScan->Key > end + N2)
            {
                return;
            }
            pScan = pScan->p3Next;
        }
        pHead = pHead->pNext;
        if (pHead == NULL)
        {
            return;
        }
    }
}

//释放内存
void hash_table_release()
{
    int i;
    for (i = 0; i < HASH_TABLE_MAX_SIZE; ++i)
    {
        if (hashTable[i] != NULL)
        {
            HashNode *pHead = hashTable[i];
            while (pHead)
            {
                if (pHead->p3Head != NULL)
                {
                    Hash3Node *pScan = pHead->p3Head;
                    while (pScan)
                    {
                        Hash3Node *pTemp = pScan;
                        pScan = pScan->p3Next;
                        if (pTemp)
                        {
                            free(pTemp);
                        }
                    }
                }
                HashNode *pTemp = pHead;
                pHead = pHead->pNext;
                if (pTemp)
                {
                    free(pTemp);
                }
            }
        }
    }
}

/* ============================主测试函数============================*/
#define MAX_STR_LEN 20
#define MIN_STR_LEN 10
void rand_str(char r[])
{
    int i;
    int len = MIN_STR_LEN + rand() % (MAX_STR_LEN - MIN_STR_LEN);
    for (i = 0; i < len - 1; ++i)
        r[i] = 'a' + rand() % ('z' - 'a');
    r[len - 1] = '\0';
}

int main(int argc, char **argv)
{
    // srand(time(NULL));
    // cout<<1<<endl;
    hash_table_init();
    // cout<<2<<endl;
    // int n = 10;

    // LARGE_INTEGER t1,t2,tc;
    // QueryPerformanceFrequency(&tc);
    // QueryPerformanceCounter(&t1);

    // for(int i = 0; i < 100000; i++)
    // {
    //     hash_table_insert(i, rand());
    // }

    ifstream insert_data("./data.txt", ios::in);
    int x, y;
    LARGE_INTEGER t11, t21, tc1;
    QueryPerformanceFrequency(&tc1);
    QueryPerformanceCounter(&t11);
    while (insert_data >> x >> y)
    {
        // cout<<"IN"<<endl;
        hash_tabel_insert(x, y);
        // cout<<sum<<endl;
    }
    QueryPerformanceCounter(&t21);
    double time1 = (double)(t21.QuadPart - t11.QuadPart) / (double)tc1.QuadPart;
    cout << "time = " << time1 << "" << endl; //输出时间（单位：
    for (int i = 0; i < HASH_TABLE_MAX_SIZE; i++)
    {
        if (hash_if_init[i] == false)
        {
            hash_2_init(i);
        }
    }
    // cout<<sum<<endl;

    // QueryPerformanceCounter(&t2);
    // double time=(double)(t2.QuadPart-t1.QuadPart)/(double)tc.QuadPart;
    // cout<<"time = "<<time<<"s"<<endl;  //输出时间（单位：ｓ）

    // while(n--)
    // {
    //     rand_str(str);
    //     hash_table_insert(str, n);
    // }
    // printf("\nBefore Insert\n\n");
    // hash_table_print();

    // printf("\nAftet Insert\n\n");
    // hash_table_print();
    // int s = 0;
    // hash_table_scan(100, 10000000, s);
    // hash_table_scan(100, 10000000, s);
    // hash_table_scan(100, 10000000, s);
    // hash_table_scan(100, 10000000, s);
    int sum = 0;
    LARGE_INTEGER t1, t2, tc;
    QueryPerformanceFrequency(&tc);
    QueryPerformanceCounter(&t1);
    // Hash3Node* res = hash_table_lookup(9735709);
    // if (res == NULL)
    // {
    //     cout << "NOT FIND" << endl;
    // }
    // else
    // {
    //     cout << res->Key << " " << res->Value << endl;
    // }

    
    hash_table_scan(1,1000, sum);
    hash_table_scan(1000,2000, sum);
    hash_table_scan(2000,3000, sum);
    hash_table_scan(3000,4000, sum);
    hash_table_scan(4000,5000, sum);
    hash_table_scan(1,5000, sum);
    hash_table_scan(1,1000, sum);
    hash_table_scan(1000,2000, sum);
    hash_table_scan(2000,3000, sum);
    hash_table_scan(3000,4000, sum);
    hash_table_scan(4000,5000, sum);
    hash_table_scan(1,5000, sum);
    // hash_table_scan(1,5000, sum);
    // hash_table_scan(1,5000, sum);
    // hash_table_scan(1,5000, sum);
    // hash_table_scan(1,5000, sum);
   
    QueryPerformanceCounter(&t2);
    double time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
    cout << "time = " << time << "" << endl; //输出时间（单位：ｓ）
    cout << sum << endl;
    // HashNode* pNode = hash_table_lookup(key1);
    // printf("\nQuery Result:%d\n\n", pNode->nValue);
    // pNode = hash_table_lookup(key2);
    // printf("\nQuery Result:%d\n\n", pNode->nValue);

    // printf("\nBefore Delete:\n\n");
    // hash_table_print();
    // hash_table_remove(key3);
    // printf("\nAfter Delete:\n\n");
    // hash_table_print();

    // int sum = 0;
    // for(int i = 0; i < HASH_TABLE_MAX_SIZE; i++)
    // {
    //     HashNode* pHead = hashTable[i];

    //     cout<<i<<endl;
    //     while (pHead)
    //     {
    //         printf("Key:%d, Value:%d\n", pHead->sKey, pHead->nValue);
    //         sum++;
    //         pHead = pHead->pNext;
    //     }
    //     cout<<endl;

    // }
    // cout<<sum<<endl;

    // cout<<hashTable[1]->pNext->p3Head->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->p3Next->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->p3Next->p3Next->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->p3Next->p3Next->p3Next->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->p3Next->p3Next->p3Next->p3Next->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->p3Next->p3Next->p3Next->p3Next->p3Next->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->Key<<endl;
    // cout<<hashTable[1]->pNext->p3Head->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->p3Next->Key<<endl;
    hash_table_release();
    return 0;
}