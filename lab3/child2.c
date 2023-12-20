#include "lib.h"

int main() {
    int file_descriptor = shm_open(SHARED_MEMORY_NAME, O_RDWR, ACCESS_MODE);

    char* mmap_buffer_data = mmap(NULL, SIZE_OF_SHARED_MEMORY, PROT_READ | PROT_WRITE, MAP_SHARED,
                                          file_descriptor, 0);


    for (int char_in_buffer = 0; char_in_buffer < SIZE_OF_SHARED_MEMORY; char_in_buffer++) {
        if (isspace(mmap_buffer_data[char_in_buffer])) {
            mmap_buffer_data[char_in_buffer] = '_';
        }
    }

    munmap(mmap_buffer_data, SIZE_OF_SHARED_MEMORY);
    close(file_descriptor);

    return 0;
}