// tts.cpp
// Made By Jay @ J~Net 2024

#include <gtk/gtk.h>
#include <string>
#include <vector>
#include <iostream>

// Struct for the MainWindow
struct MainWindow {
    GtkWidget* window;
    GtkWidget* comboBox;
    GtkWidget* entry;
    std::string default_voice;

    void init(const std::string& voice);
    void on_speak_button_clicked();
    void show_about_dialog();

    static void on_speak_button_clicked_static(GtkWidget*, gpointer data) {
        static_cast<MainWindow*>(data)->on_speak_button_clicked();
    }

    static void show_about_dialog_static(GtkWidget*, gpointer data) {
        static_cast<MainWindow*>(data)->show_about_dialog();
    }
};

// Initialization function
void MainWindow::init(const std::string& voice) {
    default_voice=voice;

    // Create window
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Text-to-Speech");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box
    GtkWidget* vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create a combo box
    comboBox=gtk_combo_box_text_new();
    gtk_box_pack_start(GTK_BOX(vbox), comboBox, FALSE, FALSE, 5);

    // Populate combo box with example voices
    std::vector<std::string> voices={"Voice1", "Voice2", "Voice3"};
    for (const auto& voice : voices) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), voice.c_str());
    }

    // Create text entry
    entry=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 5);

    // Create Speak button
    GtkWidget* speak_button=gtk_button_new_with_label("Speak");
    gtk_box_pack_start(GTK_BOX(vbox), speak_button, FALSE, FALSE, 5);
    g_signal_connect(speak_button, "clicked", G_CALLBACK(on_speak_button_clicked_static), this);

    // Create About button
    GtkWidget* about_button=gtk_button_new_with_label("About");
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 5);
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about_dialog_static), this);

    // Show all widgets
    gtk_widget_show_all(window);
}

// Function to handle "Speak" button click
void MainWindow::on_speak_button_clicked() {
    const gchar* text=gtk_entry_get_text(GTK_ENTRY(entry));
    if (strlen(text) == 0) {
        std::cerr << "No text entered." << std::endl;
        return;
    }

    std::string command="espeak \"" + std::string(text) + "\" &";
    int result=system(command.c_str());

    if (result != 0) {
        std::cerr << "Failed to execute espeak." << std::endl;
    }
}

// Function to display "About" dialog
void MainWindow::show_about_dialog() {
    GtkWidget* dialog=gtk_message_dialog_new(GTK_WINDOW(window),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Author: Jay Mee\nJ~Net 2024");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int main(int argc, char* argv[]) {
    gtk_init(&argc, &argv);

    MainWindow mainWindow;
    mainWindow.init("Voice1"); // Initialize with default voice

    gtk_main();
    return 0;
}

