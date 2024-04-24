#include "Constants.h"
#include "Client.h"
#define byte_to_show 500
struct descriptor *memory_desc;
int shared_memory_length;
time_t *datetimes;
void *buffer;
struct statistics *statistics_desc;

void *data;

unsigned int interval=1;

int shared_memory_fd;
int Data_fd;
void *shared_memory_ptr;
struct stat stat_buffer;
struct stat stat_file;
GtkTextBuffer *Gtkbuffer;
char* file_Content=NULL;

clock_t start_clock, end_clock, start_slp_time, end_slp_time;
struct timeval start_time, end_time;
double sys_time, user_time, sleep_t_pointer, sleep_t_read;

void ReadWrite(){
        if (memory_desc->client_done)
        {
            return;
        }
        
        //Get position to read from file
        start_slp_time = clock(); //timing stats
        sem_wait(&(memory_desc->writer_semaphore));
        //timing stats
        end_slp_time = clock();
        sleep_t_pointer = (double)(end_slp_time-start_slp_time) / CLOCKS_PER_SEC;
        int offset=memory_desc->writer_pointer;

        if (offset>=stat_file.st_size)
        {
            printf("%d,%ld",offset,stat_file.st_size);
            sem_post(&(memory_desc->writer_semaphore));
            memory_desc->client_done=1;
            return;
        }
        

        memory_desc->writer_pointer+=memory_desc->data_size;

        //update kernel and user times
        statistics_desc->client_slp_t=statistics_desc->client_slp_t+sleep_t_pointer+sleep_t_read;
        gettimeofday(&end_time, NULL);
        end_clock = clock();
        sys_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
        user_time = (double)(end_clock - start_clock) / CLOCKS_PER_SEC;

        statistics_desc->client_krnl_mode_t = statistics_desc->client_krnl_mode_t + sys_time;
        statistics_desc->client_usr_mode_t = statistics_desc->client_usr_mode_t + user_time;
        gettimeofday(&start_time,NULL);
        start_clock = clock();
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
        start_slp_time = clock(); //timing stats
        
        sem_wait(&(memory_desc->buffer_writer_semaphore));
        //timing stats
        end_slp_time = clock();
        sleep_t_pointer = (double)(end_slp_time-start_slp_time) / CLOCKS_PER_SEC;
        
        memmove(buffer_write_pos,data,memory_desc->data_size);
        *date_write_pos=time_to_save;
        
        

        sem_post(&(memory_desc->buffer_reader_semaphore));
        printf("data: %c \n",*((char*)data));
        sleep(interval);
    
}

void *write_loop(){
        while (!memory_desc->client_done)
        {
            ReadWrite();
            printf("%d,%ld",0,stat_file.st_size);
        }

            printf("%d,%ld",1,stat_file.st_size);
        return NULL;
}



gboolean updateGTKBuffer(gboolean data){

        int error=pread(Data_fd,file_Content,byte_to_show, memory_desc->writer_pointer);//Read from file and save on data;
        if (error==-1)
        {
            perror("pread");
        }
        for (size_t i = 0; i < byte_to_show; i++)
        gtk_text_buffer_set_text(Gtkbuffer,file_Content,-1);
        return G_SOURCE_CONTINUE;
}

void check_enter(GtkWidget *widget, GdkEventKey *event, gpointer user_data){
    
    switch (event->keyval)
    {
    case GDK_KEY_Return:
        ReadWrite();
        break;
    
    default:
        break;
    }
}

int UI(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);
    Gtkbuffer = gtk_text_buffer_new(NULL);
    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Cliente");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Create a vertical box container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // Create a scrolled window to contain the text area
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    
    // Create the text buffer and set initial text


    gtk_text_buffer_set_text(Gtkbuffer, "myString\0", -1);
    
    // Create the text view and set the buffer
    GtkWidget *text_view = gtk_text_view_new_with_buffer(Gtkbuffer);
    
    // Set text view as not editable
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    
    // Show all widgets
    gtk_widget_show_all(window);

    g_idle_add((GSourceFunc)updateGTKBuffer,NULL);

    //add manual write
    if (!interval)
    {
        g_signal_connect(window, "key-press-event", G_CALLBACK(check_enter), NULL);
    }else{
        pthread_t thread;
        pthread_create(&thread, NULL, write_loop,NULL);
    }


    // Start the GTK main loop
    gtk_main();
}

int main(int argc, char *argv[]) {
    start_clock = clock();
    gettimeofday(&start_time,NULL);

    if (argc < 3) {
        fprintf(stderr, "Usage: %s <File> <Mode> [interval]\n Mode = Automatic|Manual\n", argv[0]);
        return 1;
    }

    const char *file_path = argv[1];
    const char *mode = argv[2];
    if (strcmp(mode,"Automatic")==0)
    {
        if (argc==4)
        {
        interval = atoi( argv[3]);
        }else{
        fprintf(stderr, "Interval not defined, 1 sec set as default");
        }       
    }else{
        interval=0;
    }

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
        perror("fstat Mem");
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
        perror("open");
        close(Data_fd);
    }
        // Get information about the file object
    if (fstat(Data_fd, &stat_file) == -1) {
        perror("fstat Data");
        close(Data_fd);
        return 1;
    }
    
    memory_desc=shared_memory_ptr;
    shared_memory_length=memory_desc->buffer_size;
    datetimes=shared_memory_ptr+descriptor_size+statistics_size;
    buffer=shared_memory_ptr+shared_memory_length*sizeof(time_t)+descriptor_size+statistics_size;
    
    data=malloc(memory_desc->data_size);//Reserve space for memory
    file_Content=(char*)malloc(byte_to_show+1);
    file_Content[byte_to_show]='\0';

    statistics_desc=shared_memory_ptr + descriptor_size;


    UI(argc, argv);
}