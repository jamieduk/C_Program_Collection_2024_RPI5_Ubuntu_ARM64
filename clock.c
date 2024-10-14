#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

GtkWidget *alarm_hour_entry, *alarm_minute_entry, *countdown_hour_entry, *countdown_minute_entry, *countdown_second_entry, *about_label, *time_label;
gboolean alarm_set=FALSE;
gboolean countdown_active=FALSE;
int alarm_hour, alarm_minute;
int countdown_seconds=0; // Variable to track countdown seconds

// Function to update the current time label
void update_time_label() {
    time_t now=time(NULL);
    struct tm *local=localtime(&now);
    
    char time_string[9]; // Format HH:MM:SS
    snprintf(time_string, sizeof(time_string), "%02d:%02d:%02d", local->tm_hour, local->tm_min, local->tm_sec);
    gtk_label_set_text(GTK_LABEL(time_label), time_string);
}

// Beep function
void beep(int count) {
    for (int i=0; i < count; i++) {
        printf("\a"); // Generates a beep sound
        fflush(stdout);
        sleep(1); // Pause for a second between beeps
    }
}

// Alarm check function
gboolean check_alarm() {
    if (alarm_set) {
        time_t now=time(NULL);
        struct tm *local=localtime(&now);

        if (local->tm_hour == alarm_hour && local->tm_min == alarm_minute) {
            g_print("Alarm ringing!\n");
            beep(5); // Beep 5 times
            alarm_set=FALSE; // Reset alarm
        }
    }
    return TRUE; // Keep the timeout active
}

// Countdown function
gboolean countdown_timer() {
    if (countdown_active) {
        if (countdown_seconds > 0) {
            countdown_seconds--;
            int hours=countdown_seconds / 3600;
            int minutes=(countdown_seconds % 3600) / 60;
            int seconds=countdown_seconds % 60;

            gchar msg[100];
            snprintf(msg, sizeof(msg), "Countdown: %02d:%02d:%02d remaining.", hours, minutes, seconds);
            gtk_label_set_text(GTK_LABEL(about_label), msg);
        } else {
            gtk_label_set_text(GTK_LABEL(about_label), "Time's up!");
            beep(5); // Beep 5 times at the end of the countdown
            countdown_active=FALSE; // Stop countdown
        }
    }
    return TRUE; // Keep the timeout active
}

// Set alarm function
void set_alarm(GtkWidget *widget, gpointer data) {
    const gchar *hour_text=gtk_entry_get_text(GTK_ENTRY(alarm_hour_entry));
    const gchar *minute_text=gtk_entry_get_text(GTK_ENTRY(alarm_minute_entry));

    alarm_hour=atoi(hour_text);
    alarm_minute=atoi(minute_text);
    alarm_set=TRUE;

    gchar msg[50];
    snprintf(msg, sizeof(msg), "Alarm set for %02d:%02d.", alarm_hour, alarm_minute);
    gtk_label_set_text(GTK_LABEL(about_label), msg);
}

// Cancel alarm function
void cancel_alarm(GtkWidget *widget, gpointer data) {
    alarm_set=FALSE;
    gtk_label_set_text(GTK_LABEL(about_label), "Alarm cancelled.");
}

// Start countdown function
void start_countdown(GtkWidget *widget, gpointer data) {
    countdown_active=TRUE;

    int hours=atoi(gtk_entry_get_text(GTK_ENTRY(countdown_hour_entry)));
    int minutes=atoi(gtk_entry_get_text(GTK_ENTRY(countdown_minute_entry)));
    int seconds=atoi(gtk_entry_get_text(GTK_ENTRY(countdown_second_entry)));

    countdown_seconds=hours * 3600 + minutes * 60 + seconds;

    gchar msg[100];
    snprintf(msg, sizeof(msg), "Countdown started: %02d:%02d:%02d.", hours, minutes, seconds);
    gtk_label_set_text(GTK_LABEL(about_label), msg);
}

// Cancel countdown function
void cancel_countdown(GtkWidget *widget, gpointer data) {
    countdown_active=FALSE;
    countdown_seconds=0;
    gtk_label_set_text(GTK_LABEL(about_label), "Countdown cancelled.");
}

// Show about function
void show_about(GtkWidget *widget, gpointer data) {
    gtk_label_set_text(GTK_LABEL(about_label), "About:\nAuthor: Jay Mee\nThis program implements a clock with an alarm feature.");
}

// Timer thread for updating current time
gboolean update_time() {
    update_time_label();
    return TRUE; // Keep the timeout active
}

// Main function
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Clock with Alarm Feature");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid=gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    time_label=gtk_label_new("");
    alarm_hour_entry=gtk_entry_new();
    alarm_minute_entry=gtk_entry_new();
    
    countdown_hour_entry=gtk_entry_new();
    countdown_minute_entry=gtk_entry_new();
    countdown_second_entry=gtk_entry_new();

    GtkWidget *set_alarm_button=gtk_button_new_with_label("Set Alarm");
    g_signal_connect(set_alarm_button, "clicked", G_CALLBACK(set_alarm), NULL);

    GtkWidget *cancel_alarm_button=gtk_button_new_with_label("Cancel Alarm");
    g_signal_connect(cancel_alarm_button, "clicked", G_CALLBACK(cancel_alarm), NULL);

    GtkWidget *countdown_button=gtk_button_new_with_label("Start Countdown");
    g_signal_connect(countdown_button, "clicked", G_CALLBACK(start_countdown), NULL);

    GtkWidget *cancel_countdown_button=gtk_button_new_with_label("Cancel Countdown");
    g_signal_connect(cancel_countdown_button, "clicked", G_CALLBACK(cancel_countdown), NULL);

    GtkWidget *about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about), NULL);

    about_label=gtk_label_new("");

    // Add widgets to the grid layout
    gtk_grid_attach(GTK_GRID(grid), time_label, 0, 0, 2, 1); // Current time label at the top
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Alarm Hour (0-23):"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), alarm_hour_entry, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Alarm Minute (0-59):"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), alarm_minute_entry, 1, 2, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Countdown (HH:MM:SS):"), 0, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Hours:"), 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), countdown_hour_entry, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Minutes:"), 0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), countdown_minute_entry, 1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Seconds:"), 0, 6, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), countdown_second_entry, 1, 6, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), set_alarm_button, 0, 7, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cancel_alarm_button, 1, 7, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), countdown_button, 0, 8, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cancel_countdown_button, 1, 8, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), about_button, 0, 9, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), about_label, 0, 10, 2, 1);

    gtk_widget_show_all(window);

    // Start the timers for updating time, checking alarm, and countdown
    g_timeout_add(1000, update_time, NULL); // Update time every second
    g_timeout_add(1000, check_alarm, NULL); // Check alarm every second
    g_timeout_add(1000, countdown_timer, NULL); // Update countdown every second

    gtk_main();
    return 0;
}

