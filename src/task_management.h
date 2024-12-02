#ifndef TASK_MANAGEMENT_H
#define TASK_MANAGEMENT_H
int generate_task_id(cJSON *tasks);
void create_task(const char *filename, const char *project_id);
void display_tasks(const char *filename, const char *project_id);
void view_task_details(const char *filename, const char *project_id);
#endif