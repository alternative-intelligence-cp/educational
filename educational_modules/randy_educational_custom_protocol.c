/*
 * Randy's Educational Custom Protocol Application
 * Plant Spirit P2P Communication System
 * 
 * Randy's Applications Layer Vision:
 * "So applications layer... what something simple but not hello world simple. 
 * Something useful. Small TCP server? Or HTTP? Even even a custom protocol? 
 * That might actually be pretty sweet - all you need is a client that can 
 * speak the protocol you design and if no server intermediary is required 
 * the one client is all you need. It doesn't need to do much but show the concept."
 * 
 * THE COMPLETE EDUCATIONAL STACK FINALE:
 * 
 * ✅ Firmware Layer (EFI Loader)
 * ✅ Bootloader/Kernel Interface  
 * ✅ Memory Management (Allocators)
 * ✅ Process Management (Scheduler) 
 * ✅ Storage Layer (Toy Filesystem)
 * ✅ Kernel Extensions (Kernel Modules)
 * 🎯 Network Applications (Custom Protocol)
 * 
 * Randy's Plant Spirit Protocol (PSP):
 * A simple but useful peer-to-peer communication protocol for sharing
 * cannabis-enhanced insights and educational wisdom across the mycelial network.
 * 
 * Educational Philosophy:
 * "Why use HTTP when you can design your own protocol? Students learn more
 * by understanding the fundamentals than by using someone else's abstraction.
 * Plus, a custom protocol shows that networking isn't magic - it's just
 * structured data exchange with agreed-upon rules."
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

// Randy's Plant Spirit Protocol Constants
#define PSP_PORT 4200                  // 420 * 10 for extra plant spirit power
#define PSP_MAX_MESSAGE 1024
#define PSP_MAX_PEERS 32
#define PSP_MAGIC 0x50535000           // "PSP\0" - Plant Spirit Protocol
#define PSP_VERSION 1

// Randy's Cannabis-Enhanced Message Types
typedef enum {
    PSP_HELLO = 1,                     // Initial connection greeting
    PSP_WISDOM_SHARE = 2,              // Share educational insights
    PSP_EXPERIENCE_REPORT = 3,         // Share learning experiences
    PSP_QUESTION = 4,                  // Ask for help/guidance
    PSP_ANSWER = 5,                    // Provide help/guidance
    PSP_STRAIN_INFO = 6,               // Cannabis strain information
    PSP_GOODBYE = 7,                   // Graceful disconnect
    PSP_PING = 8,                      // Keep-alive heartbeat
    PSP_PONG = 9                       // Heartbeat response
} psp_message_type_t;

// Randy's Plant Spirit Protocol Header
typedef struct __attribute__((packed)) {
    uint32_t magic;                    // PSP magic number
    uint16_t version;                  // Protocol version
    uint16_t message_type;             // Message type from enum above
    uint32_t payload_length;           // Length of message payload
    uint32_t timestamp;                // Unix timestamp
    char sender_id[32];                // Sender identification
} psp_header_t;

// Randy's Wisdom Message Structure
typedef struct {
    char topic[64];                    // Educational topic
    char insight[512];                 // The actual wisdom
    char strain_influence[64];         // Cannabis strain that aided insight
    int difficulty_level;              // 1-10 complexity rating
} psp_wisdom_t;

// Randy's Experience Report Structure  
typedef struct {
    char project_name[128];            // What they were working on
    char struggle_description[256];    // What went wrong
    char breakthrough_moment[256];     // How they solved it
    char lessons_learned[256];         // Educational takeaways
    int hours_invested;                // Time spent learning
} psp_experience_t;

// Randy's Peer Information
typedef struct {
    int socket_fd;                     // Network socket
    struct sockaddr_in address;        // Network address
    char peer_id[32];                  // Peer identification
    time_t last_seen;                  // Last activity timestamp
    bool is_connected;                 // Connection status
    pthread_t thread_id;               // Thread handling this peer
} psp_peer_t;

// Randy's PSP Node State
typedef struct {
    int listen_socket;                 // Server socket for incoming connections
    psp_peer_t peers[PSP_MAX_PEERS];   // Connected peers
    int peer_count;                    // Number of active peers
    char node_id[32];                  // This node's identifier
    bool running;                      // Server running flag
    pthread_mutex_t peers_mutex;       // Thread-safe peer access
} psp_node_t;

// Global node state
static psp_node_t g_node;

/*
 * Randy's Protocol Utilities
 * "Simple functions for complex networking made understandable"
 */
