//
// Created By J~Net(c) 2024
// 
// gcc -o archive-viewer archive-viewer.c `pkg-config --cflags --libs gtk+-3.0` -larchive
//
// ./archive-viewer
//
//
#include <gtk/gtk.h>
#include <archive.h>
#include <archive_entry.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

static GtkWidget *tree;
static GtkListStore *listStore;
static gchar *archivePath=NULL;
static GtkWidget *progressBar;
static GtkWidget *extractButton;

void show_message(GtkWindow *parent, const gchar *message, GtkMessageType type) {
    GtkWidget *dialog=gtk_message_dialog_new(parent,
                                             GTK_DIALOG_MODAL,
                                             type,
                                             GTK_BUTTONS_OK,
                                             "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void show_error(GtkWindow *parent, const gchar *message) {
    show_message(parent, message, GTK_MESSAGE_ERROR);
}

void show_about_dialog(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_about_dialog_new();
    gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), "Archive Viewer");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "1.0");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), 
        "A simple archive viewer for ZIP files.");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog), 
        "© 2024 Jay Mee");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

char* format_size(goffset size) {
    if (size < 1024) {
        return g_strdup_printf("%ld B", (long)size);
    } else if (size < 1024 * 1024) {
        return g_strdup_printf("%.1f KB", size / 1024.0);
    } else if (size < 1024 * 1024 * 1024) {
        return g_strdup_printf("%.1f MB", size / (1024.0 * 1024.0));
    } else {
        return g_strdup_printf("%.1f GB", size / (1024.0 * 1024.0 * 1024.0));
    }
}

void load_archive_contents(const gchar *path, GtkWindow *parent) {
    if (!path) {
        show_error(parent, "No archive selected!");
        return;
    }

    gtk_list_store_clear(listStore);

    struct archive *arch=archive_read_new();
    struct archive_entry *entry;
    
    archive_read_support_format_all(arch);
    archive_read_support_filter_all(arch);

    if (archive_read_open_filename(arch, path, 10240) != ARCHIVE_OK) {
        show_error(parent, archive_error_string(arch));
        archive_read_free(arch);
        return;
    }

    while (archive_read_next_header(arch, &entry) == ARCHIVE_OK) {
        GtkTreeIter iter;
        gtk_list_store_append(listStore, &iter);
        
        const char *pathname=archive_entry_pathname(entry);
        goffset size=archive_entry_size(entry);
        time_t mtime=archive_entry_mtime(entry);
        
        // Format size string
        char *size_str=format_size(size);
        
        // Format date string
        char date_str[64];
        struct tm *tm=localtime(&mtime);
        strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", tm);

        gtk_list_store_set(listStore, &iter,
                          0, pathname,    // Name
                          1, size_str,    // Size
                          2, date_str,    // Date Modified
                          -1);
        
        g_free(size_str);
    }

    archive_read_free(arch);
    gtk_widget_set_sensitive(extractButton, TRUE);
}

void on_open_archive(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_file_chooser_dialog_new("Choose an archive",
                                                   GTK_WINDOW(data),
                                                   GTK_FILE_CHOOSER_ACTION_OPEN,
                                                   "Cancel", GTK_RESPONSE_CANCEL,
                                                   "Open", GTK_RESPONSE_ACCEPT,
                                                   NULL);

    GtkFileFilter *filter=gtk_file_filter_new();
    gtk_file_filter_add_pattern(filter, "*.zip");
    gtk_file_filter_set_name(filter, "ZIP files (*.zip)");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        g_free(archivePath);
        archivePath=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        load_archive_contents(archivePath, GTK_WINDOW(data));
    }
    gtk_widget_destroy(dialog);
}

void extract_file(const gchar *entry_name) {
    struct archive *arch=archive_read_new();
    struct archive *ext=archive_write_disk_new();
    struct archive_entry *entry;
    
    archive_read_support_format_all(arch);
    archive_read_support_filter_all(arch);
    archive_write_disk_set_options(ext, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM);

    if (archive_read_open_filename(arch, archivePath, 10240) != ARCHIVE_OK) {
        g_print("Failed to open archive: %s\n", archive_error_string(arch));
        archive_read_free(arch);
        archive_write_free(ext);
        return;
    }

    while (archive_read_next_header(arch, &entry) == ARCHIVE_OK) {
        const char *path=archive_entry_pathname(entry);
        if (g_strcmp0(path, entry_name) == 0) {
            archive_entry_set_pathname(entry, path);
            if (archive_write_header(ext, entry) == ARCHIVE_OK) {
                const void *buff;
                size_t size;
                int64_t offset;
                while (archive_read_data_block(arch, &buff, &size, &offset) == ARCHIVE_OK)
                    archive_write_data_block(ext, buff, size, offset);
            }
            break;
        }
    }

    archive_read_free(arch);
    archive_write_free(ext);
}

