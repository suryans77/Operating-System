#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h> // Include for errno

#define BUFFER_SIZE 1024

// Function to safely send data over the socket
ssize_t safe_send(int sock, const void *buf, size_t len)
{
    ssize_t total_sent = 0;
    while (total_sent < len)
    {
        ssize_t bytes_sent = send(sock, buf + total_sent, len - total_sent, 0);
        if (bytes_sent < 0)
        {
            perror("send failed");
            return -1;
        }
        total_sent += bytes_sent;
    }
    return total_sent;
}

void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        // Keep reading characters until newline or EOF
    }
}

void set_socket_timeout(int sock, int seconds)
{
    struct timeval tv;
    if (seconds > 0)
    {
        tv.tv_sec = seconds;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
    }
    else
    {
        // Disable timeout
        tv.tv_sec = 0;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
    }
}

int main(int argc, char *argv[])
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char username[50], password[50], userID[50], choice[50];

    if (argc != 2)
    {
        printf("\nUsage: %s <server IP>\n", argv[0]);
        return -1;
    }

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    // Get server prompt for username
    ssize_t bytes = read(sock, buffer, BUFFER_SIZE - 1);
    if (bytes > 0)
    {
        buffer[bytes] = '\0'; // Null terminate
        printf("%s", buffer);
    }

    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    safe_send(sock, username, strlen(username));

    // Get server prompt for userID
    bytes = read(sock, buffer, BUFFER_SIZE - 1);
    if (bytes > 0)
    {
        buffer[bytes] = '\0'; // Null terminate
        printf("%s", buffer);
    }
    fgets(userID, sizeof(userID), stdin);
    userID[strcspn(userID, "\n")] = '\0';
    safe_send(sock, userID, strlen(userID));

    // Get server prompt for password
    memset(buffer, 0, BUFFER_SIZE);
    bytes = read(sock, buffer, BUFFER_SIZE - 1);
    if (bytes > 0)
    {
        buffer[bytes] = '\0'; // Null terminate
        printf("%s", buffer);
    }

    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';
    safe_send(sock, password, strlen(password));

    // Get authentication result
    memset(buffer, 0, BUFFER_SIZE);
    bytes = read(sock, buffer, BUFFER_SIZE - 1);
    if (bytes > 0)
    {
        buffer[bytes] = '\0'; // Null terminate
        printf("%s", buffer);
    }

    // If login successful, handle menu
    if (strstr(buffer, "Login successful") != NULL)
    {

        while (1)
        {

            set_socket_timeout(sock, 1);

            // Get server prompt for menu
            memset(buffer, 0, BUFFER_SIZE);
            bytes = read(sock, buffer, BUFFER_SIZE - 1);

            set_socket_timeout(sock, 0);

            if (bytes > 0)
            {
                buffer[bytes] = '\0';
                printf("%s", buffer);
            }
            else if (bytes == 0)
            {
                printf("Server disconnected.\n");
                break;
            }
            else
            {
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
                    printf("");
                }
                else
                {
                    perror("Error reading server response");
                    break;
                }
            }

            if (fgets(choice, sizeof(choice), stdin) == NULL)
            {
                printf("Error reading input\n");
                break;
            }
            choice[strcspn(choice, "\n")] = '\0';

            safe_send(sock, choice, strlen(choice));

            set_socket_timeout(sock, 1);

            // Get server response
            memset(buffer, 0, BUFFER_SIZE);
            bytes = read(sock, buffer, BUFFER_SIZE - 1);

            set_socket_timeout(sock, 0);

            if (bytes > 0)
            {
                buffer[bytes] = '\0';
                printf("%s\n", buffer);
            }
        }

        close(sock);
        return 0;
    }
}
