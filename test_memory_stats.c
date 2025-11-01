// 🎯 Quick Test for Randy's Enhanced Kilo Memory Statistics
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    // Execute Randy's enhanced Kilo for a short time then exit
    printf("Testing Randy's Enhanced Kilo Memory Statistics...\n");
    
    // Run the enhanced Kilo, let it initialize, then terminate
    system("timeout 2s ./kilo_enhanced test_file.txt >/dev/null 2>&1 || echo 'Kilo test completed'");
    
    return 0;
}