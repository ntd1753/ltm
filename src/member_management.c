#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "file_utils.h"
#include <string.h>
#include "project_management.h"

// Function to generate a unique member ID
const char* generate_member_id() {
    static int id_counter = 1;
    static char id_str[10];
    snprintf(id_str, sizeof(id_str), "%d", id_counter++);
    return id_str;
}

void add_member_to_project(const char *filename, const char *user_id, cJSON *project, const char *current_project_id) {
    char member_name[50];
    printf("Nhập tên thành viên mới: ");
    getchar(); // Đọc bỏ ký tự xuống dòng
    fgets(member_name, sizeof(member_name), stdin);
    member_name[strcspn(member_name, "\n")] = '\0';

    // Tải dữ liệu từ file JSON
    char *file_content = read_file(filename);
    if (file_content == NULL) {
        printf("Lỗi đọc file.\n");
        return;
    }

    // Parse nội dung file JSON
    cJSON *root = cJSON_Parse(file_content);
    free(file_content);
    if (root == NULL) {
        printf("Lỗi phân tích cú pháp JSON.\n");
        return;
    }

    // Tìm mảng projects
    cJSON *projects = cJSON_GetObjectItem(root, "projects");
    if (!cJSON_IsArray(projects)) {
        printf("Dữ liệu không hợp lệ, không tìm thấy mảng projects.\n");
        cJSON_Delete(root);
        return;
    }

    cJSON *target_project = NULL;
    cJSON *project_item = NULL;
    cJSON_ArrayForEach(project_item, projects) {
        cJSON *project_id = cJSON_GetObjectItem(project_item, "project_id");
        if (cJSON_IsString(project_id) && strcmp(project_id->valuestring, current_project_id) == 0) {
            target_project = project_item;
            break;
        }
    }

    if (target_project == NULL) {
        printf("Không tìm thấy dự án.\n");
        cJSON_Delete(root);
        return;
    }

    // Lấy danh sách thành viên của dự án
    cJSON *members = cJSON_GetObjectItem(target_project, "members");
    if (!cJSON_IsArray(members)) {
        members = cJSON_CreateArray();
        cJSON_AddItemToObject(target_project, "members", members);
    }

    // Kiểm tra xem thành viên đã tồn tại chưa
    cJSON *member_item = NULL;
    cJSON_ArrayForEach(member_item, members) {
        cJSON *name = cJSON_GetObjectItem(member_item, "name");
        if (cJSON_IsString(name) && strcmp(name->valuestring, member_name) == 0) {
            printf("Thành viên đã tồn tại.\n");
            cJSON_Delete(root);
            return;
        }
    }

    // Tải dữ liệu từ file users.json
    char *users_content = read_file("../database/users.json");
    if (users_content == NULL) {
        printf("Lỗi đọc file users.json.\n");
        cJSON_Delete(root);
        return;
    }

    // Parse nội dung file users.json
    cJSON *users_root = cJSON_Parse(users_content);
    free(users_content);
    if (users_root == NULL) {
        printf("Lỗi phân tích cú pháp JSON của users.json.\n");
        cJSON_Delete(root);
        return;
    }

    // Tìm mảng users
    cJSON *users = cJSON_GetObjectItem(users_root, "users");
    if (!cJSON_IsArray(users)) {
        printf("Dữ liệu không hợp lệ, không tìm thấy mảng users.\n");
        cJSON_Delete(users_root);
        cJSON_Delete(root);
        return;
    }

    // Tìm user_id theo member_name
    const char *member_id = NULL;
    cJSON *user_item = NULL;
    cJSON_ArrayForEach(user_item, users) {
        cJSON *username = cJSON_GetObjectItem(user_item, "username");
        if (cJSON_IsString(username) && strcmp(username->valuestring, member_name) == 0) {
            member_id = cJSON_GetObjectItem(user_item, "user_id")->valuestring;
            break;
        }
    }

    if (member_id == NULL) {
        printf("Không tìm thấy thành viên với tên đã nhập.\n");
        cJSON_Delete(users_root);
        cJSON_Delete(root);
        return;
    }

    // Thêm thành viên mới
    cJSON *new_member = cJSON_CreateObject();
    cJSON_AddStringToObject(new_member, "name", member_name);
    cJSON_AddStringToObject(new_member, "id", member_id);
    cJSON_AddItemToArray(members, new_member);

    // Chuyển đổi lại dữ liệu thành chuỗi JSON
    char *updated_content = cJSON_Print(root);
    if (!write_file(filename, updated_content)) {
        printf("Lỗi ghi file project.\n");
    } else {
        printf("Thành viên được thêm thành công!\n");
    }

    // Giải phóng bộ nhớ
    free(updated_content);
    cJSON_Delete(users_root);
    cJSON_Delete(root);

    // Gọi lại hàm manage_projects sau khi cập nhật
    return manage_projects(filename, user_id); // Sửa lại cú pháp gọi hàm
}

void view_members_of_project(const char *filename, const char *user_id, cJSON *project) {
    cJSON *members = cJSON_GetObjectItem(project, "members");
    if (!cJSON_IsArray(members)) {
        printf("khoong co thanh vien nao.\n");
        return manage_projects(filename, user_id);
    }

    printf("\n--- DANH SÁCH THÀNH VIÊN ---\n");
    cJSON *member;
    int index = 1;
    cJSON_ArrayForEach(member, members) {
        const char *member_name = cJSON_GetObjectItem(member, "name")->valuestring;
        const char *member_id = cJSON_GetObjectItem(member, "id")->valuestring;
        printf("%d. %s (ID: %s)\n", index++, member_name, member_id);
    }
    return manage_projects(filename, user_id);
}