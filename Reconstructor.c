#include "Reconstructor.h"
#include "Constants.h"

//UI elements definitin

GtkWidget *label_cur_char;
GtkWidget *label_date_time;
GtkWidget *text_view;
GtkTextBuffer *buffer_ui;

struct descriptor *memory_desc;
int shared_memory_lenght;
time_t *datetimes;
void *buffer;

void *data;

unsigned int interval = 1;

int shared_memory_fd;
FILE *Data_fd;
void *shared_memory_ptr;

struct stat stat_buffer;
const char *filepath;
int main(int argc, char *argv[]) {
    
    if(argc < 3){
        fprintf(stderr, "Usage: %s <File> <Mode> [interval]\n Mode = Automatic|Manual\n", argv[0]);
        return 1;
    }

    filepath = argv[1];
    const char *mode = argv[2];

    if (strcmp(mode,"Automatic"))
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
    
    
    //UI STUFF
    struct ui_arguments *args = malloc(sizeof(struct ui_arguments));
    time_t t = time(NULL);
    args->date_time = t;
    char curr = 's';
    args->curr_char = curr;
    args->argc = argc;

    pthread_t ui_thread;

    //creating ui thread
    if (pthread_create(&ui_thread, NULL, UI, args) != 0){
        printf("nope\n");
        return -1;
    }
    
    pthread_join(ui_thread, NULL);
    return 0;

    restart:

    //open shared memory object
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

    //open file in append mode
    Data_fd = fopen(filepath, "a");
    if (Data_fd==NULL)
    {
        perror("fstat");
        fclose(Data_fd);
    }

    struct descriptor *memory_desc=shared_memory_ptr;
    int shared_memory_length=memory_desc->buffer_size;
    time_t *datetimes=shared_memory_ptr+descriptor_size+statistics_size;
    void *buffer=datetimes+shared_memory_length*sizeof(time_t);
    
    void *data=malloc(memory_desc->data_size);//Reserve space for memory

    if (strcmp(mode,"Automatic"))
    {
        while (memory_desc->data_size>0)
        {
            dequeue();
        }
        
    }
}

void dequeue(){
    //get index to read from buffer
    sem_wait(&(memory_desc->reader_semaphore));
    int offset = memory_desc->reader_pointer;
    memory_desc->reader_pointer += memory_desc->data_size;
    sem_post(&(memory_desc->reader_semaphore));
    
    //index in circular buffer
    offset = offset%(memory_desc->buffer_size*memory_desc->data_size);
    void* buffer_read_pos = offset + buffer;
    time_t* date_read_pos = offset +  datetimes;

    //down reader semaphore
    sem_wait(&(memory_desc->buffer_reader_semaphore));
    char cur_char;
    time_t date_time;
    //read char and copy it in file
    memcpy(&cur_char,buffer_read_pos,memory_desc->data_size);
    fprintf(Data_fd, "%c",cur_char);
    fclose(Data_fd);
    Data_fd = fopen(filepath, "a");
    memcpy(&date_time, date_read_pos,sizeof(time_t));

    //update UI
    update_cur_char(cur_char);
    update_date_time(date_time);
    //up writer semaphore
    sem_post(&(memory_desc->buffer_writer_semaphore));
    

}
void update_date_time(long date_time){
    struct tm *local_time = localtime(&date_time);
    char *date_time_str = malloc(strlen(asctime(local_time)) + 1);
    strcpy(date_time_str, asctime(local_time));
    char *date_time_msg = malloc(30 + strlen(date_time_str) + 1);
    sprintf(date_time_msg, "Fecha y hora de insersión: \n%s", date_time_str);
    gtk_label_set_text(GTK_LABEL(label_date_time),date_time_msg);
    free(date_time_str);
    free(date_time_msg);
}

void update_cur_char(char curr_char){
    char *curr_char_msg = malloc(sizeof(char)*21+1);
    sprintf(curr_char_msg, "Se está insertando: %c",curr_char);
    gtk_label_set_text(GTK_LABEL(label_cur_char),curr_char_msg);
    free(curr_char_msg);
}

void update_text_view(char *text){
    
    gtk_text_buffer_set_text(buffer_ui, text, -1);
}
void *UI(void *arguments){
    // Initialize GTK

    struct ui_arguments *args = (struct ui_arguments*)arguments;
    int argc = args->argc;
    gtk_init(&argc, NULL);
    
    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Re-constructor");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Create a vertical box container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    
    // Create the first label
    label_cur_char = gtk_label_new("");
    update_cur_char(args->curr_char);
    gtk_box_pack_start(GTK_BOX(vbox), label_cur_char, FALSE, FALSE, 0);

    
    // Create the second label
    label_date_time = gtk_label_new("");
    update_date_time(args->date_time);
    gtk_box_pack_start(GTK_BOX(vbox), label_date_time, FALSE, FALSE, 0);
    
    // Create a scrolled window to contain the text area
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    
    // Create the text buffer and set initial text
    buffer_ui = gtk_text_buffer_new(NULL);

    // Create a 300-character string
    char myString[301]; // 300 characters + null terminator
    for (int i = 0; i < 300; ++i) {
        myString[i] = 'A' + i % 26; // Filling with letters from A to Z in a circular manner
    }
    myString[300] = '\0'; // Null terminator

    update_text_view(myString);
    // Create the text view and set the buffer
    text_view = gtk_text_view_new_with_buffer(buffer_ui);
    
    // Set text view as not editable
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    
    // Show all widgets
    gtk_widget_show_all(window);
    
    // Start the GTK main loop
    gtk_main();
}