#include <gtk/gtk.h>
// 
// Developed by Jay @ J~Net© 2024
// https://github.com/jamieduk/ubuntu-OnScreenKeyboard/
//
// Function to handle button clicks and append characters to the entry

#include <string.h>

typedef struct {
    GtkWidget *entry;
} AppWidgets;

void on_key_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets=(AppWidgets *)data;
    const gchar *label=gtk_button_get_label(GTK_BUTTON(widget));
    
    // Get current text in the entry
    const gchar *current_text=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));
    
    // Combine the current text with the new character
    gchar *new_text=g_strdup_printf("%s%s", current_text, label);
    
    // Set the new text in the entry
    gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), new_text);
    
    // Free the allocated memory for new_text
    g_free(new_text);
}

void on_backspace_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets=(AppWidgets *)data;
    const gchar *current_text=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));
    if (strlen(current_text) > 0) {
        // Create a mutable copy of current_text to modify it
        gchar *updated_text=g_strdup(current_text);
        updated_text[strlen(updated_text) - 1]='\0';  // Remove last character
        gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), updated_text);
        g_free(updated_text);  // Free the mutable copy
    }
}

void on_space_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets=(AppWidgets *)data;
    const gchar *current_text=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));
    
    // Combine current text with a space
    gchar *new_text=g_strdup_printf("%s ", current_text);
    
    // Set the new text in the entry
    gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), new_text);
    
    // Free the allocated memory for new_text
    g_free(new_text);
}

void on_clear_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets=(AppWidgets *)data;
    gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), "");  // Clear the entry
}

void on_copy_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets=(AppWidgets *)data;
    const gchar *text=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));
    GtkClipboard *clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clipboard, text, -1);
}

void show_about_dialog(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(data),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "On-Screen Keyboard\nDeveloped by Jay @ J~Net© 2024\nhttps://github.com/jamieduk/ubuntu-OnScreenKeyboard/");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "On-Screen Keyboard");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create text entry
    AppWidgets *app_widgets=g_malloc(sizeof(AppWidgets));
    app_widgets->entry=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), app_widgets->entry, FALSE, FALSE, 0);

    // Create notebook for tabs
    GtkWidget *notebook=gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
    
    // Create letter buttons
    GtkWidget *letter_grid=gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(letter_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(letter_grid), 5);
    
    const char *letters[]={
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
        "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
        "U", "V", "W", "X", "Y", "Z"
    };

    for (int i=0; i < sizeof(letters) / sizeof(letters[0]); i++) {
        GtkWidget *letter_button=gtk_button_new_with_label(letters[i]);
        g_signal_connect(letter_button, "clicked", G_CALLBACK(on_key_button_clicked), app_widgets);
        gtk_grid_attach(GTK_GRID(letter_grid), letter_button, i % 10, i / 10, 1, 1);
    }

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), letter_grid, gtk_label_new("Letters"));

    // Create Numbers Tab with Roman numerals
    GtkWidget *numbers_grid=gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(numbers_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(numbers_grid), 5);
    
    const char *numbers[]={
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"
    };

    for (int i=0; i < sizeof(numbers) / sizeof(numbers[0]); i++) {
        GtkWidget *number_button=gtk_button_new_with_label(numbers[i]);
        g_signal_connect(number_button, "clicked", G_CALLBACK(on_key_button_clicked), app_widgets);
        gtk_grid_attach(GTK_GRID(numbers_grid), number_button, i % 10, i / 10, 1, 1);
    }

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), numbers_grid, gtk_label_new("Numbers"));

    // Create Emojis Tab
    GtkWidget *emojis_grid=gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(emojis_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(emojis_grid), 5);
    
    const char *emojis[]={
        "😀", "😁", "😂", "🤣", "😃", "😄", "😅", "😆", "😉", "😊",
        "😋", "😎", "😍", "😏", "😐", "😑", "😒", "😓", "😔", "😕",
        "😖", "😔", "😗", "😘", "😙", "😚", "😜", "😝", "😞", "😟",
        "😠", "😡", "😢", "😣", "😤", "😥", "😨", "😩", "😪", "😫",
        "😬", "😭", "😮", "😯", "😲", "😳", "😵", "😶", "😷", "😸",
        "😺", "😻", "😼", "😽", "🙀", "😿", "😾", "👐", "🙌", "👏"
    };

    for (int i=0; i < sizeof(emojis) / sizeof(emojis[0]); i++) {
        GtkWidget *emoji_button=gtk_button_new_with_label(emojis[i]);
        g_signal_connect(emoji_button, "clicked", G_CALLBACK(on_key_button_clicked), app_widgets);
        gtk_grid_attach(GTK_GRID(emojis_grid), emoji_button, i % 10, i / 10, 1, 1);
    }

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), emojis_grid, gtk_label_new("Emojis"));

    // Create About Tab
    GtkWidget *about_grid=gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(about_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(about_grid), 5);
    
    GtkWidget *about_label=gtk_label_new("On-Screen Keyboard\nDeveloped by Jay @ J~Net© 2024\nhttps://github.com/jamieduk/ubuntu-OnScreenKeyboard/");
    gtk_grid_attach(GTK_GRID(about_grid), about_label, 0, 0, 1, 1);

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), about_grid, gtk_label_new("About"));

    // Create a horizontal box for special keys
    GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    // Create backspace button
    GtkWidget *backspace_button=gtk_button_new_with_label("←");
    g_signal_connect(backspace_button, "clicked", G_CALLBACK(on_backspace_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), backspace_button, TRUE, TRUE, 0);

    // Create space button
    GtkWidget *space_button=gtk_button_new_with_label("Space");
    g_signal_connect(space_button, "clicked", G_CALLBACK(on_space_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), space_button, TRUE, TRUE, 0);

    // Create clear button
    GtkWidget *clear_button=gtk_button_new_with_label("Clear");
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), clear_button, TRUE, TRUE, 0);

    // Create copy button
    GtkWidget *copy_button=gtk_button_new_with_label("Copy");
    g_signal_connect(copy_button, "clicked", G_CALLBACK(on_copy_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), copy_button, TRUE, TRUE, 0);
    
    // Create About button
    GtkWidget *about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about_dialog), window);
    gtk_box_pack_start(GTK_BOX(hbox), about_button, TRUE, TRUE, 0);

    // Show all widgets
    gtk_widget_show_all(window);
    
    gtk_main();

    g_free(app_widgets); // Free allocated memory for app_widgets
    return 0;
}

