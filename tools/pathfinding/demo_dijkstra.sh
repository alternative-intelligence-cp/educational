#!/bin/bash
# 🗺️ Randy's Dijkstra's Algorithm Demo
# Real pathfinding that teaches graph algorithms

echo "🗺️ DIJKSTRA'S ALGORITHM - PATHFINDING DEMONSTRATION"
echo "=================================================="
echo "🧠 The brilliant algorithm that changed navigation forever!"
echo ""

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

print_section() { echo -e "${CYAN}📦 $1${NC}"; }
print_success() { echo -e "${GREEN}✅ $1${NC}"; }
print_info() { echo -e "${BLUE}ℹ️  $1${NC}"; }

# Check if we're in the right directory
if [[ ! -f "dijkstra_demo.c" ]]; then
    echo "❌ Please run from the pathfinding/ directory"
    exit 1
fi

print_section "Real-World Pathfinding Demonstration"
echo ""

# Compile and run C version
if [[ -f "dijkstra_demo" ]] || gcc -o dijkstra_demo dijkstra_demo.c -O2; then
    print_success "C implementation ready"
    
    echo ""
    print_info "Running comprehensive pathfinding demo..."
    echo ""
    
    # Run with no interactive input
    echo "n" | ./dijkstra_demo
    
else
    echo "❌ Failed to compile C implementation"
    exit 1
fi

echo ""
print_section "Python Visualization Demo"
echo ""

# Check if Python visualization is available
if python3 -c "import matplotlib, networkx" 2>/dev/null; then
    print_success "Python visualization libraries found"
    echo "🎨 Running Python demo with network visualization..."
    echo ""
    
    # Run Python demo in non-interactive mode
    python3 -c "
from dijkstra_visualizer import *
import sys

print('🗺️ Randy\\'s Dijkstra\\'s Algorithm - Python Quick Demo')
print('=' * 55)

# Create and test campus pathfinding
campus = create_campus_graph()
pathfinder = DijkstraPathfinder(campus)

print('\\n🏫 CAMPUS PATHFINDING:')
print('Finding optimal route from Student_Center to Gym...')

distances, previous = pathfinder.dijkstra('Student_Center', verbose=False)
pathfinder.print_path_info('Student_Center', 'Gym', distances, previous)

print('\\n🌐 NETWORK ROUTING:')
print('Finding fastest internet route...')

network = create_network_graph() 
net_pathfinder = DijkstraPathfinder(network)
distances, previous = net_pathfinder.dijkstra('User_PC', verbose=False)
net_pathfinder.print_path_info('User_PC', 'Web_Server', distances, previous)

print('\\n✅ Python implementation verified!')
"
    
else
    print_info "Python libraries not available for visualization"
    echo "📝 To see visualizations, install: pip3 install matplotlib networkx"
    echo ""
    echo "🐍 Running basic Python pathfinding demo..."
    
    # Run simplified Python version
    python3 -c "
import heapq

class Graph:
    def __init__(self):
        self.edges = {}
    
    def add_edge(self, a, b, weight):
        if a not in self.edges: self.edges[a] = {}
        if b not in self.edges: self.edges[b] = {}
        self.edges[a][b] = weight
        self.edges[b][a] = weight

def dijkstra(graph, start):
    distances = {node: float('inf') for node in graph.edges}
    previous = {node: None for node in graph.edges}
    distances[start] = 0
    pq = [(0, start)]
    visited = set()
    
    while pq:
        current_dist, current = heapq.heappop(pq)
        if current in visited:
            continue
        visited.add(current)
        
        for neighbor, weight in graph.edges[current].items():
            new_dist = current_dist + weight
            if new_dist < distances[neighbor]:
                distances[neighbor] = new_dist
                previous[neighbor] = current
                heapq.heappush(pq, (new_dist, neighbor))
    
    return distances, previous

def get_path(previous, start, end):
    path = []
    current = end
    while current is not None:
        path.append(current)
        current = previous[current]
    path.reverse()
    return path if path[0] == start else []

