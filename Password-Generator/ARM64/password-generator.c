// Created By J~Net 2024
//
//
//
//
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// Function to copy text from GtkEntry to clipboard
void copy_to_clipboard(GtkWidget *widget, gpointer entry_result) {
    const char *password=gtk_entry_get_text(GTK_ENTRY(entry_result));

    // Check if the password is valid
    if (strlen(password) == 0 || strcmp(password, "Minimum length is 8") == 0) {
        gtk_entry_set_text(GTK_ENTRY(entry_result), "No valid password to copy!");
        return;
    }

    GtkClipboard *clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clipboard, password, -1); // Copy the password to clipboard
    gtk_entry_set_text(entry_result, "Password copied to clipboard!"); // Set the status message to the entry
}


// Password Generator Functionality
void password_generator(GtkWidget *widget, gpointer data) {
    GtkEntry *entry_length=GTK_ENTRY(((GtkWidget **)data)[0]);
    GtkEntry *entry_result=GTK_ENTRY(((GtkWidget **)data)[1]);

    const int MIN_LENGTH=8;
    const char *length_text=gtk_entry_get_text(entry_length);
    int length=atoi(length_text);

    // Default length to 8 if no value is provided
    if (length < MIN_LENGTH && strlen(length_text) > 0) {
        gtk_entry_set_text(entry_result, "Minimum length is 8");
        return;
    }

    // Set length to 8 if the input is invalid or empty
    if (length < MIN_LENGTH || strlen(length_text) == 0) {
        length=MIN_LENGTH;
    }

    const char lower[]="abcdefghijklmnopqrstuvwxyz";
    const char upper[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char digits[]="0123456789";
    const char special[]="!@#$%^&*()";

    char password[length + 1];
    srand(time(NULL));

    // Ensure the password meets the requirements
    password[0]=upper[rand() % (sizeof(upper) - 1)]; // At least 3 uppercase
    password[1]=upper[rand() % (sizeof(upper) - 1)];
    password[2]=upper[rand() % (sizeof(upper) - 1)];
    password[3]=digits[rand() % (sizeof(digits) - 1)]; // At least 2 digits
    password[4]=digits[rand() % (sizeof(digits) - 1)];
    password[5]=special[rand() % (sizeof(special) - 1)]; // At least 1 special char

    // Fill the remaining characters
    for (int i=6; i < length; i++) {
        int key=rand() % 4; // Choose from 4 character types
        if (key == 0) {
            password[i]=lower[rand() % (sizeof(lower) - 1)];
        } else if (key == 1) {
            password[i]=upper[rand() % (sizeof(upper) - 1)];
        } else if (key == 2) {
            password[i]=digits[rand() % (sizeof(digits) - 1)];
        } else {
            password[i]=special[rand() % (sizeof(special) - 1)];
        }
    }

    password[length]='\0'; // Null-terminate the password

    // Shuffle the password to randomize character positions
    for (int i=length - 1; i > 0; i--) {
        int j=rand() % (i + 1);
        char temp=password[i];
        password[i]=password[j];
        password[j]=temp;
    }

    // Display generated password in the GtkEntry widget
    gtk_entry_set_text(entry_result, password);
}

// Function to create the About section
void create_about_section(GtkWidget *notebook) {
    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label=gtk_label_new("Author: Jay Mee @ J~Net 2024");
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);

    GtkWidget *tab_label=gtk_label_new("About");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box, tab_label);
}

// Create the GUI with tabs for each tool
int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *tab_label;
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *button;
    GtkWidget *entry_length;
    GtkWidget *entry_result;

    gtk_init(&argc, &argv);

    // Create main window
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Password Generator - J~Net 2024");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create notebook (tabs container)
    notebook=gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // --- Password Generator Tab ---
    box=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    label=gtk_label_new("Enter password length (min 8):");
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);

    // Text input for password length
    entry_length=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entry_length, TRUE, TRUE, 0);

    // Result entry for generated password
    entry_result=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entry_result, TRUE, TRUE, 0);

    // Allocate GtkWidgets array for passing both length and result entries
    GtkWidget *entries[2]={ entry_length, entry_result };

    // Button to generate password
    button=gtk_button_new_with_label("Generate Password");
    g_signal_connect(button, "clicked", G_CALLBACK(password_generator), entries);
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);

    // Button to copy to clipboard
    GtkWidget *copy_button=gtk_button_new_with_label("Copy to Clipboard");
    g_signal_connect(copy_button, "clicked", G_CALLBACK(copy_to_clipboard), entry_result);
    gtk_box_pack_start(GTK_BOX(box), copy_button, TRUE, TRUE, 0);

    tab_label=gtk_label_new("Password Generator");
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box, tab_label);

    // --- About Tab ---
    create_about_section(notebook);

    // Show all widgets
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

