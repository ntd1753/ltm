#ifndef PROJECT_MANAGEMENT_H
#define PROJECT_MANAGEMENT_H

void manage_projects(const char *filename, const char *user_id);
void create_project(const char *filename, const char *user_id);
void display_projects(const char *filename, const char *user_id);
void view_project_details(const char *filename, const char *user_id);
void register_user(const char *filename);
#endif
