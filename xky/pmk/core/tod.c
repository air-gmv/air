/**
 * @file
 * @author jaac
 * @brief
 */

#include <tod.h>
#include <workspace.h>
#include <configurations.h>

/**
 *  @brief Number of nano-seconds in a second
 *  @ingroup pmk_tod
 */
#define NANOSECONDS_PER_SECOND              (1000000000)
/**
 *  @brief Initial Seconds count (1970 to 1988)
 *  @ingroup pmk_tod
 */
#define INITIAL_SECOND_COUNT                 (567993600)

/**
 * @brief Flag to check whether the system time of day was defined
 */
int pmk_tod_defined;

/**
 * @brief System Time of Day
 */
xky_time_t pmk_tod_now;

/**
 * @brief System nano-seconds per tick
 */
static xky_u32_t ns_per_tick;

void pmk_tod_initialization(void) {
   
   /* Obtain nanoseconds. Here we trade space for speed. */
   ns_per_tick = pmk_get_usr_us_per_tick() * 1000;

   /* initialize the number of seconds */
   pmk_tod_now.tv_sec = INITIAL_SECOND_COUNT;

   /* initialize the number of nanoseconds */
   pmk_tod_now.tv_nsec = 0;

   /* TOD has not been set */
   pmk_tod_defined = 0;

}

void pmk_tod_update(void) {

    /* increment time of day */
    pmk_tod_now.tv_nsec = pmk_tod_now.tv_nsec + ns_per_tick;

    /* if the number of seconds surpasses a second */
    if(pmk_tod_now.tv_nsec >= NANOSECONDS_PER_SECOND){

        /* increment second count */
        pmk_tod_now.tv_sec = pmk_tod_now.tv_sec + 1;

        /* decrement nanoseconds count */
        pmk_tod_now.tv_nsec = pmk_tod_now.tv_nsec - NANOSECONDS_PER_SECOND;

    }
    return;
}
