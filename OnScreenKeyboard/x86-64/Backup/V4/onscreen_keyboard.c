#include <gtk/gtk.h>
// 
// Developed by Jay @ J~Net© 2024
// https://github.com/jamieduk/ubuntu-OnScreenKeyboard/
//
// Function to handle button clicks and append characters to the entry

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#define BUTTONS_PER_ROW 10
#define BUTTONS_PER_COL 8
#define BUTTON_WIDTH 40
#define BUTTON_HEIGHT 40
#define EMOJI_PER_TAB 80

typedef struct {
    GtkWidget *entry;
    GtkWidget *grid; // Add grid to AppWidgets
    gboolean caps_lock;
} AppWidgets;

void on_caps_button_clicked(GtkWidget *button, AppWidgets *app_widgets) {
    app_widgets->caps_lock=!app_widgets->caps_lock; // Toggle caps lock state

    // Update button labels for letters
    for (int i=0; i < 26; i++) {
        GtkWidget *grid_button=gtk_grid_get_child_at(GTK_GRID(app_widgets->grid), i % BUTTONS_PER_ROW, i / BUTTONS_PER_ROW);
        const gchar *label=gtk_button_get_label(GTK_BUTTON(grid_button));
        gchar *new_label;

        new_label=app_widgets->caps_lock ? g_ascii_strup(label, -1) : g_ascii_strdown(label, -1);
        gtk_button_set_label(GTK_BUTTON(grid_button), new_label);
        g_free(new_label); // Free the allocated memory
    }
}

void on_key_button_clicked(GtkWidget *button, gpointer user_data) {
    AppWidgets *app_widgets=(AppWidgets *)user_data;
    const gchar *button_label=gtk_button_get_label(GTK_BUTTON(button));
    const gchar *current_text=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));

    gchar new_char[2]={0};
    new_char[0]=app_widgets->caps_lock ? g_ascii_toupper(button_label[0]) : button_label[0];

    gchar *new_text=g_strdup_printf("%s%c", current_text, new_char[0]);
    gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), new_text);
    g_free(new_text);
}

void on_backspace_button_clicked(GtkWidget *button, gpointer user_data) {
    AppWidgets *app_widgets=(AppWidgets *)user_data;
    const gchar *current_text=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));
    int len=strlen(current_text);
    if (len > 0) {
        gchar *new_text=g_strndup(current_text, len - 1);
        gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), new_text);
        g_free(new_text);
    }
}

void on_space_button_clicked(GtkWidget *button, gpointer user_data) {
    AppWidgets *app_widgets=(AppWidgets *)user_data;
    const gchar *current_text=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));
    gchar *new_text=g_strdup_printf("%s ", current_text);
    gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), new_text);
    g_free(new_text);
}

void on_copy_button_clicked(GtkWidget *button, gpointer user_data) {
    AppWidgets *app_widgets=(AppWidgets *)user_data;
    const gchar *current_text=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));

    // Copy to clipboard
    GtkClipboard *clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clipboard, current_text, -1);
}

void show_about_dialog(GtkWidget *widget, gpointer user_data) {
    GtkWidget *dialog=gtk_dialog_new_with_buttons("About",
        GTK_WINDOW(gtk_widget_get_toplevel(widget)),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        "_OK", GTK_RESPONSE_OK,
        NULL);

    GtkWidget *content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label=gtk_label_new("Developed By Jay @ J~Net");
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_widget_destroy), NULL);
    gtk_dialog_run(GTK_DIALOG(dialog));
}

void add_buttons_to_grid(GtkWidget *grid, const gchar *buttons[], int num_buttons, AppWidgets *app_widgets) {
    for (int i=0; i < num_buttons; i++) {
        GtkWidget *button=gtk_button_new_with_label(buttons[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_key_button_clicked), app_widgets);
        gtk_grid_attach(GTK_GRID(grid), button, i % BUTTONS_PER_ROW, i / BUTTONS_PER_ROW, 1, 1);
    }
}

