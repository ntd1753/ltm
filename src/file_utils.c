#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Hàm đọc nội dung file vào chuỗi
char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Không thể mở file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *content = (char *)malloc(length + 1);
    if (!content) {
        perror("Không đủ bộ nhớ");
        fclose(file);
        return NULL;
    }

    fread(content, 1, length, file);
    content[length] = '\0';

    fclose(file);
    return content;
}

// Hàm ghi dữ liệu vào file
int write_file(const char *filename, const char *data) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Không thể mở file để ghi");
        return 0;
    }

    fwrite(data, 1, strlen(data), file);
    fclose(file);
    return 1;
}
