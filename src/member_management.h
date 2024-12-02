#include "cJSON.h"
#include "project_management.h"
#ifndef MEMBER_MANAGER_H
#define MEMBER_MANAGER_H
void project_management(const char *filename, const char *user_id);
void add_member_to_project(const char *filename, const char *user_id, cJSON *project);
#endif