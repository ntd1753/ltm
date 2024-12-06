#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "file_utils.h"
#include "task_management.h"

// Hàm tạo task_id mới
int generate_task_id(cJSON *tasks) {
    int max_id = 0;
    cJSON *task;
    cJSON_ArrayForEach(task, tasks) {
        int task_id = atoi(cJSON_GetObjectItem(task, "task_id")->valuestring);
        if (task_id > max_id) {
            max_id = task_id;
        }
    }
    return max_id + 1;
}
// Hàm tạo task mới
void create_task(const char *filename, const char *project_id) {
    char name[50], description[200], deadline[20], status[20];

    printf("\n--- TẠO TASK MỚI ---\n");
    printf("Nhập tên task: ");
    getchar(); // Đọc bỏ ký tự xuống dòng
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Nhập mô tả task: ");
    fgets(description, sizeof(description), stdin);
    description[strcspn(description, "\n")] = '\0';

    printf("Nhập deadline (YYYY-MM-DD): ");
    fgets(deadline, sizeof(deadline), stdin);
    deadline[strcspn(deadline, "\n")] = '\0';

    printf("Nhập trạng thái (chưa bắt ₫ầu/ đang làm/ đã xong): ");
    fgets(status, sizeof(status), stdin);
    status[strcspn(status, "\n")] = '\0';

    // Đọc file JSON
    char *file_content = read_file(filename);
    cJSON *json;
    if (file_content) {
        json = cJSON_Parse(file_content);
        free(file_content);
    } else {
        json = cJSON_CreateObject();
        cJSON_AddItemToObject(json, "tasks", cJSON_CreateArray());
    }

    if (!json) {
        printf("Lỗi: Không thể tạo dữ liệu task.\n");
        return;
    }

    cJSON *tasks = cJSON_GetObjectItem(json, "tasks");
    if (!cJSON_IsArray(tasks)) {
        printf("Danh sách task không hợp lệ.\n");
        cJSON_Delete(json);
        return;
    }

    int new_task_id = generate_task_id(tasks);

    cJSON *new_task = cJSON_CreateObject();
    char task_id_str[10];
    sprintf(task_id_str, "%d", new_task_id);
    cJSON_AddStringToObject(new_task, "task_id", task_id_str);
    cJSON_AddStringToObject(new_task, "name", name);
    cJSON_AddStringToObject(new_task, "description", description);
    cJSON_AddStringToObject(new_task, "project_id", project_id);
    cJSON_AddStringToObject(new_task, "deadline", deadline);
    cJSON_AddStringToObject(new_task, "status", status);
    cJSON_AddItemToArray(tasks, new_task);

    char *updated_content = cJSON_Print(json);
    if (!write_file(filename, updated_content)) {
        printf("Lỗi ghi file task.\n");
    } else {
        printf("Task được tạo thành công!\n");
    }

    free(updated_content);
    cJSON_Delete(json);
}
// Hàm hiển thị danh sách các task
void display_tasks(const char *filename, const char *project_id) {
    char *file_content = read_file(filename);
    if (!file_content) {
        printf("Không có task nào được lưu.\n");
        return;
    }

    cJSON *json = cJSON_Parse(file_content);
    free(file_content);

    if (!json) {
        printf("Lỗi: Không thể đọc dữ liệu task.\n");
        return;
    }

    cJSON *tasks = cJSON_GetObjectItem(json, "tasks");
    if (!cJSON_IsArray(tasks)) {
        printf("Danh sách task không hợp lệ.\n");
        cJSON_Delete(json);
        return;
    }

    printf("\n--- DANH SÁCH TASK CỦA PROJECT ID %s ---\n", project_id);
    cJSON *task;
    int index = 1;
    cJSON_ArrayForEach(task, tasks) {
        const char *task_project_id = cJSON_GetObjectItem(task, "project_id")->valuestring;
        if (strcmp(task_project_id, project_id) == 0) {
            const char *name = cJSON_GetObjectItem(task, "name")->valuestring;
            const char *deadline = cJSON_GetObjectItem(task, "deadline")->valuestring;
            const char *status = cJSON_GetObjectItem(task, "status")->valuestring;
            printf("%d. %s\n", index++, name);
            printf("   Deadline: %s\n", deadline);
            printf("   Trạng thái: %s\n", status);
        }
    }

    cJSON_Delete(json);
}
// Hàm hiển thị chi tiết task
void view_task_details(const char *filename, const char *project_id) {
    int choice;
    printf("\nChọn số thứ tự của task để xem chi tiết: ");
    scanf("%d", &choice);

    char *file_content = read_file(filename);
    if (!file_content) {
        printf("Không có task nào được lưu.\n");
        return;
    }

    cJSON *json = cJSON_Parse(file_content);
    free(file_content);

    if (!json) {
        printf("Lỗi: Không thể đọc dữ liệu task.\n");
        return;
    }

    cJSON *tasks = cJSON_GetObjectItem(json, "tasks");
    if (!cJSON_IsArray(tasks)) {
        printf("Danh sách task không hợp lệ.\n");
        cJSON_Delete(json);
        return;
    }

    int index = 1;
    cJSON *task;
    cJSON_ArrayForEach(task, tasks) {
        const char *task_project_id = cJSON_GetObjectItem(task, "project_id")->valuestring;
        if (strcmp(task_project_id, project_id) == 0) {
            if (index == choice) {
                const char *name = cJSON_GetObjectItem(task, "name")->valuestring;
                const char *description = cJSON_GetObjectItem(task, "description")->valuestring;
                const char *deadline = cJSON_GetObjectItem(task, "deadline")->valuestring;
                const char *status = cJSON_GetObjectItem(task, "status")->valuestring;
                cJSON *assigned_member = cJSON_GetObjectItem(task, "assigned_member");
                const char *assigned_member_name = cJSON_GetObjectItem(assigned_member, "name")->valuestring;
                const char *assigned_member_id = cJSON_GetObjectItem(assigned_member, "id")->valuestring;

                printf("\n--- CHI TIẾT TASK ---\n");
                printf("Tên: %s\n", name);
                printf("Mô tả: %s\n", description);
                printf("Deadline: %s\n", deadline);
                printf("Thành viên được phân công: %s (ID: %s)\n", assigned_member_name, assigned_member_id);
                printf("Tiến độ công việc : %s\n", status);
                cJSON_Delete(json);
                return;
            }
            index++;
        }
    }

    printf("Số thứ tự không hợp lệ hoặc task không thuộc về project này.\n");
    cJSON_Delete(json);
}

