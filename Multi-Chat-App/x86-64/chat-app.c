#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <json-c/json.h>

#define BUF_SIZE 1024
#define MAX_CLIENTS 10
#define DEFAULT_PORT 9595
#define SETTINGS_FILE "settings.json"

GtkWidget *entry_message;
GtkWidget *textview_chat;
GtkWidget *window;
GtkWidget *room_list;
int server_sock, client_sock;
struct sockaddr_in server_addr, client_addr;
socklen_t client_addr_size=sizeof(client_addr);
char room[50]="Lobby";
char username[50]="User";
int server_port=DEFAULT_PORT;
PangoFontDescription *font_description;
GdkRGBA font_color;
int private_messages_enabled=0; // 0 for disabled, 1 for enabled
pthread_t tid;

// Function prototypes
void *client_listener(void *arg);
void send_message(const char *message);
void append_to_chat(const char *message, const char *sender);
void on_send_message(GtkWidget *widget, gpointer data);
void show_about(GtkWidget *widget, gpointer data);
void settings_menu(GtkWidget *widget, gpointer data);
void create_room(GtkWidget *widget, gpointer data);
void join_room(const char *room_name);
void update_room_list();
void connect_as_client(int port);
void load_settings();
void save_settings();
void apply_font_settings();

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    load_settings();

    // Create main window
    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Chat Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create horizontal box for layout
    GtkWidget *hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_container_add(GTK_CONTAINER(window), hbox);

    // Create vertical box for room list
    room_list=gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(hbox), room_list, FALSE, TRUE, 0);

    // Create vertical box for chat
    GtkWidget *vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, TRUE, 0);

    // Create text view for chat
    textview_chat=gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textview_chat), FALSE);
    apply_font_settings(); // Apply the font settings
    gtk_box_pack_start(GTK_BOX(vbox), textview_chat, TRUE, TRUE, 0);

    // Create entry for message input
    entry_message=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry_message, FALSE, FALSE, 0);

    // Create send button
    GtkWidget *send_button=gtk_button_new_with_label("Send");
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_message), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, FALSE, 0);

    // Create new room button
    GtkWidget *new_room_button=gtk_button_new_with_label("Create Room");
    g_signal_connect(new_room_button, "clicked", G_CALLBACK(create_room), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), new_room_button, FALSE, FALSE, 0);

    // Create menu bar
    GtkWidget *menubar=gtk_menu_bar_new();
    GtkWidget *file_menu=gtk_menu_new();
    GtkWidget *file_item=gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), file_menu);
    GtkWidget *about_item=gtk_menu_item_new_with_label("About");
    g_signal_connect(about_item, "activate", G_CALLBACK(show_about), NULL);
    GtkWidget *settings_item=gtk_menu_item_new_with_label("Settings");
    g_signal_connect(settings_item, "activate", G_CALLBACK(settings_menu), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), about_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), settings_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), file_item);
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);

    gtk_widget_show_all(window);

    // Create socket
    server_sock=socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=INADDR_ANY; // Accept connections from any IP
    server_addr.sin_port=htons(server_port); // Use user-defined port

    // Bind socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        printf("Attempting to connect as a client to port %d...\n", server_port);
        connect_as_client(server_port);
    } else {
        // Start listening
        if (listen(server_sock, MAX_CLIENTS) < 0) {
            perror("Listen failed");
            close(server_sock);
            exit(EXIT_FAILURE);
        }
        printf("Server is listening on port %d...\n", server_port);

        pthread_create(&tid, NULL, client_listener, NULL);
    }

    gtk_main();
    close(server_sock);
    return 0;
}

void *client_listener(void *arg) {
    while (1) {
        client_sock=accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected: %d\n", client_sock);

        char buf[BUF_SIZE];
        int len;
        while ((len=read(client_sock, buf, BUF_SIZE)) > 0) {
            buf[len]='\0';
            append_to_chat(buf, "Client"); // Change to actual sender later
        }

        close(client_sock);
        printf("Client disconnected: %d\n", client_sock);
    }
    return NULL;
}

void append_to_chat(const char *message, const char *sender) {
    // Retrieve the chat buffer
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(textview_chat));

    // Create a new message string
    char full_message[256];
    snprintf(full_message, sizeof(full_message), "%s: %s\n", sender, message);

    // Get the end of the buffer
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);

    // Insert new text at the end of the buffer
    gtk_text_buffer_insert(buffer, &end, full_message, -1);

    // Scroll to the end of the text view
    gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(textview_chat), &end, 0.0, FALSE, 0.0, 0.0);
}

void on_send_message(GtkWidget *widget, gpointer data) {
    const char *message=gtk_entry_get_text(GTK_ENTRY(entry_message));
    if (strlen(message) > 0) {
        send_message(message);
        append_to_chat(message, username); // Display sent message in chat
        gtk_entry_set_text(GTK_ENTRY(entry_message), ""); // Clear the entry
    }
}

void send_message(const char *message) {
    if (client_sock > 0) {
        write(client_sock, message, strlen(message)); // Send message to the server
    }
}

