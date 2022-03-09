#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <map>
// #include "LA-index.hpp"
// #include "bitmap.hpp"

#define BMPS 1LL<<6
#define BUCKET_1_NUM 1LL<<16
#define BUCKET_2_NUM 1LL<<10
#define NODE_NUM 1LL<<6
#define MAX 0x3f3f3f3f
#define MIN -0x3f3f3f3f


class BitMap
{
public:
    BitMap(int range):_bits(range >> 5 + 1, 0), len(0){}
    void Set(int x) {
        int index = x >> 5;//确定哪个数据（区间）
        int temp = x & 31;//确定哪个Bit位
        _bits[index] |= (1 << temp);//位操作即可
        len++;
    }
    void Reset(int x) {
        int index = x >> 5;//确定哪个数据（区间）
        int temp = x & 31;//确定哪个Bit位
        _bits[index] |= (1 << temp);//位操作即可
        len++;
    }c
    bool Test(int x) {
        int index = x >> 5;
        int temp = x & 31;
        if (_bits[index] & (1 << temp))
            return 1;
        else
            return 0;
    }
    int Getlen() {
        return len;
    }
    

private:
    vector<int> _bits;
    long long len;
};


using namespace std;
typedef long long int LL;

struct NODE
{
    LL skey;
    LL nvalue;
};

struct BUCKET_2 {
    LL min;
    LL max;
    BUCKET_2 *next;
    bool split_flag;
    BitMap *BM = new BitMap(BMPS);
    NODE *node_entry[BMPS];
};

struct ARRAY {
    LL min;
    BUCKET_2 *LINK;
};

struct BUCKET_1 {
    BUCKET_2 *B2_head;
    LL len;
    ARRAY *array = new ARRAY;
};
BUCKET_1 *B1_Table[BUCKET_1_NUM];

void init_B1() {
    memset(B1_Table, 0, sizeof(B1_Table) * BUCKET_1_NUM);
}

LL hash_func(LL key) {
    return key / BUCKET_1_NUM;
}

void init_B2(BUCKET_2 *B2, LL key, LL value) {
    B2->min = B2->max = key;

    NODE *node;
    memset(node, 0, sizeof(node));
    node->skey = key;
    node->nvalue = value;
    B2->node_entry[0] = node;
    B2->BM->Set(0);
    B2->split_flag = 0;
}

void split(BUCKET_2 *B2) {
    int mid = (B2->max + B2->min)/2;
    BUCKET_2 *NEW_B2S;
    memset(NEW_B2S, 0, sizeof(NEW_B2S));
    for(int i = 0, j = 0; i < BUCKET_2_NUM; i++) {
        if(B2->node_entry[i]->skey >= mid){
            NEW_B2S->node_entry[j] = B2->node_entry[i];
            NEW_B2S->BM->Set(j);
            B2->BM->Reset(i);
        }
    }
    B2->max = mid;
    NEW_B2S->min = mid;

    B2->split_flag = 1;
    NEW_B2S->split_flag = 1;
    NEW_B2S->next = B2->next;
    B2->next = NEW_B2S;

    if (NEW_B2S->next)
        NEW_B2S->max = NEW_B2S->next->min;
    else
        NEW_B2S->max = MAX;
}

void insert_first(LL key, LL value) {
    LL B1_pos = hash_func(key);
    B1_Table[B1_pos]->len++;

    if(!B1_Table[B1_pos]->B2_head) {
        BUCKET_2 *NEW_B2;
        memset(NEW_B2, 0, sizeof(NEW_B2));
        init_B2(NEW_B2, key, value);
        return ;
    } 

    BUCKET_2 *B2 = B1_Table[B1_pos]->B2_head;
    if (!B2->split_flag){
        for (int i = 0; i < BUCKET_2_NUM; i++) {
            if (!B2->BM->Test(i)) {
                B2->node_entry[i]->skey = key;
                B2->node_entry[i]->nvalue = value;
                B2->BM->Set(i);
                B2->max = (key>B2->max)?key:B2->max;
                B2->min = (key<B2->min)?key:B2->min;
                if (B2->BM->Getlen() == BMPS){
                    split(B2);
                    if(B2 == B1_Table[B1_pos]->B2_head)
                        B2->min = MIN;
                }
                return ;
            }
        }
    }
    else {
        while (B2) {
            if (B2->min <= key && key <= B2->max){
                for (int i = 0; i < BUCKET_2_NUM; i++) {
                    if (!B2->BM->Test(i)) {
                        B2->node_entry[i]->skey = key;
                        B2->node_entry[i]->nvalue = value;
                        B2->BM->Set(i);
                        B2->max = (key>B2->max)?key:B2->max;
                        B2->min = (key<B2->min)?key:B2->min;
                        if (B2->BM->Getlen() == BMPS){
                            split(B2);
                            if(B2 == B1_Table[B1_pos]->B2_head)
                                B2->min = MIN;
                        }
                    }
                }
            }
            B2 = B2->next;
        }
    }     
}

void build_curve() {
    for (int i = 0; i <= BUCKET_1_NUM; i++) {
        LL l = B1_Table[i]->len;
        BUCKET_2 *head = B1_Table[i]->B2_head;
        for (int j = 0; j < l; j ++, head=head->next) {
            B1_Table[i]->array[j].LINK = head;
            B1_Table[i]->array[j].min = head->min;
        }
    }
}

LL binary_search(LL pos, LL key, LL &res) {
    
    LL left = 0;
    LL right = B1_Table[pos]->len - 1;
    LL middle;
    LL mint = B1_Table[pos]->array[middle].min;
    while (left <= right) {
        middle = left + ((right - left) / 2);
        if (mint > key)
        {
            right = middle - 1;
        }
        else if (mint < key)
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

void query(LL key) {
    LL B1_pos = hash_func(key);
    LL res;
    LL target = binary_search(B1_pos, key, res);
    LL mint = B1_Table[B1_pos]->array[target].min;
    if (!res) {
        if (target == 0 && key < mint) {
            printf("CANNOT FIND KEK:%d", key);
            return;
        }
        else if (key < mint)
            target--;
    }
    BUCKET_2 *head = B1_Table[B1_pos]->array[target].LINK;
    while (head) {
        if (head->min <= key && key <= head->max){
            for (int i = 0; i < BUCKET_2_NUM; i++) {
                if (head->node_entry[i]->skey == key){
                    printf("FIND KEK:%d, VALUE:%d", key, head->node_entry[i]->nvalue);
                    return;
                }
            }
        }
        else if (head->min > key) {
            printf("CANNOT FIND KEK:%d", key);
            return;
        }
        else
            head = head->next;
    }

}



int main () {
    insert_first(100, 1000);
    insert_first(1020, 5452);
    insert_first(101, 434);
    insert_first(22, 13213);
    insert_first(44, 2);
    insert_first(333, 4);
    build_curve();
    query(44);
}


