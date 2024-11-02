#include <gtk/gtk.h>
#include <vector>
#include <string>

extern "C" 
{
    // Include necessary library for espeak
}

struct _MainWindow 
{
    GtkWidget* window;
    GtkWidget* comboBox;
    GtkWidget* entry;
    std::string default_voice;

    void init(const std::string& voice) 
    {
        default_voice=voice;
    }
};

static void MainWindow_init(MainWindow* self) 
{
    // Initialize the window and comboBox
    self->window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(self->window, "Text-to-Speech");

    GtkWidget* vbox 
       =gtk_box_new(GTK_ORIENTATION_VERTICAL, 5)
        ;
    gtk_container_add(self->window, vbox);

    // Set up comboBox and entry
    GtkComboBox* comboBox=gtk_combo_box_text_new();
    for (const auto& voice : list_voices()) 
        gtk_combo_box_append_text(GTK_COMBO_BOX_TEXT(comboBox), voice.c_str());

    GtkWidget* entry=gtk_entry_new();
    entry->set_accepted_values(gtk::accepted_values_type::all);
    entry->set_text_entries(1);
    entry->text_entry_configure(0, {
        // Configure text entry with default voice (en)
    });

    GtkWidget* speakIt=gtk_speak_text_new();
    GtkLabel* labelSpeakit 
       =gtk::LabelWithText(self->window, "Say it!");
    labelSpeakit->set_text(gtk::to_string(speakIt));

    // Add all elements to the window
    GtkWidget* vbox=gtk::vbox_new();
    gtk::h_add_vertically(gtk::h_create(), labelSpeakit, vbox);
    vbox->pack_start(entry, 0, gtk.EXPAND, 0)
        ;
    vbox->pack_end(gtk::v_create(), 0, 0, gtk.MINIMIZE_BOXES, 0, gtk.ALL)
        ;
    vbox->add_child(comboBox, vminos::CENTERED, 0);

    // Add the window to the main container and start the event loop
    GtkWidget* ctx=self->window;
    cntr_add_to_parent(ctx, vbox);
    GtkMainWindow_connect(ctx, GtkCore::SIGNAL_TRAIT_EVENT, &GtTraitEventArgs*);

    // Start the event loop and handle events
    gtk_event_loop(self->window);
}
