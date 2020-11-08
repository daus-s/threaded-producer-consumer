//includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>


//define
#define MAX_DATA 65536

char mem[MAX_DATA];

void *thread_function(void *arg);
sem_t *semid = NULL;

int main(int argc, char** argv)
{
    int  status;

    pthread_t tid; /* the thread identifier */
    pthread_attr_t attr; /* set of thread attributes */

    int n = atoi(argv[1]);
    if (argc!=2)
    {
        perror("incorrect usage parameters:\n________________\nusage: ./prodcon [n]\nwhere n is a positive integer");
        return -1;
    }
    if (40*n > MAX_DATA)
    {
        perror("data size to large, n must be less than 1638\n40*n must be less than 65536(64K)");
        return -1;
    }


    /* create the named semaphore (choose a name) */
    semid = sem_open("semid", O_CREAT | O_EXCL, 0644, 0);

    if (semid == NULL)
    {
        perror("Semaphore initialization failed");
        return -1;
    }


    /* get the default attributes
    create the thread */
    pthread_attr_init(&attr);
    pthread_create(&tid,&attr,thread_function, NULL);

    /* generate data */
    while (strncmp("end", work_area, 3) != 0)
    {
        fgets(work_area, WORK_SIZE, stdin);
        sem_post(semid);
    }

    /* wait for the thread to exit */
    pthread_join(tid,NULL);

    printf("Thread finished \n");
    sem_unlink("pSem");
    free(mem);

    return 0;

}

void *thread_function(void *arg)
{
    int i, upper;

    sem_wait(semid);

    while (strncmp("end", work_area, 3) != 0)
    {
        printf("You input %d characters\n", strlen(work_area) -1);
        sem_wait(semid);
    }

    pthread_exit(0);
}
