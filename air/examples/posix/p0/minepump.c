#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <time.h>

#include "minteger.h"
#include "msg_box.h"
#include "periodic_task.h"
#include "simu.h"
#include "utils.h"

#define MS_L1 70
#define MS_L2 100

#define Normal 0
#define Alarm1 1
#define Alarm2 2
#define LowLevel 0
#define HighLevel 1

/*****************************************************************************/
/* These global variables support communication and synchronization
   between tasks
*/

msg_box mbox_alarm;
sem_t synchro;
m_integer LvlWater, LvlMeth;

/*****************************************************************************/
/* WaterLevelMonitoring_Task is a periodic task, period = 250 ms. At
   each dispatch, it reads the HLS and LLS sensors.
   - If HLS is true, it sends "HighValue" to LvlWater m_integer
   - else, if LLS is false, it sends "LowValue" to LvlWater m_integer
*/
void WaterLevelMonitoring_Body(void)
{
    int level = LowLevel;

    if ((ReadHLS()) != 0)
    {
        level = HighLevel;
        MI_write(LvlWater, level);
    }
    else if (!ReadLLS())
    {
        level = LowLevel;
        MI_write(LvlWater, level);
    }
}

/*****************************************************************************/
/* MethaneMonitoring_Task is a periodic task, period = 100 ms. At each
   dispatch, it reads the MS sensor. Depending on the methane level
   (constant MS_L1 and MS_L2), it sends either normal, Alert1 or
   Alert2 to LvlMeth. At the end of the dispatch, it triggers the
   synchronization (semaphore synchro).
*/

void MethaneMonitoring_Body(void)
{
    int level = Normal;
    BYTE MS;

    MS = ReadMS();
    if (MS > MS_L2)
    {
        level = Alarm2;
    }
    else if (MS > MS_L1)
    {
        level = Alarm1;
    }
    else
    {
        level = Normal;
    }
    MI_write(LvlMeth, level);
    sem_post(&synchro);
}

/*****************************************************************************/
/* PumpCtrl_Task is a sporadic task, it is triggered by a synchronization
   semaphore, upon completion of the MethaneMonitoring task. This task
   triggers the alarm logic, and the pump.
   - if the alarm level is different from Normal, it sends the value 1
     to the mbox_alarm message box, otherwise it sends 0;
   - if the alarm level is Alarm2 then the pump is deactivated (cmd =
     0 sent to CmdPump); else, if the water level is high, then the
     pump is activated (cmd = 1 sent to CmdPump), else if the water
     level is low, then the pump is deactivate, otherwise the pump is
     left off.
*/

void *PumpCtrl_Body(void *no_argument)
{
    int niveau_eau, niveau_alarme, alarme;
    int cmd = 0;
    for (;;)
    {
        sem_wait(&synchro);
        niveau_alarme = MI_read(LvlMeth);
        if (niveau_alarme == Normal)
        {
            alarme = 0;
        }
        else
        {
            alarme = 1;
        }
        (void)msg_box_send(mbox_alarm, (char *)&alarme);

        if (niveau_alarme == Alarm2)
        {
            cmd = 0;
        }
        else
        {
            niveau_eau = MI_read(LvlWater);
            if (niveau_eau == HighLevel)
            {
                cmd = 1;
            }
            else if (niveau_eau == LowLevel)
            {
                cmd = 0;
            }
        }
        CmdPump(cmd);
    }
}

/*****************************************************************************/
/* CmdAlarm_Task is a sporadic task, it waits on a message from
   mbox_alarm, and calls CmdAlarm with the value read.
*/

void *CmdAlarm_Body()
{
    int value;
    for (;;)
    {
        (void)msg_box_receive(mbox_alarm, (char *)&value);

        CmdAlarm(value);
    }
}

/*****************************************************************************/
#ifdef RTEMS
void *POSIX_Init()
{
#else
int main(void)
{
#endif /* RTEMS */

    pthread_t T3, T4;
    (void)printf("START\n");

    InitSimu(); /* Initialize simulator */

    /* Initialize communication and synchronization primitives */
    mbox_alarm = msg_box_init(sizeof(int));
    sem_init(&synchro, 0, 0);
    LvlWater = MI_init(10);
    LvlMeth = MI_init(12);

    /* Create task WaterLevelMonitoring_Task */
    struct timespec period;
    period.tv_nsec = 250 * 1000 * 1000;
    period.tv_sec = 0;

    create_periodic_task(period, WaterLevelMonitoring_Body);

    /* Create task MethaneMonitoring_Task */
    struct timespec period2;
    period2.tv_nsec = 100 * 1000 * 1000;
    period2.tv_sec = 0;

    create_periodic_task(period2, MethaneMonitoring_Body);

    /* Create task PumpCtrl_Task */
    pthread_create(&T3, NULL, PumpCtrl_Body, NULL);

    /* Create task CmdAlarm_Task */
    pthread_create(&T4, NULL, CmdAlarm_Body, NULL);

    pthread_join(T3, 0);

#ifndef RTEMS
    return 0;
#else
    return NULL;
#endif
}

#ifdef RTEMS
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

#define CONFIGURE_UNIFIED_WORK_AREAS
#define CONFIGURE_UNLIMITED_OBJECTS

#define CONFIGURE_POSIX_INIT_THREAD_TABLE
#define CONFIGURE_INIT

#include <rtems/confdefs.h>
#endif /* RTEMS */
