#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "file_utils.h"
#include <string.h>
#include "project_management.h"

void add_member_to_project(const char *filename, const char *user_id, cJSON *project) {
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

    // Tìm dự án với project_id = 5
    cJSON *target_project = NULL;
    cJSON *project_item = NULL;
    cJSON_ArrayForEach(project_item, projects) {
        cJSON *project_id = cJSON_GetObjectItem(project_item, "project_id");
        if (cJSON_IsString(project_id) && strcmp(project_id->valuestring, "5") == 0) {
            target_project = project_item;
            break;
        }
    }

    if (target_project == NULL) {
        printf("Không tìm thấy dự án với project_id = 5.\n");
        cJSON_Delete(root);
        return;
    }

    // Lấy danh sách thành viên của dự án
    cJSON *members = cJSON_GetObjectItem(target_project, "members");
    if (!cJSON_IsArray(members)) {
        members = cJSON_CreateArray();
        cJSON_AddItemToObject(target_project, "members", members);
    }

    // Thêm thành viên mới
    cJSON *new_member = cJSON_CreateObject();
    cJSON_AddStringToObject(new_member, "name", member_name);
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
    cJSON_Delete(root);

    // Gọi lại hàm manage_projects sau khi cập nhật
    manage_projects(filename, user_id); // Sửa lại cú pháp gọi hàm
}
