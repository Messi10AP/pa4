#ifndef fast_h
#define fast_h

typedef unsigned long long addr_t;
typedef struct cache_entry{
    int valid;
    int set;
    int recent;
    int dirty;
    addr_t _tag;
} cache_entry_t;

int c_hits = 0;
int c_miss = 0;
int mem_reads = 0;
int mem_writes = 0;
int num_indexes = 0;
int c_size = 0;
int bk_size = 0;
int assoc = 0;
int bk_bits = 0;
int set_bits = 0;
int shift_bits;
//cache_entry_t* cache;

cache_entry_t *init_cache();
void block_set_sizes();
void update_index(cache_entry_t *cs, int new, int index);
void cache_access(cache_entry_t *cs, addr_t address, int write, int prefetch);

#endif
