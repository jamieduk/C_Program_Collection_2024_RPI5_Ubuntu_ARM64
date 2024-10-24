#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define DEFAULT_PORT 5555

// Global variables for sockets and GUI elements
int server_socket, client_socket;
struct sockaddr_in serv_addr, cli_addr;
GtkWidget *text_entry, *text_display, *ip_entry, *port_entry, *status_label;
char buffer[1024] = {0};
pthread_t server_thread;

gint update_label(gpointer data) {
    gtk_label_set_text(GTK_LABEL(status_label), (const char *)data);
    g_free(data);
    return G_SOURCE_REMOVE; // Indicate that the function should be removed from the idle queue
}

void update_status(const char *message) {
    char *msg_copy = g_strdup(message);
    g_idle_add(update_label, msg_copy);
}

void display_received_message(const char *message) {
    GtkTextBuffer *buffer;
    GtkTextIter end;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_display));
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, message, -1);
    gtk_text_buffer_insert(buffer, &end, "\n", -1);
}

void *start_server(void *arg) {
    int new_socket;
    socklen_t addrlen = sizeof(cli_addr);
    char client_message[1024] = {0};

    // Accept and handle clients in a loop
    while (1) {
        if ((new_socket = accept(server_socket, (struct sockaddr *)&cli_addr, &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        update_status("Client connected.");
        read(new_socket, client_message, 1024);
        display_received_message(client_message);
        
        // Send confirmation message to the client
        char *response_message = "Message received by server!";
        send(new_socket, response_message, strlen(response_message), 0);
        
        // Close the client socket
        close(new_socket);
    }
    return NULL;
}

void setup_server_socket() {
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(DEFAULT_PORT);

    if (bind(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    update_status("Server started.");
    pthread_create(&server_thread, NULL, start_server, NULL);
}

void connect_to_server() {
    const char *ip_address = gtk_entry_get_text(GTK_ENTRY(ip_entry));
    const char *port_text = gtk_entry_get_text(GTK_ENTRY(port_entry));
    int port = atoi(port_text); // Get port from user input

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error\n");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); // Use user-defined port

    if (inet_pton(AF_INET, ip_address, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported\n");
        exit(EXIT_FAILURE);
    }

    if (connect(client_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        exit(EXIT_FAILURE);
    }

    update_status("Connected to server.");
}

void send_message(GtkWidget *widget, gpointer data) {
    const char *message = gtk_entry_get_text(GTK_ENTRY(text_entry));

    if (client_socket != 0) {
        send(client_socket, message, strlen(message), 0);
        update_status("Message sent to server.");

        // Allow receiving reply after sending a message
        read(client_socket, buffer, 1024);
        display_received_message(buffer);
        update_status("Reply received from server.");
    } else {
        update_status("Error: Not connected to server.");
    }
}

void about_popup() {
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                               "About: Combined Server-Client Application\nAuthor: Jay Mee");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void gui_setup(int argc, char *argv[]) {
    GtkWidget *window, *send_button, *server_button, *client_button, *about_button, *vbox, *hbox;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Server-Client Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 5);

    server_button = gtk_button_new_with_label("Start Server");
    gtk_box_pack_start(GTK_BOX(hbox), server_button, TRUE, TRUE, 5);
    g_signal_connect(server_button, "clicked", G_CALLBACK(setup_server_socket), NULL);

    client_button = gtk_button_new_with_label("Connect as Client");
    gtk_box_pack_start(GTK_BOX(hbox), client_button, TRUE, TRUE, 5);
    g_signal_connect(client_button, "clicked", G_CALLBACK(connect_to_server), NULL);

    // IP Entry
    ip_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(ip_entry), "127.0.0.1"); // Default IP
    gtk_box_pack_start(GTK_BOX(vbox), ip_entry, FALSE, FALSE, 5);

    // Port Entry
    port_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(port_entry), "5555"); // Default Port
    gtk_box_pack_start(GTK_BOX(vbox), port_entry, FALSE, FALSE, 5);

    text_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), text_entry, FALSE, FALSE, 5);

    send_button = gtk_button_new_with_label("Send Message");
    gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, FALSE, 5);
    g_signal_connect(send_button, "clicked", G_CALLBACK(send_message), NULL);

    text_display = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_display), FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), text_display, TRUE, TRUE, 5);

    about_button = gtk_button_new_with_label("About");
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 5);
    g_signal_connect(about_button, "clicked", G_CALLBACK(about_popup), NULL);

    status_label = gtk_label_new("Status: Waiting...");
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 5);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}

int main(int argc, char *argv[]) {
    gui_setup(argc, argv);
    return 0;
}

