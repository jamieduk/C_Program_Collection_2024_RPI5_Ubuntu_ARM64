#include <gtk/gtk.h>
// gcc Area_of_Square.c -o Area_of_Square `pkg-config --cflags --libs gtk+-3.0`

static void calculate_area(GtkWidget *widget, gpointer data) {
    float r;
    GtkEntry *entry=GTK_ENTRY(data);
    const char *radius_str=gtk_entry_get_text(entry);
    r=atof(radius_str); // Convert string to float

    float area=r * r;

    // Display the result in a message dialog
    gchar result[100];
    snprintf(result, sizeof(result), "Area: %.2f", area);
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", result);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Callback function for the about button
static void on_about_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Author: Jay @ J~Net 2024 \n\nArea Calculator\n\nCalculate the area of a square using the length of its side.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Area Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    // Create a vertical box layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a label and entry for radius input
    GtkWidget *label=gtk_label_new("Enter the Radius:");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    GtkWidget *entry=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    // Create a button to calculate the area
    GtkWidget *button=gtk_button_new_with_label("Calculate Area");
    g_signal_connect(button, "clicked", G_CALLBACK(calculate_area), entry);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    // Create an about button
    GtkWidget *about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 0);

    // Connect the window's destroy event to quit the application
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

