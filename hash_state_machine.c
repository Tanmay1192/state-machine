/*================================================================================================================================
* Name           : hash_state_machine
*
* Synopsis       : This is a standalone program to implement a algorithm of state machine. It is also a minimal perfect hash used to 
*                  compare a incoming keyword (eg: session ID). It uses 2D array to create state table, 1st dimension is of length 
*                  128 to keep  ASCII characters and 2nd dimension is used to keep the state of the keyword(s).
*
*
*                   0   1   2   3   4   5   6   7     States -->
*                 +---+---+---+---+---+---+---+---+
*                m|   |   | 3 |   |   |   |   |   |
*       a         +---+---+---+---+---+---+---+---+
*       s        t|   |   | 4 |   |   |   |   |   |
*       c         +---+---+---+---+---+---+---+---+
*       i        r| 1 |   |   |   |   |   |   |   |
*       i         +---+---+---+---+---+---+---+---+
*                a|   | 2 |   |   |   |   |   |   |
*                 +---+---+---+---+---+---+---+---+
* Hash idx[128th] |   |   |   | T1| T2|   |   |   |
*
*               Hash Id -> it is unqiue identification of provided keywords 
*
*
* Input             : ram  T1
*                     rat  T2
*
* Formula used      : Next State(NS)= (Current State(CS), INPUT) --> INPUT will be the ASCII character
*
* Flow of Algorithm :
*                         INPUT = 'r'
*                         CS=0 | NS=1; NS++; CS=NS
*                         INPUT = 'a'
*                         CS=1 | NS=2; NS++; CS=NS
*                         INPUT = 'm'
*                         CS=2 | NS=3; NS++; CS=NS
*
*                         Set hash ID :- array[128][CS + 1] = HashId <start with 0>
*                         HashId++
*
*
* Output            : Return the ID from the state machine of that particular pattern otherwise print pattern not found in the file
*
* Author            : Tanmay Prakash
==================================================================================================================================*/

#define _NSLB_DEF_HASH_MACHINE
#include <nslb_o1hash_state_machine.h>
#include <stdarg.h>

// All the headers macro are present in hash_state_machine.h
/*---------------------------------------------------------------------------------------------------------
 * Name       :hash_table_init
 *
 * Purpose    :This function is used to allocate size to state machine to the size of max state, by default
 *              max state is set to 128
 *
 * Input      :Input will be hash table which will be allocated memory
 *            
 * Return     : -1 for Failure
 *               0 for success
 *--------------------------------------------------------------------------------------------------------*/
int hash_table_init(nslbHashTable *hash_table)
{
  int i=0;
  NSLB_HASH_DEBUG_LOG1("Method Called, hash_table = %x", hash_table);
  if(hash_table == NULL)
  {
    NSLB_HASH_ERROR_LOG("Error:hash table is given NULL. Hence returning form here\n");
    return -1;
  }
  // if max_state and delta state is not set, set these to their default values
  if(hash_table->max_state == 0)
  {
    hash_table->max_state =  HASH_TABLE_INIT_SIZE;
  }
  if(hash_table->delta_state == 0)
  {
    hash_table->delta_state =  HASH_TABLE_INIT_SIZE;
  }
  hash_table->next_state = 1;
  hash_table->hash_index = 0;

 /*
  *Allocating 2nd dimension of state machine:
  *
  *                                               0   1   2   3   4   5   6   7              126  127
  *                                     +---+    +---+---+---+---+---+---+---+---+           +---+---+
  *                             31      | * | -> |   |   |   |   |   |   |   |   |...........|   |   |
  *                                     +---+    +---+---+---+---+---+---+---+---+           +---+---+
  *                             32      |   |
  *                                     +---+
  *                             33      |   |
  *                                     +---+
  *                                       .                                      .
  *                                       .
  *                                       .                              MAX_HASH_TABLE_ROW = ASCII character - non printable
  *                                       .                                                                      ascii character
  *                                     +---+
  *                             127     |   |
  *                                     +---+
  *
  *
  *  0-31 ASCII character are non printable character therefore we can eliminate them from malloc, helps to save memory
  *  By default max_state is set to 128 but can be altered by user (i.e. HASH_TABLE_INIT_SIZE has a default value set to 128)
  *  malloc is done only first time next time when next state fall short than max state we will realloc as per required.
  *
  */
  
  for(i = 0; i < MAX_HASH_TABLE_ROW; i++)
  {
    hash_table->state_machine[i] = (int *)malloc(hash_table->max_state * sizeof(int));
    if(hash_table->state_machine[i] == NULL)
    {
      NSLB_HASH_ERROR_LOG("Error: unable to allocate size to %d th index of state machine, errno = %d error = (%s)\n", i, errno,
                                                                                                              strerror(errno));
      return -1;
    }
    NSLB_HASH_DEBUG_LOG3("i = %d, max_state = %u, delta_state = %u", i, hash_table->max_state, hash_table->delta_state);
    memset(hash_table->state_machine[i], 0, hash_table->max_state * sizeof(int));
  }
  return 0;
}