void on_extract_all(GtkWidget *widget, gpointer data) {
    if (!archivePath) {
        show_error(GTK_WINDOW(data), "No archive opened!");
        return;
    }

    struct archive *arch=archive_read_new();
    struct archive *ext=archive_write_disk_new();
    struct archive_entry *entry;
    
    archive_read_support_format_all(arch);
    archive_read_support_filter_all(arch);
    archive_write_disk_set_options(ext, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM);

    if (archive_read_open_filename(arch, archivePath, 10240) != ARCHIVE_OK) {
        show_error(GTK_WINDOW(data), archive_error_string(arch));
        archive_read_free(arch);
        archive_write_free(ext);
        return;
    }

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.0);

    while (archive_read_next_header(arch, &entry) == ARCHIVE_OK) {
        if (archive_write_header(ext, entry) == ARCHIVE_OK) {
            const void *buff;
            size_t size;
            int64_t offset;
            while (archive_read_data_block(arch, &buff, &size, &offset) == ARCHIVE_OK) {
                archive_write_data_block(ext, buff, size, offset);
            }
        }
        // Update progress bar (simplified version)
        gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progressBar));
        while (gtk_events_pending()) gtk_main_iteration();
    }

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 1.0);
    archive_read_free(arch);
    archive_write_free(ext);
    show_message(GTK_WINDOW(data), "All files extracted successfully!", GTK_MESSAGE_INFO);
}

void on_tree_selection_changed(GtkTreeSelection *selection, gpointer data) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    
    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *file_path;
        gtk_tree_model_get(model, &iter, 0, &file_path, -1);
        // Enable extract button for single file if needed
        g_free(file_path);
    }
}

void setup_ui(GtkWidget *window) {
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *scrolled_window=gtk_scrolled_window_new(NULL, NULL);

    // Buttons
    GtkWidget *openButton=gtk_button_new_with_label("Open Archive");
    extractButton=gtk_button_new_with_label("Extract All");
    GtkWidget *aboutButton=gtk_button_new_with_label("About");

    gtk_widget_set_sensitive(extractButton, FALSE);

    g_signal_connect(openButton, "clicked", G_CALLBACK(on_open_archive), window);
    g_signal_connect(extractButton, "clicked", G_CALLBACK(on_extract_all), window);
    g_signal_connect(aboutButton, "clicked", G_CALLBACK(show_about_dialog), window);

    // Progress bar
    progressBar=gtk_progress_bar_new();
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progressBar), TRUE);

    // Tree view setup
    tree=gtk_tree_view_new();
    listStore=gtk_list_store_new(3, 
                                  G_TYPE_STRING,  // Name
                                  G_TYPE_STRING,  // Size
                                  G_TYPE_STRING); // Date Modified

    gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(listStore));

    // Create columns
    GtkCellRenderer *renderer=gtk_cell_renderer_text_new();
    GtkTreeViewColumn *nameColumn=gtk_tree_view_column_new_with_attributes(
        "Name", renderer, "text", 0, NULL);
    gtk_tree_view_column_set_resizable(nameColumn, TRUE);
    gtk_tree_view_column_set_min_width(nameColumn, 200);

    GtkTreeViewColumn *sizeColumn=gtk_tree_view_column_new_with_attributes(
        "Size", renderer, "text", 1, NULL);
    gtk_tree_view_column_set_resizable(sizeColumn, TRUE);

    GtkTreeViewColumn *dateColumn=gtk_tree_view_column_new_with_attributes(
        "Date Modified", renderer, "text", 2, NULL);
    gtk_tree_view_column_set_resizable(dateColumn, TRUE);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), nameColumn);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), sizeColumn);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), dateColumn);

    // Selection
    GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(selection, "changed", G_CALLBACK(on_tree_selection_changed), NULL);

    // Pack everything
    gtk_box_pack_start(GTK_BOX(hbox), openButton, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), extractButton, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox), aboutButton, FALSE, FALSE, 5);

    gtk_container_add(GTK_CONTAINER(scrolled_window), tree);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);

    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), progressBar, FALSE, FALSE, 5);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Archive Viewer");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    setup_ui(window);
    gtk_widget_show_all(window);

    gtk_main();
    return 0;
}

