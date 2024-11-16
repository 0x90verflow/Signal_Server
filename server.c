#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

void clear_buffer();

// Buffer pour stocker le message reÃ§u (unique variable globale)
char buffer[MAX_BUFFER_SIZE];



// Fonction pour assembler les bits dans un caractÃ¨re et gÃ©rer l'assemblage du message
void handle_signal(int sig) {
    static int current_length = 0;  // Indice actuel dans le buffer (local)
    static char current_char = 0;   // CaractÃ¨re en construction (local)
    static int bit_count = 0;       // Compteur de bits pour chaque caractÃ¨re (local)

    // DÃ©coder le signal reÃ§u : SIGUSR1 pour 1, SIGUSR2 pour 0
    if (sig == SIGUSR1) {
        current_char |= (1 << (7 - bit_count));  // Ajouter 1 au bit en cours
    }
    // SIGUSR2 signifie 0, donc rien Ã  faire

    bit_count++;  // Passer au bit suivant

    // Si 8 bits ont Ã©tÃ© reÃ§us, un caractÃ¨re complet a Ã©tÃ© assemblÃ©
    if (bit_count >= 8) {
        if (current_length < MAX_BUFFER_SIZE - 1) {
            buffer[current_length] = current_char;  // Ajouter le caractÃ¨re dans le buffer
            current_length++;
        }
        // RÃ©initialiser pour le prochain caractÃ¨re
        current_char = 0;
        bit_count = 0;
    }
}

// Fonction pour afficher le message complet lorsque SIGINT est reÃ§u
void print_buffer(int sig) {
    buffer[strlen(buffer)] = '\0';  // Ajouter un caractÃ¨re de fin de chaÃ®ne
    printf("Message reÃ§u : %s\r\n", buffer);  // Afficher le message complet
    printf("En attente de nouveau message ....\r\n");
}

void exit_server(){
	exit(0);
	printf("\n");
}
int main() {
    // Initialiser le buffer Ã  zÃ©ro
    memset(buffer, 0, sizeof(buffer));

    // Configurer les gestionnaires de signaux pour SIGUSR1 et SIGUSR2
    signal(SIGUSR1, handle_signal);  // SIGUSR1 signifie bit = 1
    signal(SIGUSR2, handle_signal);  // SIGUSR2 signifie bit = 0

    // Configurer SIGINT pour afficher le message complet
    signal(SIGINT, exit_server);

    // Afficher un message de dÃ©marrage
    printf("PID du serveur : %d\n", getpid());
    printf("Serveur en attente des signaux...\n");

    // Boucle infinie pour recevoir les signaux
    while (1) {
        pause();  // Attendre l'arrivÃ©e d'un signal
    }

    return 0;
}
