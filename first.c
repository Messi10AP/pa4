#include "first.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int prefetch_run = 0;

cache_entry_t *init_cache() {
	int i;
	cache_entry_t *cs;
	cs = malloc(sizeof(cache_entry_t) * num_indexes);
	i = 0;
	for (; i < num_indexes; i++) {
		cs[i].valid = 0;
		cs[i]._tag = -1;
		cs[i].set = i / assoc;
		cs[i].recent = 0;
		cs[i].dirty = 0;
	}
	return cs;
}

void block_set_sizes() {
	int i = 1;
	bk_bits = 0;

	for (; i < bk_size; i *= 2) {
		bk_bits++;
	}
	if (i != bk_size && bk_bits != 0) {
		printf("Invalid block size..\n");
		exit(1);
	}
	i = 1;
	set_bits = 0;
	for (; i < num_indexes / assoc; i *= 2) {
		set_bits++;
	}
	if (i != num_indexes / assoc && set_bits != 0) {
		printf("Invalid assoc. \n");
		exit(1);
	}
	shift_bits = bk_bits + set_bits;
}

addr_t getIndex(addr_t addr) {
	addr_t tag = addr >> (shift_bits);
	addr_t set = ((addr - (tag << (shift_bits))) >> bk_bits);
	return set;
}

addr_t getTag(addr_t addr) {
	return addr >> (shift_bits);
}

void update_index(cache_entry_t *cs, int new_ind, int index) {
	int i = index * assoc;
	for (; i < index * assoc + assoc; i++) {
		cs[i].recent++;
	}
	cs[new_ind].recent = 0;
}

void set_cache(cache_entry_t *cs, int index, addr_t tag) {
	mem_reads++;
	int i = index;
	for (; i < index + assoc; i++) {
		if (cs[i].valid == 0) {
			cs[i].valid = 1;
			cs[i]._tag = tag;
			update_index(cs, i, cs[i].set);
			return;
		}
	}

	i = index;
	int largest = 0;
	int large_index = index;
	for (; i < index + assoc; i++) {
		if (cs[i].recent > largest) {
			largest = cs[i].recent;
			large_index = i;
		}
	}
	cs[large_index]._tag = tag;
	update_index(cs, large_index, cs[large_index].set);

}

void cache_access(cache_entry_t *cs, addr_t address, int write, int prefetch) {
	int index = getIndex(address) * assoc;
	int i = index;
	addr_t tag = getTag(address);

	//printf("%d,%d,%d,0x%llx,0x%llx,%d,%d\n", write, prefetch, index, tag, address, c_miss, c_hits);
	if (write) {
		mem_writes++;
	}

	for (; i < index + assoc; i++) {
		if (cs[i]._tag == tag) {
			if (cs[i].valid == 1) {
			    c_hits++;
				if (write) {
					cs[i]._tag = tag;
				}
				update_index(cs, i, cs[i].set);
				return;
			}
		}
	}
	if (!prefetch_run || (prefetch_run && prefetch)) {
		  c_miss++;
	}
	set_cache(cs, index, tag);
	if (prefetch) {
		addr_t next_addr = (address + (1 << bk_bits));
		cache_access(cs, next_addr, 0, 0);
	}
}

int parse_args(int argc, char *argv[]) {
	c_size = atoi(argv[1]);
	bk_size = atoi(argv[3]);
	assoc = 0;
	if (strcmp(argv[2], "direct") == 0) {
		assoc = 1;
	} else if (strcmp(argv[2], "assoc") == 0) {
		assoc = c_size / bk_size;
	} else {
		char* temp = argv[2];
		const char *c = strtok(temp, ":");

		c = strtok(NULL, ":");
		if (c == NULL) {
			printf("Invalid associativity \n");
			return 1;
		}
		assoc = atoi(c);
	}
	return 0;
}

void print_stat(int prefetch) {
	if (prefetch) {
		printf("with-prefetch\n");
	} else {
		printf("no-prefetch\n");
	}
	printf("Memory reads: %d\n", mem_reads);
	printf("Memory writes: %d\n", mem_writes);
	printf("Cache hits: %d\n", c_hits);
	printf("Cache misses: %d\n", c_miss);
	printf("\n");
}

int main(int argc, char * argv[]) {
	char ip[20];
	char wr[2];
	char addstr[20];

	if (argc != 6) {
		printf(
				"USAGE: fast <cache_size> <associativity> <cache_policy=fifo> <block_size> <trace file> \n");
		return 1;
	}
	int p = parse_args(argc, argv);
	if (p != 0) {
		return p;
	return 0;
	}

	num_indexes = c_size / bk_size;
	if (num_indexes / assoc * assoc * bk_size != c_size || c_size == 0) {
		fprintf(stderr, "Invalid size\n");
		return 1;
	}
	int k;
	for (k = 0; k < 2; k++) {
		mem_reads = 0;
		mem_writes = 0;
		c_hits = 0;
		c_miss = 0;
		FILE *trace = fopen(argv[5], "r");
		if (trace == NULL) {
			fprintf(stderr, "Cannot open file.\n");
			return 1;
		}
		cache_entry_t *cs = init_cache();
		block_set_sizes();
		addr_t address;
		int prefetch = k==0;
		prefetch_run = prefetch;
		//printf("write,prefetch,index,tag,address,c_miss,c_hits\n");

		while (fscanf(trace, "%s %s %s", ip, wr, addstr) != EOF) {
			if (strcmp(ip, "#eof") == 0) {
				break;
			}
			if (sscanf(addstr, "0x%llx", &address) == 0) {
				continue;
			}
			int write = strcmp(wr, "W") == 0;
			cache_access(cs, address, write, prefetch);
		}
		print_stat(prefetch);
		fclose(trace);
		free(cs);
	}
	return 0;
}
