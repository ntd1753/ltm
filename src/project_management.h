#ifndef PROJECT_MANAGEMENT_H
#define PROJECT_MANAGEMENT_H

void manage_projects(const char *filename, const char *user_id);
void create_project(const char *filename, const char *user_id);
void display_projects(const char *filename, const char *user_id, int mode);
void view_project_details(const char *filename, const char *user_id, int mode);
void register_user(const char *filename);
void add_member_to_project(const char *filename, const char *user_id, cJSON *project, const char *current_project_id);
void view_members_of_project(const char *filename, const char *user_id, cJSON *project);
const char* get_member_id_by_name(cJSON *members, const char *member_name);
void edit_project(const char *filename, const char *project_id);
void chat_with_member(const char *filename, const char *user_id, cJSON *project);

#endif
