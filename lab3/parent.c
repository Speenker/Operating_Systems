#include "lib.h"

int main() {
    int file_descriptor = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, ACCESS_MODE);
    ftruncate(file_descriptor, SIZE_OF_SHARED_MEMORY);
    char *mmap_buffer_data = mmap(NULL, SIZE_OF_SHARED_MEMORY, PROT_WRITE | PROT_READ, MAP_SHARED, file_descriptor, 0);
    
    char input_buffer[SIZE_OF_BUFFER];
    printf("Enter a string: ");
    fgets(input_buffer, SIZE_OF_BUFFER, stdin);

    strcpy(mmap_buffer_data, input_buffer);
    
    pid_t first_child_process_id = fork();

    if (first_child_process_id == -1) {
        printf("first_child fork error");
        return -1;
    }

    if (first_child_process_id == 0) {
        execl("child1", "child1", NULL);
    } 
    
    pid_t second_child_process_id = fork();
         
    if (second_child_process_id == -1) {
        printf("second_child fork error");
        return -1;
    }

    if (second_child_process_id == 0) {
        execl("child2", "child2", NULL);
    } 

    wait(NULL);
    wait(NULL);

    printf("Modified string: %s\n", mmap_buffer_data);

    shm_unlink(SHARED_MEMORY_NAME);
    return 0;
}