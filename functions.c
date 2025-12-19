
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include "headers.h"
#include <fcntl.h>

extern sem_t file_lock;

void send_msg(int client_socket, const char *msg)
{
    send(client_socket, msg, strlen(msg), 0);
}

void add_student(int client_socket)
{
    User student;
    char buffer[BUFFER_SIZE];
    int fd;

    memset(buffer, 0, BUFFER_SIZE);

    // Prompt and receive Student ID
    write(client_socket, "Enter Student ID: ", 18);
    read(client_socket, buffer, sizeof(buffer));
    strcpy(student.id, buffer);
    student.id[strcspn(student.id, "\n")] = 0;

    memset(buffer, 0, BUFFER_SIZE);

    // Prompt and receive Student Name
    write(client_socket, "Enter Student Name: ", 20);
    read(client_socket, buffer, sizeof(buffer));
    strcpy(student.name, buffer);
    student.name[strcspn(student.name, "\n")] = 0;

    memset(buffer, 0, BUFFER_SIZE);

    // Prompt and receive Password
    write(client_socket, "Enter Password: ", 16);
    read(client_socket, buffer, sizeof(buffer));
    strcpy(student.password, buffer);
    student.password[strcspn(student.password, "\n")] = 0;

    student.is_active = 1;

    // Check for existing student
    sem_wait(&file_lock);
    fd = open("students.txt", O_RDONLY);
    if (fd >= 0)
    {
        char read_buf[BUFFER_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(fd, read_buf, sizeof(read_buf) - 1)) > 0)
        {
            read_buf[bytes_read] = '\0';
            char *line = strtok(read_buf, "\n");
            while (line)
            {
                char existing_id[50];
                sscanf(line, "%s", existing_id);
                if (strcmp(existing_id, student.id) == 0)
                {
                    write(client_socket, "Error: Student ID already exists.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 268);
                    close(fd);
                    sem_post(&file_lock);
                    return;
                }
                line = strtok(NULL, "\n");
            }
        }
        close(fd);
    }

    // Write to student.txt
    fd = open("students.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0)
    {
        write(client_socket, "Error opening student file.\n", 28);
        sem_post(&file_lock);
        return;
    }

    memset(buffer, 0, BUFFER_SIZE);
    // Format: Student_id name password is_active
    snprintf(buffer, sizeof(buffer), "%s %s %s %d\n", student.id, student.name, student.password, student.is_active);
    write(fd, buffer, strlen(buffer));
    close(fd);
    sem_post(&file_lock);

    write(client_socket, "Student added successfully.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 262);

}

void view_student_details(int client_socket)
{
    char buffer[BUFFER_SIZE], response[BUFFER_SIZE * 10] = "";
    User student;

    sem_wait(&file_lock);
    FILE *fp = fopen("students.txt", "r");
    if (fp)
    {
        while (fscanf(fp, "%s %s %s %d", student.id, student.name, student.password, &student.is_active) != EOF)
        {
            sprintf(buffer, "ID: %s, Name: %s, Active: %d\n", student.id, student.name, student.is_active);
            strcat(response, buffer);
        }
        fclose(fp);
    }
    else
    {
        strcat(response, "Could not open student file.\n");
    }
    sem_post(&file_lock);

    send_msg(client_socket, response);

    const char *menu = "\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n";
    send(client_socket, menu, strlen(menu), 0);
}

void add_faculty(int client_socket)
{
    User faculty;
    char buffer[BUFFER_SIZE];
    int fd;

    memset(buffer, 0, BUFFER_SIZE);

    // Prompt and receive Faculty ID
    write(client_socket, "Enter Faculty ID: ", 18);
    read(client_socket, buffer, sizeof(buffer));
    strcpy(faculty.id, buffer);
    faculty.id[strcspn(faculty.id, "\n")] = 0;

    memset(buffer, 0, BUFFER_SIZE);

    // Prompt and receive Faculty Name
    write(client_socket, "Enter Faculty Name: ", 20);
    read(client_socket, buffer, sizeof(buffer));
    strcpy(faculty.name, buffer);
    faculty.name[strcspn(faculty.name, "\n")] = 0;

    memset(buffer, 0, BUFFER_SIZE);

    // Prompt and receive Password
    write(client_socket, "Enter Password: ", 16);
    read(client_socket, buffer, sizeof(buffer));
    strcpy(faculty.password, buffer);
    faculty.password[strcspn(faculty.password, "\n")] = 0;

    faculty.is_active = 1;

    // Check for existing faculty
    sem_wait(&file_lock);
    fd = open("faculty.txt", O_RDONLY);
    if (fd >= 0)
    {
        char read_buf[BUFFER_SIZE];
        ssize_t bytes_read;
        while ((bytes_read = read(fd, read_buf, sizeof(read_buf) - 1)) > 0)
        {
            read_buf[bytes_read] = '\0';
            char *line = strtok(read_buf, "\n");
            while (line)
            {
                char existing_id[50];
                sscanf(line, "%s", existing_id);
                if (strcmp(existing_id, faculty.id) == 0)
                {
                    write(client_socket, "Error: Faculty ID already exists.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 268);
                    close(fd);
                    sem_post(&file_lock);
                    return;
                }
                line = strtok(NULL, "\n");
            }
        }
        close(fd);
    }

    // Write to faculty.txt
    fd = open("faculty.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0)
    {
        write(client_socket, "Error opening faculty file.\n", 28);
        sem_post(&file_lock);
        return;
    }

    memset(buffer, 0, BUFFER_SIZE);
    // Format: faculty_id name password is_active
    snprintf(buffer, sizeof(buffer), "%s %s %s %d\n", faculty.id, faculty.name, faculty.password, faculty.is_active);
    write(fd, buffer, strlen(buffer));
    close(fd);
    sem_post(&file_lock);

    write(client_socket, "Faculty added successfully.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 262);
}

void view_faculty_details(int client_socket)
{
    char buffer[BUFFER_SIZE], response[BUFFER_SIZE * 10] = "";
    User faculty;

    sem_wait(&file_lock);
    FILE *fp = fopen("faculty.txt", "r");
    if (fp)
    {
        while (fscanf(fp, "%s %s %s %d", faculty.id, faculty.name, faculty.password, &faculty.is_active) != EOF)
        {
            sprintf(buffer, "ID: %s, Name: %s, Active: %d\n", faculty.id, faculty.name, faculty.is_active);
            strcat(response, buffer);
        }
        fclose(fp);
    }
    else
    {
        strcat(response, "Could not open faculty file.\n");
    }
    sem_post(&file_lock);

    send_msg(client_socket, response);

    const char *menu = "\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n";
    send(client_socket, menu, strlen(menu), 0);
}

void activate_student(int client_socket)
{
    char student_id[50], buffer[BUFFER_SIZE];
    int found = 0;
    int fd, temp_fd;

    // Request student ID
    write(client_socket, "Enter Student ID to activate: ", 29);
    read(client_socket, student_id, sizeof(student_id));
    student_id[strcspn(student_id, "\n")] = 0;

    sem_wait(&file_lock);

    // Open the original students file and a temporary file
    fd = open("students.txt", O_RDONLY);
    temp_fd = open("temp_students.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0 || temp_fd < 0)
    {
        write(client_socket, "Error accessing student data.\n", 30);
        sem_post(&file_lock);
        return;
    }

    User student;
    ssize_t bytes_read;
    char read_buf[BUFFER_SIZE];

    // Read file content and process each line
    while ((bytes_read = read(fd, read_buf, sizeof(read_buf) - 1)) > 0)
    {
        read_buf[bytes_read] = '\0';
        char *line = strtok(read_buf, "\n");
        while (line)
        {
            sscanf(line, "%s %s %s %d", student.id, student.name, student.password, &student.is_active);
            if (strcmp(student.id, student_id) == 0)
            {
                if (student.is_active == 1)
                {
                    // Student is already active
                    write(client_socket, "Student account is already active.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 269);
                    sem_post(&file_lock);
                    close(fd);
                    close(temp_fd);
                    return;
                }
                student.is_active = 1; // Activate the student
                found = 1;
            }
            // Write the student data to the temporary file
            snprintf(buffer, sizeof(buffer), "%s %s %s %d\n", student.id, student.name, student.password, student.is_active);
            write(temp_fd, buffer, strlen(buffer));
            line = strtok(NULL, "\n");
        }
    }

    close(fd);
    close(temp_fd);

    // Replace original file with updated one
    remove("students.txt");
    rename("temp_students.txt", "students.txt");

    sem_post(&file_lock);

    if (found)
    {
        write(client_socket, "Student account activated.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 261);
    }
    else
    {
        write(client_socket, "Student ID not found.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 256);
    }
}

void block_student(int client_socket)
{
    char student_id[50], buffer[BUFFER_SIZE];
    int found = 0;
    int fd, temp_fd;

    // Request student ID
    write(client_socket, "Enter Student ID to block: ", 26);
    read(client_socket, student_id, sizeof(student_id));
    student_id[strcspn(student_id, "\n")] = 0;

    sem_wait(&file_lock);

    // Open the original students file and a temporary file
    fd = open("students.txt", O_RDONLY);
    temp_fd = open("temp_students.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0 || temp_fd < 0)
    {
        write(client_socket, "Error accessing student data.\n", 30);
        sem_post(&file_lock);
        return;
    }

    User student;
    ssize_t bytes_read;
    char read_buf[BUFFER_SIZE];

    // Read file content and process each line
    while ((bytes_read = read(fd, read_buf, sizeof(read_buf) - 1)) > 0)
    {
        read_buf[bytes_read] = '\0';
        char *line = strtok(read_buf, "\n");
        while (line)
        {
            sscanf(line, "%s %s %s %d", student.id, student.name, student.password, &student.is_active);
            if (strcmp(student.id, student_id) == 0)
            {
                if (student.is_active == 0)
                {
                    // Student is already blocked
                    write(client_socket, "Student account is already blocked.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 269);
                    sem_post(&file_lock);
                    close(fd);
                    close(temp_fd);
                    return;
                }
                student.is_active = 0; // Block the student
                found = 1;
            }
            // Write the student data to the temporary file
            snprintf(buffer, sizeof(buffer), "%s %s %s %d\n", student.id, student.name, student.password, student.is_active);
            write(temp_fd, buffer, strlen(buffer));
            line = strtok(NULL, "\n");
        }
    }

    close(fd);
    close(temp_fd);

    // Replace original file with updated one
    remove("students.txt");
    rename("temp_students.txt", "students.txt");

    sem_post(&file_lock);

    if (found)
    {
        write(client_socket, "Student account blocked.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 258);
    }
    else
    {
        write(client_socket, "Student ID not found.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 256);
    }
}

void modify_student_details(int client_socket)
{
    char student_id[50], buffer[BUFFER_SIZE];
    int found = 0;
    int fd, temp_fd;

    // Request student ID
    write(client_socket, "Enter Student ID to modify: ", 28);
    memset(student_id, 0, sizeof(student_id));
    read(client_socket, student_id, sizeof(student_id));
    student_id[strcspn(student_id, "\n")] = 0;

    sem_wait(&file_lock);

    fd = open("students.txt", O_RDONLY);
    temp_fd = open("temp_students.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0 || temp_fd < 0)
    {
        write(client_socket, "Error accessing student data.\n", 31);
        if (fd >= 0)
            close(fd);
        if (temp_fd >= 0)
            close(temp_fd);
        sem_post(&file_lock);
        return;
    }

    User student;
    char line_buf[BUFFER_SIZE];
    int i = 0;
    char ch;
    ssize_t n;

    // Read line-by-line
    while ((n = read(fd, &ch, 1)) > 0)
    {
        if (ch == '\n' || i >= BUFFER_SIZE - 1)
        {
            line_buf[i] = '\0';
            i = 0;

            if (sscanf(line_buf, "%s %s %s %d", student.id, student.name, student.password, &student.is_active) == 4)
            {
                if (strcmp(student.id, student_id) == 0)
                {
                    found = 1;

                    write(client_socket, "Enter new Student Name: ", 25);
                    memset(student.name, 0, sizeof(student.name));
                    read(client_socket, student.name, sizeof(student.name));
                    student.name[strcspn(student.name, "\n")] = 0;

                    write(client_socket, "Enter new Password: ", 20);
                    memset(student.password, 0, sizeof(student.password));
                    read(client_socket, student.password, sizeof(student.password));
                    student.password[strcspn(student.password, "\n")] = 0;
                }

                snprintf(buffer, sizeof(buffer), "%s %s %s %d\n", student.id, student.name, student.password, student.is_active);
                write(temp_fd, buffer, strlen(buffer));
            }
        }
        else
        {
            line_buf[i++] = ch;
        }
    }

    close(fd);
    close(temp_fd);

    remove("students.txt");
    rename("temp_students.txt", "students.txt");

    sem_post(&file_lock);

    if (found)
    {
        write(client_socket, "Student details modified successfully.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 273);
    }
    else
    {
        write(client_socket, "Student ID not found.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 256);
    }
}

void modify_faculty_details(int client_socket)
{
    char faculty_id[50], buffer[BUFFER_SIZE];
    int found = 0;
    int fd, temp_fd;

    write(client_socket, "Enter Faculty ID to modify: ", 28);
    memset(faculty_id, 0, sizeof(faculty_id));
    read(client_socket, faculty_id, sizeof(faculty_id));
    faculty_id[strcspn(faculty_id, "\n")] = 0;

    sem_wait(&file_lock);

    fd = open("faculty.txt", O_RDONLY);
    temp_fd = open("temp_faculty.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd < 0 || temp_fd < 0)
    {
        write(client_socket, "Error accessing faculty data.\n", 30);
        if (fd >= 0)
            close(fd);
        if (temp_fd >= 0)
            close(temp_fd);
        sem_post(&file_lock);
        return;
    }

    User faculty;
    char line_buf[BUFFER_SIZE];
    int i = 0;
    char ch;
    ssize_t n;

    // Read line-by-line manually
    while ((n = read(fd, &ch, 1)) > 0)
    {
        if (ch == '\n' || i >= BUFFER_SIZE - 1)
        {
            line_buf[i] = '\0';
            i = 0;

            // Parse the line into a faculty record
            if (sscanf(line_buf, "%s %s %s %d", faculty.id, faculty.name, faculty.password, &faculty.is_active) == 4)
            {
                if (strcmp(faculty.id, faculty_id) == 0)
                {
                    found = 1;
                    write(client_socket, "Enter new Faculty Name: ", 24);
                    memset(faculty.name, 0, sizeof(faculty.name));
                    read(client_socket, faculty.name, sizeof(faculty.name));
                    faculty.name[strcspn(faculty.name, "\n")] = 0;

                    write(client_socket, "Enter new Password: ", 20);
                    memset(faculty.password, 0, sizeof(faculty.password));
                    read(client_socket, faculty.password, sizeof(faculty.password));
                    faculty.password[strcspn(faculty.password, "\n")] = 0;
                }

                snprintf(buffer, sizeof(buffer), "%s %s %s %d\n", faculty.id, faculty.name, faculty.password, faculty.is_active);
                write(temp_fd, buffer, strlen(buffer));
            }
        }
        else
        {
            line_buf[i++] = ch;
        }
    }

    close(fd);
    close(temp_fd);

    remove("faculty.txt");
    rename("temp_faculty.txt", "faculty.txt");

    sem_post(&file_lock);

    if (found)
    {
        write(client_socket, "Faculty details modified successfully.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 272);
    }
    else
    {
        write(client_socket, "Faculty ID not found.\n\nWelcome to Admin Menu\n1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n6. Block Student\n7. Modify Student Details\n8. Modify Faculty Details\n9. Logout and Exit\nEnter Your Choice Below\n", 256);
    }
}

void view_offering_courses(int client_socket, char *faculty_id)
{
    int fd;
    char buffer[BUFFER_SIZE], line[BUFFER_SIZE];
    Course course;
    ssize_t bytes_read;
    int line_pos = 0;

    sem_wait(&file_lock);

    fd = open("courses.txt", O_RDONLY);
    if (fd < 0)
    {
        write(client_socket, "Error opening courses file.\n", 29);
        sem_post(&file_lock);
        return;
    }

    write(client_socket, "Courses Offered by You:\n", 25);

    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytes_read] = '\0'; // Null-terminate for safety
        int i = 0;

        while (i < bytes_read)
        {
            if (buffer[i] == '\n' || line_pos >= BUFFER_SIZE - 1)
            {
                line[line_pos] = '\0'; // Complete the line

                // Parse only if the line is non-empty
                if (line_pos > 0)
                {
                    if (sscanf(line, "%s %s %s %d %d", course.id, course.name, course.faculty_id, &course.seats, &course.enrolled) == 5)
                    {
                        if (strcmp(course.faculty_id, faculty_id) == 0)
                        {
                            char output[BUFFER_SIZE];
                            snprintf(output, sizeof(output),
                                     "Course ID: %s, Name: %s, Seats: %d, Enrolled: %d\n",
                                     course.id, course.name, course.seats, course.enrolled);
                            write(client_socket, output, strlen(output));
                        }
                    }
                }
                line_pos = 0; // Reset for the next line
                i++;
            }
            else
            {
                line[line_pos++] = buffer[i++];
            }
        }
    }

    // Handle any remaining partial line
    if (line_pos > 0)
    {
        line[line_pos] = '\0';
        if (sscanf(line, "%s %s %s %d %d", course.id, course.name, course.faculty_id, &course.seats, &course.enrolled) == 5)
        {
            if (strcmp(course.faculty_id, faculty_id) == 0)
            {
                char output[BUFFER_SIZE];
                snprintf(output, sizeof(output),
                         "Course ID: %s, Name: %s, Seats: %d, Enrolled: %d\n",
                         course.id, course.name, course.seats, course.enrolled);
                write(client_socket, output, strlen(output));
            }
        }
    }

    close(fd);
    sem_post(&file_lock);

    const char *menu = "\nWelcome to Faculty Menu\n1. View Offered Courses\n2. Add New Course\n3. Remove Course\n4. Update Course Details\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n";
    write(client_socket, menu, strlen(menu));
}
void add_new_course(int client_socket, char *faculty_id)
{
    char buffer[BUFFER_SIZE];
    Course course;
    int fd;

    // Prompt and receive Course ID
    write(client_socket, "Enter Course ID: ", 17);
    memset(course.id, 0, sizeof(course.id));
    read(client_socket, course.id, sizeof(course.id));
    course.id[strcspn(course.id, "\n")] = 0;

    // Prompt and receive Course Name
    write(client_socket, "Enter Course Name: ", 19);
    memset(course.name, 0, sizeof(course.name));
    read(client_socket, course.name, sizeof(course.name));
    course.name[strcspn(course.name, "\n")] = 0;

    // Prompt and receive number of seats
    write(client_socket, "Enter number of seats: ", 23);
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer));
    course.seats = atoi(buffer);
    course.enrolled = 0;

    // Set faculty_id
    strncpy(course.faculty_id, faculty_id, sizeof(course.faculty_id) - 1);
    course.faculty_id[sizeof(course.faculty_id) - 1] = '\0';

    // Check for existing course using raw system calls
    sem_wait(&file_lock);
    fd = open("courses.txt", O_RDONLY);
    if (fd >= 0)
    {
        char read_buf[BUFFER_SIZE];
        ssize_t bytes_read;
        int start = 0;

        while ((bytes_read = read(fd, read_buf + start, BUFFER_SIZE - start - 1)) > 0)
        {
            bytes_read += start;
            read_buf[bytes_read] = '\0';

            char *line = strtok(read_buf, "\n");
            while (line)
            {
                char existing_id[50];
                sscanf(line, "%s", existing_id);
                if (strcmp(existing_id, course.id) == 0)
                {
                    write(client_socket, "Error: Course ID already exists.\n\nWelcome to Faculty Menu\n1. View Offered Courses\n2. Add New Course\n3. Remove Course\n4. Update Course Details\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 205);
                    close(fd);
                    sem_post(&file_lock);
                    return;
                }
                line = strtok(NULL, "\n");
            }

            // This version assumes course IDs do not cross buffer boundaries.
            // For long lines, you’d need to implement a partial-line carryover.
        }
        close(fd);
    }

    // Append new course to file
    fd = open("courses.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (fd < 0)
    {
        write(client_socket, "Error opening course file.\n", 28);
        sem_post(&file_lock);
        return;
    }

    // Format: course_id name faculty_id seats enrolled
    snprintf(buffer, sizeof(buffer), "%s %s %s %d %d\n",
             course.id, course.name, course.faculty_id,
             course.seats, course.enrolled);

    write(fd, buffer, strlen(buffer));
    close(fd);
    sem_post(&file_lock);

    write(client_socket, "Course added successfully.\n\nWelcome to Faculty Menu\n1. View Offered Courses\n2. Add New Course\n3. Remove Course\n4. Update Course Details\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 199);
}

void remove_course(int client_socket, char *faculty_id)
{
    char buffer[BUFFER_SIZE];
    char course_id[20];
    Course course;

    // Prompt for Course ID
    write(client_socket, "Enter Course ID to remove: ", 27);
    read(client_socket, course_id, sizeof(course_id));
    course_id[strcspn(course_id, "\n")] = 0;

    sem_wait(&file_lock);

    int fd_old = open("courses.txt", O_RDONLY);
    int fd_new = open("temp_courses.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd_old < 0 || fd_new < 0)
    {
        write(client_socket, "Error opening course file.\n", 28);
        if (fd_old >= 0)
            close(fd_old);
        if (fd_new >= 0)
            close(fd_new);
        sem_post(&file_lock);
        return;
    }

    int found = 0;
    ssize_t bytes_read;
    char ch, line[BUFFER_SIZE];
    int idx = 0;

    while ((bytes_read = read(fd_old, &ch, 1)) > 0)
    {
        if (ch == '\n')
        {
            line[idx] = '\0';

            // Parse line into course struct
            sscanf(line, "%s %s %s %d %d", course.id, course.name,
                   course.faculty_id, &course.seats, &course.enrolled);

            // Check if it's the course to remove
            if (strcmp(course.id, course_id) == 0 &&
                strcmp(course.faculty_id, faculty_id) == 0)
            {
                found = 1;
                // Skip writing this line
            }
            else
            {
                strcat(line, "\n");
                write(fd_new, line, strlen(line));
            }

            idx = 0;
        }
        else
        {
            line[idx++] = ch;
        }
    }

    close(fd_old);
    close(fd_new);

    // Replace old file with new one
    rename("temp_courses.txt", "courses.txt");

    sem_post(&file_lock);

    if (found)
    {
        write(client_socket, "Course removed successfully.\n\nWelcome to Faculty Menu\n1. View Offered Courses\n2. Add New Course\n3. Remove Course\n4. Update Course Details\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 201);
    }
    else
    {
        write(client_socket, "Course not found or unauthorized.\n\nWelcome to Faculty Menu\n1. View Offered Courses\n2. Add New Course\n3. Remove Course\n4. Update Course Details\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 205);
    }
}

void update_course_details(int client_socket, char *faculty_id)
{
    char buffer[BUFFER_SIZE], course_id[20];
    Course course;
    int found = 0;

    // Get course ID to update
    write(client_socket, "Enter Course ID to update: ", 28);
    read(client_socket, course_id, sizeof(course_id));
    course_id[strcspn(course_id, "\n")] = '\0';

    sem_wait(&file_lock);

    int fd = open("courses.txt", O_RDONLY);
    int temp_fd = open("temp_courses.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0 || temp_fd < 0)
    {
        write(client_socket, "Error opening course file.\n", 28);
        if (fd >= 0)
            close(fd);
        if (temp_fd >= 0)
            close(temp_fd);
        sem_post(&file_lock);
        return;
    }

    char line[BUFFER_SIZE];
    int line_pos = 0;
    char ch;
    ssize_t bytes_read;

    while ((bytes_read = read(fd, &ch, 1)) > 0)
    {
        if (ch == '\n')
        {
            line[line_pos] = '\0';
            line_pos = 0;

            // Parse the line (assuming format: ID NAME FACULTY_ID SEATS ENROLLED)
            if (sscanf(line, "%s %s %s %d %d",
                       course.id, course.name, course.faculty_id,
                       &course.seats, &course.enrolled) != 5)
            {
                // Invalid line format - skip it
                write(temp_fd, line, strlen(line));
                write(temp_fd, "\n", 1);
                continue;
            }

            if (strcmp(course.id, course_id) == 0 &&
                strcmp(course.faculty_id, faculty_id) == 0)
            {
                found = 1;

                memset(buffer, 0, BUFFER_SIZE);

                // Get new course name
                write(client_socket, "Enter new course name: ", 24);
                read(client_socket, buffer, sizeof(buffer));
                buffer[strcspn(buffer, "\n")] = '\0';
                strncpy(course.name, buffer, sizeof(course.name));

                memset(buffer, 0, BUFFER_SIZE);

                // Get new seat count
                write(client_socket, "Enter new seat count: ", 23);
                read(client_socket, buffer, sizeof(buffer));
                int new_seats = atoi(buffer);

                if (new_seats < course.enrolled)
                {
                    write(client_socket, "Error: Cannot reduce seats below enrolled count.\n\nWelcome to Faculty Menu\n1. View Offered Courses\n2. Add New Course\n3. Remove Course\n4. Update Course Details\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 220);
                    // Write original line
                    write(temp_fd, line, strlen(line));
                    write(temp_fd, "\n", 1);
                    continue;
                }
                course.seats = new_seats;

                memset(buffer, 0, BUFFER_SIZE);

                // Format updated line
                snprintf(buffer, sizeof(buffer), "%s %s %s %d %d\n",
                         course.id, course.name, course.faculty_id,
                         course.seats, course.enrolled);
                write(temp_fd, buffer, strlen(buffer));
            }
            else
            {
                // Write original line
                write(temp_fd, line, strlen(line));
                write(temp_fd, "\n", 1);
            }
        }
        else
        {
            line[line_pos++] = ch;
            if (line_pos >= BUFFER_SIZE - 1)
            {
                // Handle buffer overflow (though unlikely with course records)
                line_pos = BUFFER_SIZE - 1;
            }
        }
    }

    close(fd);
    close(temp_fd);

    // Atomic file replacement
    unlink("courses.txt");
    rename("temp_courses.txt", "courses.txt");

    sem_post(&file_lock);

    if (found)
    {
        write(client_socket, "Course updated successfully.\n\nWelcome to Faculty Menu\n1. View Offered Courses\n2. Add New Course\n3. Remove Course\n4. Update Course Details\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 201);
    }
    else
    {
        write(client_socket, "Error: Course not found or unauthorized.\n\nWelcome to Faculty Menu\n1. View Offered Courses\n2. Add New Course\n3. Remove Course\n4. Update Course Details\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 211);
    }
}

void change_password(int client_socket, char *user_id, char *role)
{
    char buffer[BUFFER_SIZE];
    char file_buffer[BUFFER_SIZE]; // For reading file content
    char filename[32], line[BUFFER_SIZE];
    int found = 0;

    if (strcmp(role, "admin") == 0)
        strcpy(filename, "admin.txt");
    else if (strcmp(role, "faculty") == 0)
    {
        strcpy(filename, "faculty.txt");
    }
    else if (strcmp(role, "student") == 0)
    {
        strcpy(filename, "students.txt");
    }
    else
    {
        write(client_socket, "Invalid role specified.\n", 25);
        return;
    }

    memset(buffer, 0, BUFFER_SIZE);

    // Prompt for new password
    write(client_socket, "Enter new password: ", 21);
    read(client_socket, buffer, sizeof(buffer));
    buffer[strcspn(buffer, "\n")] = 0;
    char new_password[50];
    strncpy(new_password, buffer, sizeof(new_password));

    sem_wait(&file_lock);

    int fd = open(filename, O_RDONLY);
    int temp_fd = open("temp_users.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0 || temp_fd < 0)
    {
        write(client_socket, "Error opening user file.\n", 26);
        if (fd >= 0)
            close(fd);
        if (temp_fd >= 0)
            close(temp_fd);
        sem_post(&file_lock);
        return;
    }

    // Read the entire file content
    ssize_t total_read = read(fd, file_buffer, sizeof(file_buffer) - 1);
    file_buffer[total_read] = '\0';

    char *line_ptr = strtok(file_buffer, "\n");
    while (line_ptr)
    {
        char id[50], name[50], password[50];
        int is_active;

        sscanf(line_ptr, "%s %s %s %d", id, name, password, &is_active);

        if (strcmp(id, user_id) == 0)
        {
            found = 1;
            snprintf(line, sizeof(line), "%s %s %s %d\n", id, name, new_password, is_active);
        }
        else
        {
            snprintf(line, sizeof(line), "%s\n", line_ptr);
        }

        write(temp_fd, line, strlen(line));
        line_ptr = strtok(NULL, "\n");
    }

    close(fd);
    close(temp_fd);

    rename("temp_users.txt", filename);
    sem_post(&file_lock);

    if (found && strcmp(role, "faculty") == 0)
    {
        write(client_socket, "Password updated successfully.\n\nWelcome to Faculty Menu\n1. View Offered Courses\n2. Add New Course\n3. Remove Course\n4. Update Course Details\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 203);
    }
    else if(found && strcmp(role, "student") == 0){
        write(client_socket, "Password updated successfully.\n\nWelcome to Student Menu\n1. View All Courses\n2. Enroll in Course\n3. Drop Course\n4. View Enrolled Courses\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n",199);
    }
    else
        write(client_socket, "User not found.\n", 17);
}

void view_all_courses(int client_socket)
{
    char buffer[BUFFER_SIZE];

    sem_wait(&file_lock);

    int fd = open("courses.txt", O_RDONLY);
    if (fd < 0)
    {
        write(client_socket, "Error opening courses file.\n", 29);
        sem_post(&file_lock);
        return;
    }

    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
    {
        write(client_socket, buffer, bytes_read);
    }

    close(fd);
    sem_post(&file_lock);

    const char *menu = "\nWelcome to Student Menu\n1. View All Courses\n2. Enroll in Course\n3. Drop Course\n4. View Enrolled Courses\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n";
    write(client_socket, menu, strlen(menu));
}

void enroll_course(int client_socket, char *student_id)
{
    char course_id[20], buffer[BUFFER_SIZE];
    Course c;
    int found = 0;

    // Prompt user
    write(client_socket, "Enter Course ID to enroll: ", 28);
    ssize_t n = read(client_socket, course_id, sizeof(course_id) - 1);
    if (n <= 0)
    {
        write(client_socket, "Invalid input.\n", 15);
        return;
    }
    course_id[n] = '\0';
    course_id[strcspn(course_id, "\r\n")] = '\0'; // strip newline(s)

    sem_wait(&file_lock);

    // Check if student already enrolled in this course
    int efd = open("enrollments.txt", O_RDONLY);
    if (efd >= 0)
    {
        char ch, line[BUFFER_SIZE];
        int idx = 0;
        while (read(efd, &ch, 1) > 0)
        {
            if (ch == '\n')
            {
                line[idx] = '\0';
                char s_id[20], c_id[20];
                if (sscanf(line, "%19s %19s", s_id, c_id) == 2)
                {
                    if (strcmp(s_id, student_id) == 0 && strcmp(c_id, course_id) == 0)
                    {
                        write(client_socket, "Already enrolled in this course.\n\nWelcome to Student Menu\n1. View All Courses\n2. Enroll in Course\n3. Drop Course\n4. View Enrolled Courses\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 201);
                        close(efd);
                        sem_post(&file_lock);
                        return;
                    }
                }
                idx = 0;
            }
            else if (idx < BUFFER_SIZE - 1)
            {
                line[idx++] = ch;
            }
        }
        close(efd);
    }

    // Open courses file and temporary file for updating
    int cfd = open("courses.txt", O_RDONLY);
    int tfd = open("courses_tmp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (cfd < 0 || tfd < 0)
    {
        write(client_socket, "Error opening course file.\n\nWelcome to Student Menu\n1. View All Courses\n2. Enroll in Course\n3. Drop Course\n4. View Enrolled Courses\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 195);
        if (cfd >= 0)
            close(cfd);
        if (tfd >= 0)
            close(tfd);
        sem_post(&file_lock);
        return;
    }

    char ch, line[BUFFER_SIZE];
    int idx = 0;
    while (read(cfd, &ch, 1) > 0)
    {
        if (ch == '\n')
        {
            line[idx] = '\0';

            // Parse the course line safely
            int ret = sscanf(line, "%19s %49s %19s %d %d",
                             c.id, c.name, c.faculty_id, &c.seats, &c.enrolled);
            if (ret != 5)
            {
                // malformed line, copy as is
                write(tfd, line, strlen(line));
                write(tfd, "\n", 1);
            }
            else if (strcmp(c.id, course_id) == 0)
            {
                found = 1;
                if (c.enrolled < c.seats)
                {
                    c.enrolled++;
                    char newline[BUFFER_SIZE];
                    snprintf(newline, sizeof(newline), "%s %s %s %d %d\n",
                             c.id, c.name, c.faculty_id, c.seats, c.enrolled);
                    write(tfd, newline, strlen(newline));
                }
                else
                {
                    write(client_socket, "Course is full.\n\nWelcome to Student Menu\n1. View All Courses\n2. Enroll in Course\n3. Drop Course\n4. View Enrolled Courses\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 183);
                    // Write original line back to file
                    write(tfd, line, strlen(line));
                    write(tfd, "\n", 1);
                    close(cfd);
                    close(tfd);
                    sem_post(&file_lock);
                    remove("courses_tmp.txt");
                    return;
                }
            }
            else
            {
                // Write line as is
                write(tfd, line, strlen(line));
                write(tfd, "\n", 1);
            }
            idx = 0;
        }
        else if (idx < BUFFER_SIZE - 1)
        {
            line[idx++] = ch;
        }
    }

    close(cfd);
    close(tfd);

    if (!found)
    {
        write(client_socket, "Course ID not found.\n\nWelcome to Student Menu\n1. View All Courses\n2. Enroll in Course\n3. Drop Course\n4. View Enrolled Courses\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 189);
        remove("courses_tmp.txt");
        sem_post(&file_lock);
        return;
    }

    // Replace original courses file with updated file
    remove("courses.txt");
    rename("courses_tmp.txt", "courses.txt");

    // Append enrollment record
    efd = open("enrollments.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    if (efd >= 0)
    {
        char entry[50];
        snprintf(entry, sizeof(entry), "%s %s\n", student_id, course_id);
        write(efd, entry, strlen(entry));
        close(efd);
    }
    else
    {
        write(client_socket, "Error updating enrollments.\n", 29);
        sem_post(&file_lock);
        return;
    }

    write(client_socket, "Enrollment successful.\n\nWelcome to Student Menu\n1. View All Courses\n2. Enroll in Course\n3. Drop Course\n4. View Enrolled Courses\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n", 191);
    sem_post(&file_lock);
}

void drop_course(int client_socket, char *student_id)
{
    char buffer[BUFFER_SIZE], course_id[50];
    int found = 0, student_found = 0;

    write(client_socket, "Enter Course ID to drop: ", 24);
    read(client_socket, course_id, sizeof(course_id));
    course_id[strcspn(course_id, "\n")] = '\0';

    sem_wait(&file_lock);

    // First handle the enrollment file
    int efd = open("enrollments.txt", O_RDONLY);
    int etfd = open("enrollments_tmp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (efd < 0 || etfd < 0)
    {
        write(client_socket, "Error opening enrollment file.\n", 31);
        if (efd >= 0)
            close(efd);
        if (etfd >= 0)
            close(etfd);
        sem_post(&file_lock);
        return;
    }

    // Then handle the courses file
    int cfd = open("courses.txt", O_RDONLY);
    int ctfd = open("courses_tmp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (cfd < 0 || ctfd < 0)
    {
        write(client_socket, "Error opening courses file.\n", 28);
        if (efd >= 0)
            close(efd);
        if (etfd >= 0)
            close(etfd);
        if (cfd >= 0)
            close(cfd);
        if (ctfd >= 0)
            close(ctfd);
        sem_post(&file_lock);
        return;
    }

    // Process enrollments file
    char line[BUFFER_SIZE];
    int idx = 0;
    char ch;

    while (read(efd, &ch, 1) > 0)
    {
        if (ch == '\n')
        {
            line[idx] = '\0';
            char s_id[50], c_id[50];
            sscanf(line, "%s %s", s_id, c_id);

            if (strcmp(c_id, course_id) == 0 && strcmp(s_id, student_id) == 0)
            {
                student_found = 1;
                found = 1;
                // Skip writing this line (dropping enrollment)
            }
            else
            {
                write(etfd, line, strlen(line));
                write(etfd, "\n", 1);
                if (strcmp(c_id, course_id) == 0)
                    found = 1;
            }
            idx = 0;
        }
        else
        {
            line[idx++] = ch;
        }
    }

    // Process courses file (to update enrolled count)
    idx = 0;
    Course course;
    int course_updated = 0;

    while (read(cfd, &ch, 1) > 0)
    {
        if (ch == '\n')
        {
            line[idx] = '\0';
            sscanf(line, "%s %s %s %d %d",
                   course.id, course.name, course.faculty_id,
                   &course.seats, &course.enrolled);

            if (strcmp(course.id, course_id) == 0 && student_found)
            {
                course.enrolled--; // Decrement enrolled count
                course_updated = 1;
            }

            // Write the line (updated or original)
            snprintf(line, sizeof(line), "%s %s %s %d %d",
                     course.id, course.name, course.faculty_id,
                     course.seats, course.enrolled);
            write(ctfd, line, strlen(line));
            write(ctfd, "\n", 1);
            idx = 0;
        }
        else
        {
            line[idx++] = ch;
        }
    }

    // Close all files
    close(efd);
    close(etfd);
    close(cfd);
    close(ctfd);

    if (!found)
    {
        write(client_socket, "Course ID not found.\n", 22);
        remove("enrollments_tmp.txt");
        remove("courses_tmp.txt");
        sem_post(&file_lock);
        return;
    }

    if (!student_found)
    {
        write(client_socket, "Student not enrolled in this course.\n", 37);
        remove("enrollments_tmp.txt");
        remove("courses_tmp.txt");
        sem_post(&file_lock);
        return;
    }

    // Atomically update both files
    remove("enrollments.txt");
    rename("enrollments_tmp.txt", "enrollments.txt");

    if (course_updated)
    {
        remove("courses.txt");
        rename("courses_tmp.txt", "courses.txt");
    }
    else
    {
        remove("courses_tmp.txt");
    }

    write(client_socket, "Course dropped successfully.\n", 28);
    sem_post(&file_lock);

    const char *menu = "\nWelcome to Student Menu\n1. View All Courses\n2. Enroll in Course\n3. Drop Course\n4. View Enrolled Courses\n5. Change Password\n6. Logout and Exit\nEnter Your Choice Below\n";
    write(client_socket, menu, strlen(menu));
}

void view_enrolled_courses(int client_socket, char *student_id)
{
    char buffer[BUFFER_SIZE];
    char enrolled_courses[100][50]; // assume max 100 courses
    int course_count = 0;

    sem_wait(&file_lock);

    // Read enrollments.txt to get all courses student enrolled in
    int efd = open("enrollments.txt", O_RDONLY);
    if (efd < 0)
    {
        const char *msg = "Error opening enrollment file.\n";
        write(client_socket, msg, strlen(msg));
        sem_post(&file_lock);
        return;
    }

    char ch, line[BUFFER_SIZE];
    int idx = 0;
    while (read(efd, &ch, 1) > 0)
    {
        if (ch == '\n')
        {
            line[idx] = '\0';
            idx = 0;

            // Manually parse the line
            char *token = strtok(line, " ");
            if (!token) continue;
            char s_id[50];
            strcpy(s_id, token);

            token = strtok(NULL, " ");
            if (!token) continue;
            char c_id[50];
            strcpy(c_id, token);

            if (strcmp(s_id, student_id) == 0)
            {
                strcpy(enrolled_courses[course_count++], c_id);
            }
        }
        else
        {
            line[idx++] = ch;
        }
    }
    close(efd);

    if (course_count == 0)
    {
        const char *msg = "You are not enrolled in any courses.\n\n"
                          "Welcome to Student Menu\n"
                          "1. View All Courses\n"
                          "2. Enroll in Course\n"
                          "3. Drop Course\n"
                          "4. View Enrolled Courses\n"
                          "5. Change Password\n"
                          "6. Logout and Exit\n"
                          "Enter Your Choice Below\n";
        write(client_socket, msg, strlen(msg));
        sem_post(&file_lock);
        return;
    }

    // Open courses.txt to lookup course details
    int cfd = open("courses.txt", O_RDONLY);
    if (cfd < 0)
    {
        const char *msg = "Error opening courses file.\n";
        write(client_socket, msg, strlen(msg));
        sem_post(&file_lock);
        return;
    }

    Course c;
    idx = 0;
    int found_any = 0;
    while (read(cfd, &ch, 1) > 0)
    {
        if (ch == '\n')
        {
            line[idx] = '\0';
            idx = 0;

            // Manually parse line into course struct
            sscanf(line, "%s %s %s %d %d", c.id, c.name, c.faculty_id, &c.seats, &c.enrolled);

            for (int i = 0; i < course_count; i++)
            {
                if (strcmp(c.id, enrolled_courses[i]) == 0)
                {
                    found_any = 1;
                    snprintf(buffer, sizeof(buffer),
                             "Course ID: %s\nCourse Name: %s\nFaculty ID: %s\nSeats: %d\nEnrolled: %d\n\n",
                             c.id, c.name, c.faculty_id, c.seats, c.enrolled);
                    write(client_socket, buffer, strlen(buffer));
                    break;
                }
            }
        }
        else
        {
            line[idx++] = ch;
        }
    }

    close(cfd);

    if (!found_any)
    {
        const char *msg = "You are not enrolled in any courses.\n";
        write(client_socket, msg, strlen(msg));
    }

    // Send student menu
    const char *menu = "\nWelcome to Student Menu\n"
                       "1. View All Courses\n"
                       "2. Enroll in Course\n"
                       "3. Drop Course\n"
                       "4. View Enrolled Courses\n"
                       "5. Change Password\n"
                       "6. Logout and Exit\n"
                       "Enter Your Choice Below\n";
    write(client_socket, menu, strlen(menu));

    sem_post(&file_lock);
}
