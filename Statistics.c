#include <gtk/gtk.h>
#include "Statistics.h"
#include "Constants.h"

struct descriptor *memory_desc;
struct statistics *statistics_desc;
void *shared_memory_ptr;
struct stat stat_buffer;

GtkWidget *main_window;
GtkWidget *output_label;

void update_output_label() {
    int shared_memory_fd;
    
    // Open the shared memory object
    shared_memory_fd = shm_open(shared_memory_name, O_RDWR,  S_IRWXO|S_IRUSR | S_IWUSR | S_IXUSR);
    if (shared_memory_fd == -1) {
        perror("shm_open");//try again in 10 seconds
        fprintf(stdout,"Memory Space not found, trying again in 10s"); 
        sleep(10);
        update_output_label(); // Recursive call to retry
        return;
    }

    // Get information about the shared memory object
    if (fstat(shared_memory_fd, &stat_buffer) == -1) {
        perror("fstat");
        close(shared_memory_fd);
        return;
    }

    // Map the shared memory object into the address space
    shared_memory_ptr = mmap(NULL, stat_buffer.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_fd, 0);
    if (shared_memory_ptr == MAP_FAILED) {
        perror("mmap");
        close(shared_memory_fd);
        return;
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

    char output_text[600]; // Adjust size as needed
    snprintf(output_text, sizeof(output_text), "Tiempo bloqueado del cliente:                   %f s\n"
                    "Tiempo bloqueado del reconstructor:             %f s\n"
                    "Caracteres transferidos:                        %d\n"
                    "Caracteres en buffer:                           %d\n"
                    "Espacio total de memoria utilizado              %d bytes\n"
                    "Tiempo total en modo usuario del cliente:       %f s\n"
                    "Tiempo total en modo kernel del cliente:        %f s\n"
                    "Tiempo total en modo usuario del reconstructor: %f s\n"
                    "Tiempo total en modo kernel del reconstructor:  %f s\n",
                    local_statistics.client_slp_t,
                    local_statistics.reconstructor_slp_t,
                    local_statistics.transfered_characters_n,
                    local_statistics.characters_in_buffer_n,
                    memory_used,
                    local_statistics.client_usr_mode_t,
                    local_statistics.client_krnl_mode_t,
                    local_statistics.reconstructor_usr_mode_t,
                    local_statistics.reconstructor_krnl_mode_t);


    gtk_label_set_text(GTK_LABEL(output_label), output_text);
}

int main(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);

    // Create the main window
    main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(main_window), "Statistics Viewer");
    g_signal_connect(main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a label to display the output
    output_label = gtk_label_new(NULL);
    gtk_label_set_line_wrap(GTK_LABEL(output_label), TRUE); // Enable text wrapping
    gtk_container_add(GTK_CONTAINER(main_window), output_label);

    // Update the label initially
    update_output_label();

    // Show all widgets
    gtk_widget_show_all(main_window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}
