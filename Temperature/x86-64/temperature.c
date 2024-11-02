#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
// gcc -o Temperature Temperature.c `pkg-config --cflags --libs gtk+-3.0`

// Function to calculate temperature
void calculate_temperature(GtkWidget *button, gpointer data) {
    GtkWidget **widgets=(GtkWidget **)data; // Get the input field and the dropdown
    GtkEntry *input=GTK_ENTRY(widgets[0]);
    GtkComboBoxText *combo=GTK_COMBO_BOX_TEXT(widgets[1]);
    GtkWindow *main_window=GTK_WINDOW(widgets[2]); // Get the main window

    const gchar *input_text=gtk_entry_get_text(input);
    float temperature=atof(input_text);
    float converted_temperature;
    char result[100];

    // Determine conversion direction
    if (g_strcmp0(gtk_combo_box_text_get_active_text(combo), "Celsius to Fahrenheit") == 0) {
        converted_temperature=temperature * (9.0 / 5.0) + 32.0;
        snprintf(result, sizeof(result), "Temperature in Fahrenheit: %.2f", converted_temperature);
    } else {
        converted_temperature=(temperature - 32.0) * (5.0 / 9.0);
        snprintf(result, sizeof(result), "Temperature in Celsius: %.2f", converted_temperature);
    }

    // Create a dialog to display the result
    GtkWidget *dialog=gtk_message_dialog_new(main_window,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "%s", result);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Function to show about dialog
void on_about(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(data),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Author: Jay @ J~Net 2024 \n\nTemperature Converter\n\nConvert between Celsius and Fahrenheit.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Check if temperature is provided as CLI argument
    if (argc > 1) {
        float temperature=atof(argv[1]);
        float converted_temperature;

        if (argc > 2 && g_strcmp0(argv[2], "F") == 0) {
            // Convert Fahrenheit to Celsius
            converted_temperature=(temperature - 32.0) * (5.0 / 9.0);
            printf("Temperature in Celsius: %.2f\n", converted_temperature);
        } else {
            // Convert Celsius to Fahrenheit
            converted_temperature=temperature * (9.0 / 5.0) + 32.0;
            printf("Temperature in Fahrenheit: %.2f\n", converted_temperature);
        }
        return 0; // Exit after CLI calculation
    }

    // GUI code
    GtkWidget *window;
    GtkWidget *input, *button, *about_button, *vbox;
    GtkComboBoxText *combo;

    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Temperature Converter");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_widget_set_margin_top(window, 20);
    gtk_widget_set_margin_bottom(window, 20);
    gtk_widget_set_margin_start(window, 20);
    gtk_widget_set_margin_end(window, 20);

    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Input field for temperature
    input=gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(input), "Enter Temperature");
    gtk_box_pack_start(GTK_BOX(vbox), input, TRUE, TRUE, 0);

    // Combo box for conversion type
    combo=GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
    gtk_combo_box_text_append(combo, NULL, "Celsius to Fahrenheit");
    gtk_combo_box_text_append(combo, NULL, "Fahrenheit to Celsius");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0); // Default selection
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(combo), TRUE, TRUE, 0);

    // Button to convert
    button=gtk_button_new_with_label("Convert");
    gtk_box_pack_start(GTK_BOX(vbox), button, TRUE, TRUE, 0);

    // About button
    about_button=gtk_button_new_with_label("About");
    gtk_box_pack_start(GTK_BOX(vbox), about_button, TRUE, TRUE, 0);

    // Connect button signals
    GtkWidget *widgets[]={ input, GTK_WIDGET(combo), window }; // Pass window as well
    g_signal_connect(button, "clicked", G_CALLBACK(calculate_temperature), widgets);
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about), window);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_widget_set_name(window, "main-window");
    gtk_main();

    return 0;
}

