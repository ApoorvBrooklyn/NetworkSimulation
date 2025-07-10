#include <iostream>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>

struct Edge {
    int src, dest, weight;
    Edge(int s, int d, int w) : src(s), dest(d), weight(w) {}
};

class PrimsAlgorithm {
private:
    int V;
    std::vector<std::vector<int>> graph;
    std::vector<Edge> mst_edges;
    std::vector<int> key;
    std::vector<bool> mst_set;
    std::vector<int> parent;
    int total_weight;

public:
    PrimsAlgorithm(int vertices) : V(vertices) {
        graph.resize(V, std::vector<int>(V, 0));
        key.resize(V, INT_MAX);
        mst_set.resize(V, false);
        parent.resize(V, -1);
        total_weight = 0;
    }

    void add_edge(int src, int dest, int weight) {
        graph[src][dest] = weight;
        graph[dest][src] = weight; // Undirected graph
    }

    void find_mst() {
        // Initialize key values
        key[0] = 0;
        parent[0] = -1;

        for (int count = 0; count < V - 1; count++) {
            // Find vertex with minimum key value
            int u = min_key();
            mst_set[u] = true;

            // Update key values of adjacent vertices
            for (int v = 0; v < V; v++) {
                if (graph[u][v] && !mst_set[v] && graph[u][v] < key[v]) {
                    parent[v] = u;
                    key[v] = graph[u][v];
                }
            }
        }

        // Build MST edges
        mst_edges.clear();
        total_weight = 0;
        for (int i = 1; i < V; i++) {
            if (parent[i] != -1) {
                mst_edges.emplace_back(parent[i], i, graph[i][parent[i]]);
                total_weight += graph[i][parent[i]];
            }
        }
    }

    int min_key() {
        int min_val = INT_MAX;
        int min_index = -1;

        for (int v = 0; v < V; v++) {
            if (!mst_set[v] && key[v] < min_val) {
                min_val = key[v];
                min_index = v;
            }
        }

        return min_index;
    }

    std::vector<Edge> get_mst_edges() const { return mst_edges; }
    int get_total_weight() const { return total_weight; }
    std::vector<std::vector<int>> get_graph() const { return graph; }
    int get_vertex_count() const { return V; }
}; 