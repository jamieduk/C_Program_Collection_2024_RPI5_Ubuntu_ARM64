#include <gtk/gtk.h>
#include <math.h>
// gcc AreaAndCircumference.c -o AreaAndCircumference `pkg-config --cflags --libs gtk+-3.0` -lm

static void calculate_area_and_circumference(GtkWidget *widget, gpointer data) {
    double r;
    GtkEntry *entry=GTK_ENTRY(data);
    const char *radius_str=gtk_entry_get_text(entry);
    r=atof(radius_str); // Convert string to double

    double area=M_PI * r * r;
    double circumference=2 * M_PI * r;

    // Display the results in a message dialog
    gchar result[100];
    snprintf(result, sizeof(result), "Area: %.2f\nCircumference: %.2f", area, circumference);
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
        "Author: Jay @ J~Net 2024 \n\nArea and Circumference Calculator\n\nCalculate the area and circumference of a circle using its radius.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create the main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Area and Circumference Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    // Create a vertical box layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a label and entry for radius input
    GtkWidget *label=gtk_label_new("Enter radius:");
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);

    GtkWidget *entry=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    // Create a button to calculate area and circumference
    GtkWidget *button=gtk_button_new_with_label("Calculate");
    g_signal_connect(button, "clicked", G_CALLBACK(calculate_area_and_circumference), entry);
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

