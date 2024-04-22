#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <unistd.h>
#include <semaphore.h>
const char *shared_memory_name = "/Memory_space";

struct descriptor{
    int data_size; //In bytes
    long reader_pointer;
    long writer_pointer;
    sem_t reader_semaphore;
    sem_t writer_semaphore;
};

struct statistics{
    double reconstructor_slp_t;
    double client_slp_t;
    double reconstructor_usr_mode_t;
    double reconstructor_krnl_mode_t;
    double client_usr_mode_t;
    double client_krnl_mode_t;
    long transfered_characters_n;
    long characters_in_buffer_n;
};

#endif