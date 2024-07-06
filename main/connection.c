#include "main.h"

#define CONNECTION_DEBUG 1
#define CONNECTION_WAIT_TIME_MS 1000

#define OT_DISCONNECTED(role) \
  (role == OT_DEVICE_ROLE_DISABLED) || (role == OT_DEVICE_ROLE_DETACHED)

void checkConnection(otInstance *aInstance)
{
  otDeviceRole role;
  do {
    role = otThreadGetDeviceRole(aInstance);
    otLogNotePlat("Device role: %d", role);
    vTaskDelay(CONNECTION_WAIT_TIME_MS / portTICK_PERIOD_MS);
  }
  while(OT_DISCONNECTED(role));

#if CONNECTION_DEBUG
  otLogNotePlat("OpenThread Connection has been established.");
#endif
  return;
}