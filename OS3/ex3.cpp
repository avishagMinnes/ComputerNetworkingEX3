#include <iostream>
#include <vector>
#include <stack>
#include <list>
#include <unordered_set>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

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

int main() {
    Graph* g = nullptr;
    string command;

    while (getline(cin, command)) {
        if (command.find("Newgraph") == 0) {
            int n, m;
            sscanf(command.c_str(), "Newgraph %d,%d", &n, &m);
            delete g;
            g = new Graph(n, m);
            for (int i = 0; i < m; ++i) {
                int u, v;
                cin >> u >> v;
                g->addEdge(u, v);
            }
            cin.ignore(); // To ignore the newline after reading the edges
        } else if (command.find("Kosaraju") == 0) {
            if (g != nullptr) {
                auto start = high_resolution_clock::now();
                vector<vector<int>> sccs = g->kosaraju();
                auto end = high_resolution_clock::now();
                auto duration = duration_cast<milliseconds>(end - start).count();
                cout << "Strongly connected components:" << endl;
                for (const auto& component : sccs) {
                    for (int vertex : component) {
                        cout << vertex << " ";
                    }
                    cout << endl;
                }
                cout << "Time taken: " << duration << " ms" << endl;
            }
        } else if (command.find("Newedge") == 0) {
            int u, v;
            sscanf(command.c_str(), "Newedge %d,%d", &u, &v);
            if (g != nullptr) {
                g->addEdge(u, v);
            }
        } else if (command.find("Removeedge") == 0) {
            int u, v;
            sscanf(command.c_str(), "Removeedge %d,%d", &u, &v);
            if (g != nullptr) {
                g->removeEdge(u, v);
            }
        }
    }

    delete g;
    return 0;
}