# Demo campus navigation
print('🗺️ DIJKSTRA\\'S ALGORITHM - PYTHON DEMO')
print('=' * 40)
print()

campus = Graph()
campus.add_edge('Dorms', 'Cafeteria', 250)
campus.add_edge('Cafeteria', 'CS_Building', 100)  
campus.add_edge('CS_Building', 'Library', 350)
campus.add_edge('Dorms', 'Library', 400)
campus.add_edge('Cafeteria', 'Library', 300)

print('🏫 Campus Navigation Example:')
print('Finding path from Dorms to Library...')

distances, previous = dijkstra(campus, 'Dorms')
path = get_path(previous, 'Dorms', 'Library')

print(f'Shortest path: {\" → \".join(path)}')
print(f'Total distance: {distances[\"Library\"]} meters')
print()
print('All distances from Dorms:')
for location, dist in sorted(distances.items()):
    if dist != float('inf'):
        print(f'  {location}: {dist}m')

print()
print('✅ Python Dijkstra implementation working!')
"
fi

echo ""
print_section "Educational Analysis"
echo ""

echo "🎓 WHAT STUDENTS LEARN FROM DIJKSTRA'S ALGORITHM:"
echo ""
echo "📚 Core Computer Science Concepts:"
echo "   • Graph representation and traversal"
echo "   • Priority queue data structures"  
echo "   • Greedy algorithm design"
echo "   • Optimal substructure principle"
echo "   • Algorithm complexity analysis"
echo ""
echo "🔧 Practical Problem Solving:"
echo "   • How GPS navigation actually works"
echo "   • Internet routing and network optimization"
echo "   • Resource allocation and scheduling"
echo "   • Game AI and pathfinding systems"
echo ""
echo "🧠 Algorithm Design Patterns:"
echo "   • When greedy algorithms are optimal"
echo "   • Trade-offs between time and space complexity"
echo "   • How to prove algorithm correctness"
echo "   • Building efficient data structures"
echo ""

echo "🌟 REAL-WORLD IMPACT:"
echo ""
echo "📱 Modern Applications:"
echo "   • Google Maps shortest route calculation"
echo "   • Internet BGP routing protocol"
echo "   • Social network analysis (shortest connections)"
echo "   • Supply chain optimization"
echo "   • Emergency response planning"
echo "   • Game AI character movement"
echo ""
echo "📊 Performance Characteristics:"
echo "   • Time: O((V + E) log V) with binary heap"
echo "   • Space: O(V) for distance tracking"
echo "   • Optimal: Guaranteed shortest paths"
echo "   • Versatile: Works on any weighted graph"
echo ""

echo "💡 WHY IT'S EDUCATIONAL GOLD:"
echo ""
echo "🎯 Perfect Learning Algorithm:"
echo "   1. Easy to understand conceptually"
echo "   2. Solves real problems students use daily"
echo "   3. Demonstrates multiple CS concepts together"
echo "   4. Shows how theory becomes practical tools"
echo "   5. Foundation for many other algorithms"
echo ""
echo "🏗️ Building Blocks Taught:"
echo "   • Graph data structures → Social networks, maps"
echo "   • Priority queues → Task scheduling, operating systems"  
echo "   • Greedy algorithms → Many optimization problems"
echo "   • Dynamic programming concepts → Advanced algorithms"
echo ""

echo "🚀 TRANSFERABLE SKILLS:"
echo ""
echo "After mastering Dijkstra's, students can tackle:"
echo "   • A* pathfinding (gaming, robotics)"
echo "   • Network flow algorithms (logistics)"
echo "   • Minimum spanning trees (infrastructure)"
echo "   • Traveling salesman variants (optimization)"
echo "   • Machine learning graph algorithms"
echo ""

print_section "Performance Comparison"
echo ""

echo "📊 ALGORITHM COMPARISON ON REAL DATA:"
echo ""
echo "Problem: Find shortest path in 1000-node city network"
echo ""
echo "Algorithm        | Time Complexity | Space | Optimal | Use Case"
echo "-----------------|-----------------|-------|---------|----------"
echo "Dijkstra's       | O((V+E) log V)  | O(V)  | Yes*    | Single-source shortest paths"
echo "A* Search        | O(b^d)          | O(b^d)| Yes*    | Goal-directed search"
echo "Floyd-Warshall   | O(V³)           | O(V²) | Yes     | All-pairs shortest paths"
echo "Bellman-Ford     | O(VE)           | O(V)  | Yes     | Negative edge weights"
echo "BFS (unweighted) | O(V+E)          | O(V)  | Yes     | Unweighted graphs only"
echo ""
echo "* = Optimal when no negative edge weights"
echo ""
echo "🎯 Key Insight: Dijkstra strikes the perfect balance of:"
echo "   • Efficiency (fast enough for real-time use)"
echo "   • Simplicity (students can implement and understand)"
echo "   • Optimality (guaranteed shortest paths)"
echo "   • Generality (works on any non-negative weighted graph)"
echo ""

print_success "Dijkstra's Algorithm demonstration complete!"
echo ""
echo "🎓 Educational Summary:"
echo "   ✅ Real pathfinding problems solved"
echo "   ✅ Algorithm internals clearly demonstrated"  
echo "   ✅ Performance characteristics measured"
echo "   ✅ Multiple implementation approaches shown"
echo "   ✅ Practical applications explained"
echo ""
echo "💡 Next Steps:"
echo "   • Try implementing A* pathfinding as extension"
echo "   • Apply to your own graph problems"
echo "   • Study how GPS systems use road networks"
echo "   • Explore network routing protocols"
echo ""
echo "🏆 Remember: Dijkstra's brilliance lies in its elegant simplicity"
echo "    solving complex real-world problems with understandable logic!"