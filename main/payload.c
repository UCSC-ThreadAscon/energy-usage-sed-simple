#include "main.h"

void generateUUID(uuid *uuid)
{
  uint32_t *uuidWords = (uint32_t *) uuid->bytes;

  for (int i = 0; i < UUID_SIZE_WORDS; i++)
  {
    uuidWords[i] = esp_random();
  }
  return;
}

#define ONE_BYTE_HEX_STRING sizeof(char) * 2

EventPayload createEventPayload(uuid deviceId)
{
  EventPayload newEvent;

  EmptyMemory(&newEvent, sizeof(EventPayload));

  newEvent.deviceId = deviceId;
  newEvent.eventOccured = true;

  return newEvent;
}

BatteryPayload createBatteryPayload(uuid deviceId)
{
  BatteryPayload battery;

  EmptyMemory(&battery, sizeof(BatteryPayload));

  battery.deviceId = deviceId;
  battery.batteryLife = 100;
  return battery;
}