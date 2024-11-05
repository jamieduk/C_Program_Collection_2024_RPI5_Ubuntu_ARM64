// Made By Jay @ J~Net 2024
//
// gcc AES-FLTD.c -o AES-FLTD `pkg-config --cflags --libs gtk+-3.0` -lcrypto
//
//
#include <gtk/gtk.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AES_KEY_SIZE 32   // 256-bit key
#define AES_BLOCK_SIZE 16 // AES block size

unsigned char default_key[AES_KEY_SIZE]="key-here-key-here-key-here-key-h"; // exactly 32 bytes
unsigned char iv[AES_BLOCK_SIZE];

// Function Prototypes
void cli_mode();
void gui_mode();
void encrypt_file(const char *input_file, const char *output_file, const unsigned char *key);
void decrypt_file(const char *input_file, const char *output_file, const unsigned char *key);
void show_about(GtkWidget *widget, gpointer data);
void adjust_key(unsigned char *key, int key_len);

// Adjusts the key length to fit AES requirements
void adjust_key(unsigned char *key, int key_len) {
    int original_len=strlen((char *)key);
    if (original_len < key_len) {
        for (int i=original_len; i < key_len; i++) {
            key[i]=key[i % original_len];
        }
        key[key_len]='\0';
    }
}

// Encrypt function using AES
void encrypt_file(const char *input_file, const char *output_file, const unsigned char *key) {
    FILE *in_file=fopen(input_file, "rb");
    FILE *out_file=fopen(output_file, "wb");
    if (!in_file || !out_file) {
        perror("File error");
        return;
    }

    unsigned char buffer[AES_BLOCK_SIZE];
    unsigned char ciphertext[AES_BLOCK_SIZE + AES_BLOCK_SIZE];
    int bytes_read, cipher_len;
    
    EVP_CIPHER_CTX *ctx=EVP_CIPHER_CTX_new();
    RAND_bytes(iv, AES_BLOCK_SIZE);
    fwrite(iv, 1, AES_BLOCK_SIZE, out_file); // write IV to file

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    while ((bytes_read=fread(buffer, 1, AES_BLOCK_SIZE, in_file)) > 0) {
        EVP_EncryptUpdate(ctx, ciphertext, &cipher_len, buffer, bytes_read);
        fwrite(ciphertext, 1, cipher_len, out_file);
    }
    
    EVP_EncryptFinal_ex(ctx, ciphertext, &cipher_len);
    fwrite(ciphertext, 1, cipher_len, out_file);

    EVP_CIPHER_CTX_free(ctx);
    fclose(in_file);
    fclose(out_file);
}

// Decrypt function using AES
void decrypt_file(const char *input_file, const char *output_file, const unsigned char *key) {
    FILE *in_file=fopen(input_file, "rb");
    FILE *out_file=fopen(output_file, "wb");
    if (!in_file || !out_file) {
        perror("File error");
        return;
    }

    unsigned char buffer[AES_BLOCK_SIZE + AES_BLOCK_SIZE];
    unsigned char plaintext[AES_BLOCK_SIZE];
    int bytes_read, plain_len;

    fread(iv, 1, AES_BLOCK_SIZE, in_file); // read IV from file

    EVP_CIPHER_CTX *ctx=EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    while ((bytes_read=fread(buffer, 1, AES_BLOCK_SIZE + AES_BLOCK_SIZE, in_file)) > 0) {
        EVP_DecryptUpdate(ctx, plaintext, &plain_len, buffer, bytes_read);
        fwrite(plaintext, 1, plain_len, out_file);
    }

    EVP_DecryptFinal_ex(ctx, plaintext, &plain_len);
    fwrite(plaintext, 1, plain_len, out_file);

    EVP_CIPHER_CTX_free(ctx);
    fclose(in_file);
    fclose(out_file);
}

