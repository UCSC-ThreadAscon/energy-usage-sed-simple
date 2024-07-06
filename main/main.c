/**
 * All of the code in this file is based upon the both CoAP and CoAP secure source code
 * used as a part of the OpenThread codebase. The CoAP and CoAP secure source files can
 * be found at: https://github.com/UCSC-ThreadAscon/openthread/tree/main/src/cli
 */
#include "main.h"

#define BATTERY_URI "battery"

void app_main(void)
{
  init();

  otError error = otCoapStart(esp_openthread_get_instance(), CONFIG_COAP_SERVER_PORT);
  if (error != OT_ERROR_NONE)
  {
    otLogCritPlat("Cannot start CoAP service. Reason: %s.", otThreadErrorToString(error));
    return;
  }

  return;
}
