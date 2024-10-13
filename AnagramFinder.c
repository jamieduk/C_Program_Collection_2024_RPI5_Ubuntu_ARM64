#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
// gcc AnagramFinder.c -o AnagramFinder `pkg-config --cflags --libs gtk+-3.0`

#define ASCII_SIZE 256
#define MAX_WORDS 100
#define WORD_LENGTH 100

// Function Prototypes
void findWords(GtkWidget *widget, gpointer data);
int canFormWord(const char *inputWord, const char *dictWord);
void on_about_button_clicked(GtkWidget *widget, gpointer data);

char *dictionary[]={
    "armageddon", "dragon", "garn", "dread", "magma", 
    "marg", "maged", "great", "mead", "red", "are", 
    "mad", "gar", "read", "dare", "dear", NULL
};

typedef struct {
    GtkEntry *entry;
    GtkLabel *label;
} AppWidgets;

int main(int argc, char *argv[])
{
    GtkWidget *window, *vbox, *button, *about_button;

    gtk_init(&argc, &argv);

    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Word Finder");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    AppWidgets appWidgets;

    appWidgets.entry=GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(appWidgets.entry, "Enter a word");
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(appWidgets.entry), FALSE, FALSE, 0);

    button=gtk_button_new_with_label("Find Words");
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);

    appWidgets.label=GTK_LABEL(gtk_label_new("Possible words will be displayed here"));
    gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(appWidgets.label), FALSE, FALSE, 0);

    // Create an About button
    about_button=gtk_button_new_with_label("About");
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 0);

    g_signal_connect(button, "clicked", G_CALLBACK(findWords), &appWidgets);
    g_signal_connect(appWidgets.entry, "activate", G_CALLBACK(findWords), &appWidgets);
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_button_clicked), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

void findWords(GtkWidget *widget, gpointer data)
{
    AppWidgets *appWidgets=(AppWidgets *)data;
    const char *inputWord=gtk_entry_get_text(appWidgets->entry);
    char result[512]="Possible words: ";
    int found=0;

    for (int i=0; dictionary[i] != NULL; i++) {
        if (canFormWord(inputWord, dictionary[i])) {
            strcat(result, dictionary[i]);
            strcat(result, ", ");
            found=1;
        }
    }

    if (found) {
        result[strlen(result) - 2]='\0'; // Remove last comma and space
    } else {
        strcpy(result, "No valid words found.");
    }

    gtk_label_set_text(appWidgets->label, result);
}

// Function to check whether a word can be formed from the given letters
int canFormWord(const char *inputWord, const char *dictWord)
{
    int inputChrCtr[ASCII_SIZE]={0};

    // Count frequency of characters in the input word
    for (int i=0; inputWord[i] != '\0'; i++) {
        inputChrCtr[(unsigned char)inputWord[i]]++;
    }

    // Check if the dictWord can be formed
    for (int i=0; dictWord[i] != '\0'; i++) {
        if (inputChrCtr[(unsigned char)dictWord[i]] == 0) {
            return 0; // A character is not present enough times
        }
        inputChrCtr[(unsigned char)dictWord[i]]--;
    }

    return 1; // All characters in dictWord can be formed
}

// Callback function for the About button
void on_about_button_clicked(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog=gtk_message_dialog_new(NULL,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK,
        "Author: Jay @ J~Net 2024 \n\nWord Finder Application\n\nThis application helps you find possible words that can be formed from a given set of letters.");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