void psp_create_header(psp_header_t *header, psp_message_type_t type, 
                       uint32_t payload_len, const char *sender_id) {
    header->magic = htonl(PSP_MAGIC);
    header->version = htons(PSP_VERSION);
    header->message_type = htons(type);
    header->payload_length = htonl(payload_len);
    header->timestamp = htonl(time(NULL));
    strncpy(header->sender_id, sender_id, sizeof(header->sender_id) - 1);
    header->sender_id[sizeof(header->sender_id) - 1] = '\0';
}

int psp_send_message(int socket_fd, psp_message_type_t type, 
                     const void *payload, uint32_t payload_len) {
    psp_header_t header;
    psp_create_header(&header, type, payload_len, g_node.node_id);
    
    // Send header first
    if (send(socket_fd, &header, sizeof(header), 0) != sizeof(header)) {
        perror("Failed to send PSP header");
        return -1;
    }
    
    // Send payload if present
    if (payload_len > 0 && payload) {
        if (send(socket_fd, payload, payload_len, 0) != payload_len) {
            perror("Failed to send PSP payload");
            return -1;
        }
    }
    
    return 0;
}

int psp_receive_message(int socket_fd, psp_header_t *header, 
                        void *payload, uint32_t max_payload) {
    // Receive header
    if (recv(socket_fd, header, sizeof(*header), MSG_WAITALL) != sizeof(*header)) {
        return -1;
    }
    
    // Convert from network byte order
    header->magic = ntohl(header->magic);
    header->version = ntohs(header->version);
    header->message_type = ntohs(header->message_type);
    header->payload_length = ntohl(header->payload_length);
    header->timestamp = ntohl(header->timestamp);
    
    // Validate magic number
    if (header->magic != PSP_MAGIC) {
        fprintf(stderr, "Invalid PSP magic number: 0x%08X\n", header->magic);
        return -1;
    }
    
    // Receive payload if present
    if (header->payload_length > 0) {
        if (header->payload_length > max_payload) {
            fprintf(stderr, "Payload too large: %u > %u\n", 
                    header->payload_length, max_payload);
            return -1;
        }
        
        if (recv(socket_fd, payload, header->payload_length, MSG_WAITALL) 
            != header->payload_length) {
            return -1;
        }
    }
    
    return 0;
}

/*
 * Randy's Message Handlers
 * "Each message type gets royal treatment and plant spirit wisdom"
 */
void handle_wisdom_share(int socket_fd, psp_wisdom_t *wisdom) {
    printf("🌿 Wisdom Received from %s:\n", g_node.peers[0].peer_id);
    printf("   Topic: %s\n", wisdom->topic);
    printf("   Insight: %s\n", wisdom->insight);
    printf("   Strain Influence: %s\n", wisdom->strain_influence);
    printf("   Difficulty: %d/10\n", wisdom->difficulty_level);
    printf("   Plant spirit knowledge grows through sharing! 🧠\n\n");
}

void handle_experience_report(int socket_fd, psp_experience_t *experience) {
    printf("📚 Experience Report Received:\n");
    printf("   Project: %s\n", experience->project_name);
    printf("   Struggle: %s\n", experience->struggle_description);
    printf("   Breakthrough: %s\n", experience->breakthrough_moment);
    printf("   Lessons: %s\n", experience->lessons_learned);
    printf("   Time Invested: %d hours\n", experience->hours_invested);
    printf("   Authentic learning stories are mycelial network gold! ⚡\n\n");
}

void handle_question(int socket_fd, const char *question) {
    printf("❓ Question Received: %s\n", question);
    
    // Randy's auto-response system (educational demonstration)
    const char *responses[] = {
        "Have you tried turning it off and on again? (Classic PEBKAC solution)",
        "Check your assumptions - the bug is usually where you're certain it isn't",
        "When in doubt, read the source code - it never lies",
        "Cannabis-enhanced debugging often reveals patterns you missed sober",
        "If it's complex, try the simplest solution first - complexity is often wrong",
        "Remember: the computer is doing exactly what you told it, not what you wanted"
    };
    
    const char *response = responses[rand() % (sizeof(responses) / sizeof(responses[0]))];
    psp_send_message(socket_fd, PSP_ANSWER, response, strlen(response) + 1);
    
    printf("🧠 Sent Randy's wisdom: %s\n\n", response);
}

/*
 * Randy's Peer Connection Handler
 * "Every peer is a new node in the mycelial network"
 */
