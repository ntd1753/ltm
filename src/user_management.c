#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "file_utils.h"

// Hàm tạo tài khoản mới
void create_account(const char *filename) {
    char username[50], password[50];

    printf("Nhập tên tài khoản: ");
    scanf("%s", username);
    printf("Nhập mật khẩu: ");
    scanf("%s", password);

    // Đọc file JSON
    char *file_content = read_file(filename);
    if (!file_content) {
        return;
    }

    cJSON *json = cJSON_Parse(file_content);
    free(file_content);

    if (!json) {
        printf("Lỗi parse JSON: %s\n", cJSON_GetErrorPtr());
        return;
    }

    // Thêm tài khoản mới vào danh sách
    cJSON *users = cJSON_GetObjectItem(json, "users");
    if (!cJSON_IsArray(users)) {
        printf("Lỗi: Danh sách người dùng không hợp lệ.\n");
        cJSON_Delete(json);
        return;
    }

    cJSON *new_user = cJSON_CreateObject();
    cJSON_AddStringToObject(new_user, "username", username);
    cJSON_AddStringToObject(new_user, "password", password);
    cJSON_AddItemToArray(users, new_user);

    // Ghi lại file JSON
    char *updated_content = cJSON_Print(json);
    if (!write_file(filename, updated_content)) {
        printf("Lỗi ghi file.\n");
    } else {
        printf("Tạo tài khoản thành công.\n");
    }

    free(updated_content);
    cJSON_Delete(json);
}

// Hàm xác thực tài khoản
int authenticate_user(const char *filename) {
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

        if (cJSON_IsString(json_username) && cJSON_IsString(json_password)) {
            if (strcmp(json_username->valuestring, username) == 0 &&
                strcmp(json_password->valuestring, password) == 0) {
                cJSON_Delete(json);
                return 1; // Thành công
            }
        }
    }

    printf("Sai tên tài khoản hoặc mật khẩu.\n");
    cJSON_Delete(json);
    return 0; // Thất bại
}