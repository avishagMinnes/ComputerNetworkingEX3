#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <list>
#include <unordered_map>
#include <functional>

class Graph {
public:
    Graph(int n, int m);
    void addEdge(int u, int v);
    void removeEdge(int u, int v);
    std::vector<std::vector<int>> kosaraju();

private:
    int n, m;
    std::vector<std::list<int>> graph;
    std::vector<std::list<int>> reverseGraph;
    void dfs(int v, std::vector<bool>& visited, std::list<int>& Stack);
    void reverseDfs(int v, std::vector<bool>& visited, std::vector<int>& component);
};

#endif // GRAPH_H
