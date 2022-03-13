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

#define BMPS 32
#define BUCKET_1_NUM 1024
#define BUCKET_2_NUM 2
#define HASH_MODE BMPS*BUCKET_2_NUM
#define MAX 0x3f3f3f3f
#define MIN -0x3f3f3f3f


//TODO: CURVE for INSERT

using namespace std;

typedef struct BitMap_struct BitMap;
struct BitMap_struct {
    vector<int> _bits;
    int llen;
    BitMap_struct(int range = BMPS){
        this->_bits.resize((range >> 5) + 1, 0);;
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
    return key / HASH_MODE;
}

int binary_search(int pos, int key) {
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
            return middle;
        }
    }
    return middle;
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
                    // cout<<"1:"<<key<<endl;
                    split(B2);
                    B1_Table[B1_pos].len++;
                    // B1_Table[B1_pos].B2_head = B2;
                    // cout<<"SPLIT>head"<<B1_Table[B1_pos].B2_head<<endl;
                    if(B2 == B1_Table[B1_pos].B2_head)
                        B2->min = 0;
                }
                // cout<<"B2"<<B1_Table[B1_pos].B2_head<<endl;
                return ;
            }
        }
        
    }
    else {
        // if (key == 26){
        //     cout<<"xxxxxxxxx"<<endl;
        // }
        // cout<<"in2"<<endl;
        // cout<<key<<" B2->split_flag == 1"<<endl;
        while (B2) {
            if (B2->max >= 100000)
                B2->max = key;
            // cout<<B2->min<<" "<<B2->max<<" "<<key<<endl;
            if ((B2->min <= key && key <= B2->max) || (B2->next == NULL)){
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
                            // cout<<"in split"<<endl;
                            // cout<<"2:"<<key<<endl;
                            split(B2);
                            B1_Table[B1_pos].len++;
                            // B1_Table[B1_pos].B2_head = B2;
                            // cout<<"SPLIT>head"<<B1_Table[B1_pos].B2_head<<endl;
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


void insert_second(int key, int value) {
    int B1_pos = hash_func(key);
    // cout<<B1_Table[B1_pos].B2_head<< " "<<arr[B1_pos][0].LINK<<endl;
    if(!B1_Table[B1_pos].B2_head || !arr[B1_pos][0].LINK){
        insert_first(key, value);
        return ;
    }
    int target = binary_search(B1_pos, key);
    int mint = arr[B1_pos][target].min;
    if(key < mint && target != 0)
        target--;
    cout<<target<<endl;
    BUCKET_2 *B2 = arr[B1_pos][target].LINK;

    while (B2) {
        // if (B2->max >= 100000)
        //     B2->max = key;
        if ((B2->min <= key && key <= B2->max) || (B2->next == NULL)){
            for (int i = 0; i < BMPS; i++) {
                if (!B2->BM->Test(i)) {
                    B2->node_entry[i].skey = key;
                    B2->node_entry[i].nvalue = value;
                    B2->BM->Set(i);
                    B2->max = (key>B2->max)?key:B2->max;
                    B2->min = (key<B2->min)?key:B2->min;
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
        B2 = B2->next;
    }
}


void build_curve() {
    // cout<<"build_curve"<<endl;
    for (int i = 0; i < BUCKET_1_NUM; i++) {

        BUCKET_2 *head = B1_Table[i].B2_head;
        // cout<<head<<endl;
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


void query_test() {
    // cout << 1 << endl;
    int sum = 0;
    for (int i = 0; i < BUCKET_1_NUM; i++) {
        BUCKET_2 *head = B1_Table[i].B2_head;
        // cout<<"head"<<head<<endl;
        while (head) {
           for (int j = 0; j < BMPS; j++) {
               if (head->BM->_bits[i>>5]&(1<<(i&31))) {
                   printf("%d\n", head->node_entry[j].skey);
                   sum++;
               }
           }
           head = head->next;
       }
       
    }
    cout<<"sum:"<<sum<<endl;
}
void release() {
    // cout << 1 << endl;
    for (int i = 0; i < BUCKET_1_NUM; i++) {
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
    int target = binary_search(B1_pos, key);
    int mint = arr[B1_pos][target].min;
    
    // cout<<key<<"<?"<<mint<<endl;
    if (target == 0 && key < mint) {
        printf("1,CANNOT FIND KEK:%d\n", key);
        return;
    }
    
    else if(key < mint)
        target--;
    // cout<<B1_pos<<" "<<target<<endl;
    BUCKET_2 *head = arr[B1_pos][target].LINK;
    while (head) {
        // cout<<head->min<<" "<<head->max<<endl;
        if (head->min <= key && key <= head->max) {
            // cout<<"if:"<<head<<endl;
            for (int i = 0; i < BMPS; i++) {
                if (head->node_entry[i].skey == key && head->BM->Test(i)) {
                    printf("FIND KEK:%d, VALUE:%d\n", key, head->node_entry[i].nvalue);
                    return;
                }
            }
            printf("2,CANNOT FIND KEK:%d\n", key);
            return;
        }
        else if (head->min > key) {
            cout<<"else if"<<head->min<<endl;
            printf("3,CANNOT FIND KEK:%d\n", key);
            return;
        }
        else {
            head = head->next;
            cout<<head<<endl;
        }
        
    }
}

void range_query(int start, int end, int &sum){
    int pos_start = hash_func(start);
    int target_start = binary_search(pos_start, start);
    int mint_start = arr[pos_start][target_start].min;
    if(start < mint_start && target_start != 0)
        target_start--;
    int pos_end = hash_func(end);
    int target_end = binary_search(pos_end, end);
    int mint_end = arr[pos_end][target_end].min;
    if(end < mint_end && target_end != 0)
        target_end--;

    // cout<<pos_start<<" "<<target_start<<endl;
    // cout<<pos_end<<" "<<target_end<<endl;

    BUCKET_2 *head_start = arr[pos_start][target_start].LINK;
    BUCKET_2 *head_end = arr[pos_end][target_end].LINK;
    // cout<<head_start<<" "<<head_end<<endl;
    // cout<<1<<endl;
    for(int i = 0 ; i < BMPS; i++) 
        if (head_start->BM->_bits[i>>5]&(1<<(i&31)) && start <= head_start->node_entry[i].skey){
            // printf("FIND:KEY %d, VALUE %d\n", head_start->node_entry[i].skey, head_start->node_entry[i].nvalue); 
            sum++;
        }
    // cout<<head_start<<endl;
    head_start = head_start->next;
    // cout<<head_start<<endl;
    // cout<<2<<endl;
    // cout<<pos_start<<" "<<pos_end<<endl;
    while (pos_start != pos_end) {
        // cout<<head_start<<" "<<head_end<<endl;
        if(!head_start){
            head_start = B1_Table[++pos_start].B2_head;
            // cout<<"NULL"<<endl;
            continue;
        }
        for(int i = 0 ; i < BMPS; i++){
            if (head_start->BM->_bits[i>>5]&(1<<(i&31))){
                // printf("FIND:KEY %d, VALUE %d\n", head_start->node_entry[i].skey, head_start->node_entry[i].nvalue); 
                sum++;
            }
        }
        // cout<<"endfor"<<endl;
        head_start = head_start->next; 
        // cout<<"endnext"<<endl;
    }
    // cout<<3<<endl;
    // cout<<pos_start<<" "<<pos_end<<endl;
    head_start = arr[pos_end][0].LINK;
    // cout<<arr[pos_end][0].LINK<<endl;
    // cout<<head_start<<" "<<head_end<<endl;
    while (head_start != head_end){
        for(int i = 0 ; i < BMPS; i++){
            // cout<<"test"<<head_start->BM->Test(i)<<endl;
            if (head_start->BM->_bits[i>>5]&(1<<(i&31))){
                // printf("FIND:KEY %d, VALUE %d\n", head_start->node_entry[i].skey, head_start->node_entry[i].nvalue); 
                sum++;
            }
        }
        head_start = head_start->next; 

    }
    // cout<<4<<endl;
    
    while (head_end) {
        // cout<<"in4 "<<head_end->BM<<endl;
        BitMap *BM;
        BM = new BitMap(BMPS);
        BM = head_end->BM;

        NODE *node;
        node = (NODE*)malloc(sizeof(NODE)*BMPS);
        node = head_end->node_entry;
        for(int i = 0 ; i < BMPS; i++) {
            // cout<<BM<<endl;
            // cout<<"Test:"<<BM->Test(i)<<" key:"<<node[i].skey<<endl;
            if (head_start->BM->_bits[i>>5]&(1<<(i&31)) && node[i].skey != 0 && node[i].skey <= end) {
                // printf("FIND:KEY %d, VALUE %d\n", node[i].skey, node[i].nvalue); 
                sum++;
            }
        }
        head_end = head_end->next;
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
    ifstream insert_data("./data.txt", ios::in);
    int x, y;
    int i = 0;
    LARGE_INTEGER t11, t21, tc1;
    QueryPerformanceFrequency(&tc1);
    QueryPerformanceCounter(&t11);
    while (insert_data >> x >> y) {
        insert_first(x, y);
        // if(++i >= 2000)
        //     break;
    }
    build_curve();   
    // while (insert_data >> x >> y) {
    //     insert_second(x, y);
    // }
    // cout<<"xxx"<<endl; 
    
    QueryPerformanceCounter(&t21);
    double time1 = (double)(t21.QuadPart - t11.QuadPart) / (double)tc1.QuadPart;
    cout << "time = " << time1 << "" << endl; //输出时间（单位：
    // query_test();
    int sum = 0;
    LARGE_INTEGER t1, t2, tc;
    QueryPerformanceFrequency(&tc);
    QueryPerformanceCounter(&t1);

    range_query(1,1000, sum);
    range_query(1000,2000, sum);
    range_query(2000,3000, sum);
    range_query(3000,4000, sum);
    range_query(4000,5000, sum);
    range_query(1,5000, sum);
    range_query(1,1000, sum);
    range_query(1000,2000, sum);
    range_query(2000,3000, sum);
    range_query(3000,4000, sum);
    range_query(4000,5000, sum);
    range_query(1,5000, sum);
    // range_query(1,5000, sum);

    QueryPerformanceCounter(&t2);
    double time = (double)(t2.QuadPart - t1.QuadPart) / (double)tc.QuadPart;
    cout << "time = " << time << "" << endl; //输出时间（单位：
    cout << "sum = " << sum << endl; 
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
    // query(102);
    release();
}


