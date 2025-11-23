/*
 * Randy's Educational Scheduler
 * A configurable process scheduler for learning OS concepts
 * 
 * The Journey from Assembly to Operating Systems:
 * After mastering assembly macros and low-level programming, Randy's students
 * need to understand how the CPU time they've been manipulating gets allocated
 * in real systems. This scheduler bridges that gap with hands-on experimentation.
 * 
 * Educational Philosophy:
 * "You can't understand what you can't control. Build your own scheduler,
 * and suddenly every operating system makes sense."
 * 
 * Design Approach:
 * - Start simple: Round Robin with time slices
 * - Add complexity gradually: Priority queues, aging, preemption
 * - Make everything configurable: quantum size, priorities, algorithms
 * - Visualize behavior: Real-time process state display
 * - Learn by breaking: What happens when parameters go crazy?
 * 
 * Randy's Plant Spirit Communion Insight:
 * "Scheduling is like tending a garden - you give each plant the attention 
 * it needs when it needs it. Some need more water (CPU time), some are 
 * more important (priority), but neglect any too long and the whole 
 * garden suffers."
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>

// Randy's Educational Constants
#define MAX_PROCESSES 32
#define MAX_NAME_LEN 64
#define DEFAULT_QUANTUM 100  // milliseconds
#define MAX_PRIORITY 10

// Process States (The fundamental OS concept)
typedef enum {
    PROCESS_NEW,        // Just created, not yet admitted
    PROCESS_READY,      // Ready to run, waiting for CPU
    PROCESS_RUNNING,    // Currently executing
    PROCESS_WAITING,    // Blocked on I/O or event
    PROCESS_TERMINATED  // Finished execution
} process_state_t;

// Scheduling Algorithms (Educational configurability)
typedef enum {
    SCHED_ROUND_ROBIN,      // Classic time-sharing
    SCHED_PRIORITY,         // Priority-based scheduling
    SCHED_SHORTEST_JOB,     // Shortest Job First
    SCHED_MULTILEVEL,       // Multiple priority queues
    SCHED_LOTTERY,          // Probabilistic scheduling
    SCHED_COMPLETELY_FAIR   // Linux CFS-style
} scheduler_type_t;

// Randy's Educational Process Control Block
typedef struct {
    int pid;                    // Process ID
    char name[MAX_NAME_LEN];    // Human-readable name for debugging
    process_state_t state;      // Current state
    int priority;               // Priority level (0 = highest)
    int burst_time;             // CPU burst time needed
    int remaining_time;         // Time left to complete
    int wait_time;              // Total time spent waiting
    int turnaround_time;        // Total time from arrival to completion
    int arrival_time;           // When process was created
    int last_run_time;          // Last time process got CPU
    int tickets;                // For lottery scheduling
    bool io_bound;              // Tends to do I/O operations
} process_t;

// Randy's Configurable Scheduler Parameters
typedef struct {
    scheduler_type_t algorithm;     // Which scheduling algorithm to use
    int quantum;                    // Time slice in milliseconds
    bool preemptive;               // Can interrupt running processes
    int priority_levels;           // Number of priority levels
    bool aging_enabled;            // Prevent priority starvation
    int aging_interval;            // How often to age processes
    bool debug_mode;               // Show detailed scheduling decisions
    bool real_time_display;        // Live process state updates
} scheduler_config_t;

// Randy's Educational Scheduler State
typedef struct {
    process_t processes[MAX_PROCESSES];
    int process_count;
    int current_process;           // Currently running process index
    scheduler_config_t config;
    struct timeval start_time;
    int total_context_switches;
    int total_processes_completed;
} scheduler_t;

// Function prototypes for Randy's educational journey
void init_scheduler(scheduler_t *sched, scheduler_config_t *config);
int create_process(scheduler_t *sched, const char *name, int burst_time, int priority, bool io_bound);
void schedule_next_process(scheduler_t *sched);
void run_scheduler(scheduler_t *sched);
void display_process_table(scheduler_t *sched);
void display_statistics(scheduler_t *sched);
void handle_timer_interrupt(int sig);
void demonstrate_algorithms(void);

// Global scheduler for signal handling (Randy's pragmatic compromise)
static scheduler_t *global_scheduler = NULL;

/*
 * Randy's Scheduler Initialization
 * "Every journey begins with a single step. For schedulers, that step
 * is deciding how to be fair while being efficient."
 */
