#ifndef PROJECT_MANAGEMENT_H
#define PROJECT_MANAGEMENT_H

void manage_projects(const char *filename, const char *user_id);
void create_project(const char *filename, const char *user_id);
void display_projects(const char *filename, const char *user_id);
void view_project_details(const char *filename, const char *user_id);
void register_user(const char *filename);
void add_member_to_project(const char *filename, const char *user_id, cJSON *project);
void view_members_of_project(const char *filename, const char *user_id, cJSON *project);

#endif
