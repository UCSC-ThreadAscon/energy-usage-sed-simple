#include "main.h"

/**
 * TODO:
 *
 * 1. Send a Battery packet, then go to deep sleep for 30 seconds once getting response.
 *
 * 2. Then, calculate the time it takes to send the Battery packet and receive a response.
 *    Go to sleep for:
 *        30 - (time it takes to wake up, send Battery packet, and receive response).
 */

void app_main(void)
{
  init();
  return;
}