//ham tim id tu ten task
const char* get_task_id_by_name(cJSON *tasks, const char *task_name) {
    cJSON *task;
    cJSON_ArrayForEach(task, tasks) {
        cJSON *name = cJSON_GetObjectItem(task, "name");
        if (cJSON_IsString(name) && (name->valuestring != NULL)) {
            if (strcmp(name->valuestring, task_name) == 0) {
                return cJSON_GetObjectItem(task, "task_id")->valuestring;
            }
        }
    }
    return NULL;
}

void assign_task_to_member(const char *filename, const char *task_id, const char *member_id, const char *member_name) {
    char *file_content = read_file(filename);
    if (!file_content) {
        printf("Không có task nào được lưu.\n");
        return;
    }

    cJSON *json = cJSON_Parse(file_content);
    free(file_content);

    if (!json) {
        printf("Lỗi: Không thể đọc dữ liệu task.\n");
        return;
    }

    cJSON *tasks = cJSON_GetObjectItem(json, "tasks");
    if (!cJSON_IsArray(tasks)) {
        printf("Danh sách task không hợp lệ.\n");
        cJSON_Delete(json);
        return;
    }

    cJSON *task;
    cJSON_ArrayForEach(task, tasks) {
        const char *current_task_id = cJSON_GetObjectItem(task, "task_id")->valuestring;
        if (strcmp(current_task_id, task_id) == 0) {
            cJSON *assigned_member = cJSON_CreateObject();
            cJSON_AddStringToObject(assigned_member, "id", member_id);
            cJSON_AddStringToObject(assigned_member, "name", member_name);
            cJSON_AddItemToObject(task, "assigned_member", assigned_member);
            break;
        }
    }

    char *updated_content = cJSON_Print(json);
    if (!write_file(filename, updated_content)) {
        printf("Lỗi ghi file task.\n");
    } else {
        printf("Task được gán thành công!\n");
    }

    free(updated_content);
    cJSON_Delete(json);
}
