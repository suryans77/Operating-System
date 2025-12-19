#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include "headers.h"

// Global variables
sem_t file_lock;

ssize_t safe_send(int sock, const void *buf, size_t len)
{
    size_t total_sent = 0;
    const char *ptr = (const char *)buf;

    while (total_sent < len)
    {
        ssize_t sent = send(sock, ptr + total_sent, len - total_sent, 0);
        if (sent <= 0)
            return -1;
        total_sent += sent;
    }
    return total_sent;
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    pthread_t threads[MAX_CLIENTS];
    int thread_count = 0;

    // Initialize semaphore
    sem_init(&file_lock, 0, 1);

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket to port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, MAX_CLIENTS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server started on port %d\n", PORT);

    // Accept incoming connections
    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("New connection accepted\n");

        // Create a new thread for each client
        if (pthread_create(&threads[thread_count], NULL, handle_client, (void *)&new_socket) < 0)
        {
            perror("could not create thread");
            close(new_socket); // Close the socket if thread creation failed
            continue;
        }

        thread_count++; // Increment the thread count
        if (thread_count >= MAX_CLIENTS)
        {
            // Wait for all threads to finish before accepting more connections
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                pthread_join(threads[i], NULL);
            }
            thread_count = 0; // Reset thread count after joining threads
        }
    }

    // Cleanup and close the server socket
    close(server_fd);
    sem_destroy(&file_lock);
    return 0;
}

void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;

    char buffer[BUFFER_SIZE] = {0};
    char username[50], password[50], role[10], userID[50];
    int auth_result;

    // Get username
    send(client_socket, "Enter username: ", strlen("Enter username: "), 0);
    read(client_socket, buffer, BUFFER_SIZE);
    strcpy(username, buffer);
    memset(buffer, 0, BUFFER_SIZE);

    // Get userID
    send(client_socket, "Enter userID: ", strlen("Enter userID: "), 0);
    read(client_socket, buffer, BUFFER_SIZE);
    strcpy(userID, buffer);
    memset(buffer, 0, BUFFER_SIZE);

    // Get password
    send(client_socket, "Enter password: ", strlen("Enter password: "), 0);
    read(client_socket, buffer, BUFFER_SIZE);
    strcpy(password, buffer);
    memset(buffer, 0, BUFFER_SIZE);

    // Authenticate user
    auth_result = authenticate_user(username, userID, password, role);

    if (auth_result == 1)
    {
        send(client_socket, "Login successful!", strlen("Login successful!"), 0);

        if (strcmp(role, "admin") == 0)
        {
            admin_menu(client_socket);
        }
        else if (strcmp(role, "faculty") == 0)
        {
            faculty_menu(client_socket, userID);
        }
        else if (strcmp(role, "student") == 0)
        {
            student_menu(client_socket, userID);
        }
    }
    else if (auth_result == 0)
    {
        send(client_socket, "Account is inactive. Contact admin.", strlen("Account is inactive. Contact admin."), 0);
    }
    else
    {
        send(client_socket, "Invalid credentials!", strlen("Invalid credentials!"), 0);
    }

    close(client_socket);
    pthread_exit(NULL);
}

