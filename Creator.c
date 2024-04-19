
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Constants.h"


int main(int argc, char *argv[]) {
    int shared_memory_size;
    int shared_memory_fd;
    int data_size;
    void *shared_memory_ptr;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <memory_size> <data_type>\n", argv[0]);
        return 1;
    }
    shared_memory_size = atoi(argv[1]);

    if (strcmp(argv[2], "int") == 0) {
        data_size = sizeof(int);
    } else if (strcmp(argv[2], "char") == 0) {
        data_size = sizeof(char);
    } else {
        fprintf(stderr, "Unknown data type: %s\n", argv[2]);
        return 1;
    }

      // Create or open the shared memory object
    shared_memory_fd = shm_open(shared_memory_name, O_CREAT | O_RDWR, 0666);
    if (shared_memory_fd == -1) {
        perror("shm_open");
        return 1;
    }
        // Configure the size of the shared memory object
    if (ftruncate(shared_memory_fd, shared_memory_size*data_size+sizeof(struct descriptor)) == -1) {
        perror("ftruncate");
        return 1;
    }
}