/*---------------------------------------------------------------------------------------------------------
 * Name       :hash_table_free
 *
 * Purpose    :This function is used to free the memory malloc for state machine
 *
 * Input      :Input will be hash table whose memory is to be freed
 *            
 * Return     : -1 for Failure
 *               0 for success
 *--------------------------------------------------------------------------------------------------------*/
int hash_table_free(nslbHashTable *hash_table)
{
  int i;
  NSLB_HASH_DEBUG_LOG1("Method Called, hash_table = %x\n", hash_table);
  if(hash_table == NULL)
  {
    NSLB_HASH_ERROR_LOG("Error:hash_table = %p, is NULL\n",  hash_table);
    return -1;
  }

  for(i = 0; i < MAX_HASH_TABLE_ROW; i++)
  {
    NSLB_HASH_DEBUG_LOG3("i = %d, hash_table->state_machine = %u", i, hash_table->state_machine[i]);
    if(hash_table->state_machine[i] !=  NULL)
    {
      free(hash_table->state_machine[i]);
      hash_table->state_machine[i] = NULL; // NULL
    }
    hash_table->max_state = 0;
    hash_table->next_state = 0;
    hash_table->hash_index = 0;
  }
  return 0;
}

/*--------------------------------------------------------------------------------------------------------
 * Name      : hash_table_add
 *
 * Purpose   : This function will add a new entry to the state machine while updating next state and  
 *             current state of the state machine. If the next state plus length of line in combine
 *             increases more than max state then again realloc the state machine size
 *            
 * Input     : Its input will be (1) Hash Table structure
 *                               (2) string pointer of the line fetch from file
 *                               (3) length of that complete line
 *             
 * Output    : Output will be the hash Id generated at the last index of the matrix
 *             
 * Note      : (1) Next state will be unique and always in incrementing order and so as for hash Id
 *             (2) for repeated pattern in fetched file it will return same hash Id
 *-------------------------------------------------------------------------------------------------------*/
int hash_table_add(nslbHashTable *hash_table , char * entry , int len)
{
  unsigned int cur_state = 0;
  unsigned int i,idx = 0;
  int new_entry = 0;
  unsigned long hash_size = 0;

  NSLB_HASH_DEBUG_LOG3("Method Called, hash_table = %x entry = %s, len = %d,\n", hash_table, entry?entry:"NULL", len);
  if (hash_table == NULL || entry == NULL || len == 0)
  {
    NSLB_HASH_ERROR_LOG("Invalid argument or NULL passed in nslb_hash_table_add\n");
    return -1;
  }

  // In this case current max state will not be able to handle total states, hence reallocating the array of state machine
  if((hash_table->next_state + len + 1) > hash_table->max_state)
  {
    /*
     * Reallocating size 2nd dimension (i.e. State Dimension)
     * hash_table->delta_state = Is the amount of size added to hash_table->max_state 
     *                           every time when hash_table->next_state fall short than
    *                           hash_table->max_state.
     *
     */
    unsigned int max_state = hash_table->max_state;
    hash_table->max_state += hash_table->delta_state;
    /*
     *  Here next_state + len is done the avoid ***glibc detect*** invalid relloc()
     *
     *  Description : length is added to next state to handle a situation when next state is smaller than
     *                 max stat but the length of line those not able to fit into the current block size of state machine
     *                 e.g. next_state = 120; max state = 128; len = 36
     *                 In this case above error was coming. -> ***glibc detect*** invalid relloc()
     *
     *  Therefore we added len to next state when checking the need to again realloc with only reason to handle the case when length      *  of entry string is bigger than delta_state
     *
     */
    if(len > hash_table->delta_state)
      hash_table->max_state += len;

    for(i = 0; i < MAX_HASH_TABLE_ROW; i++)
    {
      NSLB_HASH_DEBUG_LOG3("i = %d, max_state(realloc) = %d, hash_table->max_state = %u, delta_state = %u\n", i, max_state,
                                                                         hash_table->max_state, hash_table->delta_state);
      hash_table->state_machine[i] = (int *)realloc(hash_table->state_machine[i],
                                                                    hash_table->max_state * sizeof(int));

      if(hash_table->state_machine[i] == NULL)
      {
        NSLB_HASH_ERROR_LOG("Error: unable to allocate size to %d th index of state machine, errno = %d error = (%s)\n", i, errno,
                                                                                                                  strerror(errno));
        return -1;
      }
      // memset newly allocated memory only
      memset(&(hash_table->state_machine[i][max_state]),0, (hash_table->max_state - max_state)*sizeof(int));
    }
    hash_size = sizeof(nslbHashTable) + (MAX_HASH_TABLE_ROW * (unsigned long)hash_table->max_state * sizeof(int));
    NSLB_HASH_DEBUG_LOG4("Size of Hash Table = %lu\n", hash_size);
  }
  /*
   *Fill state machine with pattern(entry) data
   */
  for(i=0; i < len; i++)
  {
    //NSLB_HASH_DEBUG_LOG("i = %d, len = %d cur_state = %d, idx = %d, hash_table->next_state = %u, new_entry = %d\n", i, len, cur_state, idx, 
      //                                                          hash_table->next_state, new_entry);

    // IS_IN_PRINTABLE_RANGE: This macro check the valid printable range of ascii character
    int input = INPUT_MAP((int)entry[i]);
    if(input < 0)
    {
        continue;
    }
    cur_state = hash_table->state_machine[input][idx];
    if(cur_state != 0)
    {
      idx = cur_state;
      continue;
    }
    hash_table->state_machine[input][idx] = hash_table->next_state;
    idx = hash_table->next_state++;
    new_entry = 1;
   }


  /*
   *   Set transaction ID at last index of the array
   */
   if(new_entry)
   {
     idx = hash_table->next_state++;
     hash_table->state_machine[HASH_MAX_INPUT][idx] = hash_table->hash_index++;
     return hash_table->state_machine[HASH_MAX_INPUT][idx];
   }
   else
     return hash_table->state_machine[HASH_MAX_INPUT][cur_state + 1];
}

