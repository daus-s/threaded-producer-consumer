#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#define WORK_SIZE 1024
char    work_area[WORK_SIZE];

void *thread_function(void *arg);
sem_t *semid = NULL;


int main(int argc, char *argv[])
{

    int  status;

    pthread_t tid; /* the thread identifier */
    pthread_attr_t attr; /* set of thread attributes */

    /* create the named semaphore (choose a name) */
    semid = sem_open("pSem", O_CREAT | O_EXCL, 0644, 0);
    if (semid == NULL) {
        perror("Semaphore initialization failed");
        return -1;
    }


    /* get the default attributes */
    pthread_attr_init(&attr);

    /* create the thread */
    pthread_create(&tid,&attr,thread_function, NULL);

    /* Input test area */
    printf("Input some text. Enter 'end' to finish\n");
    while (strncmp("end", work_area, 3) != 0) {
        fgets(work_area, WORK_SIZE, stdin);
        sem_post(semid);
    }

    /* wait for the thread to exit */
    pthread_join(tid,NULL);

    printf("Thread finished \n");
    sem_unlink("pSem");

    return 0;

}

/* The thread will begin control in this function */
void *thread_function(void *arg)
{
    int i, upper;

    sem_wait(semid);

    while (strncmp("end", work_area, 3) != 0) {
        printf("You input %d characters\n", strlen(work_area) -1);
        sem_wait(semid);
    }

    pthread_exit(0);

}

