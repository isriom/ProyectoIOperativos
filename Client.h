#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <gtk/gtk.h>

#include "Constants.h"


int main(int argc, char *argv[]);

int updateGTKBuffer();
void *write_loop();
void ReadWrite();

int UI(int argc, char *argv[]);

void check_enter(GtkWidget *widget, GdkEventKey *event, gpointer user_data);