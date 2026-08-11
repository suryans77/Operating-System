# Academic Management System

A concurrent client-server academic management system written in C. The project demonstrates core operating systems and computer networks concepts through a role-based course registration workflow for administrators, faculty, and students.

## Features

- Supports multiple simultaneous clients using POSIX threads.
- Uses TCP sockets for client-server communication.
- Provides role-based access for administrators, faculty, and students.
- Stores users, courses, and enrollments in plain text files.
- Protects shared records with a semaphore to avoid race conditions.
- Updates records using temporary files before replacing the original file.

## Project Structure

| File | Purpose |
| --- | --- |
| `server.c` | Starts the TCP server, accepts clients, authenticates users, and routes them to role-specific menus. |
| `client.c` | Connects to the server, sends user input, and displays server responses. |
| `functions.c` | Implements administrator, faculty, and student operations. |
| `headers.h` | Defines constants, data structures, and function prototypes. |

## Core Concepts Used

### Computer Networks

- **Client-server architecture**: clients connect to a central server that handles authentication and operations.
- **TCP sockets**: reliable byte-stream communication using `AF_INET` and `SOCK_STREAM`.
- **Port-based communication**: the server listens on port `8080`.
- **Connection handling**: the server uses one listening socket and creates a dedicated connected socket for each client through `accept()`.

### Operating Systems

- **System calls**: uses calls such as `socket()`, `bind()`, `listen()`, `accept()`, `connect()`, `read()`, `write()`, `open()`, and `close()`.
- **Multithreading**: each connected client is served by a separate POSIX thread.
- **Synchronization**: a binary semaphore protects shared files from concurrent modification.
- **Race condition prevention**: file operations are wrapped with `sem_wait()` and `sem_post()`.
- **File descriptors**: sockets and files are both handled using integer descriptors.

## Role-Based Workflow

### Administrator

- Add students and faculty.
- View student and faculty details.
- Activate or block student accounts.
- Modify student and faculty records.

### Faculty

- View courses offered by the logged-in faculty member.
- Add new courses.
- Remove owned courses.
- Update course details.
- Change password.

### Student

- View all available courses.
- Enroll in a course.
- Drop an enrolled course.
- View enrolled courses.
- Change password.

## Data Files

The server expects text files in the same directory as the executable.

### `admin.txt`, `faculty.txt`, `students.txt`

```text
id name password is_active
```

Example:

```text
A1 admin admin123 1
F1 professor prof123 1
S1 alice alice123 1
```

### `courses.txt`

```text
course_id course_name faculty_id seats enrolled
```

Example:

```text
C101 OS F1 5 0
```

### `enrollments.txt`

```text
student_id course_id
```

Example:

```text
S1 C101
```

## Build

This project uses POSIX APIs, so build it on Linux, WSL, or a Unix-like environment.

```bash
gcc server.c functions.c -o server -pthread
gcc client.c -o client
```

## Run

Start the server in one terminal:

```bash
./server
```

Expected output:

```text
Server started on port 8080
```

Start a client in another terminal:

```bash
./client 127.0.0.1
```

For a live multi-client demo, open additional terminals and run the same client command again.

## Sample Demo Flow

1. Start the server.
2. Start an admin client and log in.
3. Add a faculty member and a student.
4. Start a faculty client and add a course.
5. Start a student client and enroll in the course.
6. Check `courses.txt` to see the enrolled count change.
7. Check `enrollments.txt` to see the student-course mapping.
8. Run multiple clients at once to demonstrate threaded handling and semaphore-protected file access.

## Notes

- The project uses plain text files as lightweight persistent storage.
- Record updates are performed by copying data to a temporary file, writing modified records there, and then replacing the original file.
- TCP is stream-based, so a production version should use message framing such as newline-delimited messages or length-prefixed packets.
- Passwords are stored in plain text for simplicity and should be hashed in a real application.
