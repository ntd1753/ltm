#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <cJSON.h>

#define BUFFER_SIZE 1024
#define PORT 8080
#define USER_FILE "../database/users.json"
#define MESSAGE_FILE "../database/message.json"
typedef struct {
    int sockfd;
    char project_id[50];
} thread_args_t;


// Hàm lấy username từ user_id
char *get_username_by_id(const char *user_id) {
    FILE *file = fopen(USER_FILE, "r");
    if (!file) {
        perror("Lỗi mở file user.json");
        return NULL;
    }

    char *buffer = NULL;
    size_t len = 0;
    fseek(file, 0, SEEK_END);
    len = ftell(file);
    fseek(file, 0, SEEK_SET);

    buffer = malloc(len + 1);
    fread(buffer, 1, len, file);
    fclose(file);

    cJSON *users_data = cJSON_Parse(buffer);
    free(buffer);
    if (!users_data) {
        printf("Lỗi parse JSON file user.json\n");
        return NULL;
    }

    cJSON *users = cJSON_GetObjectItem(users_data, "users");
    cJSON *user;
    cJSON_ArrayForEach(user, users) {
        cJSON *id = cJSON_GetObjectItem(user, "user_id");
        cJSON *username = cJSON_GetObjectItem(user, "username");
        if (id && username && strcmp(id->valuestring, user_id) == 0) {
            char *result = strdup(username->valuestring); // Copy giá trị username
            cJSON_Delete(users_data);
            return result;
        }
    }

    cJSON_Delete(users_data);
    return NULL; // Không tìm thấy user_id
}
// Hàm hiển thị tin nhắn cũ của project
void print_project_messages(const char *project_id) {
    FILE *file = fopen(MESSAGE_FILE, "r");
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
    if (!buffer) {
        perror("Lỗi cấp phát bộ nhớ");
        fclose(file);
        return;
    }
    fread(buffer, 1, len, file);
    fclose(file);

    buffer[len] = '\0'; // Đảm bảo chuỗi kết thúc đúng cách
    cJSON *json_data = cJSON_Parse(buffer);
    free(buffer);

    if (!json_data) {
        printf("Lỗi parse JSON file message.json\n");
        return;
    }

    cJSON *messages = cJSON_GetObjectItem(json_data, "messages");
    if (!cJSON_IsArray(messages)) {
        printf("Danh sách tin nhắn không hợp lệ.\n");
        cJSON_Delete(json_data);
        return;
    }

    printf("\n--- Tin nhắn cũ của project %s ---\n", project_id);
    cJSON *message;
    int found = 0;
    cJSON_ArrayForEach(message, messages) {
        cJSON *msg_project_id = cJSON_GetObjectItem(message, "project_id");
        cJSON *user_id = cJSON_GetObjectItem(message, "user_id");
        cJSON *msg_content = cJSON_GetObjectItem(message, "message");
        // Bỏ qua nếu project_id không khớp hoặc nội dung tin nhắn rỗng
        if (!msg_project_id || !msg_content || strcmp(msg_project_id->valuestring, project_id) != 0 ||
            strlen(msg_content->valuestring) == 0) {
            continue;
        }
        if (msg_project_id && user_id && msg_content &&
            strcmp(msg_project_id->valuestring, project_id) == 0) {
            char *username = get_username_by_id(user_id->valuestring);
            if (username) {
                printf("[%s]: %s\n", username, msg_content->valuestring);
                free(username);
            } else {
                printf("[Unknown User (ID: %s)]: %s\n", user_id->valuestring, msg_content->valuestring);
            }
            found = 1;
        }
    }

    if (!found) {
        printf("Không có tin nhắn nào.\n");
    }

    cJSON_Delete(json_data);
}
// Hàm nhận tin nhắn từ server
void *receive_messages(void *arg) {
    thread_args_t *args = (thread_args_t *)arg;
    int sockfd = args->sockfd;
    char *current_project_id = args->project_id;

    char buffer[BUFFER_SIZE];
    int n;

    while ((n = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        buffer[n] = '\0';

        // Parse tin nhắn JSON
        cJSON *json_message = cJSON_Parse(buffer);
        if (!json_message) {
            printf("Lỗi parse JSON từ server.\n");
            continue;
        }

        // Lấy project_id, user_id và message từ JSON
        cJSON *json_project_id = cJSON_GetObjectItem(json_message, "project_id");
        cJSON *json_user_id = cJSON_GetObjectItem(json_message, "user_id");
        cJSON *json_message_content = cJSON_GetObjectItem(json_message, "message");

        // Bỏ qua nếu project_id không khớp hoặc tin nhắn rỗng
        if (!json_project_id || strcmp(json_project_id->valuestring, current_project_id) != 0 ||
            (json_message_content && strlen(json_message_content->valuestring) == 0)) {
            cJSON_Delete(json_message);
            continue;
        }

        // Lấy tên người gửi từ user_id
        char *username = get_username_by_id(json_user_id->valuestring);
        if (username) {
            // Hiển thị username và nội dung tin nhắn
            printf("\n[%s]: %s\n", username, json_message_content->valuestring);
            free(username);
        } else {
            printf("\n[Unknown User (ID: %s)]: %s\n", json_user_id->valuestring, json_message_content->valuestring);
        }

        cJSON_Delete(json_message);
        fflush(stdout);
    }

    if (n == 0) {
        printf("Server đã đóng kết nối.\n");
    } else if (n < 0) {
        perror("recv");
    }

    free(args); // Giải phóng bộ nhớ của args
    return NULL;
}

// Hàm khởi tạo socket và kết nối tới server
int initialize_socket() {
    int sockfd;
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Lỗi tạo socket");
        return -1;
    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Lỗi kết nối tới server");
        close(sockfd);
        return -1;
    }

    return sockfd;
}

// Hàm chính để thực hiện chat
void chat_with_member(const char *filename, const char *user_id, cJSON *project) {
    const char *project_id = cJSON_GetObjectItem(project, "project_id")->valuestring;

    print_project_messages(project_id);
    int sockfd = initialize_socket();
    if (sockfd < 0) return;

    pthread_t recv_thread;

    // Tạo đối tượng thread_args_t
    thread_args_t *args = malloc(sizeof(thread_args_t));
    args->sockfd = sockfd;
    strncpy(args->project_id, project_id, sizeof(args->project_id) - 1);
    args->project_id[sizeof(args->project_id) - 1] = '\0';

    if (pthread_create(&recv_thread, NULL, receive_messages, args) < 0) {
        perror("Không thể tạo thread nhận tin nhắn");
        close(sockfd);
        free(args);
        return;
    }

    char message[BUFFER_SIZE];
    printf("Nhập tin nhắn (gõ 'exit' để thoát):\n");
    while (1) {
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "exit") == 0) {
            break;
        }

        // Tạo tin nhắn JSON với user_id và project_id
        cJSON *json_message = cJSON_CreateObject();
        cJSON_AddStringToObject(json_message, "user_id", user_id);
        cJSON_AddStringToObject(json_message, "project_id", project_id);
        cJSON_AddStringToObject(json_message, "message", message);

        char *json_str = cJSON_PrintUnformatted(json_message);
        send(sockfd, json_str, strlen(json_str), 0);

        cJSON_Delete(json_message);
        free(json_str);
    }

    close(sockfd);
    pthread_cancel(recv_thread);
    pthread_join(recv_thread, NULL);
    free(args);
}

