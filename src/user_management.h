#include <cJSON.h>
#ifndef USER_MANAGEMENT_H
#define USER_MANAGEMENT_H
int generate_user_id(const cJSON *users);
void create_account(const char *filename);
int authenticate_user(const char *filename, char *user_id);
void register_user(const char *filename);
#endif
