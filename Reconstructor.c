#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <gtk/gtk.h>
#include <time.h>
#include "Constants.h"

UI(int argc, char *argv[], long date_time, char *curr_char) {
    // Initialize GTK
    gtk_init(&argc, &argv);
    
    // Create the main window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Re-constructor");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    // Create a vertical box container
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);
    //curchar message
    char cur_char_msg[24];
    sprintf(cur_char_msg, "Se está insertando: %c",*curr_char);
    // Create the first label
    GtkWidget *label_cur_char = gtk_label_new(cur_char_msg);
    gtk_box_pack_start(GTK_BOX(vbox), label_cur_char, FALSE, FALSE, 0);

    //Date format
    struct tm *local_time = localtime(&date_time);
    char date_time_str[24];
    strcpy(date_time_str, asctime(local_time));
    
    // Create the second label

    char date_time_msg[52];

    sprintf(date_time_msg, "Fecha y hora de insersión: \n%s", date_time_str);
    GtkWidget *label_date_time = gtk_label_new(date_time_msg);
    gtk_box_pack_start(GTK_BOX(vbox), label_date_time, FALSE, FALSE, 0);
    
    // Create a scrolled window to contain the text area
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    
    // Create the text buffer and set initial text
    GtkTextBuffer *buffer = gtk_text_buffer_new(NULL);

    // Create a 300-character string
    char myString[301]; // 300 characters + null terminator
    for (int i = 0; i < 300; ++i) {
        myString[i] = 'A' + i % 26; // Filling with letters from A to Z in a circular manner
    }
    myString[300] = '\0'; // Null terminator

    gtk_text_buffer_set_text(buffer, myString, -1);
    
    // Create the text view and set the buffer
    GtkWidget *text_view = gtk_text_view_new_with_buffer(buffer);
    
    // Set text view as not editable
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);
    
    // Show all widgets
    gtk_widget_show_all(window);
    
    // Start the GTK main loop
    gtk_main();
}
int main(int argc, char *argv[]) {
    const char *shared_memory_name = "/my_shared_memory";
    int shared_memory_fd;
    char *shaed_memory_ptr;
    struct stat stat_buffer;
    /*
    // Open the shared memory object
    shared_memory_fd = shm_open(shared_memory_name, O_RDONLY, 0666);
    if (shared_memory_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // Get information about the shared memory object
    if (fstat(shared_memory_fd, &stat_buffer) == -1) {
        perror("fstat");
        close(shared_memory_fd);
        return 1;
    }*/
    time_t t = time(NULL);
    char curr = 's';
    //GUI call
    UI(argc, argv, t, &curr);

    return 0;
}