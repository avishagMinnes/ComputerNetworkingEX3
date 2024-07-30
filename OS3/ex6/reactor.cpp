#include "reactor.hpp"
#include <thread>
#include <iostream>
#include <algorithm>

Reactor::Reactor() : running(false), maxFd(0) {
    FD_ZERO(&masterSet);
}

Reactor::~Reactor() {
    stopReactor();
}

void* Reactor::startReactor() {
    running = true;
    std::thread reactorThread(&Reactor::run, this);
    reactorThread.detach();
    return this;
}

int Reactor::addFdToReactor(int fd, reactorFunc func) {
    if (fdMap.find(fd) != fdMap.end()) {
        return -1; // fd already exists
    }

    fdMap[fd] = func;
    FD_SET(fd, &masterSet);
    if (fd > maxFd) {
        maxFd = fd;
    }

    return 0;
}

int Reactor::removeFdFromReactor(int fd) {
    if (fdMap.find(fd) == fdMap.end()) {
        return -1; // fd does not exist
    }

    fdMap.erase(fd);
    FD_CLR(fd, &masterSet);
    if (fd == maxFd) {
        if (fdMap.empty()) {
            maxFd = 0;
        } else {
            maxFd = std::max_element(fdMap.begin(), fdMap.end(),
                                     [](const auto& lhs, const auto& rhs) {
                                         return lhs.first < rhs.first;
                                     })->first;
        }
    }

    return 0;
}

int Reactor::stopReactor() {
    running = false;
    return 0;
}

void Reactor::run() {
    while (running) {
        fd_set readSet = masterSet;
        int activity = select(maxFd + 1, &readSet, nullptr, nullptr, nullptr);

        if (activity < 0) {
            perror("select error");
            break;
        }

        for (int fd = 0; fd <= maxFd; ++fd) {
            if (FD_ISSET(fd, &readSet)) {
                auto it = fdMap.find(fd);
                if (it != fdMap.end()) {
                    it->second(fd);
                }
            }
        }
    }
}


