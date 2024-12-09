#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include <time.h>
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
    char name[50], description[200],start_date[20], deadline[20], status[20];

    printf("\n--- TẠO TASK MỚI ---\n");
    printf("Nhập tên task: ");
    getchar(); // Đọc bỏ ký tự xuống dòng
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Nhập mô tả task: ");
    fgets(description, sizeof(description), stdin);
    description[strcspn(description, "\n")] = '\0';

    printf("Nhập ngày bắt đầu (YYYY-MM-DD): ");
    fgets(start_date, sizeof(start_date), stdin);
    start_date[strcspn(start_date, "\n")] = '\0';


    printf("Nhập deadline (YYYY-MM-DD): ");
    fgets(deadline, sizeof(deadline), stdin);
    deadline[strcspn(deadline, "\n")] = '\0';

    printf("Nhập tiến độ công việc (%%): ");
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
    cJSON_AddStringToObject(new_task, "start_date", start_date);
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
            const char *start_date = cJSON_GetObjectItem(task, "start_date")->valuestring;
            const char *deadline = cJSON_GetObjectItem(task, "deadline")->valuestring;
            const char *status = cJSON_GetObjectItem(task, "status")->valuestring;
            cJSON *assigned_member = cJSON_GetObjectItem(task, "assigned_member");
            const char *assigned_member_name = cJSON_GetObjectItem(assigned_member, "name")->valuestring;
            const char *assigned_member_id = cJSON_GetObjectItem(assigned_member, "id")->valuestring;

            printf("%d. %s\n", index++, name);
            printf("   Ngày bắt đầu: %s\n", start_date);
            printf("   Deadline: %s\n", deadline);
            printf("   Thành viên được phân công: %s (ID: %s)\n", assigned_member_name, assigned_member_id);
            printf("   Tiến độ công việc : %s%%\n", status);
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
                const char *start_date = cJSON_GetObjectItem(task, "start_date")->valuestring;
                const char *deadline = cJSON_GetObjectItem(task, "deadline")->valuestring;
                const char *status = cJSON_GetObjectItem(task, "status")->valuestring;
                cJSON *assigned_member = cJSON_GetObjectItem(task, "assigned_member");
                const char *assigned_member_name = cJSON_GetObjectItem(assigned_member, "name")->valuestring;
                const char *assigned_member_id = cJSON_GetObjectItem(assigned_member, "id")->valuestring;

                printf("\n--- CHI TIẾT TASK ---\n");
                printf("Tên: %s\n", name);
                printf("Mô tả: %s\n", description);
                printf("Ngày bắt đầu: %s\n", start_date);
                printf("Deadline: %s\n", deadline);
                printf("Thành viên được phân công: %s (ID: %s)\n", assigned_member_name, assigned_member_id);
                printf("Tiến độ công việc : %s%%\n", status);
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
//phan cong cong viec
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

//cap nhat tien do cong viec
void update_task_progress(const char *filename, const char *project_id, const char *user_id) {
    int choice;
    printf("\nChọn số thứ tự của task để cập nhật tiến độ: ");
    scanf("%d", &choice);
    getchar(); // Đọc bỏ ký tự xuống dòng

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
    int found = 0;
    cJSON_ArrayForEach(task, tasks) {
        const char *task_project_id = cJSON_GetObjectItem(task, "project_id")->valuestring;
        if (strcmp(task_project_id, project_id) == 0) {
            if (index == choice) {
                cJSON *assigned_member = cJSON_GetObjectItem(task, "assigned_member");
                if (assigned_member) {
                    const char *assigned_member_id = cJSON_GetObjectItem(assigned_member, "id")->valuestring;
                    if (strcmp(assigned_member_id, user_id) == 0) {
                        char new_status[20];
                        printf("Nhập tiến độ công việc mới (0-100): ");
                        fgets(new_status, sizeof(new_status), stdin);
                        new_status[strcspn(new_status, "\n")] = '\0';

                        // Kiểm tra giá trị tiến độ hợp lệ
                        int progress = atoi(new_status);
                        if (progress < 0 || progress > 100) {
                            printf("Tiến độ phải từ 0 đến 100%%.\n");
                            cJSON_Delete(json);
                            return;
                        }

                        char progress_str[4];
                        sprintf(progress_str, "%d", progress);
                        cJSON_ReplaceItemInObject(task, "status", cJSON_CreateString(progress_str));
                        printf("Tiến độ công việc đã được cập nhật thành công!\n");
                        found = 1;
                        break;
                    } else {
                        printf("Bạn không có quyền cập nhật tiến độ của task này.\n");
                        cJSON_Delete(json);
                        return;
                    }
                } else {
                    printf("Task chưa được phân công thành viên.\n");
                    cJSON_Delete(json);
                    return;
                }
            }
            index++;
        }
    }

    if (!found) {
        printf("Không tìm thấy task với số thứ tự: %d\n", choice);
    }

    char *updated_content = cJSON_Print(json);
    if (!write_file(filename, updated_content)) {
        printf("Lỗi ghi file task.\n");
    }

    free(updated_content);
    cJSON_Delete(json);
}

//hien thi gantt chart



// Hàm tính số ngày giữa hai ngày
int days_between(const char *start_date, const char *end_date) {
    struct tm tm_start, tm_end;
    time_t time_start, time_end;

    memset(&tm_start, 0, sizeof(struct tm));
    memset(&tm_end, 0, sizeof(struct tm));

    strptime(start_date, "%Y-%m-%d", &tm_start);
    strptime(end_date, "%Y-%m-%d", &tm_end);

    time_start = mktime(&tm_start);
    time_end = mktime(&tm_end);

    return (int)difftime(time_end, time_start) / (60 * 60 * 24);
}

// Hàm hiển thị biểu đồ Gantt
void display_gantt_chart(const char *filename, const char *project_id) {
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

    printf("\n--- BIỂU ĐỒ GANTT CHO PROJECT ID %s ---\n", project_id);

    // Hiển thị tiêu đề các tháng
    printf("   ");
    for (int month = 1; month <= 12; month++) {
        printf("%4d ", month);
    }
    printf("\n");

    cJSON *task;
    cJSON_ArrayForEach(task, tasks) {
        const char *task_project_id = cJSON_GetObjectItem(task, "project_id")->valuestring;
        if (strcmp(task_project_id, project_id) == 0) {
            const char *name = cJSON_GetObjectItem(task, "name")->valuestring;
            const char *start_date = cJSON_GetObjectItem(task, "start_date")->valuestring;
            const char *end_date = cJSON_GetObjectItem(task, "deadline")->valuestring;

            struct tm tm_start, tm_end;
            memset(&tm_start, 0, sizeof(struct tm));
            memset(&tm_end, 0, sizeof(struct tm));
            strptime(start_date, "%Y-%m-%d", &tm_start);
            strptime(end_date, "%Y-%m-%d", &tm_end);

            int start_month = tm_start.tm_mon + 1; // Tháng bắt đầu (1-12)
            int end_month = tm_end.tm_mon + 1; // Tháng kết thúc (1-12)
            int duration = days_between(start_date, end_date);

            printf("%-3s", name); // Hiển thị tên task

            // Hiển thị các tháng trước tháng bắt đầu
            for (int i = 1; i < start_month; i++) {
                printf("     ");
            }
            // Hiển thị các tháng của task
            for (int i = start_month; i <= end_month; i++) {
                printf("#### ");
            }

            printf(" (%d ngày)\n", duration);
        }
    }

    cJSON_Delete(json);
}