#include "reactor.hpp"
#include "graph.hpp"
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <thread>

Graph* g = nullptr;

void handleClient(int fd);

void handleNewGraph(int fd, int n, int m) {
    delete g;
    g = new Graph(n, m);
    char buffer[1024];
    for (int i = 0; i < m; ++i) {
        read(fd, buffer, sizeof(buffer));
        int u, v;
        sscanf(buffer, "%d %d", &u, &v);
        g->addEdge(u, v);
    }
    std::string response = "New graph created\n";
    send(fd, response.c_str(), response.size(), 0);
}

void handleKosaraju(int fd) {
    if (g != nullptr) {
        auto sccs = g->kosaraju();
        std::string response = "Strongly connected components:\n";
        for (const auto& component : sccs) {
            for (int vertex : component) {
                response += std::to_string(vertex) + " ";
            }
            response += "\n";
        }
        send(fd, response.c_str(), response.size(), 0);
    }
}

void handleNewEdge(int fd, int u, int v) {
    if (g != nullptr) {
        g->addEdge(u, v);
        std::string response = "Edge added\n";
        send(fd, response.c_str(), response.size(), 0);
    }
}

void handleRemoveEdge(int fd, int u, int v) {
    if (g != nullptr) {
        g->removeEdge(u, v);
        std::string response = "Edge removed\n";
        send(fd, response.c_str(), response.size(), 0);
    }
}

void handleClient(int fd) {
    char buffer[1024];
    int nbytes = read(fd, buffer, sizeof(buffer));
    if (nbytes <= 0) {
        close(fd);
        return;
    }
    buffer[nbytes] = '\0';
    std::string command(buffer);

    if (command.find("Newgraph") == 0) {
        int n, m;
        sscanf(command.c_str(), "Newgraph %d,%d", &n, &m);
        handleNewGraph(fd, n, m);
    } else if (command.find("Kosaraju") == 0) {
        handleKosaraju(fd);
    } else if (command.find("Newedge") == 0) {
        int u, v;
        sscanf(command.c_str(), "Newedge %d,%d", &u, &v);
        handleNewEdge(fd, u, v);
    } else if (command.find("Removeedge") == 0) {
        int u, v;
        sscanf(command.c_str(), "Removeedge %d,%d", &u, &v);
        handleRemoveEdge(fd, u, v);
    }
}

int main() {
    Reactor* reactor = new Reactor();
    reactor->startReactor();

    int listener = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(9034);
    bind(listener, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    listen(listener, 10);

    reactor->addFdToReactor(listener, [reactor](int fd) {
        struct sockaddr_in clientaddr;
        socklen_t addrlen = sizeof(clientaddr);
        int newfd = accept(fd, (struct sockaddr*)&clientaddr, &addrlen);
        std::cout << "New connection on socket " << newfd << std::endl;
        reactor->addFdToReactor(newfd, handleClient);
    });

    std::cout << "Server is running on port 9034" << std::endl;
    std::this_thread::sleep_for(std::chrono::hours(1));

    reactor->stopReactor();
    delete reactor;
    delete g;
    return 0;
}


