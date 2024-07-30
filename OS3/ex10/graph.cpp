#include "graph.hpp"
#include <stack>
#include <algorithm>

Graph::Graph(int n, int m) : n(n), m(m) {
    graph.resize(n + 1);
    reverseGraph.resize(n + 1);
}

void Graph::addEdge(int u, int v) {
    graph[u].push_back(v);
    reverseGraph[v].push_back(u);
}

void Graph::removeEdge(int u, int v) {
    graph[u].remove(v);
    reverseGraph[v].remove(u);
}

std::vector<std::vector<int>> Graph::kosaraju() {
    std::list<int> Stack;
    std::vector<bool> visited(n + 1, false);

    for (int i = 1; i <= n; ++i) {
        if (!visited[i]) {
            dfs(i, visited, Stack);
        }
    }

    std::fill(visited.begin(), visited.end(), false);
    std::vector<std::vector<int>> sccs;

    while (!Stack.empty()) {
        int v = Stack.back();
        Stack.pop_back();

        if (!visited[v]) {
            std::vector<int> component;
            reverseDfs(v, visited, component);
            sccs.push_back(component);
        }
    }

    return sccs;
}

void Graph::dfs(int v, std::vector<bool>& visited, std::list<int>& Stack) {
    visited[v] = true;
    for (int neighbor : graph[v]) {
        if (!visited[neighbor]) {
            dfs(neighbor, visited, Stack);
        }
    }
    Stack.push_back(v);
}

void Graph::reverseDfs(int v, std::vector<bool>& visited, std::vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int neighbor : reverseGraph[v]) {
        if (!visited[neighbor]) {
            reverseDfs(neighbor, visited, component);
        }
    }
}


