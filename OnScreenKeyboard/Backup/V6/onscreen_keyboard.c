#include <gtk/gtk.h>
// 
// 
// Developed by Jay @ J~Net© 2024
// https://github.com/jamieduk/ubuntu-OnScreenKeyboard/

#include <string.h>

typedef struct {
    GtkWidget *entry;
    gboolean caps_lock;  // Track the state of Caps Lock
    GtkWidget **letter_buttons;  // Track the letter buttons
    int num_letter_buttons;  // Number of letter buttons
} AppWidgets;

// Function prototypes
void on_key_button_clicked(GtkWidget *widget, gpointer data);
void on_backspace_button_clicked(GtkWidget *widget, gpointer data);
void on_space_button_clicked(GtkWidget *widget, gpointer data);
void on_clear_button_clicked(GtkWidget *widget, gpointer data);
void on_copy_button_clicked(GtkWidget *widget, gpointer data);
void on_caps_lock_button_clicked(GtkWidget *widget, gpointer data);
void show_about_dialog(GtkWidget *widget, gpointer data);
void update_letter_buttons(AppWidgets *app_widgets);

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
    app_widgets->caps_lock=FALSE;  // Initialize Caps Lock to off

    // Create notebook for tabs
    GtkWidget *notebook=gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);

    // Function to create buttons in a grid
    void create_button_grid(GtkWidget *grid, const char **buttons, int count, void (*callback)(GtkWidget*, gpointer), gpointer data) {
        for (int i=0; i < count; i++) {
            GtkWidget *button=gtk_button_new_with_label(buttons[i]);
            g_signal_connect(button, "clicked", G_CALLBACK(callback), data);
            gtk_grid_attach(GTK_GRID(grid), button, i % 10, i / 10, 1, 1);
        }
    }

    // Create letter buttons
    GtkWidget *letter_grid=gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(letter_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(letter_grid), 5);

    const char *letters[]={
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
        "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T",
        "U", "V", "W", "X", "Y", "Z"
    };

    // Allocate memory for letter buttons
    app_widgets->num_letter_buttons=G_N_ELEMENTS(letters);
    app_widgets->letter_buttons=g_malloc(sizeof(GtkWidget*) * app_widgets->num_letter_buttons);

    // Create letter buttons and store them in the array
    for (int i=0; i < app_widgets->num_letter_buttons; i++) {
        app_widgets->letter_buttons[i]=gtk_button_new_with_label(letters[i]);
        g_signal_connect(app_widgets->letter_buttons[i], "clicked", G_CALLBACK(on_key_button_clicked), app_widgets);
        gtk_grid_attach(GTK_GRID(letter_grid), app_widgets->letter_buttons[i], i % 10, i / 10, 1, 1);
    }

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), letter_grid, gtk_label_new("Letters"));

    // Create numbers tab with Roman numerals
    GtkWidget *numbers_grid=gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(numbers_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(numbers_grid), 5);

    const char *numbers[]={
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
        "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X"
    };
    create_button_grid(numbers_grid, numbers, G_N_ELEMENTS(numbers), on_key_button_clicked, app_widgets);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), numbers_grid, gtk_label_new("Numbers"));

    // Create symbols tab
    GtkWidget *symbols_grid=gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(symbols_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(symbols_grid), 5);

    const char *symbols[]={
        "!", "@", "#", "$", "%", "^", "&", "*", "(", ")",
        "-", "_", "=", "+", "[", "]", "{", "}", "\\", "|",
        ";", ":", "'", "\"", ",", ".", "/", "<", ">", "?",
        "~", "`", "€", "£", "¥", "¢", "§", "©", "®", "™",
        "±", "∞", "µ", "Ω", "π", "∑", "√", "÷", "≠", "≈",
        "→", "←", "↑", "↓", "⇌", "∞", "∀", "∃", "⊂", "⊃",
        "∈", "∉", "⊆", "⊇", "⊕", "⊗", "∪", "∩", "∧", "∨",
        "⊥", "⊨", "∴", "∵", "↔", "⇔", "∂", "∇", "∫", "∬"
    };
    create_button_grid(symbols_grid, symbols, G_N_ELEMENTS(symbols), on_key_button_clicked, app_widgets);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), symbols_grid, gtk_label_new("Symbols"));

    // Create emojis tab
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
    create_button_grid(emojis_grid, emojis, G_N_ELEMENTS(emojis), on_key_button_clicked, app_widgets);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), emojis_grid, gtk_label_new("Emojis"));

    // Create about tab
    GtkWidget *about_grid=gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(about_grid), 5);
    gtk_grid_set_row_spacing(GTK_GRID(about_grid), 5);

    GtkWidget *about_label=gtk_label_new("On-Screen Keyboard\nDeveloped by Jay @ J~Net© 2024\nhttps://github.com/jamieduk/ubuntu-OnScreenKeyboard/");
    gtk_grid_attach(GTK_GRID(about_grid), about_label, 0, 0, 1, 1);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), about_grid, gtk_label_new("About"));

    // Create a horizontal box for special keys
    GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

    GtkWidget *backspace_button=gtk_button_new_with_label("Backspace");
    g_signal_connect(backspace_button, "clicked", G_CALLBACK(on_backspace_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), backspace_button, TRUE, TRUE, 0);

    GtkWidget *space_button=gtk_button_new_with_label("Space");
    g_signal_connect(space_button, "clicked", G_CALLBACK(on_space_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), space_button, TRUE, TRUE, 0);

    GtkWidget *clear_button=gtk_button_new_with_label("Clear");
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), clear_button, TRUE, TRUE, 0);

    GtkWidget *copy_button=gtk_button_new_with_label("Copy");
    g_signal_connect(copy_button, "clicked", G_CALLBACK(on_copy_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), copy_button, TRUE, TRUE, 0);

    GtkWidget *caps_lock_button=gtk_button_new_with_label("Caps Lock");
    g_signal_connect(caps_lock_button, "clicked", G_CALLBACK(on_caps_lock_button_clicked), app_widgets);
    gtk_box_pack_start(GTK_BOX(hbox), caps_lock_button, TRUE, TRUE, 0);

    // Connect the "destroy" event to exit the GTK main loop
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Show all widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    // Free allocated memory
    g_free(app_widgets->letter_buttons);
    g_free(app_widgets);
    
    return 0;
}

