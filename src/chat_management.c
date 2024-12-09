#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cJSON.h>
#include <stdlib.h>
#include <pthread.h>

void *receive_messages(void *socket_desc) {
    int sockfd = *(int *)socket_desc;
    char buffer[1024];
    int n;

    while ((n = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        buffer[n] = '\0';
        printf("\nThành viên: %s\nBạn: ", buffer);
        fflush(stdout);
    }

    return NULL;
}

void start_server() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server đang chờ kết nối...\n");

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        int valread = read(new_socket, buffer, 1024);
        buffer[valread] = '\0';
        printf("Client: %s\n", buffer);

        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        printf("Bạn: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        send(new_socket, buffer, strlen(buffer), 0);
    }

    close(new_socket);
    close(server_fd);
}

int initialize_socket() {
    int sockfd;
    struct sockaddr_in servaddr;

    // Tạo socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Lỗi tạo socket.\n");
        return -1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Kết nối tới server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        printf("Lỗi kết nối tới server.\n");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

void chat_with_member(const char *filename, const char *user_id, cJSON *project) {
    if (!project) {
        printf("Lỗi: Không tìm thấy project.\n");
        return;
    }

    const char *project_owner_id = cJSON_GetObjectItem(project, "user_id")->valuestring;
    cJSON *members = cJSON_GetObjectItem(project, "members");
    if (!cJSON_IsArray(members)) {
        printf("Danh sách thành viên không hợp lệ.\n");
        return;
    }

    int is_member_or_owner = 0;
    if (strcmp(project_owner_id, user_id) == 0) {
        is_member_or_owner = 1;
    } else {
        cJSON *member;
        cJSON_ArrayForEach(member, members) {
            const char *member_id = cJSON_GetObjectItem(member, "id")->valuestring;
            if (strcmp(member_id, user_id) == 0) {
                is_member_or_owner = 1;
                break;
            }
        }
    }

    if (!is_member_or_owner) {
        printf("Bạn không phải là thành viên hoặc chủ sở hữu của project này.\n");
        return;
    }

    int sockfd = initialize_socket();
    if (sockfd < 0) {
        return;
    }

    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, receive_messages, (void *)&sockfd) < 0) {
        perror("could not create thread");
        return;
    }

    char message[1024];
    printf("Nhập tin nhắn (gõ 'exit' để thoát):\n");
    while (1) {
        printf("Bạn: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "exit") == 0) {
            break;
        }

        send(sockfd, message, strlen(message), 0);
    }

    close(sockfd);
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);
}