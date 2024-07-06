/**
 * All of the code in this file is based upon the both CoAP and CoAP secure source code
 * used as a part of the OpenThread codebase. The CoAP and CoAP secure source files can
 * be found at: https://github.com/UCSC-ThreadAscon/openthread/tree/main/src/cli
 */
#include "main.h"

static inline otIp6Address getServerIp(void)
{
  otIp6Address address;
  EmptyMemory(&address, sizeof(otIp6Address));
  otIp6AddressFromString(CONFIG_SERVER_IP_ADDRESS, &address);
  return address;
}

void app_main(void)
{
  init();
  checkConnection(esp_openthread_get_instance());
  printMeshLocalEid(esp_openthread_get_instance());

  otSockAddr socket;
  otMessageInfo aMessageInfo;
  otMessage *aRequest;

  EmptyMemory(&socket, sizeof(otSockAddr));
  EmptyMemory(&aMessageInfo, sizeof(aMessageInfo));

  otError error = otCoapStart(esp_openthread_get_instance(), CONFIG_COAP_SOCK_PORT);
  if (error != OT_ERROR_NONE)
  {
    otLogCritPlat("Cannot start CoAP service. Reason: %s.", otThreadErrorToString(error));
    return;
  }

  socket.mAddress = getServerIp();
  socket.mPort = CONFIG_COAP_SERVER_PORT;

  /**
   * Send a Battery packet as a CoAP Request.
   */

  aMessageInfo.mHopLimit = 0;  // default
  aMessageInfo.mPeerAddr = socket.mAddress;
  aMessageInfo.mPeerPort = socket.mPort;
  aMessageInfo.mSockAddr = *otThreadGetMeshLocalEid(esp_openthread_get_instance());
  aMessageInfo.mSockPort = CONFIG_COAP_SOCK_PORT;

  aRequest = otCoapNewMessage(esp_openthread_get_instance(), NULL);
  if (aRequest == NULL)
  {
    otLogCritPlat("Failed to create CoAP request.");
    return;
  }

  return;
}