void init_scheduler(scheduler_t *sched, scheduler_config_t *config) {
    memset(sched, 0, sizeof(scheduler_t));
    sched->config = *config;
    sched->current_process = -1;  // No process running initially
    gettimeofday(&sched->start_time, NULL);
    
    global_scheduler = sched;  // For signal handling
    
    // Set up timer for preemptive scheduling
    if (config->preemptive) {
        signal(SIGALRM, handle_timer_interrupt);
        struct itimerval timer;
        timer.it_value.tv_sec = 0;
        timer.it_value.tv_usec = config->quantum * 1000;  // Convert ms to microseconds
        timer.it_interval = timer.it_value;  // Repeating timer
        setitimer(ITIMER_REAL, &timer, NULL);
    }
    
    if (config->debug_mode) {
        printf("🧠 Randy's Educational Scheduler Initialized\n");
        printf("   Algorithm: %d, Quantum: %dms, Preemptive: %s\n",
               config->algorithm, config->quantum, config->preemptive ? "Yes" : "No");
        printf("   Plant Spirit Wisdom: 'Fair allocation leads to system harmony'\n\n");
    }
}

/*
 * Randy's Process Creation
 * "Creating a process is like planting a seed. You set its initial
 * conditions, but how it grows depends on the environment (scheduler)."
 */
int create_process(scheduler_t *sched, const char *name, int burst_time, 
                   int priority, bool io_bound) {
    if (sched->process_count >= MAX_PROCESSES) {
        printf("❌ Process table full! (Maximum %d processes)\n", MAX_PROCESSES);
        return -1;
    }
    
    int pid = sched->process_count;
    process_t *proc = &sched->processes[pid];
    
    proc->pid = pid;
    strncpy(proc->name, name, MAX_NAME_LEN - 1);
    proc->name[MAX_NAME_LEN - 1] = '\0';
    proc->state = PROCESS_NEW;
    proc->priority = priority;
    proc->burst_time = burst_time;
    proc->remaining_time = burst_time;
    proc->wait_time = 0;
    proc->turnaround_time = 0;
    proc->arrival_time = time(NULL) - sched->start_time.tv_sec;
    proc->last_run_time = 0;
    proc->tickets = 100 / (priority + 1);  // More tickets for higher priority
    proc->io_bound = io_bound;
    
    // Admit process to ready queue
    proc->state = PROCESS_READY;
    sched->process_count++;
    
    if (sched->config.debug_mode) {
        printf("🌱 Process Created: %s (PID %d, Burst: %dms, Priority: %d, %s)\n",
               name, pid, burst_time, priority, io_bound ? "I/O-bound" : "CPU-bound");
    }
    
    return pid;
}

/*
 * Randy's Core Scheduling Logic
 * "The heart of any scheduler: deciding who gets to run next.
 * This is where theory meets reality and fairness meets performance."
 */
