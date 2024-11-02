#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>
// gcc 8ball.c -o 8ball $(pkg-config --cflags --libs gtk+-3.0)

const char *answers[]={
    "Yes",
    "No",
    "Ask again later",
    "Definitely",
    "Absolutely not",
    "Maybe",
    "It is certain",
    "Don't count on it",
    "Yes, in due time",
    "Very doubtful"
};

void on_ask_button_clicked(GtkWidget *widget, gpointer label) {
    int index=rand() % (sizeof(answers) / sizeof(answers[0])); // Get a random answer
    gtk_label_set_text(GTK_LABEL(label), answers[index]);
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
    GtkWidget *ask_button;
    GtkWidget *about_button;
    GtkWidget *label;

    // Create the main window
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Magic 8 Ball");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to arrange the widgets
    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the label to display the answer
    label=gtk_label_new("Ask the 8 Ball a question!");
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 0);

    // Create the "Ask the 8 Ball" button
    ask_button=gtk_button_new_with_label("Ask the 8 Ball");
    g_signal_connect(ask_button, "clicked", G_CALLBACK(on_ask_button_clicked), label);
    gtk_box_pack_start(GTK_BOX(vbox), ask_button, TRUE, TRUE, 0);

    // Create the "About" button
    about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_button_clicked), window);
    gtk_box_pack_start(GTK_BOX(vbox), about_button, TRUE, TRUE, 0);

    // Show all widgets
    gtk_widget_show_all(window);

    gtk_main(); // Enter the GTK main loop

    return 0;
}