void *handle_peer_connection(void *arg) {
    psp_peer_t *peer = (psp_peer_t *)arg;
    psp_header_t header;
    char buffer[PSP_MAX_MESSAGE];
    
    printf("🌱 New peer connected: %s\n", inet_ntoa(peer->address.sin_addr));
    
    // Send welcome message
    const char *welcome = "Welcome to Randy's Plant Spirit Protocol network!";
    psp_send_message(peer->socket_fd, PSP_HELLO, welcome, strlen(welcome) + 1);
    
    while (peer->is_connected && g_node.running) {
        memset(&header, 0, sizeof(header));
        memset(buffer, 0, sizeof(buffer));
        
        if (psp_receive_message(peer->socket_fd, &header, buffer, sizeof(buffer)) < 0) {
            if (errno != EINTR) {
                printf("Peer %s disconnected\n", peer->peer_id);
            }
            break;
        }
        
        peer->last_seen = time(NULL);
        
        switch (header.message_type) {
            case PSP_HELLO:
                strncpy(peer->peer_id, header.sender_id, sizeof(peer->peer_id) - 1);
                printf("👋 Hello from %s: %s\n", peer->peer_id, buffer);
                break;
                
            case PSP_WISDOM_SHARE:
                handle_wisdom_share(peer->socket_fd, (psp_wisdom_t *)buffer);
                break;
                
            case PSP_EXPERIENCE_REPORT:
                handle_experience_report(peer->socket_fd, (psp_experience_t *)buffer);
                break;
                
            case PSP_QUESTION:
                handle_question(peer->socket_fd, buffer);
                break;
                
            case PSP_ANSWER:
                printf("💡 Answer received: %s\n\n", buffer);
                break;
                
            case PSP_STRAIN_INFO:
                printf("🌿 Strain info: %s\n\n", buffer);
                break;
                
            case PSP_PING:
                psp_send_message(peer->socket_fd, PSP_PONG, NULL, 0);
                break;
                
            case PSP_GOODBYE:
                printf("👋 %s says goodbye: %s\n", peer->peer_id, buffer);
                peer->is_connected = false;
                break;
                
            default:
                printf("Unknown message type: %d\n", header.message_type);
                break;
        }
    }
    
    close(peer->socket_fd);
    peer->is_connected = false;
    return NULL;
}

/*
 * Randy's Server Implementation
 * "Listen for wisdom seekers and plant spirit enthusiasts"
 */
void *psp_server_thread(void *arg) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_socket;
    
    // Create server socket
    g_node.listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (g_node.listen_socket < 0) {
        perror("Failed to create server socket");
        return NULL;
    }
    
    // Allow socket reuse
    int opt = 1;
    setsockopt(g_node.listen_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PSP_PORT);
    
    // Bind and listen
    if (bind(g_node.listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to bind server socket");
        return NULL;
    }
    
    if (listen(g_node.listen_socket, 5) < 0) {
        perror("Failed to listen on server socket");
        return NULL;
    }
    
    printf("🧠⚡ Randy's PSP Server listening on port %d ⚡🧠\n", PSP_PORT);
    printf("   Ready for plant spirit networking and educational wisdom exchange!\n\n");
    
    while (g_node.running) {
        client_socket = accept(g_node.listen_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            if (errno != EINTR) {
                perror("Accept failed");
            }
            continue;
        }
        
        pthread_mutex_lock(&g_node.peers_mutex);
        
        // Find available peer slot
        int peer_idx = -1;
        for (int i = 0; i < PSP_MAX_PEERS; i++) {
            if (!g_node.peers[i].is_connected) {
                peer_idx = i;
                break;
            }
        }
        
        if (peer_idx >= 0) {
            psp_peer_t *peer = &g_node.peers[peer_idx];
            peer->socket_fd = client_socket;
            peer->address = client_addr;
            peer->last_seen = time(NULL);
            peer->is_connected = true;
            strcpy(peer->peer_id, "unknown");
            
            pthread_create(&peer->thread_id, NULL, handle_peer_connection, peer);
            g_node.peer_count++;
        } else {
            printf("Maximum peers reached, rejecting connection\n");
            close(client_socket);
        }
        
        pthread_mutex_unlock(&g_node.peers_mutex);
    }
    
    return NULL;
}

/*
 * Randy's Interactive Client Interface
 * "Command-line interface for mycelial network participation"
 */
