#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Data structures
typedef struct {
    char id[20];
    char name[50];
    char password[50];
    int is_active;
} User;

typedef struct {
    char id[20];
    char name[50];
    char faculty_id[20];
    int seats;
    int enrolled;
} Course;

typedef struct {
    char student_id[50];
    char course_id[50];
} Enrollment;


// Function prototypes
void *handle_client(void *arg);
int authenticate_user(char *username, char *userID, char *password, char *role);
void admin_menu(int client_socket);
void faculty_menu(int client_socket, char *faculty_id);
void student_menu(int client_socket, char *student_id);
void add_student(int client_socket);
void view_student_details(int client_socket);
void add_faculty(int client_socket);
void view_faculty_details(int client_socket);
void activate_student(int client_socket);
void block_student(int client_socket);
void modify_student_details(int client_socket);
void modify_faculty_details(int client_socket);
void view_offering_courses(int client_socket, char *faculty_id);
void add_new_course(int client_socket, char *faculty_id);
void remove_course(int client_socket, char *faculty_id);
void update_course_details(int client_socket, char *faculty_id);
void change_password(int client_socket, char *user_id, char *role);
void view_all_courses(int client_socket);
void enroll_course(int client_socket, char *student_id);
void drop_course(int client_socket, char *student_id);
void view_enrolled_courses(int client_socket, char *student_id);