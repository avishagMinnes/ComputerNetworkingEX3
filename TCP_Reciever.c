#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <time.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_TIMES 100

double calculate_bandwidth(double time_diff, int data_size) {
    double bandwidth = (data_size / time_diff) * 1000.0;
    return bandwidth;
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Failed to create socket.\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in sender_addr, rcv_addr;
    sender_addr.sin_family = AF_INET;
    sender_addr.sin_port = htons(PORT);
    sender_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr *)&sender_addr, sizeof(sender_addr)) == -1) {
        perror("Error binding socket.\n");
        close(sock);
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 5) == -1) {
        perror("Error listening.\n");
        close(sock);
        exit(EXIT_FAILURE);
    }

    int rvc_len = sizeof(rcv_addr);
    int rcv_sock = accept(sock, (struct sockaddr *)&rcv_addr, &rvc_len);
    if (rcv_sock == -1) {
        perror("Error accepting connection.\n");
        close(sock);
        exit(EXIT_FAILURE);
    }

    double bandwidth[MAX_TIMES];
    double times[MAX_TIMES];
    int timecount = 0;
    char buffer[BUFFER_SIZE];

    while (1) {
        struct timeval stop, start;
        gettimeofday(&start, NULL);
        int bytes_size = recv(rcv_sock, buffer, BUFFER_SIZE, 0);

        if (bytes_size == -1) {
            perror("Error receiving file.\n");
            close(rcv_sock);
            close(sock);
            exit(EXIT_FAILURE);
        } else if (bytes_size == 0) {
            perror("Connection closed.\n");
            exit(EXIT_FAILURE);
        } else {
            gettimeofday(&stop, NULL);
            double calc_time = (stop.tv_sec - start.tv_sec) * 1000.0 + (stop.tv_usec - start.tv_usec) / 1000.0;
            times[timecount] = calc_time;
            bandwidth[timecount] = calculate_bandwidth(calc_time, bytes_size);
            timecount++;
        }
    }

    double sum_time = 0;
    double sum_bandwidth = 0;

    for (int i = 0; i < timecount; i++) {
        sum_time += times[i];
        sum_bandwidth += bandwidth[i];
        printf("Run #%d Data: Time=%.2fms; Speed=%.2fMB/s\n", i + 1, times[i], bandwidth[i]);
    }

    double avg_time = sum_time / timecount;
    double avg_bandwidth = sum_bandwidth / timecount;

    printf("Average time: %.2fms\n", avg_time);
    printf("Average bandwidth: %.2fMB/s\n", avg_bandwidth);

    close(rcv_sock);
    close(sock);

    return 0;
}