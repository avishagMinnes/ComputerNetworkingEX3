#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <unordered_set>
#include <algorithm>
#include <chrono>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>

using namespace std;
using namespace std::chrono;

#define PORT 9034

class Graph {
public:
    Graph(int n, int m) : n(n), m(m) {
        graph.resize(n + 1);
        reverseGraph.resize(n + 1);
    }

    void addEdge(int u, int v) {
        graph[u].push_back(v);
        reverseGraph[v].push_back(u);
    }

    void removeEdge(int u, int v) {
        graph[u].remove(v);
        reverseGraph[v].remove(u);
    }

    vector<vector<int>> kosaraju() {
        stack<int> Stack;
        vector<bool> visited(n + 1, false);

        for (int i = 1; i <= n; ++i) {
            if (!visited[i]) {
                dfs(i, visited, Stack);
            }
        }

        fill(visited.begin(), visited.end(), false);
        vector<vector<int>> sccs;

        while (!Stack.empty()) {
            int v = Stack.top();
            Stack.pop();

            if (!visited[v]) {
                vector<int> component;
                reverseDfs(v, visited, component);
                sccs.push_back(component);
            }
        }

        return sccs;
    }

private:
    int n, m;
    vector<list<int>> graph;
    vector<list<int>> reverseGraph;

    void dfs(int v, vector<bool>& visited, stack<int>& Stack) {
        visited[v] = true;
        for (int neighbor : graph[v]) {
            if (!visited[neighbor]) {
                dfs(neighbor, visited, Stack);
            }
        }
        Stack.push(v);
    }

    void reverseDfs(int v, vector<bool>& visited, vector<int>& component) {
        visited[v] = true;
        component.push_back(v);
        for (int neighbor : reverseGraph[v]) {
            if (!visited[neighbor]) {
                reverseDfs(neighbor, visited, component);
            }
        }
    }
};

// Global graph object
Graph* g = nullptr;

void handleCommand(const string& command, int client_socket) {
    if (command.find("Newgraph") == 0) {
        int n, m;
        sscanf(command.c_str(), "Newgraph %d,%d", &n, &m);
        delete g;
        g = new Graph(n, m);
        char buffer[1024];
        for (int i = 0; i < m; ++i) {
            read(client_socket, buffer, sizeof(buffer));
            int u, v;
            sscanf(buffer, "%d %d", &u, &v);
            g->addEdge(u, v);
        }
        string response = "New graph created\n";
        send(client_socket, response.c_str(), response.size(), 0);
    } else if (command.find("Kosaraju") == 0) {
        if (g != nullptr) {
            auto start = high_resolution_clock::now();
            vector<vector<int>> sccs = g->kosaraju();
            auto end = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(end - start).count();
            string response = "Strongly connected components:\n";
            for (const auto& component : sccs) {
                for (int vertex : component) {
                    response += to_string(vertex) + " ";
                }
                response += "\n";
            }
            response += "Time taken: " + to_string(duration) + " ms\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }
    } else if (command.find("Newedge") == 0) {
        int u, v;
        sscanf(command.c_str(), "Newedge %d,%d", &u, &v);
        if (g != nullptr) {
            g->addEdge(u, v);
            string response = "Edge added\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }
    } else if (command.find("Removeedge") == 0) {
        int u, v;
        sscanf(command.c_str(), "Removeedge %d,%d", &u, &v);
        if (g != nullptr) {
            g->removeEdge(u, v);
            string response = "Edge removed\n";
            send(client_socket, response.c_str(), response.size(), 0);
        }
    }
}

int main() {
    int listener;
    fd_set master;
    fd_set read_fds;
    int fdmax;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    socklen_t addrlen;
    char buf[1024];
    int nbytes;
    int yes = 1;
    int i, j;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    memset(&(serveraddr.sin_zero), '\0', 8);

    if (bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(1);
    }

    FD_SET(listener, &master);
    fdmax = listener;

    for (;;) {
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(1);
        }

        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listener) {
                    addrlen = sizeof(clientaddr);
                    int newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master);
                        if (newfd > fdmax) {
                            fdmax = newfd;
                        }
                        cout << "New connection from "
                             << inet_ntoa(clientaddr.sin_addr)
                             << " on socket " << newfd << endl;
                    }
                } else {
                    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
                        if (nbytes == 0) {
                            cout << "Socket " << i << " hung up" << endl;
                        } else {
                            perror("recv");
                        }
                        close(i);
                        FD_CLR(i, &master);
                    } else {
                        buf[nbytes] = '\0';
                        handleCommand(string(buf), i);
                    }
                }
            }
        }
    }

    return 0;
}