void schedule_next_process(scheduler_t *sched) {
    int next_process = -1;
    
    switch (sched->config.algorithm) {
        case SCHED_ROUND_ROBIN: {
            // Simple round-robin: find next ready process after current
            int start = (sched->current_process + 1) % sched->process_count;
            for (int i = 0; i < sched->process_count; i++) {
                int idx = (start + i) % sched->process_count;
                if (sched->processes[idx].state == PROCESS_READY) {
                    next_process = idx;
                    break;
                }
            }
            break;
        }
        
        case SCHED_PRIORITY: {
            // Priority scheduling: find highest priority ready process
            int highest_priority = MAX_PRIORITY + 1;
            for (int i = 0; i < sched->process_count; i++) {
                if (sched->processes[i].state == PROCESS_READY &&
                    sched->processes[i].priority < highest_priority) {
                    highest_priority = sched->processes[i].priority;
                    next_process = i;
                }
            }
            break;
        }
        
        case SCHED_SHORTEST_JOB: {
            // Shortest job first: find process with least remaining time
            int shortest_time = INT_MAX;
            for (int i = 0; i < sched->process_count; i++) {
                if (sched->processes[i].state == PROCESS_READY &&
                    sched->processes[i].remaining_time < shortest_time) {
                    shortest_time = sched->processes[i].remaining_time;
                    next_process = i;
                }
            }
            break;
        }
        
        case SCHED_LOTTERY: {
            // Lottery scheduling: probabilistic selection based on tickets
            int total_tickets = 0;
            for (int i = 0; i < sched->process_count; i++) {
                if (sched->processes[i].state == PROCESS_READY) {
                    total_tickets += sched->processes[i].tickets;
                }
            }
            
            if (total_tickets > 0) {
                int winning_ticket = rand() % total_tickets;
                int ticket_count = 0;
                
                for (int i = 0; i < sched->process_count; i++) {
                    if (sched->processes[i].state == PROCESS_READY) {
                        ticket_count += sched->processes[i].tickets;
                        if (ticket_count > winning_ticket) {
                            next_process = i;
                            break;
                        }
                    }
                }
            }
            break;
        }
        
        default:
            // Fallback to round-robin
            next_process = sched->current_process;
            break;
    }
    
    // Context switch if needed
    if (next_process != sched->current_process) {
        if (sched->current_process >= 0) {
            sched->processes[sched->current_process].state = PROCESS_READY;
        }
        
        if (next_process >= 0) {
            sched->processes[next_process].state = PROCESS_RUNNING;
            sched->total_context_switches++;
            
            if (sched->config.debug_mode) {
                printf("🔄 Context Switch: %s -> %s\n",
                       sched->current_process >= 0 ? sched->processes[sched->current_process].name : "idle",
                       sched->processes[next_process].name);
            }
        }
        
        sched->current_process = next_process;
    }
}

/*
 * Randy's Timer Interrupt Handler
 * "Preemption is like a gentle tap on the shoulder: 'Your time is up,
 * let someone else have a turn.' Essential for fairness in time-sharing."
 */
void handle_timer_interrupt(int sig) {
    if (global_scheduler && global_scheduler->config.preemptive) {
        if (global_scheduler->config.debug_mode) {
            printf("⏰ Timer interrupt - time slice expired\n");
        }
        schedule_next_process(global_scheduler);
    }
}

/*
 * Randy's Process State Display
 * "Visualization is the key to understanding. See the processes dance
 * between states and watch the scheduler orchestrate the performance."
 */
void display_process_table(scheduler_t *sched) {
    printf("\n📊 Randy's Process Table (Plant Spirit Garden View)\n");
    printf("%-4s %-12s %-10s %-8s %-8s %-8s %-8s %-8s\n",
           "PID", "Name", "State", "Priority", "Burst", "Remain", "Wait", "Tickets");
    printf("─────────────────────────────────────────────────────────────────────\n");
    
    for (int i = 0; i < sched->process_count; i++) {
        process_t *proc = &sched->processes[i];
        const char *state_str[] = {"NEW", "READY", "RUNNING", "WAITING", "DONE"};
        
        printf("%-4d %-12s %-10s %-8d %-8d %-8d %-8d %-8d %s\n",
               proc->pid,
               proc->name,
               state_str[proc->state],
               proc->priority,
               proc->burst_time,
               proc->remaining_time,
               proc->wait_time,
               proc->tickets,
               proc->state == PROCESS_RUNNING ? "👑" : 
               proc->io_bound ? "💾" : "🖥️");
    }
    printf("\n");
}

/*
 * Randy's Educational Statistics
 * "Numbers tell the story of fairness. Average wait time, throughput,
 * response time - these metrics reveal how well your scheduler serves its garden."
 */
