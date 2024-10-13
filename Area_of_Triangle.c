#include <gtk/gtk.h>
// gcc Area_of_Triangle.c -o AreaofTriangle `pkg-config --cflags --libs gtk+-3.0`

static void calculate_area(GtkWidget *widget, gpointer data) {
    float base, height;
    GtkEntry **entries=(GtkEntry **)data;  // Array of GtkEntry pointers
    const char *base_str=gtk_entry_get_text(entries[0]);  // Get base from entry
    const char *height_str=gtk_entry_get_text(entries[1]); // Get height from entry

    base=atof(base_str);  // Convert string to float
    height=atof(height_str);

    float area=0.5 * base * height; // Calculate area of the triangle

    // Display the result in a message dialog
    gchar result[100];
    snprintf(result, sizeof(result), "Area of the triangle: %.2f", area);
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
        "Author: Jay @ J~Net 2024 \n\nTriangle Area Calculator\n\nCalculate the area of a triangle using its base and height.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Triangle Area Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    // Create a vertical box layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create labels and entries for base and height input
    GtkWidget *base_label=gtk_label_new("Enter the base:");
    gtk_box_pack_start(GTK_BOX(vbox), base_label, FALSE, FALSE, 0);
    
    GtkEntry *base_entry=GTK_ENTRY(gtk_entry_new());
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(base_entry), FALSE, FALSE, 0);

    GtkWidget *height_label=gtk_label_new("Enter the height:");
    gtk_box_pack_start(GTK_BOX(vbox), height_label, FALSE, FALSE, 0);
    
    GtkEntry *height_entry=GTK_ENTRY(gtk_entry_new());
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(height_entry), FALSE, FALSE, 0);

    // Create an array to hold entries
    GtkEntry *entries[2]={ base_entry, height_entry };

    // Create a button to calculate the area
    GtkWidget *button=gtk_button_new_with_label("Calculate Area");
    g_signal_connect(button, "clicked", G_CALLBACK(calculate_area), entries);
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

