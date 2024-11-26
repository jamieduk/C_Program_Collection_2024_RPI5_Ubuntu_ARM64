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

// Add these global variables at the top with other globals
static gboolean replace_all=FALSE;
static gboolean skip_all=FALSE;

// Add this helper function to handle the overwrite dialog
GtkResponseType show_overwrite_dialog(GtkWindow *parent, const char *filename) {
    GtkWidget *dialog=gtk_dialog_new_with_buttons("File Exists",
                                                   parent,
                                                   GTK_DIALOG_MODAL,
                                                   "Replace", GTK_RESPONSE_YES,
                                                   "Replace All", GTK_RESPONSE_APPLY,
                                                   "Skip", GTK_RESPONSE_NO,
                                                   "Skip All", GTK_RESPONSE_REJECT,
                                                   "Cancel", GTK_RESPONSE_CANCEL,
                                                   NULL);

    GtkWidget *content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    char *message=g_strdup_printf("The file '%s' already exists.\nDo you want to replace it?", filename);
    GtkWidget *label=gtk_label_new(message);
    g_free(message);

    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);

    GtkResponseType response=gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return response;
}


// Forward declarations
void load_archive_contents(const gchar *path, GtkWindow *parent);
void show_message(GtkWindow *parent, const gchar *message, GtkMessageType type);
void show_error(GtkWindow *parent, const gchar *message);
void show_about_dialog(GtkWidget *widget, gpointer data);
char* format_size(goffset size);
void on_open_archive(GtkWidget *widget, gpointer data);
void extract_file(const gchar *entry_name);
void on_extract_all(GtkWidget *widget, gpointer data);
void on_extract_to(GtkWidget *widget, gpointer data);
void on_tree_selection_changed(GtkTreeSelection *selection, gpointer data);
void setup_ui(GtkWidget *window);
void extract_all_to(const char *extract_path, GtkWindow *parent);
char* get_directory_path(const char *file_path);

static GtkWidget *tree;
static GtkListStore *listStore;
static gchar *archivePath=NULL;
static GtkWidget *progressBar;
static GtkWidget *extractButton;
static GtkWidget *extractToButton;

char* get_directory_path(const char *file_path) {
    char *last_slash=strrchr(file_path, '/');
    if (last_slash) {
        size_t dir_length=last_slash - file_path + 1;
        char *dir_path=g_strndup(file_path, dir_length);
        return dir_path;
    }
    return g_strdup("./");
}

static void open_archive_from_path(const char *path, GtkWindow *parent) {
    if (path) {
        g_free(archivePath);
        archivePath=g_strdup(path);
        load_archive_contents(archivePath, parent);
    }
}

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
        
        char *size_str=format_size(size);
        char date_str[64];
        struct tm *tm=localtime(&mtime);
        strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", tm);

        gtk_list_store_set(listStore, &iter,
                          0, pathname,
                          1, size_str,
                          2, date_str,
                          -1);
        
        g_free(size_str);
    }

    archive_read_free(arch);
    gtk_widget_set_sensitive(extractButton, TRUE);
    gtk_widget_set_sensitive(extractToButton, TRUE);
}


void extract_all_to(const char *extract_path, GtkWindow *parent) {
    if (!archivePath) {
        show_error(parent, "No archive opened!");
        return;
    }

    // Reset global flags
    replace_all=FALSE;
    skip_all=FALSE;

    struct archive *arch=archive_read_new();
    struct archive *ext=archive_write_disk_new();
    struct archive_entry *entry;
    
    archive_read_support_format_all(arch);
    archive_read_support_filter_all(arch);
    archive_write_disk_set_options(ext, ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_PERM | 
                                      ARCHIVE_EXTRACT_ACL | ARCHIVE_EXTRACT_FFLAGS);

    if (archive_read_open_filename(arch, archivePath, 10240) != ARCHIVE_OK) {
        show_error(parent, archive_error_string(arch));
        archive_read_free(arch);
        archive_write_free(ext);
        return;
    }

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.0);
    gboolean extraction_cancelled=FALSE;

    while (archive_read_next_header(arch, &entry) == ARCHIVE_OK && !extraction_cancelled) {
        const char *current_file=archive_entry_pathname(entry);
        char *full_path=g_build_filename(extract_path, current_file, NULL);
        
        // Check if file exists
        if (g_file_test(full_path, G_FILE_TEST_EXISTS) && !replace_all && !skip_all) {
            GtkResponseType response=show_overwrite_dialog(parent, current_file);
            
            switch (response) {
                case GTK_RESPONSE_YES:
                    // Replace this file
                    break;
                case GTK_RESPONSE_APPLY:
                    replace_all=TRUE;
                    break;
                case GTK_RESPONSE_NO:
                    g_free(full_path);
                    continue;
                case GTK_RESPONSE_REJECT:
                    skip_all=TRUE;
                    g_free(full_path);
                    continue;
                case GTK_RESPONSE_CANCEL:
                    extraction_cancelled=TRUE;
                    g_free(full_path);
                    continue;
                default:
                    g_free(full_path);
                    continue;
            }
        }

        if (skip_all) {
            g_free(full_path);
            continue;
        }

        archive_entry_set_pathname(entry, full_path);
        
        if (archive_write_header(ext, entry) == ARCHIVE_OK) {
            const void *buff;
            size_t size;
            int64_t offset;
            
            while (archive_read_data_block(arch, &buff, &size, &offset) == ARCHIVE_OK) {
                if (archive_write_data_block(ext, buff, size, offset) != ARCHIVE_OK) {
                    show_error(parent, archive_error_string(ext));
                    break;
                }
            }
        }
        
        g_free(full_path);
        gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progressBar));
        while (gtk_events_pending()) gtk_main_iteration();
    }

    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 1.0);
    archive_read_free(arch);
    archive_write_free(ext);

    if (!extraction_cancelled) {
        show_message(parent, "Files extracted successfully!", GTK_MESSAGE_INFO);
    }
}


