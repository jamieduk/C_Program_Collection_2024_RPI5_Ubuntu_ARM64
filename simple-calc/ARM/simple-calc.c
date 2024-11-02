// gcc -o simple-calc simple-calc.c `pkg-config --cflags --libs gtk+-3.0` -lm
//
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>

// Function prototypes
void on_button_click(GtkWidget *widget, gpointer data);
void calculate_result();
void show_author_info();
void copy_to_clipboard(GtkWidget *widget);
void toggle_advanced_mode(GtkWidget *widget);
void update_display(const gchar *text);

// Global variables
GtkWidget *entry_display, *grid_advanced;
gchar operation='\0';
gdouble first_number=0, memory_value=0;
gboolean advanced_mode=FALSE;

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simple Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);  // Smaller default size
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);  // Remove extra padding

    // Create a grid layout with zero spacing
    GtkWidget *grid=gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 0);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create entry display (non-editable)
    entry_display=gtk_entry_new();
    gtk_widget_set_sensitive(entry_display, FALSE);

    // Apply CSS for larger font
    GtkCssProvider *css_provider=gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, "entry { font-size: 28px; }", -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(entry_display), GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    gtk_grid_attach(GTK_GRID(grid), entry_display, 0, 0, 4, 1);

    // Define button labels (including updated "CLR" label)
    const gchar *button_labels[]={
        "1", "2", "3", "+",
        "4", "5", "6", "-",
        "7", "8", "9", "*",
        "0", ".", "=", "/",
        "CLR", "Copy", "MC", 
        "%", "sin","cos", "tan",
        "log","sqrt", "M+", "MR", 
        "About"
    };

    // Create buttons and attach to grid
    int num_buttons=sizeof(button_labels) / sizeof(button_labels[0]);
    for (int i=0; i<num_buttons; i++) {
        GtkWidget *button=gtk_button_new_with_label(button_labels[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_click), (gpointer)button_labels[i]);
        gtk_widget_set_size_request(button, 60, 60);
        gtk_grid_attach(GTK_GRID(grid), button, i % 4, (i / 4) + 1, 1, 1);
    }

    // Advanced mode grid for scientific functions
    grid_advanced=gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid_advanced), 0);
    gtk_grid_set_column_spacing(GTK_GRID(grid_advanced), 0);
    gtk_grid_attach(GTK_GRID(grid), grid_advanced, 0, 6, 4, 4);
    const gchar *advanced_labels[]={"sin", "cos", "tan", "log", "sqrt"};
    for (int i=0; i<5; i++) {
        GtkWidget *adv_button=gtk_button_new_with_label(advanced_labels[i]);
        g_signal_connect(adv_button, "clicked", G_CALLBACK(on_button_click), (gpointer)advanced_labels[i]);
        gtk_widget_set_size_request(adv_button, 60, 60);
        gtk_grid_attach(GTK_GRID(grid_advanced), adv_button, i % 3, i / 3, 1, 1);
    }
    gtk_widget_set_visible(grid_advanced, FALSE);

    // Connect the destroy signal for the window
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Show all widgets in the window
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}

// Update display with text
void update_display(const gchar *text) {
    gtk_entry_set_text(GTK_ENTRY(entry_display), text);
}

// Callback function for button clicks
void on_button_click(GtkWidget *widget, gpointer data) {
    const gchar *label=(const gchar *)data;

    if (g_strcmp0(label, "CLR") == 0) {
        update_display("");
        first_number=0;
        operation='\0';
    } else if (g_strcmp0(label, "=") == 0) {
        calculate_result();
    } else if (g_strcmp0(label, "Copy") == 0) {
        copy_to_clipboard(widget);
    } else if (g_strcmp0(label, "About") == 0) {
        show_author_info();
    } else if (g_strcmp0(label, "ADV") == 0) {
        advanced_mode=!advanced_mode;
        gtk_widget_set_visible(grid_advanced, advanced_mode);
    } else if (g_strcmp0(label, "M+") == 0) {
        memory_value=atof(gtk_entry_get_text(GTK_ENTRY(entry_display)));
    } else if (g_strcmp0(label, "MR") == 0) {
        gchar *mem_text=g_strdup_printf("%.2f", memory_value);
        update_display(mem_text);
        g_free(mem_text);
    } else if (g_strcmp0(label, "MC") == 0) {
        memory_value=0;
    } else if (g_strcmp0(label, "+") == 0 || g_strcmp0(label, "-") == 0 ||
               g_strcmp0(label, "*") == 0 || g_strcmp0(label, "/") == 0 || g_strcmp0(label, "%") == 0) {
        if (operation == label[0]) {
            // If same operator is pressed consecutively, repeat last operation
            calculate_result();
        } else {
            first_number=atof(gtk_entry_get_text(GTK_ENTRY(entry_display)));
            operation=label[0];
            update_display("");
        }
    } else if (g_strcmp0(label, "sin") == 0 || g_strcmp0(label, "cos") == 0 ||
               g_strcmp0(label, "tan") == 0 || g_strcmp0(label, "log") == 0 ||
               g_strcmp0(label, "sqrt") == 0) {
        gdouble value=atof(gtk_entry_get_text(GTK_ENTRY(entry_display)));
        gdouble result=0;

        if (g_strcmp0(label, "sin") == 0) result=sin(value);
        else if (g_strcmp0(label, "cos") == 0) result=cos(value);
        else if (g_strcmp0(label, "tan") == 0) result=tan(value);
        else if (g_strcmp0(label, "log") == 0) result=log10(value);
        else if (g_strcmp0(label, "sqrt") == 0) result=sqrt(value);

        gchar *result_text=g_strdup_printf("%.2f", result);
        update_display(result_text);
        g_free(result_text);
    } else {
        gchar *current_text=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_display)));
        gchar *new_text=g_strjoin("", current_text, label, NULL);
        update_display(new_text);
        g_free(new_text);
        g_free(current_text);
    }
}

// Function to calculate the result
void calculate_result() {
    gchar *current_text=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_display)));
    gdouble second_number=atof(current_text);
    gdouble result=0;

    switch (operation) {
        case '+': result=first_number + second_number; break;
        case '-': result=first_number - second_number; break;
        case '*': result=first_number * second_number; break;
        case '/': result=second_number != 0 ? first_number / second_number : 0; break;
        case '%': result=fmod(first_number, second_number); break;
        default: result=second_number; break;
    }

    gchar *result_text=g_strdup_printf("%.2f", result);
    update_display(result_text);
    g_free(result_text);
    g_free(current_text);
}

// Function to copy result to clipboard
void copy_to_clipboard(GtkWidget *widget) {
    GtkClipboard *clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    const gchar *text=gtk_entry_get_text(GTK_ENTRY(entry_display));
    gtk_clipboard_set_text(clipboard, text, -1);
}

// Function to show author information
void show_author_info() {
    GtkWidget *dialog=gtk_message_dialog_new(NULL,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Author: Jay Mee @ J~Net 2024");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

