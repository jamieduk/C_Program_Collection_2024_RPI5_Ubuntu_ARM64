#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// gcc -o calendar calendar.c `pkg-config --cflags --libs gtk+-3.0`

GtkWidget *entry_day, *entry_month, *entry_year, *result_label;

// Global variable for days in each month
int days_in_month[12]={ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

void show_about(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog=gtk_message_dialog_new(GTK_WINDOW(data),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "This Software Is Developed By: Jay @  J~Net 2024");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

int is_valid_date(int dd, int mm, int yy) {
    if (yy < 1 || yy > 30200) return 0;
    if (mm < 1 || mm > 12) return 0;

    // Adjust February for leap years
    if (mm == 2 && (yy % 400 == 0 || (yy % 100 != 0 && yy % 4 == 0))) {
        days_in_month[1]=29; // February has 29 days in a leap year
    } else {
        days_in_month[1]=28; // February has 28 days in a non-leap year
    }

    if (dd < 1 || dd > days_in_month[mm - 1]) return 0;

    return 1;
}

void calculate_calendar(GtkWidget *widget, gpointer data) {
    int dd=atoi(gtk_entry_get_text(GTK_ENTRY(entry_day)));
    int mm=atoi(gtk_entry_get_text(GTK_ENTRY(entry_month)));
    int yy=atoi(gtk_entry_get_text(GTK_ENTRY(entry_year)));

    if (!is_valid_date(dd, mm, yy)) {
        gtk_label_set_text(GTK_LABEL(result_label), "INVALID DATE");
        return;
    }

    int yr=yy - 1;
    int ndd=1;
    int mnth[12]={3, 0, 3, 2, 3, 2, 3, 3, 2, 3, 2, 3};
    int tm=0;

    for (int i=0; i <= mm - 2; i++)
        tm += mnth[i];

    int x=yr % 400;
    int y=x / 100;
    int z=x % 100;
    int w=z / 4;
    int u=z % 4;
    int td=y * 5 + w * 5 + u;
    int rs=(td + tm + ndd) % 7;

    char output[512];  // Increased size for output
    snprintf(output, sizeof(output), "\n\n Calendar for %02d/%02d/%d\n\n M\tT\tW\tT\tF\tS\tS\n", mm, dd, yy);
    int dn=(mm == 2 && (yy % 400 == 0 || (yy % 100 != 0 && yy % 4 == 0))) ? 29 : days_in_month[mm - 1];

    // Align the first day of the month
    for (int i=0; i < rs; i++) {
        strcat(output, "\t");
    }

    // Print the days
    for (int k=1; k <= dn; k++) {
        char day_str[12];  // Increased size to 12 to avoid truncation warning
        snprintf(day_str, sizeof(day_str), "%02d\t", k);
        strcat(output, day_str);
        if ((k + rs) % 7 == 0) {
            strcat(output, "\n");
        }
    }

    gtk_label_set_text(GTK_LABEL(result_label), output);
}


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Calendar Application");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

    GtkWidget *grid=gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *label_day=gtk_label_new("Day:");
    GtkWidget *label_month=gtk_label_new("Month:");
    GtkWidget *label_year=gtk_label_new("Year:");

    entry_day=gtk_entry_new();
    entry_month=gtk_entry_new();
    entry_year=gtk_entry_new();

    result_label=gtk_label_new("");

    GtkWidget *calculate_button=gtk_button_new_with_label("Calculate");
    g_signal_connect(calculate_button, "clicked", G_CALLBACK(calculate_calendar), NULL);

    GtkWidget *about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(show_about), window);

    gtk_grid_attach(GTK_GRID(grid), label_day, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_day, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_month, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_month, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), label_year, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_year, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), calculate_button, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), about_button, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), result_label, 0, 4, 2, 1);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

