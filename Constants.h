#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <unistd.h>
#include <semaphore.h>
const char *shared_memory_name = "/Memory_space";

struct descriptor{
    int data_size; //In bytes
    int buffer_size; //In decimal
    long reader_pointer;
    long writer_pointer;
    sem_t reader_semaphore;
    sem_t writer_semaphore;
    sem_t buffer_reader_semaphore;
    sem_t buffer_writer_semaphore;
};



const int descriptor_size = sizeof(struct descriptor);
const int semaphore_size = sizeof(sem_t);


#endif