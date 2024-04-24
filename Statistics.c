#include "Statistics.h"
#include "Constants.h"

struct descriptor *memory_desc;
struct statistics *statistics_desc;
void *shared_memory_ptr;
struct stat stat_buffer;

int main(){
    int shared_memory_fd;
    restart:
    // Open the shared memory object
    shared_memory_fd = shm_open(shared_memory_name, O_RDWR,  S_IRWXO|S_IRUSR | S_IWUSR | S_IXUSR);
    if (shared_memory_fd == -1) {
        perror("shm_open");//try again in 10 seconds
        fprintf(stdout,"Memory Space not found, trying again in 10s"); 
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

    int buffer_unread;

    memory_desc=shared_memory_ptr;
    statistics_desc = shared_memory_ptr + descriptor_size;

    sem_getvalue(&(memory_desc->buffer_reader_semaphore), &buffer_unread);


    struct statistics local_statistics;
    local_statistics = *statistics_desc;

    local_statistics.characters_in_buffer_n = buffer_unread;
    
    shm_unlink(shared_memory_name);//Restart memory section

    int memory_used = (local_statistics.transfered_characters_n+buffer_unread)*sizeof(char);

    printf("Tiempo bloqueado del cliente:                   %f\n",local_statistics.client_slp_t);
    printf("Tiempo bloqueado del reconstructor:             %f\n",local_statistics.reconstructor_slp_t);
    printf("Caracteres transferidos:                        %d\n",local_statistics.transfered_characters_n);
    printf("Caracteres en buffer:                           %d\n",local_statistics.characters_in_buffer_n);
    printf("Espacio total de memoria utilizado              %d\n",memory_used);
    printf("Tiempo total en modo usuario del cliente:       %f\n",local_statistics.client_usr_mode_t);
    printf("Tiempo total en modo kernel del cliente:        %f\n",local_statistics.client_krnl_mode_t);
    printf("Tiempo total en modo usuario del reconstructor: %f\n",local_statistics.reconstructor_usr_mode_t);
    printf("Tiempo total en modo kernel del reconstructor:  %f\n",local_statistics.reconstructor_krnl_mode_t);
}