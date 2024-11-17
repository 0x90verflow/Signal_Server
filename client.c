#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


void send_char(int pid, char c) {
    for (int i = 0; i < 8; i++) {
        if ((c >> (7 - i)) & 1) {
            kill(pid, SIGUSR1);
        } else {
            kill(pid, SIGUSR2);
        }
        usleep(700);  // pour éviter les collisions
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <PID_SERVEUR> <Message>\n", argv[0]);
        exit(1);
    }

    int pid = atoi(argv[1]);
    char *message = argv[2];

    for (int i = 0; message[i] != '\0'; i++) {
        send_char(pid, message[i]);
    }

    kill(pid, SIGTERM);

    return 0;
}
