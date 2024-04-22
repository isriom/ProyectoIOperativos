#include "Constants.h"
#include "Client.h"

struct descriptor *memory_desc;
int shared_memory_length;
time_t *datetimes;
void *buffer;

void *data;

unsigned int interval=1;

int shared_memory_fd;
int Data_fd;
void *shared_memory_ptr;
struct stat stat_buffer;
GtkTextBuffer *Gtkbuffer;
int main(int argc, char *argv[]) {


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
        perror("open");
        close(Data_fd);
    }
    
    
    memory_desc=shared_memory_ptr;
    shared_memory_length=memory_desc->buffer_size;
    datetimes=shared_memory_ptr+descriptor_size+statistics_size;
    buffer=shared_memory_ptr+shared_memory_length*sizeof(time_t)+descriptor_size+statistics_size;
    
    data=malloc(memory_desc->data_size);//Reserve space for memory

    //UI(argc,argv);
    printf("out GUI");
    if (strcmp(mode,"Automatic")==0)
    {
        while (memory_desc->data_size>0)
        {
            ReadWrite();
        }
        
    }
}

void updateGTKBuffer(){
        #define byte_to_show 50
        #define line_length 10
        char tmpbuffer[byte_to_show]; 
        int error=pread(Data_fd,tmpbuffer,byte_to_show, memory_desc->writer_pointer);//Read from file and save on data;
        if (error==-1)
        {
            perror("pread");
        }
        gchar finalbuffer[byte_to_show+(byte_to_show/line_length)*2];
        for (size_t i = 0; i < byte_to_show/line_length; i++)
        {
            for (size_t a = 0; a < line_length; a++)
            {
                finalbuffer[a+i*line_length+i]=tmpbuffer[a+i*line_length];
            }
            finalbuffer[line_length+1+(line_length+1)*i]=*"\r";
        }
        gtk_text_buffer_set_text(Gtkbuffer,finalbuffer,(byte_to_show+(byte_to_show/line_length)*2));
}

void ReadWrite(){

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
        //updateGTKBuffer();
        printf("data: %c \n",*((char*)data));
        sleep(interval);
    
}

int UI(int argc, char *argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);
    
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
    Gtkbuffer = gtk_text_buffer_new(NULL);

    gtk_text_buffer_set_text(Gtkbuffer, "myString\0", -1);
    
    // Create the text view and set the buffer
    GtkWidget *text_view = gtk_text_view_new_with_buffer(Gtkbuffer);
    
    // Set text view as not editable
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    
    // Show all widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();
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