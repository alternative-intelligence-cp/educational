#!/usr/bin/env python3
"""
🗺️ Randy's Dijkstra's Algorithm - Python Implementation
Interactive graph pathfinding with visualization
"""

import heapq
import matplotlib.pyplot as plt
import networkx as nx
from typing import Dict, List, Tuple, Set
import random

class Graph:
    def __init__(self):
        self.nodes: Set[str] = set()
        self.edges: Dict[str, Dict[str, int]] = {}
    
    def add_node(self, node: str):
        """Add a node to the graph"""
        self.nodes.add(node)
        if node not in self.edges:
            self.edges[node] = {}
    
    def add_edge(self, from_node: str, to_node: str, weight: int):
        """Add an edge between two nodes with given weight"""
        self.add_node(from_node)
        self.add_node(to_node)
        self.edges[from_node][to_node] = weight
        self.edges[to_node][from_node] = weight  # Undirected graph
    
    def get_neighbors(self, node: str) -> Dict[str, int]:
        """Get all neighbors of a node with their edge weights"""
        return self.edges.get(node, {})

class DijkstraPathfinder:
    def __init__(self, graph: Graph):
        self.graph = graph
        self.visited_order: List[str] = []
        self.relaxation_steps: List[Tuple[str, str, int]] = []
    
    def dijkstra(self, start: str, verbose: bool = False) -> Tuple[Dict[str, int], Dict[str, str]]:
        """
        🧠 Dijkstra's Algorithm Implementation
        
        Returns:
            distances: Dict mapping node to shortest distance from start
            previous: Dict mapping node to previous node in shortest path
        """
        distances = {node: float('inf') for node in self.graph.nodes}
        previous = {node: None for node in self.graph.nodes}
        distances[start] = 0
        
        # Priority queue: (distance, node)
        pq = [(0, start)]
        visited = set()
        
        self.visited_order = []
        self.relaxation_steps = []
        
        if verbose:
            print(f"\n🧠 DIJKSTRA'S ALGORITHM EXECUTION:")
            print(f"Starting from: {start}")
            print(f"Initial distances: {distances}")
            print(f"\nStep-by-step exploration:")
        
        step = 1
        while pq:
            current_distance, current_node = heapq.heappop(pq)
            
            if current_node in visited:
                continue
                
            visited.add(current_node)
            self.visited_order.append(current_node)
            
            if verbose:
                print(f"\nStep {step}: Visiting '{current_node}' (distance: {current_distance})")
                step += 1
            
            # Examine all neighbors (relaxation step)
            for neighbor, weight in self.graph.get_neighbors(current_node).items():
                if neighbor not in visited:
                    new_distance = current_distance + weight
                    
                    if new_distance < distances[neighbor]:
                        # Relaxation: found shorter path
                        distances[neighbor] = new_distance
                        previous[neighbor] = current_node
                        heapq.heappush(pq, (new_distance, neighbor))
                        
                        self.relaxation_steps.append((current_node, neighbor, new_distance))
                        
                        if verbose:
                            print(f"  → Updated distance to '{neighbor}': {new_distance} (via {current_node})")
        
        return distances, previous
    
    def get_shortest_path(self, start: str, end: str, previous: Dict[str, str]) -> List[str]:
        """Reconstruct shortest path from start to end"""
        path = []
        current = end
        
        while current is not None:
            path.append(current)
            current = previous[current]
        
        path.reverse()
        
        # Check if path exists
        if path[0] != start:
            return []
        
        return path
    
    def print_path_info(self, start: str, end: str, distances: Dict[str, int], previous: Dict[str, str]):
        """Print detailed path information"""
        if distances[end] == float('inf'):
            print(f"❌ No path exists from {start} to {end}")
            return
        
        path = self.get_shortest_path(start, end, previous)
        
        print(f"\n🗺️ Shortest path from {start} to {end}:")
        print(f"   {' → '.join(path)}")
        print(f"   📏 Total distance: {distances[end]}")
        
        # Show step-by-step path costs
        if len(path) > 1:
            print(f"   📋 Path breakdown:")
            total = 0
            for i in range(len(path) - 1):
                edge_cost = self.graph.get_neighbors(path[i])[path[i + 1]]
                total += edge_cost
                print(f"      {path[i]} → {path[i + 1]}: {edge_cost} (running total: {total})")

