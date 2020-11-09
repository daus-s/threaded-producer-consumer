//
// Created by Daus Carmichael on 11/8/20.
//
#include <unistd.h>
#include "prodcon_a.c"

#define TRIALS 1000

int success[1365][TRIALS];
float rate[TRIALS];

int main()
{
    int fd = open("data.txt", O_RDWR | O_APPEND);


    for (int i = 0; i < 1365; ++i)
    {
        for (int j = 0; j < TRIALS; ++j)
        {
            success[i][j] = simulate(i);
        }
    }
    for (int i = 0; i < 1365; ++i)
    {
        float sum = 0.0;
        for (int j = 0; j < TRIALS; ++j)
        {
            sum += success[i][j];
        }
        rate[i] = sum/(float)TRIALS;
    }
    for (int l = 0; l < 1365; ++l)
    {
        char* str;
        str = malloc(32);
        sprintf(str, "%d: %f\n", l, rate[l]);
        printf("%s", str);
        //write(fd, str, 32);
    }
    close(fd);
}