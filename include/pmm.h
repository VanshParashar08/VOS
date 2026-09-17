#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096
#define BLOCKS_PER_BYTE 8
#define BLOCK_ALIGN(x) (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

void init_pmm(size_t mem_size);
void pmm_init_region(uint32_t base, size_t size);
void pmm_deinit_region(uint32_t base, size_t size);

void* pmm_alloc_block(void);
void pmm_free_block(void* p);

#endif