def create_campus_graph() -> Graph:
    """Create a realistic campus navigation graph"""
    campus = Graph()
    
    # Campus locations with realistic walking distances (meters)
    locations = [
        ("Library", "Student_Center", 200),
        ("Student_Center", "CS_Building", 150),
        ("CS_Building", "Cafeteria", 100),
        ("Cafeteria", "Dorms", 250),
        ("Dorms", "Gym", 300),
        ("Gym", "Parking", 180),
        ("Library", "CS_Building", 350),  # Longer alternative
        ("Student_Center", "Cafeteria", 220),
        ("Cafeteria", "Gym", 280),
        ("Parking", "Admin", 120),
        ("Admin", "Student_Center", 190),
        ("Library", "Dorms", 400),  # Cross-campus shortcut
        ("CS_Building", "Gym", 320),
        ("Admin", "Library", 250),
    ]
    
    for from_loc, to_loc, distance in locations:
        campus.add_edge(from_loc, to_loc, distance)
    
    return campus

def create_network_graph() -> Graph:
    """Create a network topology graph for routing demonstration"""
    network = Graph()
    
    # Network connections with latency in milliseconds
    connections = [
        ("User_PC", "ISP_Router", 5),
        ("ISP_Router", "Regional_Hub", 15),
        ("Regional_Hub", "Backbone_1", 25),
        ("Backbone_1", "Backbone_2", 40),
        ("Backbone_2", "Web_Server", 20),
        ("Web_Server", "Database", 10),
        ("Regional_Hub", "CDN_Server", 30),
        ("CDN_Server", "Web_Server", 35),
        ("ISP_Router", "Backbone_1", 45),  # Direct route
        ("Backbone_1", "CDN_Server", 25),
        ("CDN_Server", "Database", 50),
    ]
    
    for from_node, to_node, latency in connections:
        network.add_edge(from_node, to_node, latency)
    
    return network

def visualize_graph_with_path(graph: Graph, path: List[str], title: str):
    """Create a visual representation of the graph and shortest path"""
    try:
        import matplotlib.pyplot as plt
        import networkx as nx
        
        # Create NetworkX graph
        G = nx.Graph()
        
        # Add nodes and edges
        for node in graph.nodes:
            G.add_node(node)
        
        for from_node, neighbors in graph.edges.items():
            for to_node, weight in neighbors.items():
                G.add_edge(from_node, to_node, weight=weight)
        
        # Create layout
        pos = nx.spring_layout(G, k=3, iterations=50)
        
        plt.figure(figsize=(12, 8))
        
        # Draw all edges
        nx.draw_networkx_edges(G, pos, alpha=0.3, width=1)
        
        # Draw path edges in red
        path_edges = [(path[i], path[i+1]) for i in range(len(path)-1)]
        nx.draw_networkx_edges(G, pos, edgelist=path_edges, edge_color='red', width=3)
        
        # Draw nodes
        node_colors = ['lightcoral' if node in path else 'lightblue' for node in G.nodes()]
        nx.draw_networkx_nodes(G, pos, node_color=node_colors, node_size=1000)
        
        # Draw labels
        nx.draw_networkx_labels(G, pos, font_size=8, font_weight='bold')
        
        # Draw edge labels (weights)
        edge_labels = nx.get_edge_attributes(G, 'weight')
        nx.draw_networkx_edge_labels(G, pos, edge_labels, font_size=7)
        
        plt.title(f"{title}\nShortest Path: {' → '.join(path)}", fontsize=14)
        plt.axis('off')
        plt.tight_layout()
        plt.savefig(f'/home/randy/WORK_SPACE/work/tools/pathfinding/{title.lower().replace(" ", "_")}.png')
        plt.show()
        
    except ImportError:
        print("📝 Visualization requires: pip install matplotlib networkx")
        print("   Graph structure and path are shown in text format above")

