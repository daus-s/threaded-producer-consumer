#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "ip_checksum.c"




void next_produced(char* item, int n)
{
    for (size_t i = 0; i < n; ++i)
    {
        int r = rand() % 256;
        item[i] = (char)r;
    }
}

int main()
{
    time_t t;
    srand((unsigned) time(&t));


    char* item;
    item = malloc(16);

    for (size_t i = 0; i < 5; i++)
    {
        next_produced(item, 16);
        printf("%s\n", item);
    }
}
