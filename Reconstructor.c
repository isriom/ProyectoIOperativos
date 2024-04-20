#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "Constants.h"

int main() {
    const char *shared_memory_name = "/my_shared_memory";
    int shared_memory_fd;
    char *shaed_memory_ptr;
    struct stat stat_buffer;
    
    // Open the shared memory object
    shared_memory_fd = shm_open(shared_memory_name, O_RDONLY, 0666);
    if (shared_memory_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // Get information about the shared memory object
    if (fstat(shared_memory_fd, &stat_buffer) == -1) {
        perror("fstat");
        close(shared_memory_fd);
        return 1;
    }
}