def demo_real_world_applications():
    """Demonstrate practical applications of Dijkstra's algorithm"""
    print("\n🌟 REAL-WORLD DIJKSTRA'S APPLICATIONS")
    print("=" * 45)
    
    # Campus Navigation Demo
    print("\n🏫 Campus Navigation System:")
    print("-" * 30)
    
    campus = create_campus_graph()
    pathfinder = DijkstraPathfinder(campus)
    
    # Find path from Dorms to CS Building
    print("📍 Route: Dorms to CS Building for morning class")
    distances, previous = pathfinder.dijkstra("Dorms", verbose=False)
    pathfinder.print_path_info("Dorms", "CS_Building", distances, previous)
    
    path = pathfinder.get_shortest_path("Dorms", "CS_Building", previous)
    visualize_graph_with_path(campus, path, "Campus Navigation")
    
    # Network Routing Demo
    print("\n\n🌐 Internet Routing Optimization:")
    print("-" * 35)
    
    network = create_network_graph()
    net_pathfinder = DijkstraPathfinder(network)
    
    print("📡 Route: User PC to Database (web request)")
    distances, previous = net_pathfinder.dijkstra("User_PC", verbose=False)
    net_pathfinder.print_path_info("User_PC", "Database", distances, previous)
    
    path = net_pathfinder.get_shortest_path("User_PC", "Database", previous)
    visualize_graph_with_path(network, path, "Network Routing")
    
    # Show alternative routes
    print("\n📊 All possible destinations from User PC:")
    for node in sorted(network.nodes):
        if node != "User_PC" and distances[node] != float('inf'):
            print(f"   {node}: {distances[node]}ms latency")

def algorithm_analysis_demo():
    """Show the algorithm's step-by-step execution"""
    print("\n🔍 ALGORITHM STEP-BY-STEP ANALYSIS")
    print("=" * 40)
    
    # Create a simple graph for clear demonstration
    demo_graph = Graph()
    demo_graph.add_edge("A", "B", 4)
    demo_graph.add_edge("A", "C", 2)
    demo_graph.add_edge("B", "C", 1)
    demo_graph.add_edge("B", "D", 5)
    demo_graph.add_edge("C", "D", 8)
    demo_graph.add_edge("C", "E", 10)
    demo_graph.add_edge("D", "E", 2)
    
    pathfinder = DijkstraPathfinder(demo_graph)
    
    print("📊 Simple graph for analysis:")
    print("   A → B (4), A → C (2)")
    print("   B → C (1), B → D (5)")  
    print("   C → D (8), C → E (10)")
    print("   D → E (2)")
    
    distances, previous = pathfinder.dijkstra("A", verbose=True)
    
    print(f"\n📈 Final distances from A: {distances}")
    print(f"🔙 Previous nodes: {previous}")
    
    print(f"\n👀 Node visitation order: {pathfinder.visited_order}")
    print(f"🔄 Relaxation steps performed: {len(pathfinder.relaxation_steps)}")

def interactive_pathfinding():
    """Interactive pathfinding demonstration"""
    print("\n🎮 INTERACTIVE PATHFINDING")
    print("=" * 30)
    
    campus = create_campus_graph()
    pathfinder = DijkstraPathfinder(campus)
    
    locations = sorted(list(campus.nodes))
    
    print("Available campus locations:")
    for i, location in enumerate(locations, 1):
        print(f"  {i}. {location}")
    
    try:
        start_idx = int(input(f"\nEnter start location (1-{len(locations)}): ")) - 1
        end_idx = int(input(f"Enter destination (1-{len(locations)}): ")) - 1
        
        if 0 <= start_idx < len(locations) and 0 <= end_idx < len(locations):
            start = locations[start_idx]
            end = locations[end_idx]
            
            print(f"\n🚀 Computing shortest path using Dijkstra's algorithm...")
            distances, previous = pathfinder.dijkstra(start, verbose=True)
            pathfinder.print_path_info(start, end, distances, previous)
            
            path = pathfinder.get_shortest_path(start, end, previous)
            if path:
                visualize_graph_with_path(campus, path, f"Path from {start} to {end}")
        else:
            print("❌ Invalid location numbers!")
            
    except ValueError:
        print("❌ Please enter valid numbers!")

