#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
// gcc account_manager.c -o account_manager `pkg-config --cflags --libs gtk+-3.0` -lssl -lcrypto

#define AES_KEY_LENGTH 128
#define AES_BLOCK_SIZE 16

// Function prototypes
void on_exit_app(GtkWidget *widget, gpointer data);
void on_signup(GtkWidget *widget, gpointer data);
void on_authenticate(GtkWidget *widget, gpointer data);
void save_user_data(GtkWidget *widget, gpointer data);
void reload_user_data(GtkWidget *widget, gpointer data);
void edit_user_data(GtkWidget *widget, gpointer data);
void encrypt_text(const char *plaintext, unsigned char *ciphertext, const unsigned char *key, int *ciphertext_len);
void decrypt_text(const unsigned char *ciphertext, char *plaintext, const unsigned char *key, int ciphertext_len);
void hash_password(const char *password, unsigned char *hashed_password);
void on_about(GtkWidget *widget, gpointer data);

// Global variables
GtkWidget *entry_username;
GtkWidget *entry_password;
char current_username[50];
GtkWidget *edit_window;
GtkWidget *text_view_edit;

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Account Manager");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *grid=gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    entry_username=gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Username");
    gtk_grid_attach(GTK_GRID(grid), entry_username, 0, 0, 1, 1);

    entry_password=gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_grid_attach(GTK_GRID(grid), entry_password, 0, 1, 1, 1);

    GtkWidget *btn_authenticate=gtk_button_new_with_label("Authenticate");
    g_signal_connect(btn_authenticate, "clicked", G_CALLBACK(on_authenticate), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_authenticate, 0, 2, 1, 1);

    GtkWidget *btn_signup=gtk_button_new_with_label("Sign Up");
    g_signal_connect(btn_signup, "clicked", G_CALLBACK(on_signup), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_signup, 0, 3, 1, 1);

    GtkWidget *btn_about=gtk_button_new_with_label("About");
    g_signal_connect(btn_about, "clicked", G_CALLBACK(on_about), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_about, 0, 4, 1, 1);

    GtkWidget *btn_exit=gtk_button_new_with_label("Exit");
    g_signal_connect(btn_exit, "clicked", G_CALLBACK(on_exit_app), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_exit, 0, 5, 1, 1);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

void on_exit_app(GtkWidget *widget, gpointer data) {
    gtk_main_quit();
}

void on_signup(GtkWidget *widget, gpointer data) {
    const char *username=gtk_entry_get_text(GTK_ENTRY(entry_username));
    const char *password=gtk_entry_get_text(GTK_ENTRY(entry_password));
    unsigned char hashed_password[SHA256_DIGEST_LENGTH];

    hash_password(password, hashed_password);

    FILE *file=fopen("accounts.txt", "a");
    if (file) {
        fprintf(file, "%s:%s\n", username, hashed_password);
        fclose(file);
        gtk_entry_set_text(GTK_ENTRY(entry_username), "");
        gtk_entry_set_text(GTK_ENTRY(entry_password), "");
        g_print("Signed up successfully\n");
    } else {
        g_print("Error saving credentials\n");
    }
}

void on_authenticate(GtkWidget *widget, gpointer data) {
    const char *username=gtk_entry_get_text(GTK_ENTRY(entry_username));
    const char *password=gtk_entry_get_text(GTK_ENTRY(entry_password));
    char line[256];
    int authenticated=0;

    unsigned char hashed_password[SHA256_DIGEST_LENGTH];
    hash_password(password, hashed_password);

    FILE *file=fopen("accounts.txt", "r");
    if (file) {
        while (fgets(line, sizeof(line), file)) {
            char *file_username=strtok(line, ":");
            char *file_password=strtok(NULL, "\n");
            if (strcmp(username, file_username) == 0 && strcmp((const char *)hashed_password, file_password) == 0) {
                authenticated=1;
                break;
            }
        }
        fclose(file);
    }

    if (authenticated) {
        strcpy(current_username, username);
        edit_user_data(NULL, NULL); // Open the edit user data window
    } else {
        g_print("Authentication failed\n");
    }
}

