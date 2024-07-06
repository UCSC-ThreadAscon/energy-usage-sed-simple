#pragma once

#include "init.h"
#include <openthread/coap.h>

#define BATTERY_URI "battery"

#define CONNECTION_WAIT_TIME_MS 5000

/**
 * Empties all memory for `size` bytes starting at memory address `pointer`.
 *
 * @param[in] pointer: the pointer of the stack memory
 * @param[in] size:    the size of the memory that `pointer` points to
 *
 * I got the idea to use `memset()` to clear stack memory from
 * the Google Search AI:
 * https://docs.google.com/document/d/1o-NaEOA-vzWPCv7VX1dONUfwos2epveDk4H_Y2Y5g1Y/edit?usp=sharing
*/
static inline void EmptyMemory(void* pointer, size_t size) {
  memset(pointer, 0, size);
  return;
}

void checkConnection(otInstance *aInstance);