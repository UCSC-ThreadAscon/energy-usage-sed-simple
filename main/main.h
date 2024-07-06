#pragma once

#include "init.h"
#include "esp_random.h"
#include <openthread/coap.h>

#define BATTERY_URI "battery"

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