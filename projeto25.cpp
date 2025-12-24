#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <utility>

using namespace std;

int main() {
    ios::sync_with_stdio(0);
    cin.tie(0);
    
    // Read basic input: number of nodes, number of trucks, truck range to print, and number of edges
    int nNodes; 
    if (!(cin >> nNodes)) return 0;
    int numTrucks; cin >> numTrucks;
    int startTruck, endTruck; cin >> startTruck >> endTruck;
    int numEdges; cin >> numEdges;

    // Build directed graph adjacency list and indegree array
    vector<vector<int>> adj(nNodes + 1);
    vector<int> indeg(nNodes + 1, 0);
    for (int i = 0; i < numEdges; ++i) {
        int from, to; cin >> from >> to;
        adj[from].push_back(to);
        ++indeg[to];
    }

    // Compute topological order using Kahn's algorithm (queue of zero-indegree nodes)
    queue<int> qNodes;
    for (int i = 1; i <= nNodes; ++i) if (indeg[i] == 0) qNodes.push(i);
    vector<int> topoOrder;
    while (!qNodes.empty()) {
        int node = qNodes.front(); qNodes.pop();
        topoOrder.push_back(node);
        for (int neighbor : adj[node]) {
            if (--indeg[neighbor] == 0) qNodes.push(neighbor);
        }
    }
    // If the graph has a cycle (not a DAG), exit early
    if ((int)topoOrder.size() != nNodes) {
        return 0;
    }

    // For each truck id, store assigned (source,destination) pairs
    vector<vector<pair<int,int>>> truckPairs(numTrucks + 1);

    // Buffers used during per-source DP
    vector<unsigned long long> pathCount(nNodes + 1); // total path counts (may overflow but kept)
    vector<int> pathCountMod(nNodes + 1);             // path counts modulo numTrucks
    vector<char> reachable(nNodes + 1);               // whether a node is reachable from current source

    // For every source node, compute number of distinct paths to all reachable nodes in the DAG
    for (int source = 1; source <= nNodes; ++source) {
        fill(pathCount.begin(), pathCount.end(), 0);
        fill(pathCountMod.begin(), pathCountMod.end(), 0);
        fill(reachable.begin(), reachable.end(), 0);
        pathCount[source] = 1;
        pathCountMod[source] = 1;
        reachable[source] = 1;

        // Propagate counts along topological order so each edge contributes to destination counts
        for (int node : topoOrder) {
            if (!reachable[node]) continue;
            for (int neighbor : adj[node]) {
                pathCount[neighbor] += pathCount[node];
                pathCountMod[neighbor] = (pathCountMod[neighbor] + pathCountMod[node]) % numTrucks;
                reachable[neighbor] = 1;
            }
        }

        // For each reachable destination (different from source), map the pair to a truck id
        // Truck id is determined by (path count mod numTrucks) mapped to 1..numTrucks
        for (int dest = 1; dest <= nNodes; ++dest) {
            if (dest == source) continue;
            if (!reachable[dest]) continue;
            int truckId = 1 + pathCountMod[dest] % numTrucks;
            truckPairs[truckId].push_back({source, dest});
        }
    }

    // Sort pairs for each truck for deterministic output
    for (int truckId = 1; truckId <= numTrucks; ++truckId) {
        auto &pairsVec = truckPairs[truckId];
        sort(pairsVec.begin(), pairsVec.end());
    }

    // Print assigned pairs for trucks in the requested range
    for (int truckId = startTruck; truckId <= endTruck; ++truckId) {
        cout << 'C' << truckId;
        for (const auto &pr : truckPairs[truckId]) {
            cout << ' ' << pr.first << ',' << pr.second;
        }
        cout << '\n';
    }

    return 0;
}
