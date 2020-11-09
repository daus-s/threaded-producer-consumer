//includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include <pthread.h>
#include "ip_checksum.c"


//define
#define MEMSIZE 65536
#define DATASIZE 40

const char* name = "shared_memory_tpc.c";
bool flag = true;

typedef struct
{
     int item_no;                           /* number of the item produced */
     unsigned int check_sum;                /* 16-bit Internet checksum */
     unsigned char arr[DATASIZE];           /* random generated data */
} item;

/*
 *mem is the shared memory that all items are written to.
 */
item* buffer;

/*
 *the thread will execute thread_function
 */
void *thread_function(void *arg);

/*
 *named semaphore
 */
sem_t *semid = NULL;


/*
 * number of items in the buffer
 */
int n;

pthread_mutex_t mutex;
/*
 *populates random data into a char* (array) passed by its pointer and size n
 *as well as giving values to the other fields of item
 */
void func_produce(item* item_ptr)
{
    for (size_t i = 0; i < DATASIZE; ++i)
    {
        item_ptr->arr[i]=(unsigned char) rand() % 256;
    }
    //printf("%s\n", item_ptr->payload);
    //item_no, check_sum are assigned in other method
}

int simulate(int c)
{

    flag = true;
    //seed random
    time_t t;
    srand((unsigned) time(&t));

    int status;

    pthread_t tid; /* the thread identifier */
    pthread_attr_t thread_attr; /* set of thread attributes */

    pthread_mutexattr_t mutex_attr;


    n = c;
    pthread_mutex_init(&mutex, &mutex_attr);

    /* create the named semaphore (choose a name) */
    semid = sem_open("semid", O_CREAT | O_EXCL, 0644, 1);

    if (semid == NULL)
    {
        perror("Semaphore initialization failed");
        return -1;
    }

    int s = sizeof(item)*n;
    if (s >= MEMSIZE)
    {
        int max = (int)((float)(MEMSIZE)/((float)(sizeof(item))));
        printf("data size to large, n must be less than %d\n40*n must be less than 65536(64K)\n", max);
        return -1;
    }
    buffer = malloc(s);


    /* get the default attributes
    create the thread */
    pthread_attr_init(&thread_attr);
    pthread_create(&tid,&thread_attr, thread_function, NULL);


    /* generate data */
    for (size_t i = 0; i < n; ++i)
    {
        pthread_mutex_lock(&mutex);
        //create item instance
        item instance;
        func_produce(&instance);
        instance.item_no = i;
        instance.check_sum = ip_checksum(instance.arr, DATASIZE);
        memcpy(&(buffer[i]), &instance, sizeof(item));
        pthread_mutex_unlock(&mutex);

        sem_post(semid);
    }

    /* wait for the thread to exit */
    pthread_join(tid, NULL);
    sem_unlink("semid");
    pthread_mutex_destroy(&mutex);
    free(buffer);

    if (flag)
        return 1;
    else
        return 0;

}

void *thread_function(void *arg)
{
    int i, upper;

    sem_wait(semid);

    item instance;
    //read data
    for (int i = 0; i < n; ++i)
    {
        pthread_mutex_lock(&mutex);

        memcpy(&instance, &(buffer[i]), sizeof(item));

        char *buffer_data = malloc(DATASIZE);
        memcpy(buffer_data, &(buffer[i]).arr, DATASIZE);

        char *instance_data = malloc(DATASIZE);
        memcpy(instance_data, instance.arr, DATASIZE);

        if (strcmp(instance_data, buffer_data) != 0) {
            //printf("%s=/%s\n", instance_data, buffer_data);
            //printf("error, incorrect data copied when checking: data\n");
            flag = false;
        }
        if (instance.item_no != i) {
            //printf("%d=/%d\n", instance.item_no, i);
            //printf("error, incorrect data copied when checking: item continuity\n");
            flag = false;
        }
        if (instance.check_sum != ip_checksum(instance.arr, DATASIZE)) {
            //printf("%d=/%d\n", instance.check_sum, ip_checksum(instance.arr, DATASIZE));
            //printf("error, incorrect data copied when checking: checksum\n");
            flag = false;
        }
        free(buffer_data);
        free(instance_data);

        pthread_mutex_lock(&mutex);
        sem_wait(semid);
    }


    pthread_exit(0);
}
