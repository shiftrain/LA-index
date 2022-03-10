#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <map>
#include <vector>
// #include "LA-index.hpp"
// #include "bitmap.hpp"

#define BMPS 8
#define BUCKET_1_NUM 32
#define BUCKET_2_NUM BUCKET_1_NUM/BMPS
#define NODE_NUM 64
#define MAX 0x3f3f3f3f
#define MIN -0x3f3f3f3f

using namespace std;

typedef struct BitMap_struct BitMap;
struct BitMap_struct {
    vector<int> _bits;
    int llen;
    BitMap_struct(int range = BMPS){
        this->_bits.resize((range >> 5) + 1);;
        this->llen = 0;
        // cout<<"bit"<<endl;
    }
    void Set(int x) {
        int index = x >> 5;//确定哪个数据（区间）
        int temp = x & 31;//确定哪个Bit位
        _bits[index] |= (1 << temp);//位操作即可
        llen++;
    }
    void Reset(int x) {
        int index = x >> 5;
        int temp = x & 31;
        _bits[index] &= ~(1 << temp);//取反
        llen--;
    }
    bool Test(int x) {
        int index = x >> 5;
        int temp = x & 31;
        if (_bits[index] & (1 << temp))
            return 1;
        else
            return 0;
    }
    int Getlen() {
        return llen;
    }
};
// typedef long long int LL;

typedef struct NODE_struct NODE;
struct NODE_struct {
    int skey = 0;
    int nvalue = 0;
};

// int x = 64;

typedef struct BUCKET_2_struct BUCKET_2;
struct BUCKET_2_struct {
    int min;
    int max;
    BUCKET_2 *next;
    bool split_flag;
    BitMap *BM;
    NODE *node_entry;
};

typedef struct ARRAY_struct ARRAY;
struct ARRAY_struct {
    int min;
    BUCKET_2 *LINK;
};

typedef struct BUCKET_1_struct BUCKET_1;
struct BUCKET_1_struct {
    BUCKET_2 *B2_head;
    int len = 0;
    // ARRAY *array[BMPS];  
};
BUCKET_1 *B1_Table;
ARRAY **arr = (ARRAY**)malloc(sizeof(ARRAY*)*BUCKET_1_NUM);
// BUCKET_1 **B1_Table;

void init_B1() {
    // cout<<1<<endl;
    // cout<<sizeof(B1_Table)<<endl;
    B1_Table = (BUCKET_1*)malloc(sizeof(BUCKET_1)*BUCKET_1_NUM);
    for (int i = 0; i < BUCKET_1_NUM; i++){
        B1_Table[i].len = 0;
        B1_Table[i].B2_head = NULL;
    }
    // cout<<sizeof(B1_Table)<<endl;
    for (int i = 0; i < BUCKET_1_NUM; i++)
        arr[i] = (ARRAY *)malloc(sizeof(ARRAY)*BUCKET_2_NUM);
    // cout<<2<<endl;
}

int hash_func(int key) {
    // cout<<"key "<<key<<endl;
    // cout<<"BUCKET_1_NUM "<<BUCKET_1_NUM<<endl;
    // cout<<"key / BUCKET_1_NUM "<<key / BUCKET_1_NUM<<endl;
    return key / (BUCKET_1_NUM);
}

void split(BUCKET_2 *B2) {
    // cout<<"split-in!!!!!!!!"<<endl;
    int mid = (B2->max + B2->min)/2;
    // cout<<"split "<<B2->min<<" "<<B2->max<<" "<<mid<<endl;
    // cout<<&B2<<endl;
    BUCKET_2 *NEW_B2S;
    NEW_B2S = (BUCKET_2 *)malloc(sizeof(BUCKET_2));
    NEW_B2S->node_entry = (NODE *)malloc(sizeof(NODE)*BMPS);
    NEW_B2S->BM = new BitMap(BMPS);
    int j = 0;
    for(int i = 0; i < BMPS; i++) {
            // cout<<1<<endl;
        if(B2->node_entry[i].skey >= mid){
                
            NEW_B2S->node_entry[j].skey = B2->node_entry[i].skey;
            NEW_B2S->node_entry[j].nvalue = B2->node_entry[i].nvalue;//cout<<1<<endl;
            // cout<<"NEW_B2S:"<<NEW_B2S->BM->Test(j)<<endl;
            NEW_B2S->BM->Set(j);//cout<<2<<endl;
            // cout<<"NEW_B2S:"<<NEW_B2S->BM->Test(j)<<endl;
            // cout<<"B2:"<<B2->BM->Test(i)<<endl;
            B2->BM->Reset(i);
            // cout<<"B2:"<<B2->BM->Test(i)<<endl;
            j++;
        }
    }
    NEW_B2S->min = mid;

    B2->split_flag = 1;
    NEW_B2S->split_flag = 1;
    NEW_B2S->next = B2->next;
    B2->next = NEW_B2S;

    if (NEW_B2S->next)
        NEW_B2S->max = NEW_B2S->next->min;
    else
        NEW_B2S->max = MAX;
    B2->max = mid;
    // cout<<"split-out!!!!!!!!"<<endl;
}

