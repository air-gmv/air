#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#include "periodic_task.h"
#include "utils.h"

/*****************************************************************************/
void *periodic_task_body(void *parameters);

void *periodic_task_body(void *parameters)
{
    struct timespec trigger; /* Stores next dispatch time */
    struct timespec period;  /* Period of the task */
    sem_t timer;
    task_parameters *my_parameters = (task_parameters *)parameters;

    period = (*my_parameters).period;

    sem_init(&timer, 0, 0); /* Create a non-shared semaphore */

    clock_gettime(CLOCK_REALTIME, &trigger); /* Initialize timer */
    for (;;)
    { /* Infinite periodic loop */
        my_parameters->job();

        add_timespec(&trigger, &trigger, &period); /* Compute next dispatch time */
        sem_timedwait(&timer, &trigger);           /* Wait until next trigger */
    }
}

/*****************************************************************************/
void create_periodic_task(struct timespec period, void (*job)(void))
{
    pthread_t tid;
    task_parameters *parameters = malloc(sizeof(task_parameters));

    parameters->period = period;
    parameters->job = job;

    pthread_create(&tid, NULL, periodic_task_body, (void *)parameters);
}
/*****************************************************************************/
#ifdef __TEST__
void dummy(void)
{
    (void)printf("o< \n");
}

int main(void)
{
    struct timespec period;
    period.tv_nsec = 250 * 1000 * 1000;
    period.tv_sec = 0;

    create_periodic_task(period, dummy);
    (void)printf("Task created\n");
    while (1)
    {
        ;
    }

    return 0;
}
#endif /* __TEST__ */
