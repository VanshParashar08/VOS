#include "../include/fs.h"
#include "../include/string.h"

// Initrd structures
typedef struct {
    uint32_t magic;
    uint32_t num_files;
} __attribute__((packed)) initrd_header_t;

typedef struct {
    char name[32];
    uint32_t offset;
    uint32_t size;
} __attribute__((packed)) initrd_file_header_t;

static uint32_t initrd_location = 0;
static initrd_header_t* initrd_header = 0;
static initrd_file_header_t* file_headers = 0;

void fs_init(uint32_t initrd_addr) {
    initrd_location = initrd_addr;
    initrd_header = (initrd_header_t*)initrd_location;
    if (initrd_header->magic == 0xBF5C) {
        file_headers = (initrd_file_header_t*)(initrd_location + sizeof(initrd_header_t));
    } else {
        initrd_header = 0;
        file_headers = 0;
    }
}

int fs_read_file(const char* name, char* buffer, uint32_t max_len) {
    if (!initrd_header) return -1;
    
    for (uint32_t i = 0; i < initrd_header->num_files; i++) {
        if (strcmp(file_headers[i].name, name) == 0) {
            uint32_t size = file_headers[i].size;
            if (size >= max_len) size = max_len - 1;
            
            char* file_data = (char*)(initrd_location + file_headers[i].offset);
            for (uint32_t j = 0; j < size; j++) {
                buffer[j] = file_data[j];
            }
            buffer[size] = '\0';
            return size;
        }
    }
    return -1;
}

void fs_list_files(void (*print_fn)(const char*)) {
    if (!initrd_header) {
        print_fn("No filesystem loaded.\n");
        return;
    }
    for (uint32_t i = 0; i < initrd_header->num_files; i++) {
        print_fn(file_headers[i].name);
        print_fn("  ");
    }
    print_fn("\n");
}
