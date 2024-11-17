#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#define MAX_BUFFER_SIZE 1024
int createlog();
// Structure pour contenir toutes les variables globales
struct ServerState {
    char buffer[MAX_BUFFER_SIZE];
    int current_length;
    char current_char;
    int bit_count;
    time_t now;
    const char *logfile;
};

// Une seule variable globale : l'état du serveur
struct ServerState server_state;

void reset_buffer() {
    for (int i = 0; i < MAX_BUFFER_SIZE; i++){
    server_state.buffer[i] = 0;
    }
    server_state.current_length = 0;
    server_state.current_char = 0;
    server_state.bit_count = 0;
}

void handle_signal(int sig) {
    // SIGUSR1 = 1 et SIGUSR2 = 0
    if (sig == SIGUSR1) {
        server_state.current_char |= (1 << (7 - server_state.bit_count));
    }

    server_state.bit_count++;

    if (server_state.bit_count >= 8) {
        if (server_state.current_length < MAX_BUFFER_SIZE - 1) {
            server_state.buffer[server_state.current_length] = server_state.current_char;
            server_state.current_length++;
        }
        server_state.current_char = 0;
        server_state.bit_count = 0;
    }
}

void print_buffer(int sig) {
    server_state.buffer[server_state.current_length] = '\0';
    server_state.now = time(NULL);
    struct tm *local = localtime(&server_state.now);
    printf("Message reçu [%02d:%02d:%d]: %s\r\n",  local->tm_hour,local->tm_min, local->tm_sec, server_state.buffer);
    printf("En attente de nouveau message ....\r\n");
    createlog();
    reset_buffer();
}
int createlog(){
    server_state.logfile = "log.txt";
    FILE *file = fopen(server_state.logfile, "a");
    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier log");
        return 1;
    }
    server_state.now = time(NULL);
    struct tm *local = localtime(&server_state.now);
    fprintf(file, "Message reçu [%02d:%02d:%d]: %s\r\n", local->tm_hour,local->tm_min, local->tm_sec, server_state.buffer);
    fclose(file);
}
void exit_server() {
    printf("Fermeture du serveur...\n");
    exit(0);
}

int main() {
    // Initialiser l'état du serveur
    reset_buffer();

    // Configurer les gestionnaires de signaux pour SIGUSR1 et SIGUSR2
    signal(SIGUSR1, handle_signal);
    signal(SIGUSR2, handle_signal);
    signal(SIGTERM, print_buffer);
    signal(SIGINT, exit_server);

    printf("PID du serveur : %d\n", getpid());
    printf("Serveur en attente des signaux...\n");

    while (1) {
        pause();
    }

    return 0;
}