void edit_user_data(GtkWidget *widget, gpointer data) {
    edit_window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(edit_window), "Edit User Data");
    gtk_window_set_default_size(GTK_WINDOW(edit_window), 400, 300);
    g_signal_connect(edit_window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

    GtkWidget *grid=gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(edit_window), grid);

    text_view_edit=gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view_edit), GTK_WRAP_WORD);
    gtk_grid_attach(GTK_GRID(grid), text_view_edit, 0, 0, 1, 1);

    GtkWidget *btn_reload=gtk_button_new_with_label("Load");
    g_signal_connect(btn_reload, "clicked", G_CALLBACK(reload_user_data), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_reload, 0, 1, 1, 1);
    
    GtkWidget *btn_save=gtk_button_new_with_label("Save");
    g_signal_connect(btn_save, "clicked", G_CALLBACK(save_user_data), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_save, 0, 2, 1, 1); // 0, 2, 1, 1

    GtkWidget *btn_exit=gtk_button_new_with_label("Exit");
    g_signal_connect(btn_exit, "clicked", G_CALLBACK(on_exit_app), NULL);
    gtk_grid_attach(GTK_GRID(grid), btn_exit, 0, 3, 1, 1);

    gtk_widget_show_all(edit_window);
}

void save_user_data(GtkWidget *widget, gpointer data) {
    GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_edit));
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gchar *text=gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

    // Encrypt the text using the password
    unsigned char key[32]; // Assuming 256-bit key
    const char *password=gtk_entry_get_text(GTK_ENTRY(entry_password));
    hash_password(password, key); // Hash the password to create a key

    unsigned char ciphertext[1024];
    int ciphertext_len;
    encrypt_text(text, ciphertext, key, &ciphertext_len);

    char filename[55]; // Adjust size to prevent truncation
    snprintf(filename, sizeof(filename), "%s.txt", current_username);
    FILE *file=fopen(filename, "wb"); // Open in binary mode
    if (file) {
        fwrite(ciphertext, sizeof(unsigned char), ciphertext_len, file);
        fclose(file);
        g_print("User data saved\n");
    } else {
        g_print("Error saving user data\n");
    }
    g_free(text); // Free the text buffer
}

void reload_user_data(GtkWidget *widget, gpointer data) {
    char filename[55]; // Adjust size to prevent truncation
    snprintf(filename, sizeof(filename), "%s.txt", current_username);
    FILE *user_file=fopen(filename, "rb"); // Open in binary mode
    if (user_file) {
        fseek(user_file, 0, SEEK_END);
        long file_size=ftell(user_file);
        fseek(user_file, 0, SEEK_SET);

        unsigned char *ciphertext=malloc(file_size);
        fread(ciphertext, sizeof(unsigned char), file_size, user_file);
        fclose(user_file);

        // Decrypt the text using the password
        char plaintext[1024];
        unsigned char key[32]; // Assuming 256-bit key
        const char *password=gtk_entry_get_text(GTK_ENTRY(entry_password));
        hash_password(password, key); // Hash the password to create a key

        decrypt_text(ciphertext, plaintext, key, file_size);
        gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view_edit)), plaintext, -1);
        free(ciphertext);
        g_print("User data loaded\n");
    } else {
        g_print("Error loading user data\n");
    }
}

void encrypt_text(const char *plaintext, unsigned char *ciphertext, const unsigned char *key, int *ciphertext_len) {
    EVP_CIPHER_CTX *ctx=EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, NULL);
    int len;
    EVP_EncryptUpdate(ctx, ciphertext, &len, (unsigned char *)plaintext, strlen(plaintext));
    *ciphertext_len=len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    *ciphertext_len += len;
    EVP_CIPHER_CTX_free(ctx);
}

void decrypt_text(const unsigned char *ciphertext, char *plaintext, const unsigned char *key, int ciphertext_len) {
    EVP_CIPHER_CTX *ctx=EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, NULL);
    int len;
    EVP_DecryptUpdate(ctx, (unsigned char *)plaintext, &len, ciphertext, ciphertext_len);
    int plaintext_len=len;
    EVP_DecryptFinal_ex(ctx, (unsigned char *)plaintext + len, &len);
    plaintext_len += len;
    plaintext[plaintext_len]='\0'; // Null-terminate the plaintext
    EVP_CIPHER_CTX_free(ctx);
}

void hash_password(const char *password, unsigned char *hashed_password) {
    SHA256((unsigned char *)password, strlen(password), hashed_password);
}

void on_about(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Account Manager v1.0\nBy Jay @ J~Net 2024");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

