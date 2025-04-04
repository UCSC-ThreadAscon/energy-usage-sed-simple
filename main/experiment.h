#pragma once

#include <sdkconfig.h>

void printVariables(void);

#define CIPHER_SUITE CONFIG_THREAD_ASCON_CIPHER_SUITE
#define AES 0
#define NONE 1
#define LIBASCON_128A 4
#define LIBASCON_128 5

/**
 * How many event packets that the devices will send per year.
 */
#define NUM_EVENTS_FRONT_DOOR 36
#define NUM_EVENTS_AIR_MONITOR 10
#define NUM_EVENTS_WINDOW 2

/**
 * Send an event packet every wakeup. Used for debugging purposes.
 */
#define NUM_EVENTS_DEBUG 365

#if (CONFIG_SCENARIO == 1)
#define NUM_EVENTS NUM_EVENTS_FRONT_DOOR
#elif (CONFIG_SCENARIO == 2)
#define NUM_EVENTS NUM_EVENTS_AIR_MONITOR
#elif (CONFIG_SCENARIO == 3)
#define NUM_EVENTS NUM_EVENTS_WINDOW
#else
#define NUM_EVENTS NUM_EVENTS_DEBUG
#endif