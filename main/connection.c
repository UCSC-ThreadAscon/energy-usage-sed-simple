#include "main.h"

void checkConnection(otInstance *aInstance)
{
  otDeviceRole role;
  do {
    role = otThreadGetDeviceRole(aInstance);
    vTaskDelay(CONNECTION_WAIT_TIME_MS);
  }
  while((role == OT_DEVICE_ROLE_DISABLED) || (role == OT_DEVICE_ROLE_DETACHED));

#if OT_DEBUG
  otLogNotePlat("OpenThread Connection has been established.");
#endif
  return;
}