// Made By Jay @ J~Net 2024
//
// gcc -o guessing_game guessing_game.c `pkg-config --cflags --libs gtk+-3.0`
//
//
#include <gtk/gtk.h>
#include <stdlib.h>
#include <time.h>

GtkWidget *guess_entry, *min_entry, *max_entry, *result_label, *guess_input_entry;
int guess_count, min_num, max_num, random_num, attempts;

void start_game(GtkWidget *widget, gpointer data) {
    const gchar *guess_text=gtk_entry_get_text(GTK_ENTRY(guess_entry));
    const gchar *min_text=gtk_entry_get_text(GTK_ENTRY(min_entry));
    const gchar *max_text=gtk_entry_get_text(GTK_ENTRY(max_entry));
    
    guess_count=atoi(guess_text);
    min_num=atoi(min_text);
    max_num=atoi(max_text);
    
    srand((unsigned)time(0));
    random_num=rand() % (max_num - min_num + 1) + min_num;
    attempts=0;

    gtk_label_set_text(GTK_LABEL(result_label), "Game Started! Make your guess.");
    gtk_widget_set_sensitive(guess_input_entry, TRUE); // Enable the guess input field
}

void check_guess(GtkWidget *widget, gpointer data) {
    const gchar *guess_text=gtk_entry_get_text(GTK_ENTRY(guess_input_entry));
    int user_guess=atoi(guess_text);
    
    attempts++;
    
    if (user_guess == random_num) {
        gtk_label_set_text(GTK_LABEL(result_label), "You guessed the number!");
        gtk_widget_set_sensitive(guess_input_entry, FALSE); // Disable guess input after winning
        return;
    }
    
    if (attempts >= guess_count) {
        gtk_label_set_text(GTK_LABEL(result_label), "No more attempts! Game over.");
        gtk_widget_set_sensitive(guess_input_entry, FALSE); // Disable guess input after losing
        return;
    }
    
    if (user_guess < random_num) {
        gtk_label_set_text(GTK_LABEL(result_label), "Your guess was too low.");
    } else {
        gtk_label_set_text(GTK_LABEL(result_label), "Your guess was too high.");
    }

    gchar buffer[50];
    snprintf(buffer, sizeof(buffer), "Attempts remaining: %d", guess_count - attempts);
    gtk_label_set_text(GTK_LABEL(result_label), buffer);
}

void show_about(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(data),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Number Guessing Game\nAuthor: Jay Mee");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    GtkWidget *window, *vbox, *start_button, *check_button, *about_button;

    gtk_init(&argc, &argv);

    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Number Guessing Game");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("Enter number of guesses:"), FALSE, FALSE, 0);
    guess_entry=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), guess_entry, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("Enter minimum number:"), FALSE, FALSE, 0);
    min_entry=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), min_entry, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("Enter maximum number:"), FALSE, FALSE, 0);
    max_entry=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), max_entry, FALSE, FALSE, 0);

    start_button=gtk_button_new_with_label("Start Game");
    g_signal_connect(start_button, "clicked", G_CALLBACK(start_game), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), start_button, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("Enter your guess:"), FALSE, FALSE, 0);
    guess_input_entry=gtk_entry_new(); // Input field for user's guess
    gtk_widget_set_sensitive(guess_input_entry, FALSE); // Disable until game starts
    gtk_box_pack_start(GTK_BOX(vbox), guess_input_entry, FALSE, FALSE, 0);

    check_button=gtk_button_new_with_label("Check Guess");
    g_signal_connect(check_button, "clicked", G_CALLBACK(check_guess), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), check_button, FALSE, FALSE, 0);

    result_label=gtk_label_new("Results will appear here.");
    gtk_box_pack_start(GTK_BOX(vbox), result_label, FALSE, FALSE, 0);

    about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about), window);
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