int authenticate_user(char *username, char *userID, char *password, char *role)
{
    FILE *fp;
    User user;
    int found = 0;
    int active = 0;

    sem_wait(&file_lock);

    // Check admin first
    fp = fopen("admin.txt", "r");
    if (fp != NULL)
    {
        while (fscanf(fp, "%s %s %s %d", user.id, user.name, user.password, &user.is_active) != EOF)
        {
            if (strcmp(user.id, userID) == 0 && strcmp(user.name, username) == 0 && strcmp(user.password, password) == 0)
            {
                strcpy(role, "admin");
                found = 1;
                active = 1;
                break;
            }
        }
        fclose(fp);
    }

    // Check faculty if not found in admin
    if (!found)
    {
        fp = fopen("faculty.txt", "r");
        if (fp != NULL)
        {
            while (fscanf(fp, "%s %s %s %d", user.id, user.name, user.password, &user.is_active) != EOF)
            {
                if (strcmp(user.id, userID) == 0 && strcmp(user.name, username) == 0 && strcmp(user.password, password) == 0)
                {
                    strcpy(role, "faculty");
                    found = 1;
                    active = user.is_active;
                    break;
                }
            }
            fclose(fp);
        }
    }

    // Check student if not found in faculty
    if (!found)
    {
        fp = fopen("students.txt", "r");
        if (fp != NULL)
        {
            while (fscanf(fp, "%s %s %s %d", user.id, user.name, user.password, &user.is_active) != EOF)
            {
                if (strcmp(user.id, userID) == 0 && strcmp(user.name, username) == 0 && strcmp(user.password, password) == 0)
                {
                    strcpy(role, "student");
                    found = 1;
                    active = user.is_active;
                    break;
                }
            }
            fclose(fp);
        }
    }

    sem_post(&file_lock);

    if (!found)
        return -1; // Invalid credentials
    if (found && !active)
        return 0; // Account inactive
    return 1;     // Valid and active
}

void admin_menu(int client_socket)
{
    int choice;
    char buffer[BUFFER_SIZE];

    const char *menu = "\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n";
    send(client_socket, menu, strlen(menu), 0);

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        read(client_socket, buffer, BUFFER_SIZE);
        choice = atoi(buffer);

        switch (choice)
        {
        case 1:
            add_student(client_socket);
            break;
        case 2:
            view_student_details(client_socket);
            break;
        case 3:
            add_faculty(client_socket);
            break;
        case 4:
            view_faculty_details(client_socket);
            break;
        case 5:
            activate_student(client_socket);
            break;
        case 6:
            block_student(client_socket);
            break;
        case 7:
            modify_student_details(client_socket);
            break;
        case 8:
            modify_faculty_details(client_socket);
            break;
        case 9:
            send(client_socket, "Logging out...", strlen("Logging out..."), 0);
            return;
        default:
            send(client_socket, "Invalid choice!", strlen("Invalid choice!"), 0);
        }
    }
}

void faculty_menu(int client_socket, char *faculty_id)
{
    int choice;
    char buffer[BUFFER_SIZE];

    const char *menu = "\nWelcome to Faculty Menu\n1. View Offered Courses\n2. Add New Course\n3. Remove Course\n4. Update Course Details\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n";
    send(client_socket, menu, strlen(menu), 0);

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        read(client_socket, buffer, BUFFER_SIZE);
        choice = atoi(buffer);

        switch (choice)
        {
        case 1:
            view_offering_courses(client_socket, faculty_id);
            break;
        case 2:
            add_new_course(client_socket, faculty_id);
            break;
        case 3:
            remove_course(client_socket, faculty_id);
            break;
        case 4:
            update_course_details(client_socket, faculty_id);
            break;
        case 5:
            change_password(client_socket, faculty_id, "faculty");
            break;
        case 6:
            send(client_socket, "Logging out...", strlen("Logging out..."), 0);
            return;
        default:
            send(client_socket, "Invalid choice!", strlen("Invalid choice!"), 0);
        }
    }
}

void student_menu(int client_socket, char *student_id)
{
    int choice;
    char buffer[BUFFER_SIZE];

    const char *menu = "\nWelcome to Student Menu\n1. View All Courses\n2. Enroll in Course\n3. Drop Course\n4. View Enrolled Courses\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n";
    send(client_socket, menu, strlen(menu), 0);

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        read(client_socket, buffer, BUFFER_SIZE);
        choice = atoi(buffer);

        switch (choice)
        {
        case 1:
            view_all_courses(client_socket);
            break;
        case 2:
            enroll_course(client_socket, student_id);
            break;
        case 3:
            drop_course(client_socket, student_id);
            break;
        case 4:
            view_enrolled_courses(client_socket, student_id);
            break;
        case 5:
            change_password(client_socket, student_id, "student");
            break;
        case 6:
            send(client_socket, "Logging out...", strlen("Logging out..."), 0);
            return;
        default:
            send(client_socket, "Invalid choice!", strlen("Invalid choice!"), 0);
        }
    }
}
