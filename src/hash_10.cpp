#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <time.h>  
#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>

using namespace std;

#define HASH_TABLE_MAX_SIZE 10000
typedef struct HashNode_Struct HashNode;  
struct HashNode_Struct  {  
    int sKey;  
    int nValue;  
    HashNode* pNext;  
};  //哈希表数据结构 

HashNode* hashTable[HASH_TABLE_MAX_SIZE]; 
int hash_table_size;  //哈希表中键值对的数量 

//初始化哈希表 
void hash_table_init()  
{  
    hash_table_size = 0;  
    memset(hashTable, 0, sizeof(HashNode*) * HASH_TABLE_MAX_SIZE);
    //memset(void *s,int c,size_t n); 
    //将s中后n个字节换成c所代表的内容 
    //该函数是对较大结构体或数组进行清零操作的一种最快的方法 
}  


//去符号化哈希表  
unsigned int hash_table_hash_str(const char* skey)  
{  //无符号unsigned能保存2倍与有符号类型的正整型数据 
    const signed char *p = (const signed char*)skey; //常量 
    unsigned int h = *p;  
    if(h)
    {  
        for(p += 1; *p != '\0'; ++p)  
            h = (h << 5) - h + *p;  
    }  
    return h;  
}
//插入 
#define N 10
void hash_table_insert(int skey, int nvalue)  
{  
    int pos = skey/N % HASH_TABLE_MAX_SIZE;  
    //用于解决冲突，pos为哈希函数 
    HashNode* pHead = hashTable[pos];
    if(!pHead)
    {
        // cout<<1<<endl;
        HashNode* pNewNode = (HashNode*)malloc(sizeof(HashNode));  
        memset(pNewNode, 0, sizeof(HashNode)); 
        pNewNode->sKey = skey;  
        pNewNode->nValue = nvalue; 

        pHead = pNewNode;
        hashTable[pos] = pHead;
        hash_table_size++;
        return ;
    }
    else if(pHead->sKey > skey)
    {
        HashNode* pNewNode = (HashNode*)malloc(sizeof(HashNode));  
        memset(pNewNode, 0, sizeof(HashNode)); 
        pNewNode->sKey = skey;  
        pNewNode->nValue = nvalue; 

        HashNode* tmp;
        tmp = pHead;
        pHead = pNewNode;
        pHead->pNext = tmp;
        hashTable[pos] = pHead;
        hash_table_size++;

        return ;
    }
    while(pHead)
    {  
        if(pHead->sKey == skey)
        {  
            pHead->nValue = nvalue;
            return ;  
        }
        else if(pHead->pNext && pHead->sKey < skey && pHead->pNext->sKey > skey)
        {

            HashNode* pNewNode = (HashNode*)malloc(sizeof(HashNode));  
            memset(pNewNode, 0, sizeof(HashNode)); 
            pNewNode->sKey = skey;  
            pNewNode->nValue = nvalue; 

            pNewNode->pNext = pHead->pNext;
            pHead->pNext = pNewNode;
            hash_table_size++;
            return ;
        }
        if(!pHead->pNext)
        {
            HashNode* pNewNode = (HashNode*)malloc(sizeof(HashNode));  
            memset(pNewNode, 0, sizeof(HashNode)); 
            pNewNode->sKey = skey;  
            pNewNode->nValue = nvalue; 

            pHead->pNext = pNewNode;
            // pNewNode->pNext = NULL;
            hash_table_size++;
            return ;
        }
        pHead = pHead->pNext;  
    } 
}  

//查找 
HashNode* hash_table_lookup(int skey)  
{  
    unsigned int pos = skey/N % HASH_TABLE_MAX_SIZE;  

    if(hashTable[pos])  
    {  
        HashNode* pHead = hashTable[pos];  
        while(pHead)  
        {  
            if(skey == pHead->sKey)  
                return pHead;//查找成功 

            pHead = pHead->pNext;  
        }  
    }  
    return NULL;  
} 

//范围查找
void hash_table_scan(int start, int end, int &sum)
{   
    for(int skey = start; skey <= end; skey = N*(skey/N + 1))
    {
        int pos = skey/N % HASH_TABLE_MAX_SIZE;
        int tmp_start = skey;
        int tmp_end = N*(skey/N + 1) - 1 >= end ? end : N*(skey/N + 1) - 1;
        if(hashTable[pos])
        {
            HashNode* pHead = hashTable[pos];
            while(pHead && pHead->sKey <= tmp_end)
            {   
                // cout<<pHead->sKey<<endl;
                if(pHead->sKey >= tmp_start && pHead->sKey <= tmp_end)
                {
                    // cout<<pHead->sKey<<endl;
                    for(int i = 0; i < N*(skey/N + 1) - skey; i++)
                    {
                        if(pHead && pHead->sKey >= tmp_start && pHead->sKey <= tmp_end)
                        {
                            // printf("FIND: Key:%d, Value:%d, hash_pos:%d\n", pHead->sKey, pHead->nValue);
                            sum++;
                        }
                        else if(!pHead || pHead->sKey > tmp_end)
                        {
                            break;
                        }
                        pHead = pHead->pNext;
                    }
                    break;
                }
                pHead = pHead->pNext;

            }
            
        }
    }
}

//释放内存 
void hash_table_release()  
{  
    int i;  
    for(i = 0; i < HASH_TABLE_MAX_SIZE; ++i)  
    {  
        if(hashTable[i])  
        {  
            HashNode* pHead = hashTable[i];  
            while(pHead)  
            {  
                HashNode* pTemp = pHead;  
                pHead = pHead->pNext;  
                if(pTemp)  
                {  
                    free(pTemp);  
                }  
                //逐个释放 
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
    for(i = 0; i < len - 1; ++i)  
        r[i] = 'a' + rand() % ( 'z' - 'a');  
    r[len - 1] = '\0';  
}  

int main(int argc, char** argv)  
{  
    srand(time(NULL));  
    hash_table_init();     
    int n = 10;  

    // LARGE_INTEGER t1,t2,tc;
    // QueryPerformanceFrequency(&tc);
    // QueryPerformanceCounter(&t1);

    // for(int i = 0; i < 100000; i++)
    // {
    //     hash_table_insert(i, rand());
    // }
    ifstream insert_data("./data.txt", ios::in);
    int x, y;
    while (insert_data >> x >> y)
    {
        hash_table_insert(x, y);
    }
    

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

    
    LARGE_INTEGER t1,t2,tc;
    QueryPerformanceFrequency(&tc);
    QueryPerformanceCounter(&t1);
    int sum = 0;
    hash_table_scan(100, 10000000, sum);
    hash_table_scan(100, 10000000, sum);
    hash_table_scan(100, 10000000, sum);
    hash_table_scan(100, 10000000, sum);
    cout << sum <<endl;
    QueryPerformanceCounter(&t2);
    double time=(double)(t2.QuadPart-t1.QuadPart)/(double)tc.QuadPart; 
    cout<<"time = "<<time<<"s"<<endl;  //输出时间（单位：ｓ）

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

    hash_table_release();  
    return 0;  
}  