#pragma once

#include "init.h"
#include "esp_random.h"
#include <openthread/coap.h>

#define PrintDelimiter() otLogNotePlat("---------------------------");
#define PrintCritDelimiter() otLogCritPlat("---------------------------");

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

void printMeshLocalEid(otInstance *aInstance);

/**
 * Bytes containing the UUID. A UUID will be 128 bits, in accordance
 * to RFC 4122:
 * https://datatracker.ietf.org/doc/html/rfc4122
*/
#define UUID_SIZE_BYTES 16
#define UUID_SIZE_WORDS UUID_SIZE_BYTES / 4
#define UUID_STRING_SIZE UUID_SIZE_BYTES * sizeof(char)

typedef struct uuid {
  uint8_t bytes[UUID_SIZE_BYTES];
} uuid;

void generateUUID(uuid *uuid);

typedef struct EventPayload
{
  uuid deviceId;
  bool eventOccured;
}
EventPayload;

typedef struct BatteryPayload
{
  uuid deviceId;
  uint8_t batteryLife;
}
BatteryPayload;

EventPayload createEventPayload(uuid deviceId);
BatteryPayload createBatteryPayload(uuid deviceId);

#define US_TO_MS(us) us / 1000
#define MS_TO_US(ms) ms * 1000

#define MS_TO_SECONDS(ms) ms / 1000
#define MS_TO_MINUTES(ms) MS_TO_SECONDS(ms) / 60
#define US_TO_MINUTES(us) MS_TO_MINUTES(US_TO_MS(us))
#define US_TO_SECONDS(us) MS_TO_SECONDS(US_TO_MS(us))

#define SECONDS_TO_MS(secs) secs * 1000
#define SECONDS_TO_US(secs) MS_TO_US(SECONDS_TO_MS(secs))

#define MINUTES_TO_SECONDS(mins) mins * 60
#define SECONDS_TO_MINUTES(secs) secs / 60

uint64_t toUs(struct timeval time);
struct timeval getTimevalNow();
struct timeval getFutureTimeval(uint64_t seconds);
uint64_t timeDiffMs(struct timeval tv1, struct timeval tv2);
uint64_t timeDiffUs(struct timeval tv1, struct timeval tv2);