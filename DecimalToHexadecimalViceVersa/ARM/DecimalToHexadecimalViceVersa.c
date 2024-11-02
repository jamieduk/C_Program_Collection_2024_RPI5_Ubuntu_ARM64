#include <gtk/gtk.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
// gcc `pkg-config --cflags gtk+-3.0` -o DecimalToHexadecimalViceVersa DecimalToHexadecimalViceVersa.c `pkg-config --libs gtk+-3.0` -lm

void decimal_hex(int n, char hex[]);
int hex_decimal(const char hex[]);
void reverse_string(char* str);
void on_convert(GtkWidget *widget, gpointer data);
void on_about(GtkWidget *widget, gpointer data);
void on_window_destroy(GtkWidget *widget, gpointer data);

GtkWidget *entry_decimal, *entry_hex, *label_result;

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Create main window
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Decimal-Hexadecimal Converter");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), NULL);

    // Create a vertical box for layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create input fields and labels
    GtkWidget *label_choice=gtk_label_new("Choose Conversion:");
    gtk_box_pack_start(GTK_BOX(vbox), label_choice, FALSE, FALSE, 0);

    GtkWidget *radio_decimal_to_hex=gtk_radio_button_new_with_label(NULL, "Decimal to Hexadecimal");
    gtk_box_pack_start(GTK_BOX(vbox), radio_decimal_to_hex, FALSE, FALSE, 0);

    GtkWidget *radio_hex_to_decimal=gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_decimal_to_hex), "Hexadecimal to Decimal");
    gtk_box_pack_start(GTK_BOX(vbox), radio_hex_to_decimal, FALSE, FALSE, 0);

    entry_decimal=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry_decimal, FALSE, FALSE, 0);

    entry_hex=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry_hex, FALSE, FALSE, 0);

    GtkWidget *button_convert=gtk_button_new_with_label("Convert");
    g_signal_connect(button_convert, "clicked", G_CALLBACK(on_convert), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_convert, FALSE, FALSE, 0);

    label_result=gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(vbox), label_result, FALSE, FALSE, 0);

    // About button
    GtkWidget *button_about=gtk_button_new_with_label("About");
    g_signal_connect(button_about, "clicked", G_CALLBACK(on_about), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), button_about, FALSE, FALSE, 0);

    // Set dark theme
    GtkCssProvider *css_provider=gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider, 
        "window { background-color: #2E2E2E; }\n"
        "label { color: #FFFFFF; }\n"
        "entry { background-color: #444444; color: #FFFFFF; }\n"
        "button { background-color: #007ACC; color: #FFFFFF; }\n", 
        -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), 
        GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_provider);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

void on_convert(GtkWidget *widget, gpointer data) {
    const gchar *decimal_input=gtk_entry_get_text(GTK_ENTRY(entry_decimal));
    const gchar *hex_input=gtk_entry_get_text(GTK_ENTRY(entry_hex));

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_radio_button_get_group(GTK_RADIO_BUTTON(gtk_widget_get_parent(widget)))))) {
        // Decimal to Hexadecimal
        int decimal_number=atoi(decimal_input);
        char hex[20];
        decimal_hex(decimal_number, hex);
        gchar *result_text=g_strdup_printf("Hexadecimal: %s", hex);
        gtk_label_set_text(GTK_LABEL(label_result), result_text);
        g_free(result_text);
    } else {
        // Hexadecimal to Decimal
        int decimal_number=hex_decimal(hex_input);
        gchar *result_text=g_strdup_printf("Decimal: %d", decimal_number);
        gtk_label_set_text(GTK_LABEL(label_result), result_text);
        g_free(result_text);
    }
}

void on_about(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(data),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Author: Jay @ J~Net 2024 \n\nDecimal-Hexadecimal Converter\n\nConvert between decimal and hexadecimal numbers.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_window_destroy(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void decimal_hex(int n, char hex[]) {
    int i=0, rem;
    while (n != 0) {
        rem=n % 16;
        if (rem < 10) {
            hex[i]=rem + '0';
        } else {
            hex[i]=rem - 10 + 'A';
        }
        ++i;
        n /= 16;
    }
    hex[i]='\0';
    reverse_string(hex);
}

int hex_decimal(const char hex[]) {
    int i, length, sum=0;
    for (length=0; hex[length] != '\0'; ++length);
    for (i=0; hex[i] != '\0'; ++i, --length) {
        if (hex[i] >= '0' && hex[i] <= '9')
            sum += (hex[i] - '0') * pow(16, length - 1);
        if (hex[i] >= 'A' && hex[i] <= 'F')
            sum += (hex[i] - 55) * pow(16, length - 1);
        if (hex[i] >= 'a' && hex[i] <= 'f')
            sum += (hex[i] - 87) * pow(16, length - 1);
    }
    return sum;
}

void reverse_string(char* str) {
    int n=strlen(str);
    for (int i=0; i < n / 2; i++) {
        char temp=str[i];
        str[i]=str[n - i - 1];
        str[n - i - 1]=temp;
    }
}