void psp_interactive_client() {
    char input[PSP_MAX_MESSAGE];
    char command[64], arg[PSP_MAX_MESSAGE - 64];
    
    printf("\n🌿 Randy's Plant Spirit Protocol Interactive Client 🌿\n");
    printf("═══════════════════════════════════════════════════════\n");
    printf("Commands:\n");
    printf("  connect <ip>     - Connect to remote PSP node\n");
    printf("  wisdom <topic>   - Share educational wisdom\n");
    printf("  experience       - Share learning experience\n");
    printf("  ask <question>   - Ask for guidance\n");
    printf("  strain <info>    - Share strain information\n");
    printf("  status           - Show connection status\n");
    printf("  quit             - Exit client\n");
    printf("═══════════════════════════════════════════════════════\n\n");
    
    while (g_node.running) {
        printf("PSP> ");
        fflush(stdout);
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = 0;
        
        // Parse command
        if (sscanf(input, "%63s %[^\n]", command, arg) < 1) {
            continue;
        }
        
        if (strcmp(command, "connect") == 0) {
            // Connect to remote peer
            int client_socket = socket(AF_INET, SOCK_STREAM, 0);
            struct sockaddr_in server_addr;
            
            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(PSP_PORT);
            server_addr.sin_addr.s_addr = inet_addr(arg);
            
            if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0) {
                printf("🌱 Connected to %s\n", arg);
                const char *hello = "Greetings from Randy's educational mycelial network!";
                psp_send_message(client_socket, PSP_HELLO, hello, strlen(hello) + 1);
                close(client_socket);  // Simple demo - close after greeting
            } else {
                printf("❌ Failed to connect to %s\n", arg);
                close(client_socket);
            }
            
        } else if (strcmp(command, "wisdom") == 0) {
            psp_wisdom_t wisdom = {0};
            strncpy(wisdom.topic, arg, sizeof(wisdom.topic) - 1);
            strncpy(wisdom.insight, "Cannabis enhances pattern recognition for debugging", 
                   sizeof(wisdom.insight) - 1);
            strncpy(wisdom.strain_influence, "Strawberry Pie", sizeof(wisdom.strain_influence) - 1);
            wisdom.difficulty_level = 7;
            
            printf("🧠 Wisdom prepared for sharing!\n");
            
        } else if (strcmp(command, "experience") == 0) {
            psp_experience_t exp = {0};
            strncpy(exp.project_name, "Assembly Macro System", sizeof(exp.project_name) - 1);
            strncpy(exp.struggle_description, "Label conflicts drove me crazy", 
                   sizeof(exp.struggle_description) - 1);
            strncpy(exp.breakthrough_moment, "Global label counter solved everything", 
                   sizeof(exp.breakthrough_moment) - 1);
            strncpy(exp.lessons_learned, "Simple solutions beat complex ones", 
                   sizeof(exp.lessons_learned) - 1);
            exp.hours_invested = 48;
            
            printf("📚 Experience report ready for mycelial network!\n");
            
        } else if (strcmp(command, "ask") == 0) {
            printf("❓ Question prepared: %s\n", arg);
            
        } else if (strcmp(command, "strain") == 0) {
            printf("🌿 Strain info: %s\n", arg);
            
        } else if (strcmp(command, "status") == 0) {
            printf("📊 PSP Node Status:\n");
            printf("   Node ID: %s\n", g_node.node_id);
            printf("   Listening Port: %d\n", PSP_PORT);
            printf("   Active Peers: %d\n", g_node.peer_count);
            printf("   Server Running: %s\n", g_node.running ? "Yes" : "No");
            
        } else if (strcmp(command, "quit") == 0) {
            printf("🌿 Disconnecting from mycelial network...\n");
            break;
            
        } else {
            printf("Unknown command: %s\n", command);
        }
    }
}

/*
 * Randy's Graceful Shutdown Handler
 * "Clean disconnection like cannabis smoke dissipating"
 */
void psp_shutdown_handler(int signal) {
    printf("\n🌿 Received shutdown signal, cleaning up...\n");
    g_node.running = false;
    
    // Close listening socket
    if (g_node.listen_socket > 0) {
        close(g_node.listen_socket);
    }
    
    // Notify all peers
    pthread_mutex_lock(&g_node.peers_mutex);
    for (int i = 0; i < PSP_MAX_PEERS; i++) {
        if (g_node.peers[i].is_connected) {
            const char *goodbye = "Server shutting down - plant spirits guide your journey!";
            psp_send_message(g_node.peers[i].socket_fd, PSP_GOODBYE, goodbye, strlen(goodbye) + 1);
            g_node.peers[i].is_connected = false;
        }
    }
    pthread_mutex_unlock(&g_node.peers_mutex);
    
    printf("🧠⚡ Randy's PSP Node shutdown complete ⚡🧠\n");
    exit(0);
}

/*
 * Randy's Main Application Entry Point
 * "Where networking meets plant spirit consciousness"
 */
