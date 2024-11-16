// Made By Jay @ J~Net 2024
//
// gcc stopwatch.c -o stopwatch `pkg-config --cflags --libs gtk+-3.0`
//
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    GtkLabel *time_label;
    guint interval_id;
    int running;
    int milliseconds; // Store total elapsed milliseconds
} StopwatchData;

// Callback to update the time
gboolean update_time(gpointer data) {
    StopwatchData *sw_data=(StopwatchData *)data;

    sw_data->milliseconds += 10; // Update every 10ms
    int total_seconds=sw_data->milliseconds / 1000;
    int ms=(sw_data->milliseconds % 1000) / 10; // Convert ms to 2 digits
    int secs=total_seconds % 60;
    int mins=(total_seconds / 60) % 60;
    int hours=total_seconds / 3600;

    char time_str[32];
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d.%02d", hours, mins, secs, ms);
    gtk_label_set_text(sw_data->time_label, time_str);

    return TRUE; // Continue the timer
}

// Start button callback
void start_stopwatch(GtkButton *button, StopwatchData *sw_data) {
    if (!sw_data->running) {
        sw_data->interval_id=g_timeout_add(10, update_time, sw_data); // Update every 10ms
        sw_data->running=1;
    }
}

// Stop button callback
void stop_stopwatch(GtkButton *button, StopwatchData *sw_data) {
    if (sw_data->running) {
        g_source_remove(sw_data->interval_id);
        sw_data->running=0;
    }
}

// Reset button callback
void reset_stopwatch(GtkButton *button, StopwatchData *sw_data) {
    if (sw_data->running) {
        g_source_remove(sw_data->interval_id);
        sw_data->running=0;
    }
    sw_data->milliseconds=0;
    gtk_label_set_text(sw_data->time_label, "00:00:00.00");
}

// About button callback
void show_about(GtkButton *button, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Author: Jay Mee");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Stopwatch");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box container
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create and add the time label
    GtkWidget *time_label=gtk_label_new("00:00:00.00");
    gtk_box_pack_start(GTK_BOX(vbox), time_label, TRUE, TRUE, 5);

    // Create buttons
    GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

    GtkWidget *start_button=gtk_button_new_with_label("Start");
    GtkWidget *stop_button=gtk_button_new_with_label("Stop");
    GtkWidget *reset_button=gtk_button_new_with_label("Reset");
    GtkWidget *about_button=gtk_button_new_with_label("About");

    gtk_box_pack_start(GTK_BOX(hbox), start_button, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), stop_button, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), reset_button, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), about_button, TRUE, TRUE, 5);

    // Stopwatch data
    StopwatchData sw_data={
        .time_label=GTK_LABEL(time_label),
        .interval_id=0,
        .running=0,
        .milliseconds=0,
    };

    // Connect button signals
    g_signal_connect(start_button, "clicked", G_CALLBACK(start_stopwatch), &sw_data);
    g_signal_connect(stop_button, "clicked", G_CALLBACK(stop_stopwatch), &sw_data);
    g_signal_connect(reset_button, "clicked", G_CALLBACK(reset_stopwatch), &sw_data);
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about), NULL);

    // Show all widgets
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

