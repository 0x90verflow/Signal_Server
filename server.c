#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

void reset_buffer();

char *buffer;
static int current_length = 0;
static char current_char = 0;
static int bit_count = 0;     

void handle_signal(int sig) {
    // SIGUSR1 = 1 et SIGUSR2 = 0
    if (sig == SIGUSR1) {
        current_char |= (1 << (7 - bit_count));
    }

    bit_count++;

    if (bit_count >= 8) {
        if (current_length < MAX_BUFFER_SIZE - 1) {
            buffer[current_length] = current_char;
            current_length++;
        }
        current_char = 0;
        bit_count = 0;
    }
}

void print_buffer(int sig) {
    buffer[current_length] = '\0';
    printf("Message reçu : %s\r\n", buffer);
    printf("En attente de nouveau message ....\r\n");
    reset_buffer();
    current_length = 0;
}

void reset_buffer() {

    for (int i = 0; i < MAX_BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
}


void exit_server() {
    exit(0);
    printf("\n");
}

int main() {
    // Allocation du buffer
    buffer = malloc(MAX_BUFFER_SIZE * sizeof(char));
    if (buffer == NULL) {
        printf("Erreur d'allocation mémoire\n");
        exit(1);
    }

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