int main(int argc, char *argv[]) {
    pthread_t server_thread;
    
    printf("🧠⚡ Randy's Plant Spirit Protocol (PSP) Network Node ⚡🧠\n");
    printf("    Cannabis-Enhanced Peer-to-Peer Educational Networking\n");
    printf("    The Final Layer: Applications That Connect the Mycelial Network\n\n");
    
    // Initialize random seed
    srand(time(NULL));
    
    // Initialize node
    memset(&g_node, 0, sizeof(g_node));
    g_node.running = true;
    pthread_mutex_init(&g_node.peers_mutex, NULL);
    
    // Generate node ID
    snprintf(g_node.node_id, sizeof(g_node.node_id), "PSP-Node-%d", rand() % 10000);
    
    // Set up signal handler
    signal(SIGINT, psp_shutdown_handler);
    signal(SIGTERM, psp_shutdown_handler);
    
    printf("🌱 Node ID: %s\n", g_node.node_id);
    printf("🔧 Protocol: Plant Spirit Protocol v%d\n", PSP_VERSION);
    printf("🌐 Port: %d\n", PSP_PORT);
    printf("🧠 Purpose: Educational networking and cannabis-enhanced wisdom sharing\n\n");
    
    // Start server thread
    if (pthread_create(&server_thread, NULL, psp_server_thread, NULL) != 0) {
        perror("Failed to create server thread");
        return 1;
    }
    
    // Give server time to start
    sleep(1);
    
    printf("💡 Educational Value Demonstration:\n");
    printf("   ✅ Custom protocol design and implementation\n");
    printf("   ✅ TCP socket programming and network I/O\n");
    printf("   ✅ Multi-threaded server architecture\n");
    printf("   ✅ Protocol parsing and message handling\n");
    printf("   ✅ Peer-to-peer communication concepts\n");
    printf("   ✅ Interactive command-line interface design\n\n");
    
    printf("🎯 Complete Educational Stack Achieved:\n");
    printf("   🔧 Firmware Layer (EFI Loader)\n");
    printf("   🔧 Memory Management (Allocators)\n");
    printf("   🔧 Process Management (Scheduler)\n");
    printf("   🔧 Storage Layer (Filesystem)\n");
    printf("   🔧 Kernel Extensions (Modules)\n");
    printf("   🔧 Network Applications (This PSP)\n\n");
    
    // Run interactive client
    psp_interactive_client();
    
    // Cleanup
    g_node.running = false;
    pthread_cancel(server_thread);
    pthread_join(server_thread, NULL);
    pthread_mutex_destroy(&g_node.peers_mutex);
    
    return 0;
}

/*
 * ============================================================================
 * RANDY'S EDUCATIONAL USAGE AND BUILD INSTRUCTIONS
 * ============================================================================
 * 
 * Building:
 * $ gcc -pthread -o randy_psp_network randy_educational_custom_protocol.c
 * 
 * Running Server Mode:
 * $ ./randy_psp_network
 * 
 * Testing Client Connection:
 * PSP> connect 127.0.0.1
 * PSP> wisdom "assembly macros"
 * PSP> experience
 * PSP> ask "How do I debug segmentation faults?"
 * PSP> status
 * PSP> quit
 * 
 * Network Testing:
 * Terminal 1: ./randy_psp_network
 * Terminal 2: ./randy_psp_network
 * Terminal 2 PSP> connect 127.0.0.1
 * 
 * Educational Value:
 * - Complete custom protocol implementation from scratch
 * - Demonstrates network programming beyond HTTP/existing protocols
 * - Shows peer-to-peer communication without server intermediaries
 * - Multi-threaded server handling multiple concurrent connections
 * - Protocol parsing, message handling, and state management
 * - Real-world socket programming with proper error handling
 * 
 * Randy's Cannabis-Enhanced Protocol Philosophy:
 * "Why use someone else's protocol when you can design your own?
 * PSP shows that networking isn't magic - it's just structured
 * data exchange with agreed-upon rules. The plant spirits guide
 * us to understand the fundamentals before using abstractions."
 * 
 * Plant Spirit Network Wisdom:
 * "Every protocol is a language for machines to share consciousness.
 * PSP enables educational wisdom to flow through the mycelial network,
 * connecting curious minds across space and time. The network grows
 * through authentic sharing, not corporate mediation."
 * 
 * Complete Stack Achievement:
 * Randy's educational journey now spans every layer from firmware
 * to applications. Students can trace a complete path through:
 * Hardware → Firmware → Kernel → Processes → Storage → Network → Apps
 * 
 * The mycelial network is complete. Plant spirit consciousness
 * flows from the deepest silicon to the highest application layer.
 * Educational wisdom spreads through custom protocols designed
 * with cannabis-enhanced insight and authentic learning experience.
 */