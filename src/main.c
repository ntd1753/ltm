#include <stdio.h>
#include "user_management.h"
#include "project_management.h"

int main() {
    const char *user_file = "../database/users.json";
    const char *project_file = "../database/project.json";
    char user_id[10]; // Đảm bảo đủ dung lượng để chứa user_id
    int choice;
    do {
        printf("\nChọn chức năng:\n");
        printf("1. Tạo tài khoản mới\n");
        printf("2. Đăng nhập\n");
        printf("0. Thoát\n");
        printf("Lựa chọn: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                register_user(user_file);
                break;
            case 2:
                if (authenticate_user(user_file, user_id)) {
                    printf("\nĐăng nhập thành công!\n");
                    manage_projects(project_file, user_id); // Chuyển sang quản lý project
                } else {
                    printf("\nĐăng nhập thất bại. Vui lòng thử lại.\n");
                }
                break;
            case 0:
                printf("Thoát chương trình.\n");
                break;
            default:
                printf("Lựa chọn không hợp lệ.\n");
        }
    } while (choice != 0);

    return 0;
}
