#include "../include/pmm.h"
#include "../include/string.h"

// Statically allocate a bitmap for tracking physical frames across up to 4GB of memory
// 4GB / 4096 = 1,048,576 blocks (frames)
// 1,048,576 blocks / 8 bits = 131,072 bytes (128 KB)
static uint8_t pmm_bitmap[131072]; 
static uint32_t max_blocks = 0;
static uint32_t used_blocks = 0;

static inline void mmap_set(int bit) {
    pmm_bitmap[bit / 8] |= (1 << (bit % 8));
}

static inline void mmap_unset(int bit) {
    pmm_bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static inline int mmap_test(int bit) {
    return pmm_bitmap[bit / 8] & (1 << (bit % 8));
}

int pmm_find_first_free(void) {
    for (uint32_t i = 0; i < max_blocks / 32; i++) {
        if (pmm_bitmap[i] != 0xFF) {
            for (int j = 0; j < 8; j++) {
                int bit = i * 8 + j;
                if (!mmap_test(bit)) {
                    return bit;
                }
            }
        }
    }
    return -1;
}

void init_pmm(size_t mem_size) {
    max_blocks = mem_size / PAGE_SIZE;
    used_blocks = max_blocks;
    
    // By default, all memory is marked as used
    memset(pmm_bitmap, 0xFF, sizeof(pmm_bitmap));
}

void pmm_init_region(uint32_t base, size_t size) {
    int align = base / PAGE_SIZE;
    int blocks = size / PAGE_SIZE;

    for (int i = 0; i < blocks; i++) {
        mmap_unset(align++);
        used_blocks--;
    }
    mmap_set(0); // Protect block 0 (NULL)
}

void pmm_deinit_region(uint32_t base, size_t size) {
    int align = base / PAGE_SIZE;
    int blocks = size / PAGE_SIZE;

    for (int i = 0; i < blocks; i++) {
        mmap_set(align++);
        used_blocks++;
    }
}

void* pmm_alloc_block(void) {
    if (max_blocks - used_blocks <= 0) return 0;

    int frame = pmm_find_first_free();
    if (frame == -1) return 0;

    mmap_set(frame);
    used_blocks++;
    return (void*)(frame * PAGE_SIZE);
}

void pmm_free_block(void* p) {
    uint32_t addr = (uint32_t)p;
    int frame = addr / PAGE_SIZE;

    mmap_unset(frame);
    used_blocks--;
}