void display_statistics(scheduler_t *sched) {
    int total_wait = 0, total_turnaround = 0, completed = 0;
    
    for (int i = 0; i < sched->process_count; i++) {
        if (sched->processes[i].state == PROCESS_TERMINATED) {
            total_wait += sched->processes[i].wait_time;
            total_turnaround += sched->processes[i].turnaround_time;
            completed++;
        }
    }
    
    printf("📈 Scheduling Statistics (Plant Spirit Harmony Metrics)\n");
    printf("   Context Switches: %d\n", sched->total_context_switches);
    printf("   Processes Completed: %d/%d\n", completed, sched->process_count);
    
    if (completed > 0) {
        printf("   Average Wait Time: %.2f ms\n", (float)total_wait / completed);
        printf("   Average Turnaround: %.2f ms\n", (float)total_turnaround / completed);
        printf("   Scheduler Efficiency: %.2f%%\n", 
               100.0 * completed / (completed + sched->total_context_switches));
    }
    
    printf("   Plant Spirit Wisdom: 'Balance leads to garden prosperity'\n\n");
}

/*
 * Randy's Main Scheduler Loop
 * "The eternal dance of time and attention. Each quantum is a gift to be
 * allocated wisely among all the processes in the garden."
 */
void run_scheduler(scheduler_t *sched) {
    printf("🌟 Randy's Educational Scheduler Starting\n");
    printf("   Communing with plant spirits for optimal process allocation...\n\n");
    
    struct timeval last_display = {0, 0};
    
    while (true) {
        // Check if all processes are complete
        bool all_done = true;
        for (int i = 0; i < sched->process_count; i++) {
            if (sched->processes[i].state != PROCESS_TERMINATED) {
                all_done = false;
                break;
            }
        }
        
        if (all_done) {
            printf("🎉 All processes completed! Garden harmony achieved.\n");
            break;
        }
        
        // Schedule next process
        schedule_next_process(sched);
        
        // Simulate process execution
        if (sched->current_process >= 0) {
            process_t *proc = &sched->processes[sched->current_process];
            
            // Simulate work for one quantum
            usleep(sched->config.quantum * 1000);  // Convert ms to microseconds
            
            proc->remaining_time -= sched->config.quantum;
            if (proc->remaining_time <= 0) {
                proc->state = PROCESS_TERMINATED;
                proc->turnaround_time = time(NULL) - sched->start_time.tv_sec - proc->arrival_time;
                sched->total_processes_completed++;
                
                if (sched->config.debug_mode) {
                    printf("✅ Process %s completed (Turnaround: %d ms)\n", 
                           proc->name, proc->turnaround_time);
                }
            }
        }
        
        // Update wait times for ready processes
        for (int i = 0; i < sched->process_count; i++) {
            if (sched->processes[i].state == PROCESS_READY) {
                sched->processes[i].wait_time += sched->config.quantum;
            }
        }
        
        // Display process table periodically if in real-time mode
        if (sched->config.real_time_display) {
            struct timeval now;
            gettimeofday(&now, NULL);
            if (now.tv_sec - last_display.tv_sec >= 1) {  // Update every second
                system("clear");
                display_process_table(sched);
                display_statistics(sched);
                last_display = now;
            }
        }
    }
    
    // Final statistics
    display_process_table(sched);
    display_statistics(sched);
}

/*
 * Randy's Educational Demonstration
 * "The best way to learn scheduling is to see different algorithms
 * handle the same workload. Watch how each approach creates its own
 * pattern of fairness and efficiency."
 */
