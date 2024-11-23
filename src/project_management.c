#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "file_utils.h"

// Hàm hiển thị danh sách các project
void display_projects(const char *filename) {
    char *file_content = read_file(filename);
    if (!file_content) {
        printf("Không có project nào được lưu.\n");
        return;
    }

    cJSON *json = cJSON_Parse(file_content);
    free(file_content);

    if (!json) {
        printf("Lỗi: Không thể đọc dữ liệu project.\n");
        return;
    }

    cJSON *projects = cJSON_GetObjectItem(json, "projects");
    if (!cJSON_IsArray(projects)) {
        printf("Danh sách project không hợp lệ.\n");
        cJSON_Delete(json);
        return;
    }

    printf("\n--- DANH SÁCH PROJECT ---\n");
    cJSON *project;
    int index = 1;
    cJSON_ArrayForEach(project, projects) {
        const char *name = cJSON_GetObjectItem(project, "name")->valuestring;
        printf("%d. %s\n", index++, name);
    }

    cJSON_Delete(json);
}

// Hàm tạo project mới
void create_project(const char *filename) {
    char name[50], description[200];

    printf("\n--- TẠO PROJECT MỚI ---\n");
    printf("Nhập tên project: ");
    getchar(); // Đọc bỏ ký tự xuống dòng
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Nhập mô tả project: ");
    fgets(description, sizeof(description), stdin);
    description[strcspn(description, "\n")] = '\0';

    // Đọc file JSON
    char *file_content = read_file(filename);
    cJSON *json;
    if (file_content) {
        json = cJSON_Parse(file_content);
        free(file_content);
    } else {
        json = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "projects", cJSON_CreateArray());
    }

    if (!json) {
        printf("Lỗi: Không thể tạo dữ liệu project.\n");
        return;
    }

    cJSON *projects = cJSON_GetObjectItem(json, "projects");
    if (!cJSON_IsArray(projects)) {
        printf("Danh sách project không hợp lệ.\n");
        cJSON_Delete(json);
        return;
    }

    cJSON *new_project = cJSON_CreateObject();
    cJSON_AddStringToObject(new_project, "name", name);
    cJSON_AddStringToObject(new_project, "description", description);
    cJSON_AddItemToArray(projects, new_project);

    char *updated_content = cJSON_Print(json);
    if (!write_file(filename, updated_content)) {
        printf("Lỗi ghi file project.\n");
    } else {
        printf("Project được tạo thành công!\n");
    }

    free(updated_content);
    cJSON_Delete(json);
}

// Hàm xem chi tiết project
void view_project_details(const char *filename) {
    int choice;
    printf("\nChọn số thứ tự của project để xem chi tiết: ");
    scanf("%d", &choice);

    char *file_content = read_file(filename);
    if (!file_content) {
        printf("Không có project nào được lưu.\n");
        return;
    }

    cJSON *json = cJSON_Parse(file_content);
    free(file_content);

    if (!json) {
        printf("Lỗi: Không thể đọc dữ liệu project.\n");
        return;
    }

    cJSON *projects = cJSON_GetObjectItem(json, "projects");
    if (!cJSON_IsArray(projects)) {
        printf("Danh sách project không hợp lệ.\n");
        cJSON_Delete(json);
        return;
    }

    int index = 1;
    cJSON *project;
    cJSON_ArrayForEach(project, projects) {
        if (index == choice) {
            const char *name = cJSON_GetObjectItem(project, "name")->valuestring;
            const char *description = cJSON_GetObjectItem(project, "description")->valuestring;

            printf("\n--- CHI TIẾT PROJECT ---\n");
            printf("Tên: %s\n", name);
            printf("Mô tả: %s\n", description);
            cJSON_Delete(json);
            return;
        }
        index++;
    }

    printf("Số thứ tự không hợp lệ.\n");
    cJSON_Delete(json);
}

// Hàm quản lý project
void manage_projects(const char *filename) {
    int choice;
    do {
        printf("\n--- QUẢN LÝ PROJECT ---\n");
        printf("1. Xem danh sách project\n");
        printf("2. Tạo project mới\n");
        printf("3. Xem chi tiết project\n");
        printf("0. Quay lại\n");
        printf("Lựa chọn: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                display_projects(filename);
                break;
            case 2:
                create_project(filename);
                break;
            case 3:
                view_project_details(filename);
                break;
            case 0:
                printf("Quay lại menu chính.\n");
                break;
            default:
                printf("Lựa chọn không hợp lệ.\n");
        }
    } while (choice != 0);
}
