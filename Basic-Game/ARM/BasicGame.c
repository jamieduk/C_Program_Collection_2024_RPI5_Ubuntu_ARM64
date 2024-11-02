#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define GAME_MAX 10  // Changed from MAX to GAME_MAX to avoid conflicts

// Function Prototypes
int getRandom(int low, int high);
unsigned int playerRoll(int low, int high);
void displayBoard(GtkTextBuffer *buffer);
void on_roll_button_clicked(GtkButton *button, gpointer user_data);
void on_quit_button_clicked(GtkButton *button, gpointer user_data);

// Game state variables
unsigned int playerPosition=0;
int playerScore=0;

// Seed random number generator
void seed(void) {
    srand(time(NULL));
}

// Generate a random number between low and high (inclusive)
int getRandom(int low, int high) {
    return rand() % (high - low + 1) + low;  // Include high in range
}

// Simulate a player roll
unsigned int playerRoll(int low, int high) {
    return getRandom(low, high);  // Roll the dice
}

// Display the game board in the text view
void displayBoard(GtkTextBuffer *buffer) {
    gtk_text_buffer_set_text(buffer, "Game Board:\n", -1);

    // Render the board based on playerPosition
    for (int i=0; i < GAME_MAX; i++) {
        if (i == playerPosition) {
            gtk_text_buffer_insert_at_cursor(buffer, "* ", -1);  // Player position
        } else {
            gtk_text_buffer_insert_at_cursor(buffer, ". ", -1);  // Empty space
        }
    }
    gtk_text_buffer_insert_at_cursor(buffer, "\n", -1);
    
    // Display the player score
    gtk_text_buffer_insert_at_cursor(buffer, "Score: ", -1);
    char scoreText[50];
    sprintf(scoreText, "%d\n", playerScore);
    gtk_text_buffer_insert_at_cursor(buffer, scoreText, -1);
}

// Handle the roll button click event
void on_roll_button_clicked(GtkButton *button, gpointer user_data) {
    playerPosition += playerRoll(1, 6);  // Roll the dice and update player position
    if (playerPosition >= GAME_MAX) { // Reset position if it exceeds board size
        playerPosition=0;
    }
    displayBoard((GtkTextBuffer *)user_data); // Update the display
}

// Handle the quit button click event
void on_quit_button_clicked(GtkButton *button, gpointer user_data) {
    gtk_main_quit(); // Exit the GTK main loop
}

// Main function
int main(int argc, char *argv[]) {
    gtk_init(NULL, NULL); // Initialize GTK without command-line arguments

    seed(); // Initialize random seed

    // Create main window and set properties
    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(on_quit_button_clicked), NULL);
    gtk_window_set_title(GTK_WINDOW(window), "Checkout Game");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    // Create vertical box layout
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create text view for displaying the board
    GtkWidget *text_view=gtk_text_view_new();
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_box_pack_start(GTK_BOX(vbox), text_view, TRUE, TRUE, 0);

    // Create roll button
    GtkWidget *roll_button=gtk_button_new_with_label("Roll Dice");
    g_signal_connect(roll_button, "clicked", G_CALLBACK(on_roll_button_clicked), buffer);
    gtk_box_pack_start(GTK_BOX(vbox), roll_button, FALSE, FALSE, 0);

    // Create quit button
    GtkWidget *quit_button=gtk_button_new_with_label("Quit");
    g_signal_connect(quit_button, "clicked", G_CALLBACK(on_quit_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), quit_button, FALSE, FALSE, 0);

    gtk_widget_show_all(window); // Show all widgets
    gtk_main(); // Start the GTK main loop

    return 0;
}

