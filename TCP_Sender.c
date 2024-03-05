#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h> 

#define PORT 8080
#define BUFFER_SIZE 1024
#define FILE_SIZE 2*1024*1024

char *util_generate_random_data(unsigned int size) {
    char *buffer = NULL;

    if (size == 0) {
        return NULL;
    }

    buffer = (char *)calloc(size, sizeof(char));

    if (buffer == NULL) {
        return NULL;
    }

    srand(time(NULL));

    for (unsigned int i = 0; i < size; i++) {
        buffer[i] = (char)((unsigned int)rand() % 256);
    }

    return buffer;
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s -ip <IP address> -alg <congestion control algorithm>\n", argv[0]);
        return 1;
    }

    char *receiver_ip;
    char *congestion_control_alg;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-ip") == 0) {
            receiver_ip = argv[i+1];
        } else if (strcmp(argv[i], "-alg") == 0) {
            congestion_control_alg = argv[i+1];
        }
    }

    char *random_data = util_generate_random_data(FILE_SIZE);

    if (random_data == NULL) {
        fprintf(stderr, "Failed to generate random data\n");
        exit(EXIT_FAILURE);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Failed to create socket.\n");
        free(random_data);
        exit(EXIT_FAILURE);
    }

    int opt;
    if (strcmp(congestion_control_alg, "reno") == 0) {
        opt = TCP_CONGESTION_RENO;
    } else if (strcmp(congestion_control_alg, "cubic") == 0) {
        opt = TCP_CONGESTION_CUBIC;
    } else {
        fprintf(stderr, "Invalid congestion control algorithm specified\n");
        free(random_data);
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, &opt, sizeof(opt)) < 0) {
        perror("Failed to set congestion control algorithm.\n");
        free(random_data);
        close(sock);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(receiver_ip);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server.\n");
        free(random_data);
        close(sock);
        exit(EXIT_FAILURE);
    }

    int bytes_sent = send(sock, random_data, FILE_SIZE, 0);
    if (bytes_sent == -1) {
        perror("Failed to send data.\n");
        free(random_data);
        close(sock);
        exit(EXIT_FAILURE);
    }

    int answer;
    printf("Do you want to send this file again? Enter 1 for yes, 0 for no.\n");
    if (scanf("%d", &answer) != 1) {
        fprintf(stderr, "Invalid input.\n");
        free(random_data);
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (answer == 1) {
        bytes_sent = send(sock, random_data, FILE_SIZE, 0);
        if (bytes_sent == -1) {
            perror("Failed to send data again.\n");
        }
    }
    else if (answer == 0) {
        const char *exit_message = "EXIT";
        bytes_sent = send(sock, exit_message, strlen(exit_message), 0);
        if (bytes_sent == -1) {
            perror("Failed to send exit message.\n");
        }
    }
    else {
        printf("Invalid answer. Enter 1 for yes, 0 for no.\n");
    }

    free(random_data);
    close(sock);

    return 0;
}