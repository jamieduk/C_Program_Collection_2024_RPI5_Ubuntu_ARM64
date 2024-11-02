// Made By Jay @ J~Net 2024
//
// gcc client.c -o client `pkg-config --cflags --libs gtk+-3.0` -Wall
//
// client_gui.c
// Made By Jay @ J~Net 2024
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int sock;
char buffer[BUFFER_SIZE];

void connect_to_server(GtkWidget *widget, gpointer data) {
    struct sockaddr_in serv_addr;

    if ((sock=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nSocket creation error\n");
        return;
    }

    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed\n");
        return;
    }

    const char *message=gtk_entry_get_text(GTK_ENTRY(data));
    send(sock, message, strlen(message), 0);

    int valread=read(sock, buffer, BUFFER_SIZE);
    buffer[valread]='\0';
    printf("Echo from server: %s\n", buffer);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *entry=gtk_entry_new();
    GtkWidget *button=gtk_button_new_with_label("Send Message");

    g_signal_connect(button, "clicked", G_CALLBACK(connect_to_server), entry);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *box=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_pack_start(GTK_BOX(box), entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box), button, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    gtk_widget_set_size_request(window, 300, 200);
    gtk_window_set_title(GTK_WINDOW(window), "Client GUI");
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

