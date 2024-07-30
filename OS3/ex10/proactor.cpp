#include "proactor.hpp"
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct ProactorArgs {
    int sockfd;
    proactorFunc func;
};

void* clientWrapper(void* arg) {
    ProactorArgs* args = static_cast<ProactorArgs*>(arg);
    int sockfd = args->sockfd;
    proactorFunc func = args->func;
    delete args;
    return func(sockfd);
}

void* proactorLoop(void* arg) {
    auto* args = static_cast<ProactorArgs*>(arg);
    int sockfd = args->sockfd;
    proactorFunc threadFunc = args->func;
    delete args;

    while (true) {
        struct sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
        int newfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addrlen);
        if (newfd == -1) {
            perror("accept");
            continue;
        }

        pthread_t clientThread;
        auto* clientArgs = new ProactorArgs{newfd, threadFunc};
        if (pthread_create(&clientThread, nullptr, clientWrapper, clientArgs) != 0) {
            perror("pthread_create");
            delete clientArgs;
            close(newfd);
            continue;
        }
        pthread_detach(clientThread);
    }

    return nullptr;
}

pthread_t startProactor(int sockfd, proactorFunc threadFunc) {
    auto* args = new ProactorArgs{sockfd, threadFunc};
    pthread_t tid;
    if (pthread_create(&tid, nullptr, proactorLoop, args) != 0) {
        perror("pthread_create");
        delete args;
        return 0;
    }
    return tid;
}

int stopProactor(pthread_t tid) {
    return pthread_cancel(tid);
}






