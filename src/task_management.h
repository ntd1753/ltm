#ifndef TASK_MANAGEMENT_H
#define TASK_MANAGEMENT_H
int generate_task_id(cJSON *tasks);
void create_task(const char *filename, const char *project_id);
void display_tasks(const char *filename, const char *project_id);
void view_task_details_for_manager(const char *filename, const char *project_id);
void assign_task_to_member(const char *filename, const char *task_id, const char *member_id, const char *member_name);
const char* get_task_id_by_name(cJSON *tasks, const char *task_name);
void update_task_progress(const char *filename, const char *project_id, const char *user_id);
int days_between(const char *start_date, const char *end_date);
void display_gantt_chart(const char *filename, const char *project_id);
void view_task_details_for_member(const char *filename, const char *project_id);
void edit_task(const char *filename, const char *task_id);
void delete_task(const char *filename, const char *task_id);

#endif