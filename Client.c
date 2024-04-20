#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#include "Constants.h"

int main(int argc, char *argv[]) {


    if (argc != 2) {
        fprintf(stderr, "Usage: %s <shared_memory_path>\n", argv[0]);
        return 1;
    }

    const char *file_path = argv[1];
    int shared_memory_fd;
    int Data_fd;
    char *shared_memory_ptr;
    struct stat stat_buffer;

    restart:
    // Open the shared memory object
    shared_memory_fd = shm_open(shared_memory_name, O_RDWR,  S_IRWXO|S_IRUSR | S_IWUSR | S_IXUSR);
    if (shared_memory_fd == -1) {
        perror("shm_open");//try again in 10 seconds 
        sleep(10);
        goto restart;
    }

    // Get information about the shared memory object
    if (fstat(shared_memory_fd, &stat_buffer) == -1) {
        perror("fstat");
        close(shared_memory_fd);
        return 1;
    }

    // Map the shared memory object into the address space
    shared_memory_ptr = mmap(NULL, stat_buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);
    if (shared_memory_ptr == MAP_FAILED) {
        perror("mmap");
        close(shared_memory_fd);
        return 1;
    }


    //Open the file to write on the buffer
    Data_fd=open(file_path,O_RDONLY);
    if (Data_fd==-1)
    {
        perror("fstat");
        close(Data_fd);
    }
    
    
    struct descriptor *memory_desc=shared_memory_ptr;
    int shared_memory_length=memory_desc->buffer_size;

    time_t *datetimes=shared_memory_ptr+descriptor_size+statistics_size;
    void *buffer=datetimes+shared_memory_length*sizeof(time_t);
    
    void *data=malloc(memory_desc->data_size);//Reserve space for memory
    
    while (memory_desc->data_size>0)
    {
        //Get position to read from file
        sem_wait(&(memory_desc->writer_semaphore));
        int offset=memory_desc->writer_pointer;
        memory_desc->writer_pointer+=memory_desc->data_size;
        sem_post(&(memory_desc->writer_semaphore));

        int error=pread(Data_fd,data,memory_desc->data_size, offset);//Read from file and save on data;
        if (error==-1)
        {
            perror("pread");
        }

        //Get position in circular buffer
        offset=offset%(memory_desc->buffer_size*memory_desc->data_size);
        void* buffer_write_pos=offset+buffer;
        time_t* date_write_pos=offset+datetimes;
        time_t time_to_save = time(NULL);

        sem_wait(&(memory_desc->buffer_writer_semaphore));
        memmove(buffer_write_pos,data,memory_desc->data_size);
        *date_write_pos=time_to_save;
        sem_post(&(memory_desc->buffer_reader_semaphore));
        
    }
    


    
    
}