void insert_first(int key, int value) {
    // cout<<"insert"<<endl;
    int B1_pos = hash_func(key);
    // cout<<"hash_func"<<endl; 
    // cout<<"pos"<<B1_pos<<endl;
    // cout<<B1_Table[B1_pos]->len<<endl;
    
    // cout<<B1_Table[B1_pos]->len<<endl;
    // cout<<"len++"<<endl;
    if(!B1_Table[B1_pos].B2_head) {
        // cout<<"NULL"<<endl;
        BUCKET_2 *NEW_B2;
        NEW_B2 = (BUCKET_2*)malloc(sizeof(BUCKET_2));
        NEW_B2->node_entry = (NODE *)malloc(sizeof(NODE)*BMPS);
        NEW_B2->min = NEW_B2->max = key;

        NEW_B2->node_entry[0].skey = key;
        NEW_B2->node_entry[0].nvalue = value;
        NEW_B2->BM = new BitMap(BMPS);
        // cout<<"end"<<endl;
        // cout<<NEW_B2->BM.Test(0);
        NEW_B2->BM->Set(0);
        // cout<<NEW_B2->BM.Test(0);
        NEW_B2->split_flag = 0;
        NEW_B2->next = NULL;
        // init_B2(NEW_B2, key, value);
        B1_Table[B1_pos].B2_head = NEW_B2;
        B1_Table[B1_pos].len++;
        // cout<<"empty1"<<endl;
        return ;
    } 
    // cout<<"empty2"<<endl;
    BUCKET_2 *B2 = B1_Table[B1_pos].B2_head;
    // cout<<B2->min<<endl;
    if (!B2->split_flag){
        // cout<<"in1"<<endl;
        for (int i = 0; i < BMPS; i++) {
            if (!B2->BM->Test(i)) {
                // cout<<B2->BM->Test(i)<<endl;
                // cout<<B2->node_entry[i].skey<<endl;
                B2->node_entry[i].skey = key;
                B2->node_entry[i].nvalue = value;
                // cout<<"in2"<<endl;
                B2->BM->Set(i);
                
                B2->max = (key>B2->max)?key:B2->max;
                B2->min = (key<B2->min)?key:B2->min;
                // cout<<B2->min<<" "<<B2->max<<endl;
                // cout<<"Getlen():"<<B2->BM->Getlen()<<endl;
                if (B2->BM->Getlen() == BMPS){
                    split(B2);
                    B1_Table[B1_pos].len++;
                    if(B2 == B1_Table[B1_pos].B2_head)
                        B2->min = 0;
                }
                return ;
            }
        }
    }
    else {
        // cout<<key<<" B2->split_flag == 1"<<endl;
        while (B2) {
            if (B2->min <= key && key <= B2->max){
                for (int i = 0; i < BMPS; i++) {
                    if (!B2->BM->Test(i)) {
                        // cout<<"insert"<<endl;
                        B2->node_entry[i].skey = key;
                        B2->node_entry[i].nvalue = value;
                        B2->BM->Set(i);
                        B2->max = (key>B2->max)?key:B2->max;
                        B2->min = (key<B2->min)?key:B2->min;
                        // cout<<B2->min<<" "<<B2->max<<endl;
                        if (B2->BM->Getlen() == BMPS){
                            cout<<"in split"<<endl;
                            split(B2);
                            B1_Table[B1_pos].len++;
                            if(B2 == B1_Table[B1_pos].B2_head)
                                B2->min = 0;
                        }
                        return ;
                    }
                }
            }
            B2 = B2->next;
        }
    }     
}

void build_curve() {
    // cout<<"build_curve"<<endl;
    for (int i = 0; i < BUCKET_1_NUM; i++) {

        BUCKET_2 *head = B1_Table[i].B2_head;
        // cout<<hash_func(2000)<<endl;
        // cout<<head->min<<endl;
        // cout<<head->max<<endl;
        // cout<<head->split_flag<<endl;
        int j = 0;
        while (head) {
            // cout<<"j"<<j<<endl;
            arr[i][j].min = head->min;
            arr[i][j].LINK = head;
            // cout<<i<<endl;
            j++;
            head = head->next;
        }
    }
    // cout<<"end build_curve"<<endl;

}

