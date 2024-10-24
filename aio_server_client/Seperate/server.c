// Made By Jay @ J~Net 2024
//
//
// server.c
// Made By Jay @ J~Net 2024
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h> 

#define PORT 8080
#define BUFFER_SIZE 1024

int server_fd, new_socket;
char buffer[BUFFER_SIZE];

void start_server(GtkWidget *widget, gpointer data) {
    int opt=1;
    struct sockaddr_in address;
    int addrlen=sizeof(address);

    printf("Starting Server ....\n");
    if ((server_fd=socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed!");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family=AF_INET;
    address.sin_addr.s_addr=INADDR_ANY;
    address.sin_port=htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    new_socket=accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    while (1) {
        int valread=read(new_socket, buffer, BUFFER_SIZE);
        if (valread <= 0) break;
        printf("Received: %s\n", buffer);
        send(new_socket, buffer, valread, 0);
        memset(buffer, 0, BUFFER_SIZE);
    }
    close(new_socket);
    close(server_fd);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *button=gtk_button_new_with_label("Start Server");

    g_signal_connect(button, "clicked", G_CALLBACK(start_server), NULL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_container_add(GTK_CONTAINER(window), button);
    gtk_widget_set_size_request(window, 300, 200);
    gtk_window_set_title(GTK_WINDOW(window), "Server GUI");
    gtk_widget_show_all(window);
    
    gtk_main();

    return 0;
}

