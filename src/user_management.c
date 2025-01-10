#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "file_utils.h"

// Hàm tạo user_id mới
int generate_user_id(cJSON *users) {
    int max_id = 0;
    cJSON *user;
    cJSON_ArrayForEach(user, users) {
        int user_id = atoi(cJSON_GetObjectItem(user, "user_id")->valuestring);
        if (user_id > max_id) {
            max_id = user_id;
        }
    }
    return max_id + 1;
}

// Hàm đăng ký tài khoản mới
void register_user(const char *filename) {
    char username[50], password[50];

    printf("\n--- ĐĂNG KÝ TÀI KHOẢN MỚI ---\n");
    printf("Nhập tên đăng nhập: ");
    getchar(); // Đọc bỏ ký tự xuống dòng
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';

    // Kiểm tra xem username có rỗng không
    if (strlen(username) == 0) {
        printf("Tên đăng nhập không được để trống.\n");
        return;
    }

    printf("Nhập mật khẩu: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    // Đọc file JSON
    char *file_content = read_file(filename);
    cJSON *json;
    if (file_content) {
        json = cJSON_Parse(file_content);
        free(file_content);
    } else {
        json = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "users", cJSON_CreateArray());
    }

    if (!json) {
        printf("Lỗi: Không thể tạo dữ liệu user.\n");
        return;
    }

    cJSON *users = cJSON_GetObjectItem(json, "users");
    if (!cJSON_IsArray(users)) {
        printf("Danh sách user không hợp lệ.\n");
        cJSON_Delete(json);
        return;
    }

    // Kiểm tra trùng lặp username
    cJSON *user;
    cJSON_ArrayForEach(user, users) {
        cJSON *existing_username = cJSON_GetObjectItem(user, "username");
        if (existing_username && strcmp(existing_username->valuestring, username) == 0) {
            printf("Tên đăng nhập đã tồn tại. Vui lòng chọn tên khác.\n");
            cJSON_Delete(json);
            return;
        }
    }

    int new_user_id = generate_user_id(users);

    cJSON *new_user = cJSON_CreateObject();
    char user_id_str[10];
    sprintf(user_id_str, "%d", new_user_id);
    cJSON_AddStringToObject(new_user, "user_id", user_id_str);
    cJSON_AddStringToObject(new_user, "username", username);
    cJSON_AddStringToObject(new_user, "password", password);
    cJSON_AddItemToArray(users, new_user);

    char *updated_content = cJSON_Print(json);
    if (!write_file(filename, updated_content)) {
        printf("Lỗi ghi file user.\n");
    } else {
        printf("Tài khoản được tạo thành công!\n");
    }

    free(updated_content);
    cJSON_Delete(json);
}



// Hàm xác thực tài khoản
int authenticate_user(const char *filename, char *user_id) {
    char username[50], password[50];

    printf("Nhập tên tài khoản: ");
    scanf("%s", username);
    printf("Nhập mật khẩu: ");
    scanf("%s", password);

    // Đọc file JSON
    char *file_content = read_file(filename);
    if (!file_content) {
        printf("Không thể đọc file người dùng.\n");
        return 0; // Thất bại
    }

    cJSON *json = cJSON_Parse(file_content);
    free(file_content);

    if (!json) {
        printf("Lỗi parse JSON: %s\n", cJSON_GetErrorPtr());
        return 0; // Thất bại
    }

    // Kiểm tra thông tin tài khoản
    cJSON *users = cJSON_GetObjectItem(json, "users");
    if (!cJSON_IsArray(users)) {
        printf("Danh sách người dùng không hợp lệ.\n");
        cJSON_Delete(json);
        return 0; // Thất bại
    }

    cJSON *user;
    cJSON_ArrayForEach(user, users) {
        const cJSON *json_username = cJSON_GetObjectItem(user, "username");
        const cJSON *json_password = cJSON_GetObjectItem(user, "password");
        const cJSON *json_user_id = cJSON_GetObjectItem(user, "user_id");

        if (cJSON_IsString(json_username) && cJSON_IsString(json_password) && cJSON_IsString(json_user_id)) {
            if (strcmp(json_username->valuestring, username) == 0 &&
                strcmp(json_password->valuestring, password) == 0) {
                strncpy(user_id, json_user_id->valuestring, 9);
                user_id[9] = '\0'; // Đảm bảo chuỗi kết thúc bằng null
                cJSON_Delete(json);
                return 1; // Thành công
            }
        }
    }

    printf("Sai tên tài khoản hoặc mật khẩu.\n");
    cJSON_Delete(json);
    return 0; // Thất bại
}