#include "main.h"
#include "experiment.h"

#include <openthread/link.h>

#define PrintDelimiter() otLogNotePlat("---------------------------");

void printVariables(void)
{
  PrintDelimiter();

#if (CIPHER_SUITE == AES)
  otLogNotePlat("Cipher Suite: AES");
#elif (CIPHER_SUITE == NONE)
  otLogNotePlat("Cipher Suite: No Encryption");
#elif (CIPHER_SUITE == LIBASCON_128A)
  otLogNotePlat("Cipher Suite: ASCON-128a (LibAscon)");
#elif (CIPHER_SUITE == LIBASCON_128)
  otLogNotePlat("Cipher Suite: ASCON-128 (LibAscon)");
#endif

  int8_t currentPower = 0;
  if (getTxPower(&currentPower) != OT_ERROR_NONE)
  {
    otLogCritPlat("Failed to get TX Power.");
  }

#if (CONFIG_SCENARIO == 1)
  otLogNotePlat("This device is the front door motion sensor.");
#elif (CONFIG_SCENARIO == 2)
  otLogNotePlat("This device is the air quality monitor.");
#else
  otLogNotePlat("This device is the back door motion sensor.");
#endif

  uint32_t pollPeriod = otLinkGetPollPeriod(esp_openthread_get_instance());
  otLogNotePlat("The Poll Period is %" PRIu32 " ms", pollPeriod);

  printMeshLocalEid(esp_openthread_get_instance());

  PrintDelimiter();
  return;
}