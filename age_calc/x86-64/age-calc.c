// Made By Jay @ J~Net 2024
//
// age_calc.c
// Made By Jay @ J~Net 2024
// age_calc_gui.c
//
// gcc `pkg-config --cflags gtk+-3.0` -o age-calc age-calc.c `pkg-config --libs gtk+-3.0`

// Made By Jay @ J~Net 2024
// age_calc_gui.c

// Made By Jay @ J~Net 2024
// age_calc_gui.c

// Made By Jay @ J~Net 2024
// age_calc_gui.c

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

GtkWidget *dob_entry;
GtkWidget *months_label;
GtkWidget *weeks_label;
GtkWidget *days_label;
GtkWidget *hours_label;
GtkWidget *seconds_label;
GtkWidget *heartbeats_label;

void calculate_age(GtkWidget *widget, gpointer data) {
    const char *dob_text=gtk_entry_get_text(GTK_ENTRY(dob_entry));
    
    // Parse the date of birth
    int day, month, year;
    sscanf(dob_text, "%d/%d/%d", &day, &month, &year);
    
    // Get current date
    time_t now=time(NULL);
    struct tm *current_time=localtime(&now);

    // Create a struct tm for the birth date
    struct tm birth_date={0};
    birth_date.tm_mday=day;
    birth_date.tm_mon=month - 1; // tm_mon is 0-11
    birth_date.tm_year=year - 1900; // tm_year is years since 1900

    // Calculate the difference in seconds
    double seconds_difference=difftime(now, mktime(&birth_date));
    
    // Calculate total age components
    int total_days=seconds_difference / (60 * 60 * 24);
    int total_weeks=total_days / 7;
    int total_months=total_days / 30; // Rough estimate
    int total_years=total_months / 12; // Rough estimate
    int total_hours=seconds_difference / 3600; // Total hours
    int total_heartbeats=total_hours * 60 * 80; // Average heartbeats (80 bpm)

    // Output results
    char months_output[50];
    char weeks_output[50];
    char days_output[50];
    char hours_output[50];
    char seconds_output[50];
    char heartbeats_output[50];

    snprintf(months_output, sizeof(months_output), "Age in months: %d", total_months);
    snprintf(weeks_output, sizeof(weeks_output), "Age in weeks: %d", total_weeks);
    snprintf(days_output, sizeof(days_output), "Age in days: %d", total_days);
    snprintf(hours_output, sizeof(hours_output), "Age in hours: %d", total_hours);
    snprintf(seconds_output, sizeof(seconds_output), "Age in seconds: %.0f", seconds_difference);
    snprintf(heartbeats_output, sizeof(heartbeats_output), "Approx. heartbeats: %d", total_heartbeats);

    gtk_label_set_text(GTK_LABEL(months_label), months_output);
    gtk_label_set_text(GTK_LABEL(weeks_label), weeks_output);
    gtk_label_set_text(GTK_LABEL(days_label), days_output);
    gtk_label_set_text(GTK_LABEL(hours_label), hours_output);
    gtk_label_set_text(GTK_LABEL(seconds_label), seconds_output);
    gtk_label_set_text(GTK_LABEL(heartbeats_label), heartbeats_output);
}

void show_about(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog=gtk_message_dialog_new(GTK_WINDOW(data),
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_INFO,
                                     GTK_BUTTONS_OK,
                                     "Author: Jay Mee @ J~Net 2024");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *calculate_button;
    GtkWidget *dob_label;
    GtkWidget *about_button;

    // Create a new window
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Age Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create grid for layout
    grid=gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Date of Birth input field
    dob_label=gtk_label_new("Please enter date of birth (dd/mm/yyyy):");
    gtk_grid_attach(GTK_GRID(grid), dob_label, 0, 0, 1, 1);

    dob_entry=gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), dob_entry, 0, 1, 3, 1);

    // Calculate button
    calculate_button=gtk_button_new_with_label("Calculate");
    g_signal_connect(calculate_button, "clicked", G_CALLBACK(calculate_age), NULL);
    gtk_grid_attach(GTK_GRID(grid), calculate_button, 1, 2, 1, 1);

    // About button
    about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about), window);
    gtk_grid_attach(GTK_GRID(grid), about_button, 2, 2, 1, 1);

    // Labels to display results
    months_label=gtk_label_new("Age in months: ");
    gtk_grid_attach(GTK_GRID(grid), months_label, 0, 3, 3, 1);

    weeks_label=gtk_label_new("Age in weeks: ");
    gtk_grid_attach(GTK_GRID(grid), weeks_label, 0, 4, 3, 1);

    days_label=gtk_label_new("Age in days: ");
    gtk_grid_attach(GTK_GRID(grid), days_label, 0, 5, 3, 1);

    hours_label=gtk_label_new("Age in hours: ");
    gtk_grid_attach(GTK_GRID(grid), hours_label, 0, 6, 3, 1);

    seconds_label=gtk_label_new("Age in seconds: ");
    gtk_grid_attach(GTK_GRID(grid), seconds_label, 0, 7, 3, 1);

    heartbeats_label=gtk_label_new("Approx. heartbeats: ");
    gtk_grid_attach(GTK_GRID(grid), heartbeats_label, 0, 8, 3, 1);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

