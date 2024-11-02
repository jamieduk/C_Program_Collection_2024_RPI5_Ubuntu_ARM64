#include <gtk/gtk.h>

// Function to handle button clicks and append characters to the entry
void on_character_button_clicked(GtkWidget *widget, gpointer data) {
    const gchar *button_label=gtk_button_get_label(GTK_BUTTON(widget));
    GtkEntry *entry=GTK_ENTRY(data);
    const gchar *current_text=gtk_entry_get_text(entry); // Use const gchar *

    // Append the button label to the entry text
    gchar *new_text=g_strconcat(current_text, button_label, NULL);
    gtk_entry_set_text(entry, new_text);
    g_free(new_text);
}

// Function to handle copy action
void on_copy_button_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry *entry=GTK_ENTRY(data);
    const gchar *text_to_copy=gtk_entry_get_text(entry);

    // Copy the text to the clipboard
    GtkClipboard *clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clipboard, text_to_copy, -1);
    g_print("Copied to clipboard: %s\n", text_to_copy);
}

// Function to toggle Caps Lock
void on_caps_lock_button_clicked(GtkWidget *widget, gpointer data) {
    static gboolean caps_on=FALSE;
    caps_on=!caps_on;

    // Update the button label based on the Caps Lock state
    gtk_button_set_label(GTK_BUTTON(widget), caps_on ? "Caps: ON" : "Caps: OFF");
    g_print("Caps Lock state: %s\n", caps_on ? "ON" : "OFF");

    // Update button labels to reflect caps lock state
    GList *buttons=(GList *)data;
    for (GList *iter=buttons; iter != NULL; iter=iter->next) {
        GtkWidget *button=(GtkWidget *)iter->data;
        const gchar *label=gtk_button_get_label(GTK_BUTTON(button));
        gchar *new_label=caps_on ? g_utf8_strup(label, -1) : g_utf8_strdown(label, -1);
        gtk_button_set_label(GTK_BUTTON(button), new_label);
        g_free(new_label);
    }
}

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *letter_grid;
    GtkWidget *number_grid;
    GtkWidget *symbol_grid;
    GtkWidget *emoji_grid;
    GtkWidget *entry; // Entry for text input
    GList *buttons_list=NULL; // List to keep track of character buttons

    gtk_init(&argc, &argv);

    // Create main window
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "On-Screen Keyboard");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create main vertical box
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create text entry
    entry=gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Type here...");
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0); // Add entry to the vertical box

    // Create notebook
    notebook=gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0); // Add notebook to the vertical box

    // Create grids for each tab
    letter_grid=gtk_grid_new();
    number_grid=gtk_grid_new();
    symbol_grid=gtk_grid_new();
    emoji_grid=gtk_grid_new();

    // Populate letter_grid (example layout)
    for (char c='A'; c <= 'Z'; c++) {
        GtkWidget *button=gtk_button_new_with_label(g_strdup_printf("%c", c));
        g_signal_connect(button, "clicked", G_CALLBACK(on_character_button_clicked), entry);
        gtk_grid_attach(GTK_GRID(letter_grid), button, (c - 'A') % 10, (c - 'A') / 10, 1, 1);
        buttons_list=g_list_append(buttons_list, button); // Keep track of buttons for caps lock
    }

    // Add Caps Lock button to letter_grid
    GtkWidget *caps_lock_button=gtk_button_new_with_label("Caps: OFF");
    g_signal_connect(caps_lock_button, "clicked", G_CALLBACK(on_caps_lock_button_clicked), buttons_list);
    gtk_grid_attach(GTK_GRID(letter_grid), caps_lock_button, 0, 3, 2, 1); // Adjust position as needed

    // Populate number_grid (example layout)
    for (int i=0; i <= 9; i++) {
        GtkWidget *button=gtk_button_new_with_label(g_strdup_printf("%d", i));
        g_signal_connect(button, "clicked", G_CALLBACK(on_character_button_clicked), entry);
        gtk_grid_attach(GTK_GRID(number_grid), button, i, 0, 1, 1);
    }

    // Populate symbol_grid (example layout)
    const char *symbols[]={"{", "}",":", ";", "!", "@", "#", "$", "%", "^", "&", "_", "=", "+", "-", "*", "(", ")"};
    for (int i=0; i < sizeof(symbols) / sizeof(symbols[0]); i++) {
        GtkWidget *button=gtk_button_new_with_label(symbols[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_character_button_clicked), entry);
        gtk_grid_attach(GTK_GRID(symbol_grid), button, i, 0, 1, 1);
    }

    // Populate emoji_grid (example layout)
    const char *emojis[]={"😀", "😂", "🥳", "😍", "😎", "😢", "😡", "👍", "👎", "❤️"};
    for (int i=0; i < sizeof(emojis) / sizeof(emojis[0]); i++) {
        GtkWidget *button=gtk_button_new_with_label(emojis[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_character_button_clicked), entry);
        gtk_grid_attach(GTK_GRID(emoji_grid), button, i, 0, 1, 1);
    }

    // Add "Copy" button to the letter_grid
    GtkWidget *copy_button=gtk_button_new_with_label("Copy");
    g_signal_connect(copy_button, "clicked", G_CALLBACK(on_copy_button_clicked), entry);
    gtk_grid_attach(GTK_GRID(letter_grid), copy_button, 8, 3, 2, 1); // Adjust position as needed

    // Append grids to the notebook
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), letter_grid, gtk_label_new("Letters"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), number_grid, gtk_label_new("Numbers"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), symbol_grid, gtk_label_new("Symbols"));
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), emoji_grid, gtk_label_new("Emojis"));

    // Show all widgets
    gtk_widget_show_all(window);

    // Start the main loop
    gtk_main();

    return 0;
}