void on_extract_to(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_file_chooser_dialog_new("Choose Extract Location",
                                                   GTK_WINDOW(data),
                                                   GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                                   "Cancel", GTK_RESPONSE_CANCEL,
                                                   "Extract", GTK_RESPONSE_ACCEPT,
                                                   NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *folder_path=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        extract_all_to(folder_path, GTK_WINDOW(data));
        g_free(folder_path);
    }
    
    gtk_widget_destroy(dialog);
}

void on_extract_all(GtkWidget *widget, gpointer data) {
    if (!archivePath) {
        show_error(GTK_WINDOW(data), "No archive opened!");
        return;
    }
    
    char *extract_path=get_directory_path(archivePath);
    extract_all_to(extract_path, GTK_WINDOW(data));
    g_free(extract_path);
}

void setup_ui(GtkWidget *window) {
    // Create main layout containers
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    GtkWidget *toolbar=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    GtkWidget *scrolled_window=gtk_scrolled_window_new(NULL, NULL);

    // Create buttons with icons
    GtkWidget *openButton=gtk_button_new_with_label("Open");
    extractButton=gtk_button_new_with_label("Extract Here");
    extractToButton=gtk_button_new_with_label("Extract To...");
    GtkWidget *aboutButton=gtk_button_new_with_label("About");

    // Set initial button states
    gtk_widget_set_sensitive(extractButton, FALSE);
    gtk_widget_set_sensitive(extractToButton, FALSE);

    // Connect signals
    g_signal_connect(openButton, "clicked", G_CALLBACK(on_open_archive), window);
    g_signal_connect(extractButton, "clicked", G_CALLBACK(on_extract_all), window);
    g_signal_connect(extractToButton, "clicked", G_CALLBACK(on_extract_to), window);
    g_signal_connect(aboutButton, "clicked", G_CALLBACK(show_about_dialog), window);

    // Progress bar
    progressBar=gtk_progress_bar_new();
    gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(progressBar), TRUE);

    // Setup tree view
    tree=gtk_tree_view_new();
    listStore=gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(tree), GTK_TREE_MODEL(listStore));

    // Create columns
    GtkCellRenderer *renderer=gtk_cell_renderer_text_new();
    GtkTreeViewColumn *nameColumn=gtk_tree_view_column_new_with_attributes(
        "Name", renderer, "text", 0, NULL);
    GtkTreeViewColumn *sizeColumn=gtk_tree_view_column_new_with_attributes(
        "Size", renderer, "text", 1, NULL);
    GtkTreeViewColumn *dateColumn=gtk_tree_view_column_new_with_attributes(
        "Date Modified", renderer, "text", 2, NULL);

    // Configure columns
    gtk_tree_view_column_set_resizable(nameColumn, TRUE);
    gtk_tree_view_column_set_resizable(sizeColumn, TRUE);
    gtk_tree_view_column_set_resizable(dateColumn, TRUE);
    gtk_tree_view_column_set_min_width(nameColumn, 200);

    // Add columns to tree view
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), nameColumn);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), sizeColumn);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), dateColumn);

    // Setup selection
    GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(selection, "changed", G_CALLBACK(on_tree_selection_changed), NULL);

    // Pack widgets
    gtk_box_pack_start(GTK_BOX(toolbar), openButton, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(toolbar), extractButton, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(toolbar), extractToButton, FALSE, FALSE, 1);
    gtk_box_pack_end(GTK_BOX(toolbar), aboutButton, FALSE, FALSE, 1);

    gtk_container_add(GTK_CONTAINER(scrolled_window), tree);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC,
                                 GTK_POLICY_AUTOMATIC);

    gtk_box_pack_start(GTK_BOX(vbox), toolbar, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), progressBar, FALSE, FALSE, 2);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_container_set_border_width(GTK_CONTAINER(window), 5);
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

void on_tree_selection_changed(GtkTreeSelection *selection, gpointer data) {
    GtkTreeIter iter;
    GtkTreeModel *model;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *file_name;
        gtk_tree_model_get(model, &iter, 0, &file_name, -1);
        g_free(file_name);
    }
}



int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Archive Viewer");
    gtk_window_set_default_size(GTK_WINDOW(window), 450, 440);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    setup_ui(window);
    gtk_widget_show_all(window);

    if (argc > 1) {
        open_archive_from_path(argv[1], GTK_WINDOW(window));
    }

    gtk_main();
    return 0;
}

