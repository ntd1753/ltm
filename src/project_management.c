#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "file_utils.h"
#include "task_management.h"
#include "member_management.h"
#include "project_management.h"

// Hàm hiển thị danh sách các project
void display_projects(const char *filename, const char *user_id) {
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

printf("\n--- DANH SÁCH PROJECT CỦA USER ID %s ---\n", user_id); 
    cJSON *project;
    int index = 1;
   cJSON_ArrayForEach(project, projects) {
        const char *project_user_id = cJSON_GetObjectItem(project, "user_id")->valuestring;
        if (strcmp(project_user_id, user_id) == 0) {
            const char *name = cJSON_GetObjectItem(project, "name")->valuestring;
            printf("%d. %s\n", index++, name);
        }
    }

    cJSON_Delete(json);
}


// Hàm tạo project_id mới
int generate_project_id(cJSON *projects) {
    int max_id = 0;
    cJSON *project;
    cJSON_ArrayForEach(project, projects) {
        int project_id = atoi(cJSON_GetObjectItem(project, "project_id")->valuestring);
        if (project_id > max_id) {
            max_id = project_id;
        }
    }
    return max_id + 1;
}
// Hàm tạo project mới
void create_project(const char *filename, const char *user_id) {
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
    int new_project_id = generate_project_id(projects);
    cJSON *new_project = cJSON_CreateObject();
    char project_id_str[10];
    sprintf(project_id_str, "%d", generate_project_id(projects));
    cJSON_AddStringToObject(new_project, "project_id", project_id_str);
    cJSON_AddStringToObject(new_project, "name", name);
    cJSON_AddStringToObject(new_project, "description", description);
    cJSON_AddStringToObject(new_project, "user_id", user_id);
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
// Hàm quản lý project
void manage_projects(const char *filename, const char *user_id) {
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
                display_projects(filename, user_id);
                break;
            case 2:
                create_project(filename, user_id);
                break;
            case 3:
                view_project_details(filename, user_id);
                break;
            case 0:
                printf("Quay lại menu chính.\n");
                break;
            default:
                printf("Lựa chọn không hợp lệ.\n");
                break;
        }
    } while (choice != 0);
}

// Hàm xem chi tiết project
void view_project_details(const char *filename, const char *user_id) {
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
        const char *project_user_id = cJSON_GetObjectItem(project, "user_id")->valuestring;
        if (strcmp(project_user_id, user_id) == 0) {
            if (index == choice) {
                const char *name = cJSON_GetObjectItem(project, "name")->valuestring;
                const char *description = cJSON_GetObjectItem(project, "description")->valuestring;
                const char *project_id = cJSON_GetObjectItem(project, "project_id")->valuestring;
                int projectChoice;
                printf("\n--- CHI TIẾT PROJECT ---\n");
                printf("Tên: %s\n", name);
                printf("Mô tả: %s\n", description);
                do{
                printf("\n--- tác vụ -- \n");
                printf("1. Thêm thành viên\n");
                printf("2. Thêm công việc\n");
                printf("3. Xem danh sách công việc\n");
                printf("4. Xem danh sach thanh vien\n");
                printf("5. Xem chi tiết công việc\n");
                printf("\n0. Quay lại\n");
                printf("Lựa chọn: ");
                scanf("%d", &projectChoice);
                switch (projectChoice) {
                    case 1:
                        add_member_to_project(filename, user_id, project, project_id);
                        break;
                    case 2:
                        create_task("../database/task.json", project_id);                      
                        break;
                    case 3:
                        display_tasks("../database/task.json", project_id);
                        break;
                    case 4:
                        view_members_of_project(filename, user_id, project);
                        break;
                    case 5:
                        view_task_details("../database/task.json", project_id);
                        break;
                    case 0:
                            manage_projects(filename, user_id);
                            break;
                    default:
                        printf("Lựa chọn không hợp lệ.\n");
                }
            }while (choice != 0);
                cJSON_Delete(json);
                return;
            }
            index++;
        }
    }

    printf("Số thứ tự không hợp lệ.\n");
    cJSON_Delete(json);
}

