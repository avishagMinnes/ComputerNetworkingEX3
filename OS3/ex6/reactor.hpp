#ifndef REACTOR_H
#define REACTOR_H

#include <functional>
#include <unordered_map>
#include <sys/select.h>
#include <unistd.h>

typedef std::function<void(int)> reactorFunc;

class Reactor {
public:
    Reactor();
    ~Reactor();
    
    void* startReactor();
    int addFdToReactor(int fd, reactorFunc func);
    int removeFdFromReactor(int fd);
    int stopReactor();

private:
    void run();
    bool running;
    std::unordered_map<int, reactorFunc> fdMap;
    fd_set masterSet;
    int maxFd;
};

#endif // REACTOR_H


