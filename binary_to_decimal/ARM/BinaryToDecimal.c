#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// gcc -o binary_to_decimal "BinaryToDecimal.c" `pkg-config --cflags --libs gtk+-3.0`

// Function to convert binary to decimal
long long binary_to_decimal(const char *binary) {
    long long decimal=0;
    int len=strlen(binary);
    for (int i=0; i < len; i++) {
        if (binary[len - 1 - i] == '1') {
            decimal += (1LL << i); // Equivalent to pow(2, i)
        }
    }
    return decimal;
}

// Function to convert decimal to binary
void decimal_to_binary(long long decimal, char *binary, size_t size) {
    if (decimal == 0) {
        snprintf(binary, size, "0");
        return;
    }

    char temp[65]; // Temporary buffer for binary string (64 bits + null terminator)
    int index=0;
    while (decimal > 0) {
        temp[index++]=(decimal % 2) + '0'; // Store the binary digit
        decimal /= 2;
    }

    // Reverse the string
    for (int i=0; i < index; i++) {
        binary[i]=temp[index - 1 - i];
    }
    binary[index]='\0'; // Null terminate the string
}

// Callback function for the convert button
void on_convert_button_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry **entries=(GtkEntry **)data; // Cast data to GtkEntry array
    const char *binary_input=gtk_entry_get_text(entries[0]); // Get binary input
    const char *decimal_input=gtk_entry_get_text(entries[1]); // Get decimal input

    // Initialize result strings
    char binary_result[65]; // Buffer for binary output
    long long decimal_value=0;

    // Perform binary to decimal conversion
    if (strlen(binary_input) > 0) {
        decimal_value=binary_to_decimal(binary_input);
        char result[100];
        snprintf(result, sizeof(result), "Decimal: %lld", decimal_value);

        // Show binary result in a message dialog
        GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "%s", result);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog); // Destroy dialog after use
    }

    // Perform decimal to binary conversion
    if (strlen(decimal_input) > 0) {
        long long decimal_input_value=atoll(decimal_input);
        decimal_to_binary(decimal_input_value, binary_result, sizeof(binary_result));

        // Show binary result in a message dialog
        char result[100];
        snprintf(result, sizeof(result), "Binary: %s", binary_result);

        GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "%s", result);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog); // Destroy dialog after use
    }
}

// Callback function for the about button
void on_about_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Author: Jay @ J~Net 2024 \n\nBinary to Decimal Converter\n\nConvert between binary and decimal numbers.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog); // Destroy dialog after use
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv); // Initialize GTK with command-line arguments

    // Create a new window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL); // Specify window type
    gtk_window_set_title(GTK_WINDOW(window), "Binary to Decimal Converter");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    // Connect the destroy signal to quit the application
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a label and entry for binary input
    GtkWidget *binary_label=gtk_label_new("Enter Binary Number:");
    gtk_box_pack_start(GTK_BOX(vbox), binary_label, FALSE, FALSE, 0);

    GtkWidget *binary_entry=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), binary_entry, FALSE, FALSE, 0);

    // Create a label and entry for decimal input
    GtkWidget *decimal_label=gtk_label_new("Enter Decimal Number:");
    gtk_box_pack_start(GTK_BOX(vbox), decimal_label, FALSE, FALSE, 0);

    GtkWidget *decimal_entry=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), decimal_entry, FALSE, FALSE, 0);

    // Create a convert button
    GtkWidget *convert_button=gtk_button_new_with_label("Convert");
    GtkEntry *entries[2]={ GTK_ENTRY(binary_entry), GTK_ENTRY(decimal_entry) };
    g_signal_connect(convert_button, "clicked", G_CALLBACK(on_convert_button_clicked), entries);
    gtk_box_pack_start(GTK_BOX(vbox), convert_button, FALSE, FALSE, 0);

    // Create an about button
    GtkWidget *about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 0);

    gtk_widget_show_all(window); // Show all widgets in the window
    gtk_main(); // Start the GTK main loop

    return 0;
}

