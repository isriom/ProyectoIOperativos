
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
    int shared_memory_length;
    int shared_memory_fd;
    int data_size;
    void *shared_memory_ptr;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <memory_size> <data_type>\n", argv[0]);
        return 1;
    }
    shared_memory_length = atoi(argv[1]);

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
    int full_memory_size=shared_memory_length*data_size+descriptor_size;
    if (ftruncate(shared_memory_fd,full_memory_size ) == -1) {
        perror("ftruncate");
        return 1;
    }


        // Map the shared memory object into the address space
    shared_memory_ptr = mmap(NULL, full_memory_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);
    if (shared_memory_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    struct descriptor *memory_desc=shared_memory_ptr;
    memory_desc->data_size=data_size;
    memory_desc->buffer_size=shared_memory_length;
    memory_desc->reader_pointer=0;
    memory_desc->writer_pointer=0;
    sem_init(&(memory_desc->reader_semaphore),1,1);
    sem_init(&(memory_desc->writer_semaphore),1,1);
    sem_init(&(memory_desc->buffer_reader_semaphore),1,0);
    sem_init(&(memory_desc->buffer_writer_semaphore),1,shared_memory_length);
    
    
}