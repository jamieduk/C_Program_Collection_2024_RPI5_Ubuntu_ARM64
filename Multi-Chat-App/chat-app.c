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
    server_addr.sin_port=htons(DEFAULT_PORT); // Use default port

    // Bind socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        printf("Attempting to connect as a client to port %d...\n", DEFAULT_PORT);
        connect_as_client(DEFAULT_PORT);
    } else {
        // Start listening
        if (listen(server_sock, MAX_CLIENTS) < 0) {
            perror("Listen failed");
            close(server_sock);
            exit(EXIT_FAILURE);
        }
        printf("Server is listening on port %d...\n", DEFAULT_PORT);

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

void settings_menu(GtkWidget *widget, gpointer data) {
    // Create the dialog with a title and modal
    GtkWidget *dialog=gtk_dialog_new_with_buttons("Settings",
                                                    GTK_WINDOW(window),
                                                    GTK_DIALOG_MODAL,
                                                    "_OK", GTK_RESPONSE_OK,
                                                    "_Cancel", GTK_RESPONSE_CANCEL,
                                                    NULL);

    GtkWidget *content_area=gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    // Create labels and entries for various settings
    GtkWidget *username_label=gtk_label_new("Username:");
    GtkWidget *username_entry=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(username_entry), username);

    gtk_box_pack_start(GTK_BOX(content_area), username_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(content_area), username_entry, FALSE, FALSE, 0);

    gtk_widget_show_all(dialog);

    // Run dialog and wait for user response
    int response=gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_OK) {
        // Save settings
        strncpy(username, gtk_entry_get_text(GTK_ENTRY(username_entry)), sizeof(username));
        save_settings();
    }

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
    strncpy(room, room_name, sizeof(room));
    update_room_list();
}

void update_room_list() {
    // Update room list view
    gtk_list_box_prepend(GTK_LIST_BOX(room_list), gtk_label_new(room));
    gtk_widget_show_all(room_list);
}

void connect_as_client(int port) {
    // Connect to the server
    client_sock=socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(port);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // Use loopback address for local testing

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server on port %d\n", port);
}

void load_settings() {
    // Load settings from the JSON file
    FILE *file=fopen(SETTINGS_FILE, "r");
    if (file) {
        char buffer[256];
        fread(buffer, 1, sizeof(buffer), file);
        fclose(file);

        struct json_object *parsed_json=json_tokener_parse(buffer);
        struct json_object *json_username;
        json_object_object_get_ex(parsed_json, "username", &json_username);
        strcpy(username, json_object_get_string(json_username));
        json_object_put(parsed_json);
    }
}

void save_settings() {
    // Save settings to the JSON file
    struct json_object *json_object=json_object_new_object();
    json_object_object_add(json_object, "username", json_object_new_string(username));

    FILE *file=fopen(SETTINGS_FILE, "w");
    if (file) {
        const char *json_str=json_object_to_json_string(json_object);
        fprintf(file, "%s", json_str);
        fclose(file);
    }
    json_object_put(json_object);
}

