#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <dirent.h>

int32_t workspace_exec(const char* cmd) {
    if (!cmd) return 0;
    int ret = system(cmd);
    return ret == 0 ? 1 : 0;
}

int32_t workspace_chdir(const char* path) {
    if (!path) return 0;
    int ret = chdir(path);
    return ret == 0 ? 1 : 0;
}

const char* workspace_getcwd(void) {
    static char buf[4096];
    if (getcwd(buf, sizeof(buf))) {
        return buf;
    }
    return "";
}

static char g_original_cwd[4096] = {0};

void workspace_set_original_cwd(const char* path) {
    if (path) strncpy(g_original_cwd, path, 4095);
}

const char* workspace_get_original_cwd(void) {
    return g_original_cwd;
}

#define MAX_BUFFER_SIZE (8 * 1024 * 1024)
static char g_workspace_io_buf[MAX_BUFFER_SIZE];

const char* nitpick_workspace_list_dir(const char* path) {
    static char buf[32768];
    buf[0] = '\0';
    DIR* d = opendir(path);
    if (d) {
        struct dirent* dir;
        while ((dir = readdir(d)) != NULL) {
            strncat(buf, dir->d_name, 32767 - strlen(buf));
            strncat(buf, "\n", 32767 - strlen(buf));
        }
        closedir(d);
    }
    return buf;
}

const char* workspace_run_sync(const char* cmd) {
    if (!cmd) return "";
    FILE* fp = popen(cmd, "r");
    if (!fp) return "Failed to execute command.";
    
    size_t len = fread(g_workspace_io_buf, 1, MAX_BUFFER_SIZE - 1, fp);
    g_workspace_io_buf[len] = '\0';
    pclose(fp);
    return g_workspace_io_buf;
}

const char* workspace_read_file(const char* path) {
    if (!path) return "";
    FILE* fp = fopen(path, "rb");
    if (!fp) return "Failed to read file (does not exist or permission denied).";
    
    size_t len = fread(g_workspace_io_buf, 1, MAX_BUFFER_SIZE - 1, fp);
    g_workspace_io_buf[len] = '\0';
    fclose(fp);
    return g_workspace_io_buf;
}

int32_t workspace_write_file(const char* path, const char* content) {
    if (!path || !content) return 0;
    FILE* fp = fopen(path, "wb");
    if (!fp) return 0;
    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, fp);
    fclose(fp);
    return written == len ? 1 : 0;
}

const char* workspace_get_env(const char* name) {
    if (!name) return "";
    const char* val = getenv(name);
    if (!val) return "";
    return val;
}

int32_t workspace_spawn_bg(const char* cmd) {
    if (!cmd) return 0;
    // We append " &" to the command to run it in the background
    char buffer[4096];
    snprintf(buffer, sizeof(buffer), "%s &", cmd);
    int ret = system(buffer);
    return ret == 0 ? 1 : 0;
}
