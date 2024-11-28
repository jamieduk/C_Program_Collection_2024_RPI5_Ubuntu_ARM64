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
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data);

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
        "%", "sin", "cos", "tan",
        "log", "sqrt", "M+", "MR",
        "About"
    };

    // Create buttons and attach to grid
    int num_buttons=sizeof(button_labels) / sizeof(button_labels[0]);
    for (int i=0; i < num_buttons; i++) {
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
    for (int i=0; i < 5; i++) {
        GtkWidget *adv_button=gtk_button_new_with_label(advanced_labels[i]);
        g_signal_connect(adv_button, "clicked", G_CALLBACK(on_button_click), (gpointer)advanced_labels[i]);
        gtk_widget_set_size_request(adv_button, 60, 60);
        gtk_grid_attach(GTK_GRID(grid_advanced), adv_button, i % 3, i / 3, 1, 1);
    }
    gtk_widget_set_visible(grid_advanced, FALSE);

    // Connect the destroy signal for the window
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Connect the key press event for the window
    g_signal_connect(window, "key-press-event", G_CALLBACK(on_key_press), NULL);

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
        gchar *mem_text=g_strdup_printf("%.10g", memory_value);
        update_display(mem_text);
        g_free(mem_text);
    } else if (g_strcmp0(label, "MC") == 0) {
        memory_value=0;
    } else if (g_strcmp0(label, "+") == 0 || g_strcmp0(label, "-") == 0 ||
               g_strcmp0(label, "*") == 0 || g_strcmp0(label, "/") == 0 || g_strcmp0(label, "%") == 0) {
        first_number=atof(gtk_entry_get_text(GTK_ENTRY(entry_display)));
        operation=label[0];
        update_display("");
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

        gchar *result_text=g_strdup_printf("%.10g", result);
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

    gchar *result_text=g_strdup_printf("%.10g", result);
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

// Key press event handler
gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer data) {
    // Handle numerical pad keys and standard keyboard numbers
    switch (event->keyval) {
        case GDK_KEY_0:
        case GDK_KEY_KP_0:
        case GDK_KEY_KP_Insert:
            on_button_click(NULL, "0");
            break;
        case GDK_KEY_1:
        case GDK_KEY_KP_1:
        case GDK_KEY_KP_End:
            on_button_click(NULL, "1");
            break;
        case GDK_KEY_2:
        case GDK_KEY_KP_2:
        case GDK_KEY_KP_Down:
            on_button_click(NULL, "2");
            break;
        case GDK_KEY_3:
        case GDK_KEY_KP_3:
        case GDK_KEY_KP_Page_Down:
            on_button_click(NULL, "3");
            break;
        case GDK_KEY_4:
        case GDK_KEY_KP_4:
        case GDK_KEY_KP_Left:
            on_button_click(NULL, "4");
            break;
        case GDK_KEY_5:
        case GDK_KEY_KP_5:
        case GDK_KEY_KP_Begin:
            on_button_click(NULL, "5");
            break;
        case GDK_KEY_6:
        case GDK_KEY_KP_6:
        case GDK_KEY_KP_Right:
            on_button_click(NULL, "6");
            break;
        case GDK_KEY_7:
        case GDK_KEY_KP_7:
        case GDK_KEY_KP_Home:
            on_button_click(NULL, "7");
            break;
        case GDK_KEY_8:
        case GDK_KEY_KP_8:
        case GDK_KEY_KP_Up:
            on_button_click(NULL, "8");
            break;
        case GDK_KEY_9:
        case GDK_KEY_KP_9:
        case GDK_KEY_KP_Page_Up:
            on_button_click(NULL, "9");
            break;
        case GDK_KEY_period:
        case GDK_KEY_KP_Decimal:
        case GDK_KEY_KP_Delete:
            on_button_click(NULL, ".");
            break;
        case GDK_KEY_plus:
        case GDK_KEY_KP_Add:
            on_button_click(NULL, "+");
            break;
        case GDK_KEY_minus:
        case GDK_KEY_KP_Subtract:
            on_button_click(NULL, "-");
            break;
        case GDK_KEY_asterisk:
        case GDK_KEY_KP_Multiply:
            on_button_click(NULL, "*");
            break;
        case GDK_KEY_slash:
        case GDK_KEY_KP_Divide:
            on_button_click(NULL, "/");
            break;
        case GDK_KEY_Return:
        case GDK_KEY_KP_Enter:
            calculate_result();
            break;
        case GDK_KEY_BackSpace:
            // Handle backspace to clear the last character
            gchar *current_text=g_strdup(gtk_entry_get_text(GTK_ENTRY(entry_display)));
            if (g_utf8_strlen(current_text, -1) > 0) {
                gchar *new_text=g_strndup(current_text, g_utf8_strlen(current_text, -1) - 1);
                update_display(new_text);
                g_free(new_text);
            }
            g_free(current_text);
            break;
        default:
            break;
    }
    return FALSE;
}

