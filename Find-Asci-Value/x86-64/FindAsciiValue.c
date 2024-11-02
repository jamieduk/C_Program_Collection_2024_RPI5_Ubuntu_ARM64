#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
// gcc -o FindAsciiValue FindAsciiValue.c `pkg-config --cflags --libs gtk+-3.0`


void on_ascii_button_clicked(GtkWidget *widget, gpointer entry) {
    const gchar *input=gtk_entry_get_text(GTK_ENTRY(entry));
    if (input[0] != '\0') {
        char c=input[0];
        char result[100];
        snprintf(result, sizeof(result), "ASCII value of %c=%d", c, c);
        GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", result);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

void on_about_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                               "ASCII Finder\n\nAuthor: Jay Mee");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "ASCII Finder");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Grid
    GtkWidget *grid=gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Entry
    GtkWidget *label=gtk_label_new("Enter a character:");
    GtkWidget *entry=gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry, 1, 0, 1, 1);

    // Button to get ASCII value
    GtkWidget *ascii_button=gtk_button_new_with_label("Get ASCII Value");
    g_signal_connect(ascii_button, "clicked", G_CALLBACK(on_ascii_button_clicked), entry);
    gtk_grid_attach(GTK_GRID(grid), ascii_button, 1, 1, 1, 1);

    // About button
    GtkWidget *about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(grid), about_button, 1, 2, 1, 1);

    // Show all widgets
    gtk_widget_show_all(window);

    // Start GTK main loop
    gtk_main();

    return 0;
}

