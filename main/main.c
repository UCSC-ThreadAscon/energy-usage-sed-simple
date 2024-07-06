/**
 * All of the code in this file is based upon the both CoAP and CoAP secure source code
 * used as a part of the OpenThread codebase. The CoAP and CoAP secure source files can
 * be found at: https://github.com/UCSC-ThreadAscon/openthread/tree/main/src/cli
 */
#include "main.h"

#define BATTERY_URI "battery"
#define SLEEP_TIME_SECONDS 30
#define SLEEP_TIME_US SECONDS_TO_US(SLEEP_TIME_SECONDS)

#define PrintError(error) otThreadErrorToString(error)

static inline otIp6Address getServerIp(void)
{
  otIp6Address address;
  EmptyMemory(&address, sizeof(otIp6Address));
  otIp6AddressFromString(CONFIG_SERVER_IP_ADDRESS, &address);
  return address;
}

void send(otSockAddr *socket,
          otMessageInfo *aMessageInfo,
          void *payload,
          uint32_t payloadSize)
{
  otMessage *aRequest = NULL;
  otError error = 0;

  aMessageInfo->mHopLimit = 0;  // default
  aMessageInfo->mPeerAddr = socket->mAddress;
  aMessageInfo->mPeerPort = socket->mPort;
  aMessageInfo->mSockAddr = *otThreadGetMeshLocalEid(esp_openthread_get_instance());
  aMessageInfo->mSockPort = CONFIG_COAP_SOCK_PORT;

  aRequest = otCoapNewMessage(esp_openthread_get_instance(), NULL);
  if (aRequest == NULL)
  {
    otLogCritPlat("Failed to create CoAP request.");
    return;
  }

  otCoapMessageInit(aRequest, OT_COAP_TYPE_CONFIRMABLE, OT_COAP_CODE_POST);
  otCoapMessageGenerateToken(aRequest, OT_COAP_DEFAULT_TOKEN_LENGTH);

  error = otCoapMessageAppendUriPathOptions(aRequest, BATTERY_URI);
  if (error != OT_ERROR_NONE)
  {
    otLogCritPlat("Failed to Append URI Path Options. Reason: %s.", PrintError(error));
    return;
  }

  error = otCoapMessageSetPayloadMarker(aRequest);
  if (error != OT_ERROR_NONE)
  {
    otLogCritPlat("Failed to Set Payload Marker. Reason: %s.", PrintError(error));
  }

  error = otMessageAppend(aRequest, payload, payloadSize);
  if (error != OT_ERROR_NONE)
  {
    otLogCritPlat("Failed to add payload to message. Reason: %s.", PrintError(error));
  }

  error = otCoapSendRequest(esp_openthread_get_instance(), aRequest, aMessageInfo,
                            NULL, NULL);
  if (error != OT_ERROR_NONE)
  {
    otLogCritPlat("Failed to send CoAP request. Reason: %s.", PrintError(error));
  }
  return;
}

void app_main(void)
{
  init();
  checkConnection(esp_openthread_get_instance());
  printMeshLocalEid(esp_openthread_get_instance());

  otSockAddr socket;
  otMessageInfo aMessageInfo;
  uuid deviceId;
  BatteryPayload payload;

  EmptyMemory(&socket, sizeof(otSockAddr));
  EmptyMemory(&aMessageInfo, sizeof(aMessageInfo));
  EmptyMemory(&aMessageInfo, sizeof(uuid));
  EmptyMemory(&payload, sizeof(BatteryPayload));

  otError error = otCoapStart(esp_openthread_get_instance(), CONFIG_COAP_SOCK_PORT);
  if (error != OT_ERROR_NONE)
  {
    otLogCritPlat("Cannot start CoAP service. Reason: %s.", PrintError(error));
    return;
  }

  socket.mAddress = getServerIp();
  socket.mPort = CONFIG_COAP_SERVER_PORT;

  generateUUID(&deviceId);
  payload = createBatteryPayload(deviceId);
  send(&socket, &aMessageInfo, (void *) &payload, sizeof(BatteryPayload));

  return;
}