void create_keyboard_tabs(GtkNotebook *notebook, AppWidgets *app_widgets) {
    GtkWidget *grid=gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    app_widgets->grid=grid; // Store the grid in app_widgets

    // Define buttons for letters
    const gchar *buttons[]={
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
        "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
        "U", "V", "W", "X", "Y", "Z"
    };

    add_buttons_to_grid(grid, buttons, G_N_ELEMENTS(buttons), app_widgets);

    // Create Caps button
    GtkWidget *caps_button=gtk_button_new_with_label("Caps");
    g_signal_connect(caps_button, "clicked", G_CALLBACK(on_caps_button_clicked), app_widgets);
    gtk_grid_attach(GTK_GRID(grid), caps_button, 0, 3, BUTTONS_PER_ROW, 1); // Adjust position as necessary

    // Create and add tabs
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), grid, gtk_label_new("Letters"));

    // Create Number and Roman Numerals tab
    GtkWidget *number_grid=gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(number_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(number_grid), 5);

    const gchar *numbers[]={
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
        "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"
    };

    add_buttons_to_grid(number_grid, numbers, G_N_ELEMENTS(numbers), app_widgets);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), number_grid, gtk_label_new("Numbers & Roman"));
}

void apply_css(GtkWidget *widget) {
    GtkCssProvider *css_provider=gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
        "entry { background-color: grey; color: white; }", -1, NULL);
    gtk_style_context_add_class(gtk_widget_get_style_context(widget), "my-entry");
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "On-Screen Keyboard");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold everything
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Text entry
    AppWidgets *app_widgets=g_new(AppWidgets, 1);
    app_widgets->entry=gtk_entry_new();
    gtk_widget_set_size_request(app_widgets->entry, 350, -1);
    gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), ""); // Clear text entry
    app_widgets->caps_lock=false; // Initialize caps lock state

    gtk_box_pack_start(GTK_BOX(vbox), app_widgets->entry, FALSE, FALSE, 0);

    // Apply CSS for entry box
    apply_css(app_widgets->entry);

    // Create a notebook for tabs
    GtkWidget *notebook=gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    // Create keyboard tabs
    create_keyboard_tabs(GTK_NOTEBOOK(notebook), app_widgets);

    // Create emojis tab
    const gchar *emojis[]={
        "😀", "😃", "😄", "😅", "😆", "😉", "😊", "😋", "😎", "😍",
        "😘", "😗", "😙", "😚", "🙂", "🤗", "🤩", "🤔", "😐", "😑",
        "😶", "🙄", "😏", "😒", "😔", "😕", "😖", "😞", "😟", "😤",
        "😢", "😭", "😧", "😩", "😬", "😮", "😯", "😲", "😳", "😵",
        "😡", "😠", "🤬", "😷", "😇", "🤒", "🤕", "🤑", "🤠", "😈",
        "👿", "💀", "👻", "👽", "🤖", "💩", "🐶", "🐱", "🐭", "🐹",
        "🐰", "🦊", "🐻", "🐼", "🐨", "🐯", "🦁", "🐮", "🐷", "🐸"
    };

    GtkWidget *emoji_grid=gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(emoji_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(emoji_grid), 5);

    for (int i=0; i < 40; i++) {
        GtkWidget *emoji_button=gtk_button_new_with_label(emojis[i]);
        g_signal_connect(emoji_button, "clicked", G_CALLBACK(on_key_button_clicked), app_widgets);
        gtk_grid_attach(GTK_GRID(emoji_grid), emoji_button, i % 10, i / 10, 1, 1);
    }

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), emoji_grid, gtk_label_new("Emojis"));

    // Create a horizontal box for special keys
    GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    // Create backspace button
    GtkWidget *backspace_button=gtk_button_new_with_label("Backspace");
    g_signal_connect(backspace_button, "clicked", G_CALLBACK(on_backspace_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), backspace_button, FALSE, FALSE, 0);

    // Create space button
    GtkWidget *space_button=gtk_button_new_with_label("Space");
    g_signal_connect(space_button, "clicked", G_CALLBACK(on_space_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), space_button, FALSE, FALSE, 0);

    // Create copy button
    GtkWidget *copy_button=gtk_button_new_with_label("Copy");
    g_signal_connect(copy_button, "clicked", G_CALLBACK(on_copy_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), copy_button, FALSE, FALSE, 0);

    // Create about button
    GtkWidget *about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about_dialog), NULL);
    gtk_box_pack_start(GTK_BOX(hbox), about_button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    g_free(app_widgets); // Free allocated memory for AppWidgets
    return 0;
}

