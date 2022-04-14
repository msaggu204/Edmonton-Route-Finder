// ------------------------------------------------------------------
// Assignment Part I: Trivial Navigation System
// -------------------------------------------------------------------

// Dijkstra's alg. implementaton goes here
#include <iostream>
#include <unordered_map>
#include <list>
#include <queue>
#include <vector>
#include <utility> // for std::pair
// pair is a simple container of two heterogeneous objects
// which can be accessed using the dot operator followed by 
// first or second keyword
#include "wdigraph.h"
#include "dijkstra.h"

using namespace std;

// typedef creates an alias for the specified type
// PIL is the value type of our searchTree 
typedef pair<int, long long> PIL;

// PIPIL is used to insert a key-value pair in our searchTree
// if we declare a variable 'x' as follows:  PIPIL x;
// x.first gives the start vertex of the edge, 
// x.second.first gives the end vertex of the edge, 
// x.second.second gives the cost of the edge
typedef pair<int, PIL> PIPIL;


struct Greater {
  bool const operator()(PIPIL& lhs, PIPIL& rhs) const{
    return lhs.second.second > rhs.second.second;
  }
};

void dijkstra(const WDigraph& graph, int startVertex, unordered_map<int, PIL>& tree) {
    // each active fire is stored as (v, (u, d)) 
    // which implies that it is a fire started at u
    // currently burning the (u,v) edge 
    // and will reach v at time d
    
    // We use a doubly linked list but it can be replaced with a min heap
    // for a more efficient implementation
    // finding and extracting the 'earliest' fire that reaches another vertex
    // is O(N) in a list with N items, but O(log N) in a (binary) min heap

    // Min heap
	priority_queue< PIPIL, vector<PIPIL>, Greater> fires;
	// fires.push(PIPIL(0, PIL(0,0)));
    // at time 0, the startVertex burns, we set the predecesor of
    // startVertex to startVertex (as it is the first vertex)
    fires.push(PIPIL(startVertex, PIL(startVertex, 0)));

    // while there is an active fire
    while (!fires.empty()) {
        // finding the fire that reaches its endpoint earliest
        auto earliestFire = fires.top();

        int v = earliestFire.first; 
        int u = earliestFire.second.first; 
        long long d = earliestFire.second.second;

        // remove this fire
        fires.pop();

        // if v is already "burned", there nothing to do
        if (tree.find(v) != tree.end()) {
            continue;
        }

        // record that 'v' is burned at time 'd' by a fire started from 'u'
        tree[v] = PIL(u, d);

        // now start fires from all edges exiting vertex 'v'
        for (auto iter = graph.neighbours(v); iter != graph.endIterator(v); iter++) {
            int nbr = *iter;
            // 'v' catches on fire at time 'd' and the fire will reach 'nbr'
            // at time d + (length of v->nbr edge)
            long long t_burn = d + graph.getCost(v, nbr);
            fires.push(PIPIL(nbr, PIL(v, t_burn)));
        }
    }
}
