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

typedef struct
{
     int item_no;                           /* number of the item produced */
     unsigned short check_sum;              /* 16-bit Internet checksum */
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
    item_ptr->check_sum = ip_checksum(item_ptr->arr, DATASIZE);
    //item_no is assigned in other method
}

int main(int argc, char** argv)
{
    //seed random
    time_t t;
    srand((unsigned) time(&t));

    int status;

    pthread_t tid; /* the thread identifier */
    pthread_attr_t thread_attr; /* set of thread attributes */

    pthread_mutexattr_t mutex_attr;


    if (argc!=2)
    {
        perror("incorrect usage parameters:\n________________\nusage: ./prodcon [n]\nwhere n is a positive integer\n");
        return -1;
    }

    n = atoi(argv[1]);
    pthread_mutex_init(&mutex, &mutex_attr);

    /* create the named semaphore (choose a name) */
    semid = sem_open("semid", O_CREAT | O_EXCL, 0644, 0);

    if (semid == NULL)
    {
        perror("Semaphore initialization failed");
        return -1;
    }

    int s = sizeof(item)*n;
    if (s > MEMSIZE)
    {
        perror("data size to large, n must be less than 1638\n40*n must be less than 65536(64K)\n");
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
        memcpy(&(buffer[i]), &instance, sizeof(item));
        pthread_mutex_unlock(&mutex);

        sem_post(semid);
    }

    /* wait for the thread to exit */
    pthread_join(tid, NULL);
    sem_unlink("semid");
    free(buffer);
    return 0;

}

void *thread_function(void *arg)
{
    int i, upper;

    sem_wait(semid);
    pthread_mutex_lock(&mutex);

    item instance;
    //read data
    for (size_t i = 0; i < n; ++i)
    {
        memcpy(&instance, &(buffer[i]), sizeof(item));

        char* buffer_data = malloc(DATASIZE);
        memcpy(buffer_data, &(buffer[i]).arr, DATASIZE);

        char* instance_data = malloc(DATASIZE);
        memcpy(instance_data, instance.arr, DATASIZE);

        if (strncmp(instance_data, buffer_data, DATASIZE)==0)
        {
            printf("error, incorrect data copied when checking: data\n");
            exit(-1);
        }
        if (instance.item_no == i)
        {
            printf("error, incorrect data copied when checking: item continuity\n");
            exit(-1);
        }
        if (instance.check_sum==ip_checksum(instance.arr, DATASIZE))
        {
            printf("error, incorrect data copied when checking: checksum\n");
            exit(-1);
        }
        free(buffer_data);
        free(instance_data);

        pthread_mutex_lock(&mutex);
        sem_wait(semid);
    }


    pthread_exit(0);
}
