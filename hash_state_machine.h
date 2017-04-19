#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <nslb_log.h>
#include <error.h>
#include <errno.h>


#define HASH_MAX_INPUT 36 
#define MAX_HASH_TABLE_ROW (HASH_MAX_INPUT+1)
#define HASH_TABLE_INIT_SIZE 1024

typedef struct o1_hash_table{
 int  *state_machine[MAX_HASH_TABLE_ROW];
 unsigned int next_state;
 unsigned int max_state;
 unsigned int delta_state;
 unsigned int hash_index;
 void (*log)(int level , char * file , int line , char *fname,char *format,...);
}nslbHashTable;


#ifndef _FL_
#define _FL_  __FILE__, __LINE__
#endif
#ifndef _FLN_
#define _FLN_  __FILE__, __LINE__, (char *)__FUNCTION__
#endif


int hash_table_init(nslbHashTable *hash_table);
int hash_table_free(nslbHashTable *hash_table);
int hash_table_add(nslbHashTable *hash_table , char * entry , int len);
int hash_table_get(nslbHashTable *hash_table , char * entry, int len);
unsigned long hash_table_size(nslbHashTable *hash_table);


/*Only for NSLB_O1_HASH_MACHINE*/

#ifdef _NSLB_DEF_HASH_MACHINE

#define INPUT_MAP(x) (input_map[x])

#define NSLB_HASH_DEBUG_LOG(X,...) \
{\
if(hash_table && hash_table->log)\
  hash_table->log(X,_FLN_,__VA_ARGS__);\
}

#define NSLB_HASH_ERROR_LOG(...)  NSLB_HASH_DEBUG_LOG(0,__VA_ARGS__)
#define NSLB_HASH_DEBUG_LOG1(...) NSLB_HASH_DEBUG_LOG(1,__VA_ARGS__)
#define NSLB_HASH_DEBUG_LOG2(...) NSLB_HASH_DEBUG_LOG(2,__VA_ARGS__)
#define NSLB_HASH_DEBUG_LOG3(...) NSLB_HASH_DEBUG_LOG(3,__VA_ARGS__)
#define NSLB_HASH_DEBUG_LOG4(...) NSLB_HASH_DEBUG_LOG(4,__VA_ARGS__)

//Indexing ASCII characters
int input_map[] = {
        -1, // 0 Index 
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,//31 Index
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
         0, //48 Index Zero
         1,
         2,
         3,
         4,
         5,
         6,
         7,
         8,
         9,
        -1,//58 Index 
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        10,//65 Index 'A' 
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        30,
        31,
        32,
        33,
        34,
        35,//90 Index 'Z' 
        -1,
        -1,
        -1,
        -1,
        -1,
        -1,
        10, //97 Index 'a' 
        11,
        12,
        13,
        14,
        15,
        16,
        17,
        18,
        19,
        20,
        21,
        22,
        23,
        24,
        25,
        26,
        27,
        28,
        29,
        30,
        31,
        32,
        33,
        34,
        35, //122 Index 'z'
        -1,
        -1,
        -1,
        -1,
        -1, //127 Index of ASCII 
};
#endif