def main():
    print("🗺️ Randy's Dijkstra's Algorithm - Python Edition")
    print("=" * 50)
    print("🧠 The algorithm that changed how we navigate the world!")
    
    # Educational overview
    print("\n💡 WHAT MAKES DIJKSTRA'S ALGORITHM BRILLIANT:")
    print("=" * 50)
    print("🎯 Core Principles:")
    print("   1. Greedy Strategy: Always choose the closest unvisited node")
    print("   2. Optimal Substructure: Shortest paths contain shortest subpaths")
    print("   3. Relaxation: Update distances when better paths are discovered")
    print("   4. Priority Queue: Efficiently find the next closest node")
    
    # Real-world applications demonstration
    demo_real_world_applications()
    
    # Algorithm analysis
    algorithm_analysis_demo()
    
    # Performance characteristics
    print("\n📊 ALGORITHM COMPLEXITY ANALYSIS")
    print("=" * 35)
    print("⏱️ Time Complexity:")
    print("   • With binary heap: O((V + E) log V)")
    print("   • With Fibonacci heap: O(E + V log V)")
    print("   • V = vertices, E = edges")
    print("\n💾 Space Complexity: O(V)")
    print("   • Distance array: O(V)")
    print("   • Previous array: O(V)")
    print("   • Priority queue: O(V)")
    
    print("\n🚀 WHY IT'S USED EVERYWHERE:")
    print("=" * 32)
    print("📱 GPS Navigation:")
    print("   • Google Maps, Apple Maps, Waze")
    print("   • Real-time traffic optimization")
    print("🌐 Internet Routing:")
    print("   • BGP protocol for internet backbone")
    print("   • CDN content delivery optimization")
    print("✈️ Transportation:")
    print("   • Flight path planning")
    print("   • Public transit route optimization")
    print("🎮 Gaming:")
    print("   • NPC pathfinding")
    print("   • Strategy game AI")
    print("📊 Network Analysis:")
    print("   • Social network shortest paths")
    print("   • Supply chain optimization")
    
    # Interactive demo
    try:
        choice = input("\n🎮 Try interactive pathfinding demo? (y/n): ").lower()
        if choice in ['y', 'yes']:
            interactive_pathfinding()
    except KeyboardInterrupt:
        print("\n👋 Demo interrupted by user")
    
    print("\n🏆 EDUCATIONAL TAKEAWAYS:")
    print("=" * 26)
    print("🧠 Algorithm Design:")
    print("   • How greedy algorithms can be optimal")
    print("   • Importance of data structure choice (priority queue)")
    print("   • Trade-offs between time and space complexity")
    print("\n🔧 Problem Solving:")
    print("   • Breaking complex problems into simple steps")
    print("   • Using mathematical proof techniques")
    print("   • Understanding when algorithms apply to real problems")
    print("\n🌍 Impact:")
    print("   • One algorithm, countless applications")
    print("   • From 1956 to today - still relevant")
    print("   • Foundation for many other graph algorithms")
    
    print(f"\n✨ Visualizations saved to:")
    print(f"   /home/randy/WORK_SPACE/work/tools/pathfinding/")
    
    print("\n🎓 Dijkstra's genius: Simple idea, profound impact!")
    print("The algorithm that taught us optimal can be achievable!")

if __name__ == "__main__":
    main()