#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <stack>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

void dfsMatrix(int v, vector<vector<int>>& graph, vector<bool>& visited, stack<int>& Stack) {
    visited[v] = true;
    for (int i = 1; i < graph[v].size(); ++i) {
        if (graph[v][i] && !visited[i]) {
            dfsMatrix(i, graph, visited, Stack);
        }
    }
    Stack.push(v);
}

void reverseDfsMatrix(int v, vector<vector<int>>& reverseGraph, vector<bool>& visited, vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int i = 1; i < reverseGraph[v].size(); ++i) {
        if (reverseGraph[v][i] && !visited[i]) {
            reverseDfsMatrix(i, reverseGraph, visited, component);
        }
    }
}

vector<vector<int>> kosarajuMatrix(int n, vector<vector<int>>& graph, vector<vector<int>>& reverseGraph) {
    stack<int> Stack;
    vector<bool> visited(n + 1, false);

    for (int i = 1; i <= n; ++i) {
        if (!visited[i]) {
            dfsMatrix(i, graph, visited, Stack);
        }
    }

    fill(visited.begin(), visited.end(), false);
    vector<vector<int>> sccs;

    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            vector<int> component;
            reverseDfsMatrix(v, reverseGraph, visited, component);
            sccs.push_back(component);
        }
    }

    return sccs;
}

void profileMatrix(int n, int m, vector<pair<int, int>>& edges) {
    auto start = high_resolution_clock::now();

    vector<vector<int>> graph(n + 1, vector<int>(n + 1, 0));
    vector<vector<int>> reverseGraph(n + 1, vector<int>(n + 1, 0));
    for (auto& edge : edges) {
        graph[edge.first][edge.second] = 1;
        reverseGraph[edge.second][edge.first] = 1;
    }

    kosarajuMatrix(n, graph, reverseGraph);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    cout << "Time taken using adjacency matrix: " << duration << " ms" << endl;
}

void dfsList(int v, vector<vector<int>>& graph, vector<bool>& visited, stack<int>& Stack) {
    visited[v] = true;
    for (int neighbor : graph[v]) {
        if (!visited[neighbor]) {
            dfsList(neighbor, graph, visited, Stack);
        }
    }
    Stack.push(v);
}

void reverseDfsList(int v, vector<vector<int>>& reverseGraph, vector<bool>& visited, vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int neighbor : reverseGraph[v]) {
        if (!visited[neighbor]) {
            reverseDfsList(neighbor, reverseGraph, visited, component);
        }
    }
}

vector<vector<int>> kosarajuList(int n, vector<vector<int>>& graph, vector<vector<int>>& reverseGraph) {
    stack<int> Stack;
    vector<bool> visited(n + 1, false);

    for (int i = 1; i <= n; ++i) {
        if (!visited[i]) {
            dfsList(i, graph, visited, Stack);
        }
    }

    fill(visited.begin(), visited.end(), false);
    vector<vector<int>> sccs;

    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            vector<int> component;
            reverseDfsList(v, reverseGraph, visited, component);
            sccs.push_back(component);
        }
    }

    return sccs;
}

void profileList(int n, int m, vector<pair<int, int>>& edges) {
    auto start = high_resolution_clock::now();

    vector<vector<int>> graph(n + 1);
    vector<vector<int>> reverseGraph(n + 1);
    for (auto& edge : edges) {
        graph[edge.first].push_back(edge.second);
        reverseGraph[edge.second].push_back(edge.first);
    }

    kosarajuList(n, graph, reverseGraph);

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    cout << "Time taken using adjacency list: " << duration << " ms" << endl;
}

vector<pair<int, int>> generateLargeGraph(int n, int m) {
    vector<pair<int, int>> edges;
    for (int i = 1; i <= m; ++i) {
        edges.emplace_back(i % n + 1, (i + 1) % n + 1);
    }
    return edges;
}

int main() {
    int n = 10000; // Number of vertices
    int m = 50000; // Number of edges

    vector<pair<int, int>> edges = generateLargeGraph(n, m);

    profileMatrix(n, m, edges);
    profileList(n, m, edges);

    return 0;
}
