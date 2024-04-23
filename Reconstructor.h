#ifndef RECONSTRUCTOR_H
#define RECONSTRUCTOR_H
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
#include<pthread.h>
struct ui_arguments{

    int argc;
    
    long date_time;
    char curr_char;
    char **argv;
};
void update_date_time(long date_time);
void update_cur_char(char curr_char);
void update_text_view(char *text);
void *UI(void *arguments);
void dequeue();
void update_text_view_with_file();
static void on_file_changed(GFileMonitor *monitor, GFile *file, GFile *other_file,
                             GFileMonitorEvent event_type, gpointer user_data);
static void setup_file_monitor(GtkTextBuffer *buffer, const gchar *file_path);
#endif