void show_about(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                               "Chat Application\nAuthor: Jay Mee @ J~Net 2024");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void create_room(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_dialog_new_with_buttons("Create Room",
                                                    GTK_WINDOW(window),
                                                    GTK_DIALOG_MODAL,
                                                    "_OK", GTK_RESPONSE_OK,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);

    GtkWidget *content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *room_entry=gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(room_entry), "Enter room name...");
    gtk_box_pack_start(GTK_BOX(content_area), room_entry, FALSE, FALSE, 0);
    gtk_widget_show_all(dialog);

    int response=gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        const char *room_name=gtk_entry_get_text(GTK_ENTRY(room_entry));
        join_room(room_name);
    }

    gtk_widget_destroy(dialog);
}

void join_room(const char *room_name) {
    // Change the room and update the UI
    snprintf(room, sizeof(room), "%s", room_name);
    update_room_list();
}

void update_room_list() {
    gtk_list_box_select_all(GTK_LIST_BOX(room_list)); // Clear current selection
    gtk_list_box_insert(GTK_LIST_BOX(room_list), gtk_label_new(room), -1);
}

void connect_as_client(int port) {
    // Connect to the server as a client
    client_sock=socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("Client socket creation failed");
        exit(EXIT_FAILURE);
    }

    client_addr.sin_family=AF_INET;
    client_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
    client_addr.sin_port=htons(port);

    if (connect(client_sock, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
        perror("Client connection failed");
        close(client_sock);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server on port %d\n", port);
}

void load_settings() {
    FILE *file=fopen(SETTINGS_FILE, "r");
    if (file) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file)) {
            json_object *json_obj=json_tokener_parse(buffer);
            if (json_obj) {
                json_object *username_obj, *room_obj, *port_obj, *font_obj, *color_obj;
                if (json_object_object_get_ex(json_obj, "username", &username_obj))
                    strcpy(username, json_object_get_string(username_obj));
                if (json_object_object_get_ex(json_obj, "room", &room_obj))
                    strcpy(room, json_object_get_string(room_obj));
                if (json_object_object_get_ex(json_obj, "port", &port_obj))
                    server_port=json_object_get_int(port_obj);
                if (json_object_object_get_ex(json_obj, "font", &font_obj))
                    font_description=pango_font_description_from_string(json_object_get_string(font_obj));
                if (json_object_object_get_ex(json_obj, "color", &color_obj)) {
                    const char *color_string=json_object_get_string(color_obj);
                    gdk_rgba_parse(&font_color, color_string);
                }
                json_object_put(json_obj); // Free memory
            }
        }
        fclose(file);
    }
}

void save_settings() {
    FILE *file=fopen(SETTINGS_FILE, "w");
    if (file) {
        fprintf(file, "{\n");
        fprintf(file, "  \"username\": \"%s\",\n", username);
        fprintf(file, "  \"room\": \"%s\",\n", room);
        fprintf(file, "  \"port\": %d,\n", server_port);
        fprintf(file, "  \"font\": \"%s\",\n", pango_font_description_to_string(font_description));
        fprintf(file, "  \"color\": \"%s\"\n", gdk_rgba_to_string(&font_color));
        fprintf(file, "}\n");
        fclose(file);
    }
}

void settings_menu(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_dialog_new_with_buttons("Settings",
                                                    GTK_WINDOW(window),
                                                    GTK_DIALOG_MODAL,
                                                    "_Save", GTK_RESPONSE_OK,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);

    GtkWidget *content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *username_entry=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(username_entry), username);
    gtk_box_pack_start(GTK_BOX(content_area), username_entry, FALSE, FALSE, 0);

    GtkWidget *port_entry=gtk_entry_new();
    char port_buffer[10];
    snprintf(port_buffer, sizeof(port_buffer), "%d", server_port);
    gtk_entry_set_text(GTK_ENTRY(port_entry), port_buffer);
    gtk_box_pack_start(GTK_BOX(content_area), port_entry, FALSE, FALSE, 0);

    gtk_widget_show_all(dialog);

    int response=gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        strcpy(username, gtk_entry_get_text(GTK_ENTRY(username_entry)));
        server_port=atoi(gtk_entry_get_text(GTK_ENTRY(port_entry)));
        save_settings();
    }

    gtk_widget_destroy(dialog);
}

void apply_font_settings() {
    // Create a CSS provider
    GtkCssProvider *css_provider=gtk_css_provider_new();
    
    // Construct the CSS string
    char css_string[256];
    snprintf(css_string, sizeof(css_string),
             "textview_chat { font-family: '%s'; font-size: 12px; color: rgba(%d, %d, %d, %f); }",
             pango_font_description_get_family(font_description),
             (int)(font_color.red * 255), (int)(font_color.green * 255),
             (int)(font_color.blue * 255), font_color.alpha);
    
    // Load the CSS string
    gtk_css_provider_load_from_data(css_provider, css_string, -1, NULL);
    
    // Apply the CSS provider to the text view
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(css_provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    // Clean up
    g_object_unref(css_provider);
}

