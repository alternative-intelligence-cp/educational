/*
 * 🗺️ Randy's Dijkstra's Algorithm - Real World Pathfinding
 * 
 * Solves actual navigation problems while teaching graph algorithms
 * From campus navigation to network routing - same brilliant algorithm!
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NODES 50
#define INF INT_MAX

// Graph representation
typedef struct {
    int nodes;
    int adj_matrix[MAX_NODES][MAX_NODES];
    char node_names[MAX_NODES][32];
} Graph;

// Priority queue for Dijkstra's algorithm
typedef struct {
    int node;
    int distance;
} PQItem;

typedef struct {
    PQItem items[MAX_NODES];
    int size;
} PriorityQueue;

// Priority queue operations
void pq_init(PriorityQueue* pq) {
    pq->size = 0;
}

void pq_insert(PriorityQueue* pq, int node, int distance) {
    int i = pq->size;
    pq->items[i].node = node;
    pq->items[i].distance = distance;
    pq->size++;
    
    // Bubble up (min-heap)
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (pq->items[parent].distance <= pq->items[i].distance) break;
        
        PQItem temp = pq->items[parent];
        pq->items[parent] = pq->items[i];
        pq->items[i] = temp;
        i = parent;
    }
}

PQItem pq_extract_min(PriorityQueue* pq) {
    PQItem min = pq->items[0];
    pq->size--;
    
    if (pq->size > 0) {
        pq->items[0] = pq->items[pq->size];
        
        // Bubble down
        int i = 0;
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int smallest = i;
            
            if (left < pq->size && pq->items[left].distance < pq->items[smallest].distance)
                smallest = left;
            if (right < pq->size && pq->items[right].distance < pq->items[smallest].distance)
                smallest = right;
                
            if (smallest == i) break;
            
            PQItem temp = pq->items[i];
            pq->items[i] = pq->items[smallest];
            pq->items[smallest] = temp;
            i = smallest;
        }
    }
    
    return min;
}

bool pq_is_empty(PriorityQueue* pq) {
    return pq->size == 0;
}

// Graph operations
void graph_init(Graph* g) {
    g->nodes = 0;
    for (int i = 0; i < MAX_NODES; i++) {
        for (int j = 0; j < MAX_NODES; j++) {
            g->adj_matrix[i][j] = (i == j) ? 0 : INF;
        }
        strcpy(g->node_names[i], "");
    }
}

int graph_add_node(Graph* g, const char* name) {
    if (g->nodes >= MAX_NODES) return -1;
    
    strcpy(g->node_names[g->nodes], name);
    return g->nodes++;
}

void graph_add_edge(Graph* g, int from, int to, int weight) {
    if (from < g->nodes && to < g->nodes) {
        g->adj_matrix[from][to] = weight;
        g->adj_matrix[to][from] = weight;  // Undirected graph
    }
}

int graph_find_node(Graph* g, const char* name) {
    for (int i = 0; i < g->nodes; i++) {
        if (strcmp(g->node_names[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

// 🧠 DIJKSTRA'S ALGORITHM - The heart of the operation
void dijkstra(Graph* g, int start, int distances[], int previous[], bool verbose) {
    bool visited[MAX_NODES] = {false};
    PriorityQueue pq;
    pq_init(&pq);
    
    // Initialize distances
    for (int i = 0; i < g->nodes; i++) {
        distances[i] = INF;
        previous[i] = -1;
    }
    distances[start] = 0;
    
    pq_insert(&pq, start, 0);
    
    if (verbose) {
        printf("\n🧠 DIJKSTRA'S ALGORITHM EXECUTION:\n");
        printf("Starting from: %s\n", g->node_names[start]);
        printf("Step-by-step exploration:\n\n");
    }
    
    int step = 1;
    while (!pq_is_empty(&pq)) {
        PQItem current = pq_extract_min(&pq);
        int u = current.node;
        
        if (visited[u]) continue;
        visited[u] = true;
        
        if (verbose) {
            printf("Step %d: Visiting '%s' (distance: %d)\n", 
                   step++, g->node_names[u], distances[u]);
        }
        
        // Relaxation step - check all neighbors
        for (int v = 0; v < g->nodes; v++) {
            if (g->adj_matrix[u][v] != INF && !visited[v]) {
                int new_distance = distances[u] + g->adj_matrix[u][v];
                
                if (new_distance < distances[v]) {
                    distances[v] = new_distance;
                    previous[v] = u;
                    pq_insert(&pq, v, new_distance);
                    
                    if (verbose) {
                        printf("  → Updated distance to '%s': %d (via %s)\n", 
                               g->node_names[v], new_distance, g->node_names[u]);
                    }
                }
            }
        }
        
        if (verbose) printf("\n");
    }
}

// Reconstruct and display shortest path
void print_path(Graph* g, int start, int end, int previous[], int distances[]) {
    if (distances[end] == INF) {
        printf("❌ No path exists from %s to %s\n", 
               g->node_names[start], g->node_names[end]);
        return;
    }
    
    // Reconstruct path
    int path[MAX_NODES];
    int path_length = 0;
    int current = end;
    
    while (current != -1) {
        path[path_length++] = current;
        current = previous[current];
    }
    
    printf("🗺️ Shortest path from %s to %s:\n", 
           g->node_names[start], g->node_names[end]);
    printf("   ");
    
    for (int i = path_length - 1; i >= 0; i--) {
        printf("%s", g->node_names[path[i]]);
        if (i > 0) printf(" → ");
    }
    
    printf("\n   📏 Total distance: %d\n", distances[end]);
}

// Real-world example: Campus navigation
Graph* create_campus_map() {
    Graph* campus = malloc(sizeof(Graph));
    graph_init(campus);
    
    // Add campus locations
    int library = graph_add_node(campus, "Library");
    int student_center = graph_add_node(campus, "Student_Center");
    int cs_building = graph_add_node(campus, "CS_Building");
    int cafeteria = graph_add_node(campus, "Cafeteria");
    int dorms = graph_add_node(campus, "Dorms");
    int gym = graph_add_node(campus, "Gym");
    int parking = graph_add_node(campus, "Parking");
    int admin = graph_add_node(campus, "Admin");
    
    // Add walking paths with distances in meters
    graph_add_edge(campus, library, student_center, 200);
    graph_add_edge(campus, student_center, cs_building, 150);
    graph_add_edge(campus, cs_building, cafeteria, 100);
    graph_add_edge(campus, cafeteria, dorms, 250);
    graph_add_edge(campus, dorms, gym, 300);
    graph_add_edge(campus, gym, parking, 180);
    graph_add_edge(campus, library, cs_building, 350);  // Long way around
    graph_add_edge(campus, student_center, cafeteria, 220);
    graph_add_edge(campus, cafeteria, gym, 280);
    graph_add_edge(campus, parking, admin, 120);
    graph_add_edge(campus, admin, student_center, 190);
    graph_add_edge(campus, library, dorms, 400);  // Cross-campus path
    
    return campus;
}

// Real-world example: Internet routing
Graph* create_network_topology() {
    Graph* network = malloc(sizeof(Graph));
    graph_init(network);
    
    // Add network nodes (routers/servers)
    int user = graph_add_node(network, "User_PC");
    int isp_router = graph_add_node(network, "ISP_Router");
    int regional_hub = graph_add_node(network, "Regional_Hub");
    int backbone1 = graph_add_node(network, "Backbone_1");
    int backbone2 = graph_add_node(network, "Backbone_2");
    int cdn = graph_add_node(network, "CDN_Server");
    int web_server = graph_add_node(network, "Web_Server");
    int database = graph_add_node(network, "Database");
    
    // Add connections with latency in milliseconds
    graph_add_edge(network, user, isp_router, 5);
    graph_add_edge(network, isp_router, regional_hub, 15);
    graph_add_edge(network, regional_hub, backbone1, 25);
    graph_add_edge(network, backbone1, backbone2, 40);
    graph_add_edge(network, backbone2, web_server, 20);
    graph_add_edge(network, web_server, database, 10);
    graph_add_edge(network, regional_hub, cdn, 30);
    graph_add_edge(network, cdn, web_server, 35);
    graph_add_edge(network, isp_router, backbone1, 45);  // Direct ISP-backbone
    graph_add_edge(network, backbone1, cdn, 25);
    
    return network;
}

// Interactive pathfinding demo
void interactive_demo(Graph* g, const char* scenario_name) {
    printf("\n🎯 INTERACTIVE %s PATHFINDING\n", scenario_name);
    printf("==================================================\n");
    
    printf("Available locations:\n");
    for (int i = 0; i < g->nodes; i++) {
        printf("  %d. %s\n", i + 1, g->node_names[i]);
    }
    
    int start_choice, end_choice;
    printf("\nEnter start location (1-%d): ", g->nodes);
    scanf("%d", &start_choice);
    printf("Enter destination (1-%d): ", g->nodes);
    scanf("%d", &end_choice);
    
    start_choice--; end_choice--;  // Convert to 0-indexed
    
    if (start_choice < 0 || start_choice >= g->nodes ||
        end_choice < 0 || end_choice >= g->nodes) {
        printf("❌ Invalid location numbers!\n");
        return;
    }
    
    int distances[MAX_NODES], previous[MAX_NODES];
    
    printf("\n🚀 Computing shortest path using Dijkstra's algorithm...\n");
    dijkstra(g, start_choice, distances, previous, true);
    
    printf("\n");
    print_path(g, start_choice, end_choice, previous, distances);
    
    // Show all distances from start
    printf("\n📊 All distances from %s:\n", g->node_names[start_choice]);
    for (int i = 0; i < g->nodes; i++) {
        if (distances[i] != INF) {
            printf("   %s: %d\n", g->node_names[i], distances[i]);
        } else {
            printf("   %s: unreachable\n", g->node_names[i]);
        }
    }
}

int main() {
    printf("🗺️ Randy's Dijkstra's Algorithm - Real World Pathfinding\n");
    printf("========================================================\n");
    printf("🧠 The algorithm that powers GPS, internet routing, and more!\n\n");
    
    // Demo 1: Campus navigation
    printf("🏫 DEMO 1: CAMPUS NAVIGATION SYSTEM\n");
    printf("-----------------------------------\n");
    
    Graph* campus = create_campus_map();
    
    // Pre-programmed interesting routes
    int distances[MAX_NODES], previous[MAX_NODES];
    
    printf("📍 Finding path from Dorms to CS Building...\n");
    int start = graph_find_node(campus, "Dorms");
    int end = graph_find_node(campus, "CS_Building");
    
    dijkstra(campus, start, distances, previous, false);
    print_path(campus, start, end, previous, distances);
    
    printf("\n📍 Finding path from Parking to Library...\n");
    start = graph_find_node(campus, "Parking");
    end = graph_find_node(campus, "Library");
    
    dijkstra(campus, start, distances, previous, false);
    print_path(campus, start, end, previous, distances);
    
    // Demo 2: Network routing
    printf("\n\n🌐 DEMO 2: INTERNET ROUTING OPTIMIZATION\n");
    printf("----------------------------------------\n");
    
    Graph* network = create_network_topology();
    
    printf("📡 Finding fastest route from User to Database...\n");
    start = graph_find_node(network, "User_PC");
    end = graph_find_node(network, "Database");
    
    dijkstra(network, start, distances, previous, false);
    print_path(network, start, end, previous, distances);
    
    printf("\n📡 Comparing routes to Web Server...\n");
    end = graph_find_node(network, "Web_Server");
    dijkstra(network, start, distances, previous, false);
    print_path(network, start, end, previous, distances);
    
    // Show the algorithm in action
    printf("\n\n🔍 ALGORITHM VISUALIZATION\n");
    printf("--------------------------\n");
    printf("Watch Dijkstra's algorithm explore the campus:\n");
    
    start = graph_find_node(campus, "Student_Center");
    dijkstra(campus, start, distances, previous, true);
    
    // Educational insights
    printf("\n💡 WHY DIJKSTRA'S ALGORITHM IS BRILLIANT:\n");
    printf("=========================================\n");
    printf("🎯 Key Insights:\n");
    printf("   1. Greedy Choice: Always pick the closest unvisited node\n");
    printf("   2. Optimal Substructure: Shortest path contains shortest subpaths\n");
    printf("   3. Relaxation: Update distances when better paths are found\n");
    printf("   4. Priority Queue: Efficiently find next closest node\n\n");
    
    printf("🚀 Real-World Applications:\n");
    printf("   📱 GPS Navigation (Google Maps, Waze)\n");
    printf("   🌐 Internet Routing (BGP protocol)\n");
    printf("   ✈️ Flight Path Planning\n");
    printf("   🚛 Delivery Route Optimization\n");
    printf("   🎮 Game AI Pathfinding\n");
    printf("   📊 Social Network Analysis\n");
    printf("   🏥 Emergency Response Planning\n\n");
    
    printf("⏱️ Time Complexity: O((V + E) log V) with binary heap\n");
    printf("💾 Space Complexity: O(V) for distances and priority queue\n\n");
    
    printf("🎓 What makes it clever:\n");
    printf("   • Never revisits nodes (each processed once)\n");
    printf("   • Guarantees optimal solution (if no negative weights)\n");
    printf("   • Works on any weighted graph\n");
    printf("   • Forms basis for many other algorithms\n\n");
    
    // Interactive demo
    char choice;
    printf("🎮 Want to try interactive pathfinding? (y/n): ");
    scanf(" %c", &choice);
    
    if (choice == 'y' || choice == 'Y') {
        interactive_demo(campus, "CAMPUS");
    }
    
    printf("\n🏆 Dijkstra's algorithm: Simple idea, profound impact!\n");
    printf("From 1956 to today - still powering the connected world.\n");
    
    free(campus);
    free(network);
    return 0;
}