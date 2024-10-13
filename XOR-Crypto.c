#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// gcc -o XOR-Crypto XOR-Crypto.c $(pkg-config --cflags --libs gtk+-3.0)

#define KEY_LENGTH 12000  // Define the key length

char XORkey[KEY_LENGTH];  // Key will be loaded or generated

void encryptDecrypt(const char *inputFilename, const char *outputFilename);
void about_dialog(GtkWidget *widget, gpointer window);
void on_encrypt_clicked(GtkWidget *widget, gpointer data);
void on_decrypt_clicked(GtkWidget *widget, gpointer data);
void on_generate_key_clicked(GtkWidget *widget, gpointer data);
void on_load_key_clicked(GtkWidget *widget, gpointer data);

GtkWidget *text_view, *window;

// Generate random key and save to keyfile
void generateKey() {
    FILE *keyFile=fopen("keyfile.txt", "w");
    if (keyFile == NULL) {
        printf("Error opening key file!\n");
        return;
    }
    srand(time(0));
    for (int i=0; i < KEY_LENGTH; i++) {
        XORkey[i]='A' + rand() % 26;  // Generate random uppercase letters
        fputc(XORkey[i], keyFile);
    }
    fclose(keyFile);
    printf("Key generated and saved to keyfile.txt\n");
}

// Load key from file
void loadKeyFromFile(const char *filename) {
    FILE *keyFile=fopen(filename, "r");
    if (keyFile == NULL) {
        GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(window),
                                GTK_DIALOG_DESTROY_WITH_PARENT,
                                GTK_MESSAGE_ERROR,
                                GTK_BUTTONS_OK,
                                "Error: Key file not found!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    int keyLength=0;
    for (int i=0; i < KEY_LENGTH; i++) {
        int ch=fgetc(keyFile);
        if (ch == EOF) {
            printf("Error: Key file is shorter than expected\n");
            break;
        }
        XORkey[i]=(char)ch;
        keyLength++;
    }
    fclose(keyFile);
    printf("Key loaded from %s, Length: %d\n", filename, keyLength);
}

// Encrypt/Decrypt function
void encryptDecrypt(const char *inputFilename, const char *outputFilename) {
    FILE *fin=fopen(inputFilename, "r");
    FILE *fout=fopen(outputFilename, "w");

    if (fin == NULL || fout == NULL) {
        printf("Error opening files!\n");
        return;
    }

    int ch;
    int i=0;

    // Read each character from the input file and XOR with key
    while ((ch=fgetc(fin)) != EOF) {
        char encryptedChar=(char)(ch ^ XORkey[i % KEY_LENGTH]);
        fputc(encryptedChar, fout);
        i++;
    }

    fclose(fin);
    fclose(fout);

    printf("Encryption/Decryption completed. Check %s.\n", outputFilename);
}

// About Dialog function
void about_dialog(GtkWidget *widget, gpointer window) {
    GtkWidget *dialog;
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "XOR Encrypt/Decrypt GUI\n"
                                    "Author: Your Name\n"
                                    "This program uses XOR encryption for basic security.\n"
                                    "Keys can be generated or loaded from a file.");
    gtk_window_set_title(GTK_WINDOW(dialog), "About");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// Encrypt Button Clicked
void on_encrypt_clicked(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkTextIter start, end;
    gchar *input_text;
    
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    input_text=gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Save input to temp.txt
    FILE *tempFile=fopen("temp.txt", "w");
    fprintf(tempFile, "%s", input_text);
    fclose(tempFile);

    // Encrypt the input text
    encryptDecrypt("temp.txt", "encrypted.txt");

    // Free allocated memory
    g_free(input_text);
}

// Decrypt Button Clicked
void on_decrypt_clicked(GtkWidget *widget, gpointer data) {
    // Decrypt the file "encrypted.txt" and save to "decrypted.txt"
    encryptDecrypt("encrypted.txt", "decrypted.txt");

    // Load decrypted content and display in text view
    FILE *decryptedFile=fopen("decrypted.txt", "r");
    if (decryptedFile == NULL) {
        printf("Error opening decrypted.txt\n");
        return;
    }

    char *decryptedText=NULL;
    size_t len=0;
    fseek(decryptedFile, 0, SEEK_END);
    len=ftell(decryptedFile);
    rewind(decryptedFile);

    decryptedText=malloc(len + 1);
    fread(decryptedText, 1, len, decryptedFile);
    decryptedText[len]='\0';

    fclose(decryptedFile);

    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buffer, decryptedText, -1);

    free(decryptedText);
}

// Generate Key Button Clicked
void on_generate_key_clicked(GtkWidget *widget, gpointer data) {
    generateKey();
    printf("Key generated and saved to keyfile.txt\n");
}

// Load Key Button Clicked - with file chooser dialog
void on_load_key_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    dialog=gtk_file_chooser_dialog_new("Open Key File",
                                         GTK_WINDOW(window),
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancel", GTK_RESPONSE_CANCEL,
                                         "_Open", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser=GTK_FILE_CHOOSER(dialog);
        filename=gtk_file_chooser_get_filename(chooser);
        loadKeyFromFile(filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Main Function
int main(int argc, char *argv[]) {
    GtkWidget *grid;
    GtkWidget *encrypt_button, *decrypt_button, *about_button, *generate_key_button, *load_key_button;
    GtkWidget *scrolled_window;

    gtk_init(&argc, &argv);

    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "XOR Encrypt/Decrypt");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid=gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Text View inside a Scrolled Window
    scrolled_window=gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_widget_set_hexpand(scrolled_window, TRUE);
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 0, 5, 1);

    text_view=gtk_text_view_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    // Buttons
    encrypt_button=gtk_button_new_with_label("Encrypt");
    gtk_grid_attach(GTK_GRID(grid), encrypt_button, 0, 1, 1, 1);
    g_signal_connect(encrypt_button, "clicked", G_CALLBACK(on_encrypt_clicked), NULL);

    decrypt_button=gtk_button_new_with_label("Decrypt");
    gtk_grid_attach(GTK_GRID(grid), decrypt_button, 1, 1, 1, 1);
    g_signal_connect(decrypt_button, "clicked", G_CALLBACK(on_decrypt_clicked), NULL);

    about_button=gtk_button_new_with_label("About");
    gtk_grid_attach(GTK_GRID(grid), about_button, 2, 1, 1, 1);
    g_signal_connect(about_button, "clicked", G_CALLBACK(about_dialog), window);

    generate_key_button=gtk_button_new_with_label("Generate Key");
    gtk_grid_attach(GTK_GRID(grid), generate_key_button, 3, 1, 1, 1);
    g_signal_connect(generate_key_button, "clicked", G_CALLBACK(on_generate_key_clicked), NULL);

    load_key_button=gtk_button_new_with_label("Load Key");
    gtk_grid_attach(GTK_GRID(grid), load_key_button, 4, 1, 1, 1);
    g_signal_connect(load_key_button, "clicked", G_CALLBACK(on_load_key_clicked), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

