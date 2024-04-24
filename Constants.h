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
    int client_done;
    int reconstructor_done;
    sem_t reader_semaphore;
    sem_t writer_semaphore;
    sem_t buffer_reader_semaphore;
    sem_t buffer_writer_semaphore;
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
    sem_t reconstructor_usr_krn_t_semaphore;
    sem_t client_usr_krn_t_semaphore;
    sem_t reconstructor_slp_t_semaphore;
    sem_t client_slp_t_semaphore;
};
const int descriptor_size = sizeof(struct descriptor);
const int semaphore_size = sizeof(sem_t);
const int statistics_size =sizeof(struct statistics);

const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J"; //https://stackoverflow.com/questions/2347770/how-do-you-clear-the-console-screen-in-c
void openStatistics();
#endif