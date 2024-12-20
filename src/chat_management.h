#ifndef CHAT_MANAGEMENT_H
#define CHAT_MANAGEMENT_H

#include <cJSON.h>
typedef struct {
    int sockfd;
    char project_id[50];
} thread_args_t;
void chat_with_member(const char *filename, const char *user_id, cJSON *project);
int check_membership(const char *user_id, const char *project_id, const char *project_file);

#endif