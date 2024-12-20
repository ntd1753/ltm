#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <cJSON.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Function to check if a client is in the same project
int is_same_project(const char *message, int client_socket, int client_sockets[], int client_projects[], int project_id) {
    cJSON *json = cJSON_Parse(message);
    if (!json) return 0;

    cJSON *msg_project_id = cJSON_GetObjectItem(json, "project_id");
    if (!msg_project_id) {
        cJSON_Delete(json);
        return 0;
    }

    int msg_project_id_int = msg_project_id->valueint;
    cJSON_Delete(json);

    return msg_project_id_int == project_id;
}

// Function to update message log
void update_message_file(const char *message) {
    FILE *file = fopen("../database/message.json", "r+");
    if (!file) {
        perror("Lỗi mở file message.json");
        return;
    }

    char *buffer = NULL;
    size_t len = 0;
    fseek(file, 0, SEEK_END);
    len = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = malloc(len + 1);
    fread(buffer, 1, len, file);

    cJSON *json = cJSON_Parse(buffer);
    free(buffer);
    if (!json) {
        printf("Lỗi parse JSON\n");
        fclose(file);
        return;
    }

    cJSON *messages = cJSON_GetObjectItem(json, "messages");
    cJSON *new_message = cJSON_Parse(message);
    if (new_message && messages) {
        cJSON_AddItemToArray(messages, new_message);

        char *json_str = cJSON_PrintUnformatted(json);
        fseek(file, 0, SEEK_SET);
        fwrite(json_str, 1, strlen(json_str), file);
        ftruncate(fileno(file), strlen(json_str));
        free(json_str);
    } else {
        printf("Lỗi thêm tin nhắn vào JSON.\n");
    }

    cJSON_Delete(json);
    fclose(file);
}

int main() {
    int server_fd, new_socket, client_sockets[MAX_CLIENTS] = {0}, client_projects[MAX_CLIENTS] = {0}, max_sd, activity, i;
    struct sockaddr_in address;
    fd_set readfds;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Allow socket reuse
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Bind address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server đang chạy trên cổng %d...\n", PORT);

    while (1) {
        // Clear the socket set
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add client sockets to set
        for (i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        // Wait for activity
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select error");
            continue;
        }

        // Incoming connection
        if (FD_ISSET(server_fd, &readfds)) {
            socklen_t addrlen = sizeof(address);
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
                perror("accept");
                continue;
            }

            printf("Kết nối mới: socket %d, IP %s, cổng %d\n",
                   new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    client_projects[i] = 0; // Chưa xác định project ID
                    break;
                }
            }
        }

        // Process messages from clients
        for (i = 0; i < MAX_CLIENTS; i++) {
            int sd = client_sockets[i];
            if (FD_ISSET(sd, &readfds)) {
                int valread = read(sd, buffer, BUFFER_SIZE);
                if (valread == 0) {
                    printf("Ngắt kết nối: socket %d\n", sd);
                    close(sd);
                    client_sockets[i] = 0;
                } else {
                    buffer[valread] = '\0';
                    printf("Tin nhắn từ socket %d: %s\n", sd, buffer);

                    // Cập nhật project ID nếu cần
                    cJSON *json = cJSON_Parse(buffer);
                    if (json) {
                        cJSON *project_id = cJSON_GetObjectItem(json, "project_id");
                        if (project_id) {
                            client_projects[i] = project_id->valueint;
                            printf("Project ID socket %d: %d\n", sd, client_projects[i]);
                        }
                        cJSON_Delete(json);
                    }

                    // Update message file
                    update_message_file(buffer);

                    // Broadcast to clients in the same project
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (client_sockets[j] > 0 && client_sockets[j] != sd && client_projects[j] == client_projects[i]) {
                            printf("Gửi tin nhắn tới socket %d: %s\n", client_sockets[j], buffer);
                            if (send(client_sockets[j], buffer, strlen(buffer), 0) < 0) {
                                perror("send");
                            } else {
                                printf("Đã gửi tin nhắn tới socket %d\n", client_sockets[j]);
                            }
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