int binary_search(int pos, int key, int &res) {
    int left = 0;
    int right = B1_Table[pos].len - 1;
    // cout<<left<<"<->"<<right<<endl;
    int middle;
    int mint;
    
    // cout<<"key:"<<key<<endl;
    while (left <= right) {
        middle = left + ((right - left) / 2);
        mint = arr[pos][middle].min;
        // maxt = arr[pos][middle].max;
        if (mint > key)
            right = middle - 1;
        else if (mint < key)
            left = middle + 1;
        else {
            res = 1;
            return middle;
        }
    }
    res = -1;
    return middle;
}

void query_test() {
    // cout << 1 << endl;
    for (int i = 0; i <= BUCKET_1_NUM; i++) {
        BUCKET_2 *head = B1_Table[i].B2_head;
        while (head) {
           for (int j = 0; j < BMPS; j++) {
               if (head->BM->Test(j) == 1) {
                   printf("%d\n", head->node_entry[j].skey);
               }
           }
           head = head->next;
       }
       
    }
}
void release() {
    // cout << 1 << endl;
    for (int i = 0; i <= BUCKET_1_NUM; i++) {
        BUCKET_2 *head = B1_Table[i].B2_head;
        while (head) {
           BUCKET_2 *temp = head;
           head = head->next;
           free(temp);
       }
       
    }
}

void query(int key) {
    int B1_pos = hash_func(key);
    // cout<<"B1_pos "<<B1_pos<<endl;
    int res;
    int target = binary_search(B1_pos, key, res);
    // cout<<"target "<<target<<" "<<res<<endl;
    int mint = arr[B1_pos][target].min;
    // cout<<"key"<<key<<"mint"<<mint<<endl;
    // cout<<1<<endl;
    if (target == 0 && key < mint) {
        printf("1,CANNOT FIND KEK:%d\n", key);
        return;
    }
    else if(key < mint)
        target--;

    // cout<<"target2 "<<target<<" "<<endl;
    // cout<<1<<endl;
    BUCKET_2 *head = arr[B1_pos][target].LINK;
    while (head) {
        if (head->min <= key && key <= head->max){
            for (int i = 0; i < BMPS; i++) {
                if (head->node_entry[i].skey == key){
                    printf("FIND KEK:%d, VALUE:%d\n", key, head->node_entry[i].nvalue);
                    return;
                }
            }
        }
        else if (head->min > key) {
            printf("2,CANNOT FIND KEK:%d\n", key);
            return;
        }
        else
            head = head->next;
    }

}



int main () {
    // cout<<sizeof(B1_Table)*BUCKET_1_NUM<<endl;
    init_B1();
    // cout<<hash_func(100)<<endl;
    // insert_first(100, 1000);
    // cout<<B1_Table[hash_func(2000)].B2_head->node_entry[0].skey<<endl;
    // cout<<B1_Table[hash_func(2000)].B2_head->node_entry[0].nvalue<<endl;
    cout<<endl;    
    insert_first(1, 532);
    insert_first(2, 5213);
    insert_first(3, 15);
    insert_first(4, 534);
    insert_first(5, 15);
    insert_first(6, 1);
    insert_first(7, 32);
    insert_first(8, 43);
    insert_first(219, 546);
    insert_first(229, 65);
    insert_first(49, 5);
    insert_first(91, 767);
    insert_first(23, 22);
    insert_first(39, 11);
    insert_first(13, 33);
    insert_first(55, 22);


    build_curve();
    // query_test();

    cout<<endl;
    query(39);

    // cout<<endl;
    // cout<<B1_Table[0].B2_head->BM->llen<<endl;
    // cout<<"min:"<<arr[0][0].min<<endl;
    // for(int i = 0; i < 3;)
    //     if(B1_Table[0].B2_head->BM->Test(i)==1)
    //         cout<<B1_Table[0].B2_head->node_entry[i++].skey<<" ";
    // cout<<endl;
    
    // cout<<B1_Table[0].B2_head->next->BM->llen<<endl;
    // cout<<"min:"<<arr[0][1].min<<endl;
    // for(int i = 0; i < 6;)
    //     if(B1_Table[0].B2_head->next->BM->Test(i)==1)
    //         cout<<B1_Table[0].B2_head->next->node_entry[i++].skey<<" ";
    release();
}