// Key button callback function
void on_key_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets=(AppWidgets *)data;
    const gchar *key=gtk_button_get_label(GTK_BUTTON(widget));

    // Update entry text with key
    const gchar *current_text=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));
    gchar *new_text=g_strdup_printf("%s%s", current_text, app_widgets->caps_lock ? g_ascii_strup(key, -1) : key);
    gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), new_text);
    g_free(new_text);
}

// Backspace button callback function
void on_backspace_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets=(AppWidgets *)data;
    const gchar *current_text=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));
    if (strlen(current_text) > 0) {
        gchar *new_text=g_strdup(current_text);
        new_text[strlen(new_text) - 1]='\0'; // Remove last character
        gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), new_text);
        g_free(new_text);
    }
}

// Space button callback function
void on_space_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets=(AppWidgets *)data;
    const gchar *current_text=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));
    gchar *new_text=g_strdup_printf("%s ", current_text);
    gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), new_text);
    g_free(new_text);
}

// Clear button callback function
void on_clear_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets=(AppWidgets *)data;
    gtk_entry_set_text(GTK_ENTRY(app_widgets->entry), "");
}

// Copy button callback function
void on_copy_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets=(AppWidgets *)data;
    const gchar *text_to_copy=gtk_entry_get_text(GTK_ENTRY(app_widgets->entry));

    GtkClipboard *clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_text(clipboard, text_to_copy, -1);
}

// Caps Lock button callback function
void on_caps_lock_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app_widgets=(AppWidgets *)data;
    app_widgets->caps_lock=!app_widgets->caps_lock;  // Toggle Caps Lock state

    // Update letter buttons to show correct case
    update_letter_buttons(app_widgets);
}

// Update letter buttons based on Caps Lock state
void update_letter_buttons(AppWidgets *app_widgets) {
    for (int i=0; i < app_widgets->num_letter_buttons; i++) {
        const gchar *label=gtk_button_get_label(GTK_BUTTON(app_widgets->letter_buttons[i]));
        gchar *new_label=app_widgets->caps_lock ? g_ascii_strup(label, -1) : g_ascii_strdown(label, -1);
        gtk_button_set_label(GTK_BUTTON(app_widgets->letter_buttons[i]), new_label);
        g_free(new_label);
    }
}

// Show about dialog
void show_about_dialog(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(data),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "On-Screen Keyboard\nDeveloped by Jay @ J~Net© 2024\nhttps://github.com/jamieduk/ubuntu-OnScreenKeyboard/");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

