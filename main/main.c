/**
 * TODO:
 *
 * 1. Send a Battery packet, then go to deep sleep for 30 seconds once getting response.
 *
 * 2. Then, calculate the time it takes to send the Battery packet and receive a response.
 *    Go to sleep for:
 *        30 - (time it takes to wake up, send Battery packet, and receive response).
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
