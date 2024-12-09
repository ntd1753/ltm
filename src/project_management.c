#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cJSON.h>
#include "file_utils.h"
#include "task_management.h"
#include "member_management.h"
#include "project_management.h"

// Hàm hiển thị danh sách các project
// void display_projects(const char *filename, const char *user_id) {
//     char *file_content = read_file(filename);
//     if (!file_content) {
//         printf("Không có project nào được lưu.\n");
//         return;
//     }

//     cJSON *json = cJSON_Parse(file_content);
//     free(file_content);

//     if (!json) {
//         printf("Lỗi: Không thể đọc dữ liệu project.\n");
//         return;
//     }

//     cJSON *projects = cJSON_GetObjectItem(json, "projects");
//     if (!cJSON_IsArray(projects)) {
//         printf("Danh sách project không hợp lệ.\n");
//         cJSON_Delete(json);
//         return;
//     }

// printf("\n--- DANH SÁCH PROJECT CỦA USER ID %s ---\n", user_id); 
//     cJSON *project;
//     int index = 1;
//    cJSON_ArrayForEach(project, projects) {
//         const char *project_user_id = cJSON_GetObjectItem(project, "user_id")->valuestring;
//         if (strcmp(project_user_id, user_id) == 0) {
//             const char *name = cJSON_GetObjectItem(project, "name")->valuestring;
//             printf("%d. %s\n", index++, name);
//         }
//     }

//     cJSON_Delete(json);
// }

