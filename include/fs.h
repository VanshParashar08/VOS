#ifndef FS_H
#define FS_H

#include <stdint.h>

void fs_init(uint32_t initrd_addr);
int fs_read_file(const char* name, char* buffer, uint32_t max_len);
void fs_list_files(void (*print_fn)(const char*));

#endif
