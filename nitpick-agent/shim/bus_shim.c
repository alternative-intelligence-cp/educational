#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <stdint.h>

static int bus_fd = -1;
static char g_bus_result[8192];

int32_t nitpick_bus_init(void) {
    mkfifo(".bus.fifo", 0666);
    // Open O_RDWR so it doesn't block waiting for a reader/writer
    bus_fd = open(".bus.fifo", O_RDWR | O_NONBLOCK);
    return (bus_fd != -1) ? 1 : 0;
}

const char* nitpick_bus_wait(void) {
    if (bus_fd == -1) return "ERROR: bus not initialized";
    
    // Flush stdout before blocking so prompt appears
    fflush(stdout);
    
    fd_set rfds;
    int max_fd = (bus_fd > STDIN_FILENO) ? bus_fd : STDIN_FILENO;
    
    while (1) {
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        FD_SET(bus_fd, &rfds);
        
        int r = select(max_fd + 1, &rfds, NULL, NULL, NULL);
        if (r < 0) return "ERROR: select failed";
        
        if (FD_ISSET(bus_fd, &rfds)) {
            // Read from bus
            char buf[4096];
            int n = read(bus_fd, buf, sizeof(buf)-1);
            if (n > 0) {
                buf[n] = '\0';
                snprintf(g_bus_result, sizeof(g_bus_result), "EVENT:%s", buf);
                return g_bus_result;
            }
        }
        
        if (FD_ISSET(STDIN_FILENO, &rfds)) {
            // Read from stdin
            char buf[4096];
            if (fgets(buf, sizeof(buf), stdin)) {
                snprintf(g_bus_result, sizeof(g_bus_result), "INPUT:%s", buf);
                return g_bus_result;
            } else {
                return "ERROR: EOF";
            }
        }
    }
}

void nitpick_bus_publish(const char* topic, const char* payload) {
    if (bus_fd != -1) {
        char buf[8192];
        snprintf(buf, sizeof(buf), "[%s] %s\n", topic, payload);
        write(bus_fd, buf, strlen(buf));
    }
}

int32_t nitpick_atoi(const char* s) {
    if (!s) return 0;
    return atoi(s);
}
