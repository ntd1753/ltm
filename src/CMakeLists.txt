# Cấu hình tối thiểu của CMake
cmake_minimum_required(VERSION 3.10)

# Tên của dự án
project(json_test_project)

# Thư mục chứa các file nguồn
set(SOURCE_DIR ${PROJECT_SOURCE_DIR})

# Chỉ định các tệp nguồn
set(SOURCES 
    ${SOURCE_DIR}/main.c
    ${SOURCE_DIR}/file_utils.c
    ${SOURCE_DIR}/user_management.c
    ${SOURCE_DIR}/project_management.c
    ${SOURCE_DIR}/member_management.c
    ${SOURCE_DIR}/task_management.c
)

# Đường dẫn tới thư viện cJSON
set(CJSON_DIR ${PROJECT_SOURCE_DIR}/../cJSON)
include_directories(${CJSON_DIR})
set(CJSON_LIB ${CJSON_DIR}/cJSON.c)

# Thêm thư mục chứa các file header
include_directories(${SOURCE_DIR})

# Tạo file thực thi
add_executable(json_test ${SOURCES} ${CJSON_LIB})
