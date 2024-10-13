#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// gcc `pkg-config --cflags gtk+-3.0` -o text2binary text2binary.c `pkg-config --libs gtk+-3.0` -lm

// Function declarations
void on_convert(GtkWidget *widget, gpointer data);
void on_about(GtkWidget *widget, gpointer data);
void on_window_destroy(GtkWidget *widget, gpointer data);
void text_to_binary(const char *text, char *binary);
void binary_to_text(const char *binary, char *text);

GtkWidget *entry_text, *entry_binary, *label_result;
GtkWidget *radio_text_to_binary, *radio_binary_to_text; // Declare radio buttons globally

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Text-Binary Converter");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Create a vertical box for layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create input fields and labels
    GtkWidget *label_choice=gtk_label_new("Choose Conversion:");
    gtk_box_pack_start(GTK_BOX(vbox), label_choice, FALSE, FALSE, 0);

    radio_text_to_binary=gtk_radio_button_new_with_label(NULL, "Text to Binary");
    gtk_box_pack_start(GTK_BOX(vbox), radio_text_to_binary, FALSE, FALSE, 0);

    radio_binary_to_text=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_text_to_binary), "Binary to Text");
    gtk_box_pack_start(GTK_BOX(vbox), radio_binary_to_text, FALSE, FALSE, 0);

    entry_text=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry_text, FALSE, FALSE, 0);

    entry_binary=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry_binary, FALSE, FALSE, 0);

    GtkWidget *button_convert=gtk_button_new_with_label("Convert");
    g_signal_connect(button_convert, "clicked", G_CALLBACK(on_convert), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_convert, FALSE, FALSE, 0);

    GtkWidget *button_about=gtk_button_new_with_label("About");
    g_signal_connect(button_about, "clicked", G_CALLBACK(on_about), window);
    gtk_box_pack_start(GTK_BOX(vbox), button_about, FALSE, FALSE, 0);

    label_result=gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), label_result, FALSE, FALSE, 0);

    // Set dark theme
    GtkCssProvider *css_provider=gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, 
        "window { background-color: #2E2E2E; }\n"
        "label { color: #FFFFFF; }\n"
        "entry { background-color: #444444; color: #FFFFFF; }\n"
        "button { background-color: #007ACC; color: #FFFFFF; }\n", 
        -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), 
        GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_provider);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

void on_convert(GtkWidget *widget, gpointer data) {
    const gchar *text_input=gtk_entry_get_text(GTK_ENTRY(entry_text));
    const gchar *binary_input=gtk_entry_get_text(GTK_ENTRY(entry_binary));
    char result[1024]={0};

    // Check which radio button is active
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_text_to_binary))) {
        // Text to Binary
        text_to_binary(text_input, result);
        gtk_label_set_text(GTK_LABEL(label_result), result);
        gtk_entry_set_text(GTK_ENTRY(entry_binary), result); // Set binary output to the binary entry
    } else {
        // Binary to Text
        binary_to_text(binary_input, result);
        gtk_label_set_text(GTK_LABEL(label_result), result);
        gtk_entry_set_text(GTK_ENTRY(entry_text), result); // Set text output to the text entry
    }
}

void on_about(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Author: Jay @ J~Net 2024 \n\nText-Binary Converter\n\nConvert text to binary and binary to text.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void text_to_binary(const char *text, char *binary) {
    binary[0]='\0'; // Clear the binary string
    for (size_t i=0; i < strlen(text); i++) {
        for (int j=7; j >= 0; j--) {
            strcat(binary, ((text[i] & (1 << j)) ? "1" : "0"));
        }
        strcat(binary, " "); // Add space between characters
    }
}

void binary_to_text(const char *binary, char *text) {
    char *token;
    char *binary_copy=strdup(binary); // Duplicate the input to avoid modifying it
    text[0]='\0'; // Clear the text string

    token=strtok(binary_copy, " ");
    while (token != NULL) {
        char character=0;
        for (int i=0; i < 8; i++) {
            if (token[i] == '1') {
                character |= (1 << (7 - i));
            }
        }
        strncat(text, &character, 1); // Append the character to the result
        token=strtok(NULL, " ");
    }

    free(binary_copy); // Free the duplicated string
}

