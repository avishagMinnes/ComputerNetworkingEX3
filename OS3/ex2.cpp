#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <stack>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Function to generate a large graph with n vertices and m edges
vector<pair<int, int>> generateLargeGraph(int n, int m) {
    vector<pair<int, int>> edges;
    for (int i = 1; i <= m; ++i) {
        edges.emplace_back(i % n + 1, (i + 1) % n + 1); // Generate a simple cyclic graph
    }
    return edges;
}

void profileDeque(int n, int m, vector<pair<int, int>>& edges) {
    auto start = high_resolution_clock::now();

    vector<deque<int>> graph(n + 1);
    vector<deque<int>> reverseGraph(n + 1);
    for (auto& edge : edges) {
        graph[edge.first].push_back(edge.second);
        reverseGraph[edge.second].push_back(edge.first);
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    cout << "Time taken using deque: " << duration << " ms" << endl;
}

void profileList(int n, int m, vector<pair<int, int>>& edges) {
    auto start = high_resolution_clock::now();

    vector<list<int>> graph(n + 1);
    vector<list<int>> reverseGraph(n + 1);
    for (auto& edge : edges) {
        graph[edge.first].push_back(edge.second);
        reverseGraph[edge.second].push_back(edge.first);
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    cout << "Time taken using list: " << duration << " ms" << endl;
}

int main() {
    int n = 10000; // Number of vertices
    int m = 50000; // Number of edges

    vector<pair<int, int>> edges = generateLargeGraph(n, m);

    profileDeque(n, m, edges);
    profileList(n, m, edges);

    return 0;
}

