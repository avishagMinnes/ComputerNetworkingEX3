#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

void dfs(int v, vector<vector<int>>& graph, vector<bool>& visited, stack<int>& Stack) {
    visited[v] = true;
    for (int neighbor : graph[v]) {
        if (!visited[neighbor]) {
            dfs(neighbor, graph, visited, Stack);
        }
    }
    Stack.push(v);
}

void reverseDfs(int v, vector<vector<int>>& reverseGraph, vector<bool>& visited, vector<int>& component) {
    visited[v] = true;
    component.push_back(v);
    for (int neighbor : reverseGraph[v]) {
        if (!visited[neighbor]) {
            reverseDfs(neighbor, reverseGraph, visited, component);
        }
    }
}

vector<vector<int>> kosaraju(int n, int m, vector<pair<int, int>>& edges) {
    vector<vector<int>> graph(n + 1);
    vector<vector<int>> reverseGraph(n + 1);
    for (auto& edge : edges) {
        graph[edge.first].push_back(edge.second);
        reverseGraph[edge.second].push_back(edge.first);
    }

    stack<int> Stack;
    vector<bool> visited(n + 1, false);

    for (int i = 1; i <= n; ++i) {
        if (!visited[i]) {
            dfs(i, graph, visited, Stack);
        }
    }

    fill(visited.begin(), visited.end(), false);
    vector<vector<int>> sccs;

    while (!Stack.empty()) {
        int v = Stack.top();
        Stack.pop();

        if (!visited[v]) {
            vector<int> component;
            reverseDfs(v, reverseGraph, visited, component);
            sccs.push_back(component);
        }
    }

    return sccs;
}

int main() {
    int n, m;
    cin >> n >> m;
    vector<pair<int, int>> edges(m);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        edges[i] = make_pair(u, v);
    }

    vector<vector<int>> sccs = kosaraju(n, m, edges);

    for (const auto& component : sccs) {
        for (int vertex : component) {
            cout << vertex << " ";
        }
        cout << endl;
    }

    return 0;
}
