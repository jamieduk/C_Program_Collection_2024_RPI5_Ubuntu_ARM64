#include <gtk/gtk.h>

int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *letter_grid;
    GtkWidget *number_grid;
    GtkWidget *symbol_grid;
    GtkWidget *emoji_grid;

    gtk_init(&argc, &argv);

    // Create main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "On-Screen Keyboard");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create notebook
    notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // Create grids for each tab
    letter_grid = gtk_grid_new();
    number_grid = gtk_grid_new();
    symbol_grid = gtk_grid_new();
    emoji_grid = gtk_grid_new();

    // Populate letter_grid (example layout)
    for (char c = 'A'; c <= 'Z'; c++) {
        GtkWidget *button = gtk_button_new_with_label(g_strdup_printf("%c", c));
        gtk_grid_attach(GTK_GRID(letter_grid), button, (c - 'A') % 10, (c - 'A') / 10, 1, 1);
    }

    // Populate number_grid (example layout)
    for (int i = 0; i <= 9; i++) {
        GtkWidget *button = gtk_button_new_with_label(g_strdup_printf("%d", i));
        gtk_grid_attach(GTK_GRID(number_grid), button, i, 0, 1, 1);
    }

    // Populate symbol_grid (example layout)
    const char *symbols[] = {"!", "@", "#", "$", "%", "^", "&", "*", "(", ")"};
    for (int i = 0; i < sizeof(symbols) / sizeof(symbols[0]); i++) {
        GtkWidget *button = gtk_button_new_with_label(symbols[i]);
        gtk_grid_attach(GTK_GRID(symbol_grid), button, i, 0, 1, 1);
    }

    // Populate emoji_grid (example layout)
    const char *emojis[] = {"😀", "😂", "🥳", "😍", "😎", "😢", "😡", "👍", "👎", "❤️"};
    for (int i = 0; i < sizeof(emojis) / sizeof(emojis[0]); i++) {
        GtkWidget *button = gtk_button_new_with_label(emojis[i]);
        gtk_grid_attach(GTK_GRID(emoji_grid), button, i, 0, 1, 1);
    }

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

