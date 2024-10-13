#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>
// gcc dice.c -o dice $(pkg-config --cflags --libs gtk+-3.0) -y

// Function prototypes
void on_roll_button_clicked(GtkWidget *widget, gpointer label);
void on_exit_button_clicked(GtkWidget *widget);
void on_about_button_clicked(GtkWidget *widget, gpointer window);

void on_roll_button_clicked(GtkWidget *widget, gpointer label) {
    int dice_value=rand() % 6 + 1; // Roll the dice (1-6)
    char result[20];
    snprintf(result, sizeof(result), "You rolled: %d", dice_value);
    gtk_label_set_text(GTK_LABEL(label), result);
}

void on_exit_button_clicked(GtkWidget *widget) {
    gtk_main_quit(); // Exit the application
}

void on_about_button_clicked(GtkWidget *widget, gpointer window) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(window),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Author: Jay @ J~Net 2024");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog); // Destroy the dialog after use
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv); // Initialize GTK

    srand(time(NULL)); // Seed for random number generation

    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *roll_button;
    GtkWidget *roll_again_button;
    GtkWidget *exit_button;
    GtkWidget *about_button;
    GtkWidget *label;

    // Create the main window
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Dice Roll Game");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to arrange the widgets
    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the label to display the result
    label=gtk_label_new("Roll the dice!");
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

    // Create buttons
    //roll_button=gtk_button_new_with_label("Roll");
    roll_again_button=gtk_button_new_with_label("Roll Again");
    exit_button=gtk_button_new_with_label("Exit");
    about_button=gtk_button_new_with_label("About");

    // Connect button signals
    g_signal_connect(roll_button, "clicked", G_CALLBACK(on_roll_button_clicked), label);
    g_signal_connect(roll_again_button, "clicked", G_CALLBACK(on_roll_button_clicked), label);
    g_signal_connect(exit_button, "clicked", G_CALLBACK(on_exit_button_clicked), NULL);
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_button_clicked), window);

    // Pack buttons into the box
    gtk_box_pack_start(GTK_BOX(vbox), roll_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), roll_again_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), exit_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), about_button, TRUE, TRUE, 0);

    // Show all widgets
    gtk_widget_show_all(window);

    gtk_main(); // Enter the GTK main loop

    return 0;
}

