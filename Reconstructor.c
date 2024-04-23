#include "Reconstructor.h"
#include "Constants.h"

//UI elements definitin

GtkWidget *label_cur_char;
GtkWidget *label_date_time;
GtkWidget *text_view;
GtkTextBuffer *buffer_ui;

struct descriptor *memory_desc;
int shared_memory_length;
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

        printf(argv[3]);
        }else{
        fprintf(stderr, "Interval not defined, 1 sec set as default");
        }       
    }else{
        interval=1;
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
    
    //open file in append mode
    Data_fd = fopen(filepath, "a");
    if (Data_fd==NULL)
    {
        perror("fstat");
        fclose(Data_fd);
    }

    memory_desc=shared_memory_ptr;
    shared_memory_length=memory_desc->buffer_size;
    datetimes=shared_memory_ptr+descriptor_size+statistics_size;
    buffer=shared_memory_ptr+shared_memory_length*sizeof(time_t)+descriptor_size+statistics_size;
    
    data=malloc(memory_desc->data_size);//Reserve space for memory

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
    
    if (strcmp(mode,"Automatic")==0)
    {
        clock_t begin = clock();
        while (memory_desc->data_size>0)
        {
            if (((double)(clock() - begin) / CLOCKS_PER_SEC)>=interval){
                dequeue();
                begin = clock();
            }
        }
        
    }
    pthread_join(ui_thread, NULL);
    return 0;
}

void dequeue(){
    
    //get index to read from buffer
    sem_wait(&(memory_desc->reader_semaphore));
    int offset = memory_desc->reader_pointer;
    memory_desc->reader_pointer += memory_desc->data_size;
    sem_post(&(memory_desc->reader_semaphore));
    
    //index in circular buffer
    offset=offset%(memory_desc->buffer_size*memory_desc->data_size);
    void* buffer_read_pos=offset+buffer;
    time_t* date_read_pos=offset+datetimes;
    
    //down reader semaphore
    sem_wait(&(memory_desc->buffer_reader_semaphore));
    

    char cur_char;
    time_t date_time;
    //read char and copy it in file
    Data_fd = fopen(filepath, "a");
    memcpy(&cur_char,buffer_read_pos,memory_desc->data_size);
    fprintf(Data_fd, "%c",cur_char);
    fclose(Data_fd);
    memcpy(&date_time, date_read_pos,sizeof(time_t));

    //update UI
    update_cur_char(cur_char);
    update_date_time(date_time);

    //update_text_view_with_file();
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
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer_ui, text, -1);
}

void update_text_view_with_file() {
    FILE * f = fopen(filepath ,"r");
    char* file_contents;
    long length;
    if (f){
        fseek(f,0,SEEK_END);
        length = ftell(f);
        fseek(f,0,SEEK_SET);
        file_contents = malloc(length+1);
        if (file_contents){
            file_contents[length] = '\0';
            fread(file_contents,1,length,f);
            gboolean is_valid = g_utf8_validate(file_contents, -1, NULL);
            if (is_valid) {
                gtk_text_buffer_set_text(buffer_ui, file_contents, -1);
            }
            free(file_contents);
        }
        fclose(f);
        
    }
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
    gtk_box_pack_start(GTK_BOX(vbox), label_cur_char, FALSE, FALSE, 0);

    
    // Create the second label
    label_date_time = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), label_date_time, FALSE, FALSE, 0);
    
    // Create a scrolled window to contain the text area
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    
    // Create the text buffer and set initial text
    buffer_ui = gtk_text_buffer_new(NULL);

    

    
    // Create the text view and set the buffer
    text_view = gtk_text_view_new_with_buffer(buffer_ui);
    // Set text view as not editable
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    
    // Show all widgets
    gtk_widget_show_all(window);

    setup_file_monitor(buffer_ui, filepath);

    // Start the GTK main loop
    gtk_main();
}

static void setup_file_monitor(GtkTextBuffer *buffer, const gchar *file_path) {
    GFile *file = g_file_new_for_path(file_path);
    GFileMonitor *monitor = g_file_monitor_file(file, G_FILE_MONITOR_NONE, NULL, NULL);

    g_signal_connect(monitor, "changed", G_CALLBACK(on_file_changed), buffer);
    g_object_unref(file);
}

static void on_file_changed(GFileMonitor *monitor, GFile *file, GFile *other_file,
                             GFileMonitorEvent event_type, gpointer user_data) {
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(user_data);
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);

    // Clear existing buffer
    gtk_text_buffer_delete(buffer, &start, &end);

    // Read the file and update buffer
    gchar *contents;
    gsize length;
    if (g_file_load_contents(file, NULL, &contents, &length, NULL, NULL)) {
        gtk_text_buffer_insert(buffer, &start, contents, length);
        g_free(contents);
    }
}