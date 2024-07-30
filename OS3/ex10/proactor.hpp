#ifndef PROACTOR_H
#define PROACTOR_H

#include <pthread.h>

typedef void* (*proactorFunc)(int sockfd);

// Starts a new proactor and returns the proactor thread ID.
pthread_t startProactor(int sockfd, proactorFunc threadFunc);

// Stops the proactor by thread ID.
int stopProactor(pthread_t tid);

#endif // PROACTOR_H




