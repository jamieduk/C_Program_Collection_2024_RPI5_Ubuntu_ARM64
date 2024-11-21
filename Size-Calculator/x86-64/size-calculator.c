//
// Made By Jay @ J~Net 2024
//
// gcc size-calculator.c -o size-calculator $(pkg-config --cflags --libs gtk+-3.0)
//
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <gtk/gtk.h>
#include <glib.h> // For GList

#define MAX_PATH 1024

// Global variables for folder lists, UI elements
GtkWidget *include_listbox, *exclude_listbox, *result_label;
GtkWidget *include_button, *exclude_button, *remove_include_button, *remove_exclude_button;
GtkWidget *exclude_name_entry, *add_name_exclude_button;
GtkWidget *about_button; // About button

// Node structure for dynamic folder lists
typedef struct Node {
    char path[MAX_PATH];
    struct Node *next;
} Node;

Node *includes=NULL;
Node *excludes=NULL;
GList *name_exclusions=NULL; // Use GList for name-based exclusions

// Add a path to a linked list
void add_to_list(Node **list, const char *path) {
    Node *new_node=(Node *)malloc(sizeof(Node));
    strncpy(new_node->path, path, sizeof(new_node->path) - 1);
    new_node->path[sizeof(new_node->path) - 1]='\0';
    new_node->next=*list;
    *list=new_node;
}

// Remove a path from the list
void remove_from_list(Node **list, const char *path) {
    Node *current=*list;
    Node *previous=NULL;

    while (current) {
        if (strcmp(current->path, path) == 0) {
            if (previous) {
                previous->next=current->next;
            } else {
                *list=current->next;
            }
            free(current);
            return;
        }
        previous=current;
        current=current->next;
    }
}

// Check if a path matches any exclusion
int is_excluded(const char *path) {
    Node *current=excludes;
    while (current) {
        if (strstr(path, current->path)) return 1;
        current=current->next;
    }

    // Check for name-based exclusions
    GList *current_name_exclusion=name_exclusions;
    while (current_name_exclusion) {
        if (strstr(path, (char *)current_name_exclusion->data)) return 1;
        current_name_exclusion=g_list_next(current_name_exclusion);
    }

    return 0;
}

// Calculate folder size recursively
long long calculate_size(const char *path) {
    struct stat st;
    long long total_size=0;

    if (stat(path, &st) == -1) return 0;

    if (S_ISDIR(st.st_mode)) {
        DIR *dir=opendir(path);
        if (!dir) return 0;

        struct dirent *entry;
        while ((entry=readdir(dir))) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

            char full_path[MAX_PATH];
            snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

            if (!is_excluded(full_path)) {
                total_size += calculate_size(full_path);
            }
        }

        closedir(dir);
    } else {
        total_size += st.st_size;
    }

    return total_size;
}

// Update total size and display result
void update_total_size() {
    long long total_size=0;
    Node *current=includes;
    while (current) {
        total_size += calculate_size(current->path);
        current=current->next;
    }

    double size_in_tb=total_size / (double)(1024LL * 1024LL * 1024LL * 1024LL);
    double size_in_gb=total_size / (double)(1024LL * 1024LL * 1024LL);
    double size_in_mb=total_size / (double)(1024LL * 1024LL);

    char result[512];
    snprintf(result, sizeof(result), "Total Size:\n%.2f TB\n%.2f GB\n%.2f MB", size_in_tb, size_in_gb, size_in_mb);
    gtk_label_set_text(GTK_LABEL(result_label), result);
}

