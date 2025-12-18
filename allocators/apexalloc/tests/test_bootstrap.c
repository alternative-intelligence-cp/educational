#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main() {
    printf("Testing bootstrap allocator...\n");
    
    // This will trigger bootstrap allocations
    void* handle = dlopen("./build/libapexalloc.so", RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        fprintf(stderr, "dlopen failed: %s\n", dlerror());
        return 1;
    }
    
    printf("✅ Bootstrap allocator survived dlopen!\n");
    
    // Try a simple allocation
    void* ptr = malloc(100);
    if (ptr) {
        printf("✅ malloc(100) succeeded: %p\n", ptr);
        free(ptr);
        printf("✅ free() succeeded\n");
    } else {
        printf("❌ malloc() returned NULL\n");
        return 1;
    }
    
    dlclose(handle);
    printf("✅ All bootstrap tests passed!\n");
    return 0;
}
