// Made By Jay @ J~Net 2024
// gcc -o times_table times_table.c `pkg-config --cflags --libs gtk+-3.0`
//
#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

void generate_table(int n) {
    printf("\n\n\n");
    for(int i=1; i <= 10; i++) {
        printf("\n\t\t\t%d * %d=%d \n", n, i, n * i);
    }
    printf("\n\n\n\n\t\t\tBy J~Net 2024\n\n\n");
}

void on_generate_clicked(GtkButton *button, gpointer user_data) {
    const char *input=gtk_entry_get_text(GTK_ENTRY(user_data));
    int n=atoi(input);
    GtkWidget *output=gtk_text_view_new();
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(output));
    gchar *result=g_strdup_printf("\n\n\t\t\tTable of %d\n\n", n);

    for(int i=1; i <= 10; i++) {
        result=g_strdup_printf("%s%d * %d=%d\n", result, n, i, n * i);
    }

    gtk_text_buffer_set_text(buffer, result, -1);
    g_free(result);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(output), buffer);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(output), FALSE);

    // Get the parent container correctly and place the output
    GtkWidget *parent=gtk_widget_get_parent(GTK_WIDGET(button));
    gtk_fixed_put(GTK_FIXED(parent), output, 50, 150);
    gtk_widget_show(output);
}

void on_about_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(user_data),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Author: Jay Mee @ J~Net 2024");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    if (argc > 1) { // CLI mode
        int n=atoi(argv[1]);
        generate_table(n);
        return 0;
    }

    // GUI mode
    gtk_init(&argc, &argv);

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Times Table Generator");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *fixed=gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    GtkWidget *entry=gtk_entry_new();
    gtk_fixed_put(GTK_FIXED(fixed), entry, 50, 50);

    GtkWidget *button=gtk_button_new_with_label("Generate Table");
    gtk_fixed_put(GTK_FIXED(fixed), button, 250, 50);
    
    g_signal_connect(button, "clicked", G_CALLBACK(on_generate_clicked), entry);
    
    // Create and add the About button
    GtkWidget *about_button=gtk_button_new_with_label("About");
    gtk_fixed_put(GTK_FIXED(fixed), about_button, 50, 100);
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_clicked), window);
    
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

