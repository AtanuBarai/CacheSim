/*
 * cache.c
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "cache.h"
#include "main.h"

/* cache configuration parameters */
static int cache_split = 0;
static int cache_usize = DEFAULT_CACHE_SIZE;
static int cache_isize = DEFAULT_CACHE_SIZE; 
static int cache_dsize = DEFAULT_CACHE_SIZE;
static int cache_block_size = DEFAULT_CACHE_BLOCK_SIZE;
static int words_per_block = DEFAULT_CACHE_BLOCK_SIZE / WORD_SIZE;
static int cache_assoc = DEFAULT_CACHE_ASSOC;
static int cache_writeback = DEFAULT_CACHE_WRITEBACK;
static int cache_writealloc = DEFAULT_CACHE_WRITEALLOC;

/* cache model data structures */
static Pcache icache;
static Pcache dcache;
static cache c1;
static cache c2;
static cache_stat cache_stat_inst;
static cache_stat cache_stat_data;

/************************************************************/
void set_cache_param(param, value)
  int param;
  int value;
{

  switch (param) {
  case CACHE_PARAM_BLOCK_SIZE:
    cache_block_size = value;
    words_per_block = value / WORD_SIZE;
    break;
  case CACHE_PARAM_USIZE:
    cache_split = FALSE;
    cache_usize = value;
    break;
  case CACHE_PARAM_ISIZE:
    cache_split = TRUE;
    cache_isize = value;
    break;
  case CACHE_PARAM_DSIZE:
    cache_split = TRUE;
    cache_dsize = value;
    break;
  case CACHE_PARAM_ASSOC:
    cache_assoc = value;
    break;
  case CACHE_PARAM_WRITEBACK:
    cache_writeback = TRUE;
    break;
  case CACHE_PARAM_WRITETHROUGH:
    cache_writeback = FALSE;
    break;
  case CACHE_PARAM_WRITEALLOC:
    cache_writealloc = TRUE;
    break;
  case CACHE_PARAM_NOWRITEALLOC:
    cache_writealloc = FALSE;
    break;
  default:
    printf("error set_cache_param: bad parameter value\n");
    exit(-1);
  }

}
/************************************************************/

/************************************************************/
void init_cache()
{
    int i;
  /* initialize the cache, and cache statistics data structures */
    if(cache_split == 0)
    {
        c1.size = cache_usize;
        c1.associativity = cache_assoc;
        c1.n_sets = (cache_usize / (cache_assoc * cache_block_size));        
        c1.index_mask_offset = LOG2(cache_block_size);        
        c1.index_mask = (0xffffffff) >> c1.index_mask_offset;
        c1.index_mask = (c1.n_sets - 1) << c1.index_mask_offset;
        c1.LRU_head = (Pcache_line *)malloc(sizeof(Pcache_line)*c1.n_sets);        
        for(i=0; i<c1.n_sets; i++)
        {
            c1.LRU_head[i] = NULL;
        }
    }
    
    /*Cache statics initialization*/
    cache_stat_inst.accesses = 0;
    cache_stat_inst.misses = 0;
    cache_stat_inst.replacements = 0;
    cache_stat_inst.demand_fetches = 0;
    cache_stat_inst.copies_back = 0;
    
    cache_stat_data.accesses = 0;
    cache_stat_data.misses = 0;
    cache_stat_data.replacements = 0;
    cache_stat_data.demand_fetches = 0;
    cache_stat_data.copies_back = 0;
}
/************************************************************/

/************************************************************/
void perform_access(addr, access_type)
  unsigned addr, access_type;
{
    int index, newtag;
    newtag = addr >> c1.index_mask_offset + LOG2(c1.n_sets);
  /* handle an access to the cache */
    if(cache_split == 0)
    {
        index = (addr & c1.index_mask) >> c1.index_mask_offset;
        cache_stat_data.accesses = (access_type == TRACE_INST_LOAD) ? (cache_stat_data.accesses)
                : (cache_stat_data.accesses + 1);
        cache_stat_inst.accesses = (access_type == TRACE_INST_LOAD) ? (cache_stat_inst.accesses + 1)
                : (cache_stat_inst.accesses);
        
        if(c1.LRU_head[index] == NULL) // cache miss not replacement
        {
            c1.LRU_head[index] = (Pcache_line)malloc(sizeof(cache_line));
            c1.LRU_head[index]->tag = newtag;
            if(access_type == TRACE_DATA_LOAD || access_type == TRACE_DATA_STORE)
                cache_stat_data.misses++;
            else
                cache_stat_inst.misses++;
        }
        else
        {
            if(c1.LRU_head[index]->tag == newtag) //hit
            {
                if(access_type == TRACE_DATA_STORE)
                {
                    c1.LRU_head[index]->dirty = 1;
                }
            }
            else //miss
            {
                if(c1.LRU_head[index]->dirty == 1)
                        cache_stat_data.copies_back++;
                if(access_type == TRACE_INST_LOAD)
                {
                    cache_stat_inst.misses++;
                    cache_stat_inst.replacements++;
                }
                else
                {                    
                    cache_stat_data.misses++;
                    cache_stat_data.replacements++;
                }
            }
        }
    }
    else
    {
        
    }

}
/************************************************************/

