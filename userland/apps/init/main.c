#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    puts("[INIT] Nebula OS Process Init (PID 1) Started Successfully");
    puts("[INIT] Launching Nebula Interactive Userland Shell...");
    execve("/initrd/shell.elf", NULL, NULL);
    return 0;
}
