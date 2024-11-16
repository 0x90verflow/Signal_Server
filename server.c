#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

void clear_buffer();

char buffer[MAX_BUFFER_SIZE];




void handle_signal(int sig) {
    static int current_length = 0; 
    static char current_char = 0;  
    static int bit_count = 0;  

    // SIGUSR1 = 1 SIGUSR2 = 0
    if (sig == SIGUSR1) {
        current_char |= (1 << (7 - bit_count));  // Ajouter 1 au bit en cours
    }

    bit_count++;  // Passer au bit suivant

    
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
    buffer[strlen(buffer)] = '\0';  // Ajouter un caractÃ¨re de fin de chaÃ®ne
    printf("Message reÃ§u : %s\r\n", buffer);  // Afficher le message complet
    printf("En attente de nouveau message ....\r\n");
}

void exit_server(){
	exit(0);
}
int main() {
    // Initialiser le buffer Ã  zÃ©ro
    memset(buffer, 0, sizeof(buffer));

    // Fonction pour la gestion des signaux pour SIGUSR1 et SIGUSR2
    signal(SIGUSR1, handle_signal); 
    signal(SIGUSR2, handle_signal);  

    signal(SIGINT, exit_server);

    printf("PID du serveur : %d\n", getpid());
    printf("Serveur en attente des signaux...\n");

    // Boucle infinie pour recevoir les signaux
    while (1) {
        pause(); 
    }

    return 0;
}