// CLI Mode
void cli_mode() {
    char input_file[256], output_file[256], key_text[AES_KEY_SIZE];
    unsigned char key[AES_KEY_SIZE];
    int choice;

    printf("1. Encrypt\n2. Decrypt\nSelect option: ");
    scanf("%d", &choice);
    printf("Enter input file: ");
    scanf("%s", input_file);
    printf("Enter output file: ");
    scanf("%s", output_file);
    printf("Enter key: ");
    scanf("%s", key_text);

    strncpy((char *)key, key_text, AES_KEY_SIZE);
    adjust_key(key, AES_KEY_SIZE);

    if (choice == 1) {
        encrypt_file(input_file, output_file, key);
        printf("File encrypted to '%s'.\n", output_file);
    } else if (choice == 2) {
        decrypt_file(input_file, output_file, key);
        printf("File decrypted to '%s'.\n", output_file);
    } else {
        printf("Invalid choice.\n");
    }
}

void show_about(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(data),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "Author: Jay Mee, 2024");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

// GUI Encryption and Decryption handlers
void on_encrypt_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry *file_entry=GTK_ENTRY(g_object_get_data(G_OBJECT(widget), "file_entry"));
    GtkEntry *key_entry=GTK_ENTRY(g_object_get_data(G_OBJECT(widget), "key_entry"));

    const char *input_file=gtk_entry_get_text(file_entry);
    const char *key_text=gtk_entry_get_text(key_entry);
    unsigned char key[AES_KEY_SIZE];
    strncpy((char *)key, key_text, AES_KEY_SIZE);
    adjust_key(key, AES_KEY_SIZE);
    encrypt_file(input_file, "encrypted_output.enc", key);
    g_print("File encrypted as 'encrypted_output.enc'.\n");
}

void on_decrypt_clicked(GtkWidget *widget, gpointer data) {
    GtkEntry *file_entry=GTK_ENTRY(g_object_get_data(G_OBJECT(widget), "file_entry"));
    GtkEntry *key_entry=GTK_ENTRY(g_object_get_data(G_OBJECT(widget), "key_entry"));

    const char *input_file=gtk_entry_get_text(file_entry);
    const char *key_text=gtk_entry_get_text(key_entry);
    unsigned char key[AES_KEY_SIZE];
    strncpy((char *)key, key_text, AES_KEY_SIZE);
    adjust_key(key, AES_KEY_SIZE);
    decrypt_file(input_file, "decrypted_output.dec", key);
    g_print("File decrypted as 'decrypted_output.dec'.\n");
}

// GUI Mode setup
void gui_mode() {
    GtkWidget *window, *grid, *encrypt_button, *decrypt_button, *about_button, *file_entry, *key_entry;

    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "AES Encrypt/Decrypt (GUI Mode)");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid=gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    file_entry=gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("File: "), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), file_entry, 1, 0, 1, 1);

    key_entry=gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(key_entry), FALSE);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Key: "), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), key_entry, 1, 1, 1, 1);

    encrypt_button=gtk_button_new_with_label("Encrypt");
    gtk_grid_attach(GTK_GRID(grid), encrypt_button, 0, 2, 1, 1);
    g_object_set_data(G_OBJECT(encrypt_button), "file_entry", file_entry);
    g_object_set_data(G_OBJECT(encrypt_button), "key_entry", key_entry);
    g_signal_connect(encrypt_button, "clicked", G_CALLBACK(on_encrypt_clicked), NULL);

    decrypt_button=gtk_button_new_with_label("Decrypt");
    gtk_grid_attach(GTK_GRID(grid), decrypt_button, 1, 2, 1, 1);
    g_object_set_data(G_OBJECT(decrypt_button), "file_entry", file_entry);
    g_object_set_data(G_OBJECT(decrypt_button), "key_entry", key_entry);
    g_signal_connect(decrypt_button, "clicked", G_CALLBACK(on_decrypt_clicked), NULL);

    about_button=gtk_button_new_with_label("About");
    gtk_grid_attach(GTK_GRID(grid), about_button, 0, 3, 2, 1);
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about), window);

    gtk_widget_show_all(window);
}

int main(int argc, char *argv[]) {
    if (argc > 1 && strcmp(argv[1], "--cli") == 0) {
        cli_mode();
    } else {
        gtk_init(&argc, &argv);
        gui_mode();
        gtk_main();
    }
    return 0;
}