/************************************************************/
void flush()
{

  /* flush the cache */

}
/************************************************************/

/************************************************************/
void delete(head, tail, item)
  Pcache_line *head, *tail;
  Pcache_line item;
{
  if (item->LRU_prev) {
    item->LRU_prev->LRU_next = item->LRU_next;
  } else {
    /* item at head */
    *head = item->LRU_next;
  }

  if (item->LRU_next) {
    item->LRU_next->LRU_prev = item->LRU_prev;
  } else {
    /* item at tail */
    *tail = item->LRU_prev;
  }
}
/************************************************************/

/************************************************************/
/* inserts at the head of the list */
void insert(head, tail, item)
  Pcache_line *head, *tail;
  Pcache_line item;
{
  item->LRU_next = *head;
  item->LRU_prev = (Pcache_line)NULL;

  if (item->LRU_next)
    item->LRU_next->LRU_prev = item;
  else
    *tail = item;

  *head = item;
}
/************************************************************/

/************************************************************/
void dump_settings()
{
  printf("*** CACHE SETTINGS ***\n");
  if (cache_split) {
    printf("  Split I- D-cache\n");
    printf("  I-cache size: \t%d\n", cache_isize);
    printf("  D-cache size: \t%d\n", cache_dsize);
  } else {
    printf("  Unified I- D-cache\n");
    printf("  Size: \t%d\n", cache_usize);
  }
  printf("  Associativity: \t%d\n", cache_assoc);
  printf("  Block size: \t%d\n", cache_block_size);
  printf("  Write policy: \t%s\n", 
	 cache_writeback ? "WRITE BACK" : "WRITE THROUGH");
  printf("  Allocation policy: \t%s\n",
	 cache_writealloc ? "WRITE ALLOCATE" : "WRITE NO ALLOCATE");
}
/************************************************************/

/************************************************************/
void print_stats()
{
  printf("\n*** CACHE STATISTICS ***\n");

  printf(" INSTRUCTIONS\n");
  printf("  accesses:  %d\n", cache_stat_inst.accesses);
  printf("  misses:    %d\n", cache_stat_inst.misses);
  if (!cache_stat_inst.accesses)
    printf("  miss rate: 0 (0)\n"); 
  else
    printf("  miss rate: %2.4f (hit rate %2.4f)\n", 
	 (float)cache_stat_inst.misses / (float)cache_stat_inst.accesses,
	 1.0 - (float)cache_stat_inst.misses / (float)cache_stat_inst.accesses);
  printf("  replace:   %d\n", cache_stat_inst.replacements);

  printf(" DATA\n");
  printf("  accesses:  %d\n", cache_stat_data.accesses);
  printf("  misses:    %d\n", cache_stat_data.misses);
  if (!cache_stat_data.accesses)
    printf("  miss rate: 0 (0)\n"); 
  else
    printf("  miss rate: %2.4f (hit rate %2.4f)\n", 
	 (float)cache_stat_data.misses / (float)cache_stat_data.accesses,
	 1.0 - (float)cache_stat_data.misses / (float)cache_stat_data.accesses);
  printf("  replace:   %d\n", cache_stat_data.replacements);

  printf(" TRAFFIC (in words)\n");
  printf("  demand fetch:  %d\n", cache_stat_inst.demand_fetches + 
	 cache_stat_data.demand_fetches);
  printf("  copies back:   %d\n", cache_stat_inst.copies_back +
	 cache_stat_data.copies_back);
}
/************************************************************/
