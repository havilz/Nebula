#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    puts("========================================");
    puts("      Welcome to Nebula OS Shell        ");
    puts("========================================");
    puts("Type 'help' to see available userland commands.");
    return 0;
}
