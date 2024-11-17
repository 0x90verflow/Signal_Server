#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

struct ServerState {
    char buffer[MAX_BUFFER_SIZE];
    int current_length;
    char current_char;
    int bit_count;
};

struct ServerState server_state;

void reset_buffer() {
    memset(server_state.buffer, 0, MAX_BUFFER_SIZE);
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
    printf("Message reçu : %s\r\n", server_state.buffer);
    printf("En attente de nouveau message ....\r\n");
    reset_buffer();
}

void exit_server() {
    printf("Fermeture du serveur...\n");
    exit(0);
}

int main() {
    reset_buffer();

    // Gestion des signaux
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
