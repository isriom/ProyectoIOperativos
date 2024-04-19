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

#endif