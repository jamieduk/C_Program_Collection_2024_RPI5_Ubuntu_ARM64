#include <gtk/gtk.h>
#include <math.h>

// Function prototypes
void on_button_click(GtkWidget *widget, gpointer data);
void calculate_result();
void show_author_info();
void copy_to_clipboard(GtkWidget *widget);
void toggle_advanced_mode();

// Global variables
GtkWidget *entry_display, *grid;
gchar operation;
gdouble first_number=0;
gboolean advanced_mode=FALSE;

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Simple Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 400);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    // Create a grid layout
    grid=gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Create entry display (non-editable)
    entry_display=gtk_entry_new();
    gtk_widget_set_sensitive(entry_display, FALSE); // Make it non-editable
    gtk_grid_attach(GTK_GRID(grid), entry_display, 0, 0, 4, 1);

    // Define basic button labels
    const gchar *button_labels[]={
        "7", "8", "9", "/",
        "4", "5", "6", "*",
        "1", "2", "3", "-",
        "0", ".", "CLR", "+",
        "Copy", "About", "=", "Adv"
    };

    // Create buttons and attach them to the grid for basic mode
    int num_buttons=sizeof(button_labels) / sizeof(button_labels[0]);
    for (int i=0; i < num_buttons; i++) {
        GtkWidget *button=gtk_button_new_with_label(button_labels[i]);
        g_signal_connect(button, "clicked", G_CALLBACK(on_button_click), (gpointer)button_labels[i]);

        // Set size for all buttons
        gtk_widget_set_size_request(button, 60, 60);

        gtk_grid_attach(GTK_GRID(grid), button, i % 4, (i / 4) + 1, 1, 1);
    }

    // Connect the destroy signal for the window
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Show all widgets in the window
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}

// Callback function for button clicks
void on_button_click(GtkWidget *widget, gpointer data) {
    const gchar *label=(const gchar *)data;

    if (g_strcmp0(label, "CLR") == 0) {
        gtk_entry_set_text(GTK_ENTRY(entry_display), "");
        first_number=0;
        operation='\0';
    } else if (g_strcmp0(label, "=") == 0) {
        calculate_result();
    } else if (g_strcmp0(label, "Copy") == 0) {
        copy_to_clipboard(widget);
    } else if (g_strcmp0(label, "About") == 0) {
        show_author_info();
    } else if (g_strcmp0(label, "Adv") == 0) {
        toggle_advanced_mode();
    } else {
        gchar *current_text=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_display)));
        
        // Handle operations and appending numbers
        if (g_strcmp0(label, "+") == 0 || g_strcmp0(label, "-") == 0 ||
            g_strcmp0(label, "*") == 0 || g_strcmp0(label, "/") == 0) {
            first_number=atof(current_text);
            operation=label[0];
            gtk_entry_set_text(GTK_ENTRY(entry_display), "");
        } else if (advanced_mode && (g_strcmp0(label, "sin") == 0 || g_strcmp0(label, "cos") == 0 || 
                  g_strcmp0(label, "tan") == 0 || g_strcmp0(label, "sqrt") == 0 || 
                  g_strcmp0(label, "log") == 0 || g_strcmp0(label, "ln") == 0 || 
                  g_strcmp0(label, "exp") == 0)) {
            gdouble value=atof(current_text);
            gdouble result=0;

            if (g_strcmp0(label, "sin") == 0) result=sin(value);
            else if (g_strcmp0(label, "cos") == 0) result=cos(value);
            else if (g_strcmp0(label, "tan") == 0) result=tan(value);
            else if (g_strcmp0(label, "sqrt") == 0) result=sqrt(value);
            else if (g_strcmp0(label, "log") == 0) result=log10(value);
            else if (g_strcmp0(label, "ln") == 0) result=log(value);
            else if (g_strcmp0(label, "exp") == 0) result=exp(value);

            gchar *result_text=g_strdup_printf("%.2f", result);
            gtk_entry_set_text(GTK_ENTRY(entry_display), result_text);
            g_free(result_text);
        } else {
            gchar *new_text=g_strjoin("", current_text, label, NULL);
            gtk_entry_set_text(GTK_ENTRY(entry_display), new_text);
            g_free(new_text);
        }
        g_free(current_text);
    }
}

// Function to calculate the result
void calculate_result() {
    gchar *current_text=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_display)));
    gdouble second_number=atof(current_text);
    gdouble result=0;

    switch (operation) {
        case '+':
            result=first_number + second_number;
            break;
        case '-':
            result=first_number - second_number;
            break;
        case '*':
            result=first_number * second_number;
            break;
        case '/':
            if (second_number != 0) {
                result=first_number / second_number;
            } else {
                gtk_entry_set_text(GTK_ENTRY(entry_display), "Error");
                g_free(current_text);
                return;
            }
            break;
        default:
            result=second_number;
            break;
    }

    gchar *result_text=g_strdup_printf("%.2f", result);
    gtk_entry_set_text(GTK_ENTRY(entry_display), result_text);
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

// Function to toggle advanced mode
void toggle_advanced_mode() {
    const gchar *advanced_labels[]={
        "sin", "cos", "tan", "sqrt",
        "log", "ln", "exp"
    };
    
    if (!advanced_mode) {
        for (int i=0; i < 7; i++) {
            GtkWidget *adv_button=gtk_button_new_with_label(advanced_labels[i]);
            g_signal_connect(adv_button, "clicked", G_CALLBACK(on_button_click), (gpointer)advanced_labels[i]);
            gtk_widget_set_size_request(adv_button, 60, 60);
            gtk_grid_attach(GTK_GRID(grid), adv_button, i % 4, 5 + (i / 4), 1, 1);
        }
        gtk_window_resize(GTK_WINDOW(gtk_widget_get_toplevel(entry_display)), 300, 500);
    } else {
        GList *children=gtk_container_get_children(GTK_CONTAINER(grid));
        for (GList *iter=children; iter!=NULL; iter=iter->next) {
            GtkWidget *widget=(GtkWidget *)iter->data;
            const gchar *label=gtk_button_get_label(GTK_BUTTON(widget));
            for (int i=0; i < 7; i++) {
                if (g_strcmp0(label, advanced_labels[i]) == 0) {
                    gtk_widget_destroy(widget);
                    break;
                }
            }
        }
        g_list_free(children);
        gtk_window_resize(GTK_WINDOW(gtk_widget_get_toplevel(entry_display)), 300, 400);
    }

    advanced_mode=!advanced_mode;
    gtk_widget_show_all(gtk_widget_get_toplevel(entry_display));
}

