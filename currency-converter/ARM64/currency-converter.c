// Made By Jay @ J~Net 2024
//
// sudo apt install -y libgtk-3-dev
//
// gcc `pkg-config --cflags gtk+-3.0` -o currency-converter currency-converter.c `pkg-config --libs gtk+-3.0`
//
// ./currency-converter
//
#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

// Conversion rates in a matrix for demonstration purposes
double conversion_rates[6][6]={
    {1.0, 0.93, 0.77, 111.54, 1.26, 1.35}, // USD to others
    {1.08, 1.0, 0.825, 119.98, 1.35, 1.45}, // EUR to others
    {1.30, 1.21, 1.0, 145.45, 1.63, 1.76}, // GBP to others
    {0.009, 0.0083, 0.0069, 1.0, 0.011, 0.012}, // JPY to others
    {0.79, 0.74, 0.61, 91.07, 1.0, 1.08}, // CAD to others
    {0.74, 0.69, 0.57, 85.10, 0.93, 1.0} // AUD to others
};

char *currencies[]={"USD", "EUR", "GBP", "JPY", "CAD", "AUD"};

GtkWidget *entries[6][6];

GtkWidget *amount_entry, *result_label, *from_currency_combo, *to_currency_combo;

// Function to update conversion rates from the entries
void apply_conversion_rates(GtkDialog *dialog, gint response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_OK) {
        for (int i=0; i < 6; ++i) {
            for (int j=0; j < 6; ++j) {
                const gchar *text=gtk_entry_get_text(GTK_ENTRY(entries[i][j]));
                conversion_rates[i][j]=atof(text);
            }
        }
    }
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

// Function to create the edit window with the conversion rate grid
void on_edit_conversion_rates(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_dialog_new_with_buttons(
        "Edit Conversion Rates",
        GTK_WINDOW(data),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Apply",
        GTK_RESPONSE_OK,
        "Cancel",
        GTK_RESPONSE_CANCEL,
        NULL
    );

    GtkWidget *content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *grid=gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    // Add column headers
    for (int i=0; i < 6; ++i) {
        GtkWidget *label=gtk_label_new(currencies[i]);
        gtk_grid_attach(GTK_GRID(grid), label, i + 1, 0, 1, 1);
    }

    // Add row headers and entry fields
    for (int i=0; i < 6; ++i) {
        GtkWidget *label=gtk_label_new(currencies[i]);
        gtk_grid_attach(GTK_GRID(grid), label, 0, i + 1, 1, 1);

        for (int j=0; j < 6; ++j) {
            GtkWidget *entry=gtk_entry_new();
            gtk_entry_set_text(GTK_ENTRY(entry), g_strdup_printf("%.4f", conversion_rates[i][j]));
            gtk_grid_attach(GTK_GRID(grid), entry, j + 1, i + 1, 1, 1);
            entries[i][j]=entry;
        }
    }

    gtk_container_add(GTK_CONTAINER(content_area), grid);
    gtk_widget_show_all(dialog);

    g_signal_connect(dialog, "response", G_CALLBACK(apply_conversion_rates), NULL);
}

// Function to perform the conversion
void on_convert_button_clicked(GtkWidget *widget, gpointer data) {
    const gchar *amount_str=gtk_entry_get_text(GTK_ENTRY(amount_entry));
    double amount=atof(amount_str);

    int from_index=gtk_combo_box_get_active(GTK_COMBO_BOX(from_currency_combo));
    int to_index=gtk_combo_box_get_active(GTK_COMBO_BOX(to_currency_combo));

    double result=amount * conversion_rates[from_index][to_index];
    gchar result_str[64];
    snprintf(result_str, sizeof(result_str), 
             "Converted Amount: %.2f %s", result, currencies[to_index]);

    gtk_label_set_text(GTK_LABEL(result_label), result_str);
}

// Author Information
void on_about_button_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL,
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Author: Jay Mee\nA simple currency converter.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Main function to initialize the GUI
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Currency Converter");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Currency selection combo boxes
    from_currency_combo=gtk_combo_box_text_new();
    to_currency_combo=gtk_combo_box_text_new();
    for (int i=0; i < 6; i++) {
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(from_currency_combo), NULL, currencies[i]);
        gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(to_currency_combo), NULL, currencies[i]);
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(from_currency_combo), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(to_currency_combo), 1);

    gtk_box_pack_start(GTK_BOX(vbox), from_currency_combo, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), to_currency_combo, FALSE, FALSE, 0);

    // Amount entry
    amount_entry=gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(amount_entry), "Enter amount...");
    gtk_box_pack_start(GTK_BOX(vbox), amount_entry, FALSE, FALSE, 0);

    // Convert button
    GtkWidget *convert_button=gtk_button_new_with_label("Convert");
    g_signal_connect(convert_button, "clicked", G_CALLBACK(on_convert_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), convert_button, FALSE, FALSE, 0);

    // Conversion result label
    result_label=gtk_label_new("Converted Amount:");
    gtk_box_pack_start(GTK_BOX(vbox), result_label, FALSE, FALSE, 0);

    // Menu bar for options
    GtkWidget *menu_bar=gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    // Create Menu(s)
    GtkWidget *edit_menu=gtk_menu_new();
    GtkWidget *edit_menu_item=gtk_menu_item_new_with_label("Edit");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(edit_menu_item), edit_menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), edit_menu_item);

    GtkWidget *edit_conversion_item=gtk_menu_item_new_with_label("Edit Conversion Rates");
    g_signal_connect(edit_conversion_item, "activate", G_CALLBACK(on_edit_conversion_rates), window);
    gtk_menu_shell_append(GTK_MENU_SHELL(edit_menu), edit_conversion_item);

    // About button
    GtkWidget *about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 0);

    // Show all widgets
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}
