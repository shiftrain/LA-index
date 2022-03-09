#ifndef LA_INDEX_HPP
#define LA_INDEX_HPP

#define WORLD_SIZE  64
#define WORLD_MASK  63
#define WORLD_SHFIT 6
#define ENTRY_NUM   64
#define BMPS        4
#define DUMMY_SIZE  7
#define LAST_BMP    60


#define LAST_BMP_MASK   ((1LL << LAST_BMP) - 1)
#define bool            unsigned char

/*
 * value of a bit in bitmap:
 * 0 for exist
 * 1 for free
 *
 * split_flag:
 * 1 for on split
 */
typedef struct BUCKET {
    long min;
    long max;
    struct BUCKET *nxt;
    bool split_flag;
    unsigned long bitmap[BMPS];
    long keys[ENTRY_NUM];
    void *values[ENTRY_NUM];

#if DUMMY_SIZE > 0
    unsigned char bytes[DUMMY_SIZE];
#endif
} bucket;


typedef struct BUCKET_LIST {
    bucket *head;
    long length;
} bucket_list;







#endif //DALA_DALA_H