/*--------------------------------------------------------------------------------------------------------
 * Name      : hash_table_get
 *
 * Purpose   : This function will return the hash Id of the pattern after searching from the input file,

 *              if the pattern is not present it will print pattern not found.
 *
 * Input     : Its input will be pattern passed through argument, its length and hash table
 *              
 *             
 * Output    : It will give the hash Id of that pattern from state machine if it is present in the state  
 *             machine
 *--------------------------------------------------------------------------------------------------------*/
int hash_table_get(nslbHashTable *hash_table , char * entry, int len)
{
  unsigned int cur_state = 0;
  int i= 0;
  if (hash_table == NULL || entry == NULL || len == 0)
  {
    NSLB_HASH_ERROR_LOG("Invalid argument or NULL passed in nslb_hash_table_get\n");
    return -1;
  }

  NSLB_HASH_DEBUG_LOG3("Method Called, entry = %s, len = %d, \n", entry?entry:"NULL", len);
  for(i=0; i < len; i++)
  {
    int input = INPUT_MAP((int)entry[i]);
    if(input < 0)
    {
        continue;
    }
    if(hash_table->state_machine[input][cur_state] != '\0')
    {
      cur_state = hash_table->state_machine[input][cur_state];
    }
    else
    {
     // error: pattern not found in file
      NSLB_HASH_ERROR_LOG("Pattern [ %s ] not found in the state machine returning -1\n", entry?entry:"NULL");
      return -1;
    }
   }
  return hash_table->state_machine[HASH_MAX_INPUT][cur_state + 1];
}

/*Size of hash table*/
unsigned long hash_table_size(nslbHashTable *hash_table)
{

  unsigned long hash_table_size = 0;
  NSLB_HASH_DEBUG_LOG1("Method Called");
  hash_table_size = sizeof(nslbHashTable) + (MAX_HASH_TABLE_ROW * (unsigned long)hash_table->next_state * sizeof(int));
  return hash_table_size;
}


#ifdef TEST

void hash_test_log(nslbHashTable *hash_table , int level, char *file, int line, char *fname, char *format, ...)
{
  int print_screen = 0;
  va_list ap;
  char buffer[2048 + 1]="";
  va_start(ap, format);
  vsnprintf(buffer, 2048, format, ap);
  va_end(ap);
  buffer[2048] = 0;
  printf("%s|%s|%s|%s\n",file,line,fname,buffer);
  return;
}

int main(void)
{
 char entries[2][37] = {"374d3d5a-2dd1-451a-bccc-7102c37bf029", "44cb7697-e0f3-45bf-bb2f-a5836c312279"};
 char search_pattern[] = "44cb7697-e0f3-45bf-bb2f-a5836c312279";
 int len = 0;
 nslbHashTable hash_table;
 //init hash table here
 memset(&hash_table,0,sizeof(nslbHashTable));
 hash_table->log = hash_test_log
 hash_table_init(&hash_table);
  //tokenising the string of entries
  // call hash add function
  len = strlen(entries[0]);
  printf("String: %s : length is:%d  ID is: %d\n",entries[0] , len , hash_table_add(&hash_table, entries[0], len));
  len = strlen(entries[1]);
  printf("String: %s : length is:%d  ID is: %d\n",entries[1] , len , hash_table_add(&hash_table, entries[1], len));

  len = strlen(search_pattern);
  //call get id function here
  printf("ID of Pattern: %s length %d , is %d\n", search_pattern, len, hash_table_get(&hash_table, search_pattern, len));
  //call free hash function here
  hash_table_free(&hash_table);
  return 0;
}
#endif