// Open folder dialog and add folder to list
void open_folder_dialog(GtkWidget *widget, gpointer data) {
    GtkFileChooser *chooser;
    GtkFileChooserAction action=GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    GtkWidget *dialog;
    const char *folder_path;

    if (widget == include_button) {
        dialog=gtk_file_chooser_dialog_new("Select Folder to Include", NULL, action, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
    } else {
        dialog=gtk_file_chooser_dialog_new("Select Folder to Exclude", NULL, action, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
    }

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        chooser=GTK_FILE_CHOOSER(dialog);
        folder_path=gtk_file_chooser_get_filename(chooser);
        if (folder_path) {
            // Add folder to the respective list (includes or excludes)
            if (widget == include_button) {
                // Add to includes list
                add_to_list(&includes, folder_path);

                // Create a label for the folder path and add to ListBox
                GtkWidget *label=gtk_label_new(folder_path);
                gtk_list_box_insert(GTK_LIST_BOX(include_listbox), label, -1);
                gtk_widget_show(label); // Ensure it is visible
            } else {
                // Add to excludes list
                add_to_list(&excludes, folder_path);

                // Create a label for the folder path and add to ListBox
                GtkWidget *label=gtk_label_new(folder_path);
                gtk_list_box_insert(GTK_LIST_BOX(exclude_listbox), label, -1);
                gtk_widget_show(label); // Ensure it is visible
            }
        }
    }

    gtk_widget_destroy(dialog);

    // Recalculate the total size when folders are added or excluded
    update_total_size();
}

// Remove folder from list (from ListBox)
void remove_folder_from_list(GtkWidget *widget, gpointer data) {
    const char *folder_path=gtk_label_get_text(GTK_LABEL(widget));
    remove_from_list(&includes, folder_path);
    remove_from_list(&excludes, folder_path);
    gtk_widget_destroy(widget);

    // Recalculate the total size when a folder is removed
    update_total_size();
}

// Function to add exclusion by name
void add_exclusion_by_name(GtkWidget *widget, gpointer data) {
    const char *exclusion_name=gtk_entry_get_text(GTK_ENTRY(exclude_name_entry));  // Get text from the exclusion entry box

    if (strlen(exclusion_name) > 0) {
        // Add the exclusion name to the exclusions list (data structure)
        name_exclusions=g_list_append(name_exclusions, g_strdup(exclusion_name));  // Using GList to store exclusions
        
        // Create a label to display the exclusion name in the exclusion list box
        GtkWidget *label=gtk_label_new(exclusion_name);
        gtk_list_box_insert(GTK_LIST_BOX(exclude_listbox), label, -1);  // Add label to the ListBox
        gtk_widget_show(label);  // Ensure the label is visible in the list box
    }

    // Recalculate total size after exclusions are added
    update_total_size();
}

// Show About dialog
void show_about(GtkWidget *widget, gpointer data) {
    (void)widget; // Unused parameter

    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                               "Author: Jay Mee\nFolder Size Calculator");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Main function
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Folder Size Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Include folder buttons (above)
    include_button=gtk_button_new_with_label("Add Include Folder");
    g_signal_connect(include_button, "clicked", G_CALLBACK(open_folder_dialog), include_button);
    gtk_box_pack_start(GTK_BOX(vbox), include_button, FALSE, FALSE, 0);

    // Include folder list
    include_listbox=gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), include_listbox, TRUE, TRUE, 0);

    // Exclude folder buttons (above)
    exclude_button=gtk_button_new_with_label("Add Exclude Folder");
    g_signal_connect(exclude_button, "clicked", G_CALLBACK(open_folder_dialog), exclude_button);
    gtk_box_pack_start(GTK_BOX(vbox), exclude_button, FALSE, FALSE, 0);

    // Exclude folder list
    exclude_listbox=gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), exclude_listbox, TRUE, TRUE, 0);

    // Add Name Exclusion button (above)
    exclude_name_entry=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), exclude_name_entry, FALSE, FALSE, 0);

    add_name_exclude_button=gtk_button_new_with_label("Add Name Exclusion");
    g_signal_connect(add_name_exclude_button, "clicked", G_CALLBACK(add_exclusion_by_name), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), add_name_exclude_button, FALSE, FALSE, 0);

    // Total size label
    result_label=gtk_label_new("Total Size:\n0 TB\n0 GB\n0 MB");
    gtk_box_pack_start(GTK_BOX(vbox), result_label, FALSE, FALSE, 0);

    // About button (added)
    about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 0);

    // Show the window
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

