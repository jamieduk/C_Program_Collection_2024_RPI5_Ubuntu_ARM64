// Made By Jay @ J~Net 2024
//
// gcc `pkg-config --cflags gtk+-3.0` -o reverse_text reverse_text.c `pkg-config --libs gtk+-3.0`
#include <gtk/gtk.h>

static void reverse_text(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer=GTK_TEXT_BUFFER(data);
    GtkTextIter start, end;
    gchar *text;

    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    text=gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Reverse the text
    int len=strlen(text);
    gchar *reversed=g_malloc(len + 1);
    for (int i=0; i < len; i++) {
        reversed[i]=text[len - 1 - i];
    }
    reversed[len]='\0';

    gtk_text_buffer_set_text(buffer, reversed, -1);
    g_free(text);
    g_free(reversed);
}

static void upside_down_text(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer=GTK_TEXT_BUFFER(data);
    GtkTextIter start, end;
    gchar *text;

    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    text=gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Upside down the text (simple character flipping)
    for (int i=0; text[i]; i++) {
        if (text[i] >= 'A' && text[i] <= 'Z') {
            text[i]='Z' - (text[i] - 'A');
        } else if (text[i] >= 'a' && text[i] <= 'z') {
            text[i]='z' - (text[i] - 'a');
        }
    }

    gtk_text_buffer_set_text(buffer, text, -1);
    g_free(text);
}

static void copy_to_clipboard(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer=GTK_TEXT_BUFFER(data);
    GtkTextIter start, end;
    gchar *text;
    GtkClipboard *clipboard=gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    text=gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    gtk_clipboard_set_text(clipboard, text, -1);
    g_free(text);
}

static void show_about(GtkWidget *widget, gpointer data) {
    GtkAboutDialog *dialog=GTK_ABOUT_DIALOG(data);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_hide(GTK_WIDGET(dialog));
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *vbox, *text_view, *reverse_button, *upside_down_button, *copy_button, *about_button, *about_dialog;

    gtk_init(&argc, &argv);

    // Create main window
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Text Manipulator");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create vertical box
    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create text view
    text_view=gtk_text_view_new();
    gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 0);
    
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    // Create buttons
    reverse_button=gtk_button_new_with_label("Reverse Text");
    g_signal_connect(reverse_button, "clicked", G_CALLBACK(reverse_text), buffer);
    gtk_box_pack_start(GTK_BOX(vbox), reverse_button, FALSE, FALSE, 0);

    upside_down_button=gtk_button_new_with_label("Upside Down Text");
    g_signal_connect(upside_down_button, "clicked", G_CALLBACK(upside_down_text), buffer);
    gtk_box_pack_start(GTK_BOX(vbox), upside_down_button, FALSE, FALSE, 0);

    // Copy to clipboard button
    copy_button=gtk_button_new_with_label("Copy to Clipboard");
    g_signal_connect(copy_button, "clicked", G_CALLBACK(copy_to_clipboard), buffer);
    gtk_box_pack_start(GTK_BOX(vbox), copy_button, FALSE, FALSE, 0);

    about_button=gtk_button_new_with_label("About");
    about_dialog=gtk_about_dialog_new();
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about), about_dialog);
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 0);

    // Set up About dialog
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "Text Manipulator");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), "1.0");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), "A simple text manipulator application By Jay Mee @ J~Net 2024.");
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), (const gchar*[]){"Jay Mee", NULL});

    // Show all widgets
    gtk_widget_show_all(window);
    
    gtk_main();
    
    return 0;
}

