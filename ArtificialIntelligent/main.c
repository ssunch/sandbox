#include <stdio.h>

int main(char argc, char *argv[])
{
    printf("Starting '%s'\n", (argv[0][0] == '.') ? &argv[0][2]: argv[0]);

    return 0;
}