void display_projects(const char *filename, const char *user_id, int mode) {
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

    if (mode == 1) {
        printf("\n--- DANH SÁCH PROJECT CỦA USER ID %s ---\n", user_id);
    } else if (mode == 2) {
        printf("\n--- DANH SÁCH PROJECT ĐƯỢC GIAO CHO USER ID %s ---\n", user_id);
    }

    cJSON *project;
    int index = 1;
    cJSON_ArrayForEach(project, projects) {
        const char *project_user_id = cJSON_GetObjectItem(project, "user_id")->valuestring;
        cJSON *members = cJSON_GetObjectItem(project, "members");
        int is_member = 0;

        if (cJSON_IsArray(members)) {
            cJSON *member;
            cJSON_ArrayForEach(member, members) {
                const char *member_id = cJSON_GetObjectItem(member, "id")->valuestring;
                if (strcmp(member_id, user_id) == 0) {
                    is_member = 1;
                    break;
                }
            }
        }

        if ((mode == 1 && strcmp(project_user_id, user_id) == 0) || (mode == 2 && is_member)) {
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
//ham tim id tu ten member
const char* get_member_id_by_name(cJSON *members, const char *member_name) {
    cJSON *member;
    cJSON_ArrayForEach(member, members) {
        const char *name = cJSON_GetObjectItem(member, "name")->valuestring;
        if (strcmp(name, member_name) == 0) {
            return cJSON_GetObjectItem(member, "id")->valuestring;
        }
    }
    return NULL;
}

// Hàm quản lý project
void manage_projects(const char *filename, const char *user_id) {
    int choice;
    do {
        printf("\n--- QUẢN LÝ PROJECT ---\n");
        printf("1. Xem project của tôi\n");
        printf("2. Xem project được giao\n");
        printf("3. Tạo project mới\n");
        printf("4. Xem chi tiết project của tôi\n");
        printf("5. Xem chi tiết project được giao\n");        
        printf("0. Quay lại\n");
        printf("Lựa chọn: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                display_projects(filename, user_id, 1);
                break;
            case 2:
                display_projects(filename, user_id, 2);
                break;
            case 3:
                create_project(filename, user_id);
                break;
            case 4:
                view_project_details(filename, user_id, 1);
                break;
            case 5:
                view_project_details(filename, user_id, 2);
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


void view_project_details(const char *filename, const char *user_id, int mode) {
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
        cJSON *members = cJSON_GetObjectItem(project, "members");
        int is_member = 0;

        if (cJSON_IsArray(members)) {
            cJSON *member;
            cJSON_ArrayForEach(member, members) {
                const char *member_id = cJSON_GetObjectItem(member, "id")->valuestring;
                if (strcmp(member_id, user_id) == 0) {
                    is_member = 1;
                    break;
                }
            }
        }

        if ((mode == 1 && strcmp(project_user_id, user_id) == 0) || (mode == 2 && is_member)) {
            if (index == choice) {
                const char *name = cJSON_GetObjectItem(project, "name")->valuestring;
                const char *description = cJSON_GetObjectItem(project, "description")->valuestring;
                const char *project_id = cJSON_GetObjectItem(project, "project_id")->valuestring;
                int projectChoice;
                printf("\n--- CHI TIẾT PROJECT ---\n");
                printf("Tên: %s\n", name);
                printf("Mô tả: %s\n", description);

                if (mode == 1) {
                    do {
                        printf("\n--- TÁC VỤ ---\n");
                        printf("1. Thêm thành viên\n");
                        printf("2. Thêm công việc\n");
                        printf("3. Xem danh sách công việc\n");
                        printf("4. Xem danh sách thành viên\n");
                        printf("5. Xem chi tiết công việc\n");
                        printf("6. Gán công việc cho thành viên\n");
                        printf("7. Chat với thành viên\n");
                        printf("8. Hiển thị biểu đồ Gantt cho project\n");
                        printf("0. Quay lại\n");
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
                             case 6: {
                                char task_name[50], member_name[50];
                                printf("Nhập tên công việc: ");
                                getchar(); // Đọc bỏ ký tự xuống dòng
                                fgets(task_name, sizeof(task_name), stdin);
                                task_name[strcspn(task_name, "\n")] = '\0';

                                printf("Nhập tên thành viên: ");
                                fgets(member_name, sizeof(member_name), stdin);
                                member_name[strcspn(member_name, "\n")] = '\0';
                                char *task_file_content = read_file("../database/task.json");
                                if (!task_file_content) {
                                    printf("Không có task nào được lưu.\n");
                                    break;
                                }
                                cJSON *task_json = cJSON_Parse(task_file_content);
                                if (!task_json) {
                                    printf("Lỗi: Không thể đọc dữ liệu task.\n");
                                    break;
                                }

                                cJSON *tasks = cJSON_GetObjectItem(task_json, "tasks");
                                if (!cJSON_IsArray(tasks)) {
                                    printf("Danh sách task không hợp lệ.\n");
                                    cJSON_Delete(task_json);
                                    break;
                                }
                                free(task_file_content);
                                const char *task_id = get_task_id_by_name(tasks, task_name);

                                const char *member_id = get_member_id_by_name(members, member_name);
                                if (task_id && member_id) {
                                    assign_task_to_member("../database/task.json", task_id, member_id, member_name);
                                } else {
                                    if (!task_id) {
                                        printf("Không tìm thấy công việc với tên: %s\n", task_name);
                                    }
                                    if (!member_id) {
                                        printf("Không tìm thấy thành viên với tên: %s\n", member_name);
                                    }
                                }
                                break;
                            }
                            case 7:
                            case 8:
                                display_gantt_chart("../database/task.json", project_id);
                                break;
                            case 0:
                                manage_projects(filename, user_id);
                                break;
                            default:
                                printf("Lựa chọn không hợp lệ.\n");
                        }
                    } while (projectChoice != 0);
                } else if (mode == 2) {
                    do {
                        printf("\n--- TÁC VỤ ---\n");
                        printf("1. Xem danh sách công việc\n");
                        printf("2. Xem chi tiết công việc\n");
                        printf("3. Cập nhật tiến độ công việc\n");
                        printf("4. Xem danh sách thành viên\n");
                        printf("5. Chat voi thanh vien\n");
                        printf("6. Hiển thị biểu đồ Gantt cho project\n");
                        printf("0. Quay lại\n");
                        printf("Lựa chọn: ");
                        scanf("%d", &projectChoice);
                        switch (projectChoice) {
                            case 1:
                                display_tasks("../database/task.json", project_id);
                                break;
                            case 2:
                                view_task_details("../database/task.json", project_id);
                                break;
                            case 3:
                                update_task_progress("../database/task.json", project_id, user_id);
                                break;
                            case 4:
                                view_members_of_project(filename, user_id, project);
                                break;
                            case 5:
                            case 6:
                                display_gantt_chart("../database/task.json", project_id);
                                break;
                            case 0:
                                manage_projects(filename, user_id);
                                break;
                            default:
                                printf("Lựa chọn không hợp lệ.\n");
                        }
                    } while (projectChoice != 0);
                }

                cJSON_Delete(json);
                return;
            }
            index++;
        }
    }

    printf("Số thứ tự không hợp lệ.\n");
    cJSON_Delete(json);
}