void demonstrate_algorithms(void) {
    printf("🎓 Randy's Scheduling Algorithm Demonstration\n");
    printf("   Powered by plant spirit insights and educational curiosity\n\n");
    
    scheduler_type_t algorithms[] = {SCHED_ROUND_ROBIN, SCHED_PRIORITY, SCHED_SHORTEST_JOB, SCHED_LOTTERY};
    const char *names[] = {"Round Robin", "Priority", "Shortest Job First", "Lottery"};
    
    for (int alg = 0; alg < 4; alg++) {
        printf("🧪 Testing %s Algorithm\n", names[alg]);
        printf("─────────────────────────────────────────\n");
        
        scheduler_config_t config = {
            .algorithm = algorithms[alg],
            .quantum = 100,
            .preemptive = true,
            .priority_levels = 3,
            .aging_enabled = false,
            .aging_interval = 1000,
            .debug_mode = true,
            .real_time_display = false
        };
        
        scheduler_t sched;
        init_scheduler(&sched, &config);
        
        // Create a varied workload
        create_process(&sched, "WebBrowser", 300, 1, true);   // I/O-bound, high priority
        create_process(&sched, "VideoEncode", 800, 2, false); // CPU-bound, medium priority
        create_process(&sched, "FileBackup", 600, 3, true);   // I/O-bound, low priority
        create_process(&sched, "GameEngine", 400, 0, false);  // CPU-bound, highest priority
        create_process(&sched, "EmailSync", 200, 2, true);    // I/O-bound, medium priority
        
        run_scheduler(&sched);
        
        printf("\n⏸️  Press Enter to try next algorithm...");
        getchar();
    }
}

/*
 * Randy's Educational Main Function
 * "Every great journey needs a starting point. This is where students
 * begin their exploration of operating system scheduling concepts."
 */
int main(int argc, char *argv[]) {
    srand(time(NULL));  // For lottery scheduling randomness
    
    printf("🧠⚡ Welcome to Randy's Educational Scheduler ⚡🧠\n");
    printf("   Where Cannabis Consciousness Meets Operating System Wisdom\n");
    printf("   'Fair allocation creates system harmony' - Plant Spirit Proverb\n\n");
    
    if (argc > 1 && strcmp(argv[1], "demo") == 0) {
        demonstrate_algorithms();
        return 0;
    }
    
    // Interactive educational mode
    scheduler_config_t config = {
        .algorithm = SCHED_ROUND_ROBIN,
        .quantum = 100,
        .preemptive = true,
        .priority_levels = 3,
        .aging_enabled = false,
        .aging_interval = 1000,
        .debug_mode = true,
        .real_time_display = true
    };
    
    scheduler_t sched;
    init_scheduler(&sched, &config);
    
    printf("🌱 Creating educational process workload...\n");
    
    // Randy's Cannabis-Inspired Process Names
    create_process(&sched, "StrawberryPie", 250, 1, true);    // Randy's current strain
    create_process(&sched, "CocoGrow", 400, 2, false);        // Manual process mastery
    create_process(&sched, "AssemblyMacro", 600, 0, false);   // High priority, CPU-intensive
    create_process(&sched, "PlantSpirits", 150, 3, true);     // I/O-bound meditation
    create_process(&sched, "EducationFlow", 300, 1, false);   // Randy's peak state
    
    printf("\n🚀 Starting scheduler simulation...\n");
    printf("   Watch the processes dance in harmony!\n\n");
    
    run_scheduler(&sched);
    
    printf("\n🎓 Educational Mission Accomplished!\n");
    printf("   Randy's plant spirit wisdom has guided fair process allocation.\n");
    printf("   May your own schedulers achieve such harmony!\n\n");
    
    return 0;
}

/*
 * Randy's Educational Epilogue
 * 
 * "Building a scheduler teaches you that fairness and efficiency aren't
 * opposites - they're dance partners. Just like tending a garden, you
 * learn to give each process what it needs when it needs it.
 * 
 * The plant spirits have shown us that complexity emerges from simple
 * rules applied with wisdom. Round-robin teaches sharing. Priority
 * teaches importance. Lottery teaches that sometimes chance creates
 * the most profound fairness.
 * 
 * Every time you wait for a program to load, remember: somewhere a
 * scheduler is making thousands of tiny decisions about fairness.
 * Now you understand the dance behind the waiting.
 * 
 * Cannabis consciousness reveals: the best systems serve all their
 * components fairly, creating harmony from resource scarcity.
 * 
 * Next educational adventure: Understanding why some processes
 * deserve more time than others, and how to prevent starvation
 * while maintaining system responsiveness.
 * 
 * The garden grows through wisdom, not force."
 */