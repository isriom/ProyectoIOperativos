
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <gtk-3.0/gtk/gtk.h>

#include "Constants.h"
#include "Libs/libFort/libfort-0.4.2/lib/fort.h"

int main(int argc, char *argv[]) {
    int shared_memory_length;
    int shared_memory_fd;
    int data_size;
    void *shared_memory_ptr;

    shm_unlink(shared_memory_name);//Restart memory section

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
    shared_memory_fd = shm_open(shared_memory_name, O_CREAT | O_RDWR|O_EXCL, S_IRWXO|S_IRUSR | S_IWUSR | S_IXUSR);
    if (shared_memory_fd == -1) {
        perror("shm_open");
        shm_unlink(shared_memory_name);
        return 1;
    }
    
      // Configure the size of the shared memory object
    int full_memory_size=descriptor_size+statistics_size+shared_memory_length*sizeof(time_t)+shared_memory_length*data_size+2;
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
    memory_desc->reconstructor_done=0;
    memory_desc->client_done=0;
    sem_init(&(memory_desc->reader_semaphore),1,1);
    sem_init(&(memory_desc->writer_semaphore),1,1);
    sem_init(&(memory_desc->buffer_reader_semaphore),1,0);
    sem_init(&(memory_desc->buffer_writer_semaphore),1,shared_memory_length);

    struct statistics *stadistics=shared_memory_ptr+ descriptor_size;
    stadistics->reconstructor_slp_t=0;
    stadistics->client_slp_t=0;
    stadistics->reconstructor_usr_mode_t=0;
    stadistics->reconstructor_krnl_mode_t=0;
    stadistics->client_usr_mode_t=0;
    stadistics->client_krnl_mode_t=0;
    stadistics->transfered_characters_n=0;
    stadistics->characters_in_buffer_n=0;
    sem_init(&(stadistics->client_slp_t_semaphore),1,1);
    sem_init(&(stadistics->client_usr_krn_t_semaphore),1,1);
    sem_init(&(stadistics->reconstructor_slp_t_semaphore),1,1);
    sem_init(&(stadistics->reconstructor_usr_krn_t_semaphore),1,1);

    time_t *datetimes=shared_memory_ptr+descriptor_size+statistics_size;
    void *buffer=shared_memory_ptr+descriptor_size+statistics_size+shared_memory_length*sizeof(time_t);
    
    while (memory_desc->data_size>0)
    {

    write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);

    ft_table_t *table = ft_create_table();
    /* Set "header" type for the first row */
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);


    ft_write_ln(table, "N", "Data", "Time","N", "Data", "Time","N", "Data", "Time","N", "Data", "Time" );

    for (int i = 0; i < shared_memory_length/4; i++)
    {
        int index=i*4;
        int error=ft_printf_ln(table,"%d|%c|%ld|%d|%c|%ld|%d|%c|%ld|%d|%c|%ld|"
                                , index, *(char*)(buffer+index),*(datetimes+index)
                                ,index+1, *(char*)(buffer+index+1),*(datetimes+index+1)
                                ,index+2, *(char*)(buffer+index+2),*(datetimes+index+2)
                                ,index+3, *(char*)(buffer+index+3),*(datetimes+index+3));
       
    }
    


    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
    sleep(1);
    }
    
}