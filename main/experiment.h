#pragma once

/**
 * How many event packets that the devices will send per year.
 */
#define NUM_EVENTS_FRONT_DOOR 36
#define NUM_EVENTS_AIR_MONITOR 10
#define NUM_EVENTS_BACK_DOOR 2

/**
 * Send an event packet every wakeup. Used for debugging purposes.
 */
#define NUM_EVENTS_DEBUG 365

#if (CONFIG_SCENARIO == 1)
#define NUM_EVENTS NUM_EVENTS_FRONT_DOOR
#elif (CONFIG_SCENARIO == 2)
#define NUM_EVENTS NUM_EVENTS_AIR_MONITOR
#elif (CONFIG_SCENARIO == 3)
#define NUM_EVENTS NUM_EVENTS_BACK_DOOR
#else
#define NUM_EVENTS NUM_EVENTS_DEBUG
#endif