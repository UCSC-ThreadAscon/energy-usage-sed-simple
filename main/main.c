/**
 * All of the code in this file is based upon the both CoAP and CoAP secure source code
 * used as a part of the OpenThread codebase. The CoAP and CoAP secure source files can
 * be found at: https://github.com/UCSC-ThreadAscon/openthread/tree/main/src/cli
 */
#include "main.h"
#include "experiment.h"

#include "math.h"

#define BATTERY_URI "battery"
#define EVENT_URI "event"
#define ONE_DAY_IN_SECONDS 30
#define ONE_DAY_IN_US SECONDS_TO_US(ONE_DAY_IN_SECONDS)

#define NVS_NAMESPACE "nvs_sed"
#define NVS_UUID "uuid"
#define NVS_WAKEUP_NUM "wakeup_num"

static struct timeval wakeup;
static uint8_t numPacketsInFlight;

#define PrintError(error) otThreadErrorToString(error)
#define IsDeepSleepWakeup() esp_sleep_get_wakeup_cause() != ESP_SLEEP_WAKEUP_UNDEFINED

/**
 * Printing out Mesh Local EID, as this is the recommended IPv6 address
 * to be used at the application layer.
 * https://openthread.io/guides/thread-primer/ipv6-addressing#link-local-address-lla
*/
void printMeshLocalEid(otInstance *aInstance)
{
  const otIp6Address *meshLocalEid = otThreadGetMeshLocalEid(aInstance);

  char mleidString[OT_IP6_ADDRESS_STRING_SIZE];
  EmptyMemory(mleidString, OT_IP6_ADDRESS_STRING_SIZE);

  otIp6AddressToString(meshLocalEid, mleidString, OT_IP6_ADDRESS_STRING_SIZE);

  otLogNotePlat("The Mesh Local EID is %s.", mleidString);
  return;
}

static inline otIp6Address getServerIp(void)
{
  otIp6Address address;
  EmptyMemory(&address, sizeof(otIp6Address));
  otIp6AddressFromString(CONFIG_SERVER_IP_ADDRESS, &address);
  return address;
}

void responseCallback(void *aContext,
                     otMessage *aMessage,
                     const otMessageInfo *aMessageInfo,
                     otError aResult)
{
  /**
   * If there are still packets left in flight, the callback returns, and
   * we wait until all packets have been ACKed.
   */
  numPacketsInFlight -= 1;
  if (numPacketsInFlight == 0)
  {
    struct timeval current = getTimevalNow();
    uint64_t wakeupDurationUs = timeDiffUs(wakeup, current);

    /**
     * If the duration of the wakeup is >= 30 seconds; TOO LONG. Stop sending packets, and
     * stop going to sleep. When the device stops sending packets, I know the experiment is
     * invalid and to rerun it.
     */
    if (wakeupDurationUs < ONE_DAY_IN_US)
    {
      uint64_t sleepTimeUs = ONE_DAY_IN_US - wakeupDurationUs;
      ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(sleepTimeUs));
      esp_deep_sleep_start();
    }
  }
  return;
}

void send(otSockAddr *socket,
          otMessageInfo *aMessageInfo,
          void *payload,
          uint32_t payloadSize,
          const char *uri)
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

  error = otCoapMessageAppendUriPathOptions(aRequest, uri);
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
                            responseCallback, NULL);
  if (error != OT_ERROR_NONE)
  {
    otLogCritPlat("Failed to send CoAP request. Reason: %s.", PrintError(error));
  }
  return;
}

void energyExperimentMain(void)
{
  otSockAddr socket;
  otMessageInfo aMessageInfo;
  uuid deviceId;
  nvs_handle_t handle = 0;
  uint32_t wakeupNum = 0;
  numPacketsInFlight = 0;

  EmptyMemory(&socket, sizeof(otSockAddr));
  EmptyMemory(&aMessageInfo, sizeof(otMessageInfo));
  EmptyMemory(&deviceId, sizeof(uuid));

  ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle));
  if (IsDeepSleepWakeup())
  {
    size_t uuidSize = sizeof(uuid);
    ESP_ERROR_CHECK(nvs_get_blob(handle, NVS_UUID, &deviceId, &uuidSize));

    ESP_ERROR_CHECK(nvs_get_u32(handle, NVS_WAKEUP_NUM, &wakeupNum));
    wakeupNum += 1;
    ESP_ERROR_CHECK(nvs_set_u32(handle, NVS_WAKEUP_NUM, wakeupNum));
  }
  else
  {
    /**
     * Device just powered on.
     */
    generateUUID(&deviceId);
    ESP_ERROR_CHECK(nvs_set_blob(handle, NVS_UUID, &deviceId, sizeof(uuid)));
  
    /**
     * To ensure the event "n" packets get sent every "floor(365/n)" wakeups,
     * we will count the device powering on as the "first wakeup".
     */
    wakeupNum = 1;
    ESP_ERROR_CHECK(nvs_set_u32(handle, NVS_WAKEUP_NUM, wakeupNum));

    /**
     * Print the independent variables of the experiment ONCE on power on.
     */
    printVariables();
  }
  nvs_close(handle);

  otError error = otCoapStart(esp_openthread_get_instance(), CONFIG_COAP_SOCK_PORT);
  if (error != OT_ERROR_NONE)
  {
    otLogCritPlat("Cannot start CoAP service. Reason: %s.", PrintError(error));
    return;
  }

  socket.mAddress = getServerIp();
  socket.mPort = CONFIG_COAP_SERVER_PORT;

  /**
   * Send an event packet every "floor(365/n)" wakeups.
   */
  uint32_t interval = (uint32_t) floor(365 / NUM_EVENTS);
  if (wakeupNum % interval == 0)
  {
    /**
     * You send both an event and battery packet. You NEVER send event packets
     * WITHOUT a battery packet.
     */
    numPacketsInFlight = 2;

    EventPayload event = createEventPayload(deviceId);
    send(&socket, &aMessageInfo, (void *) &event, sizeof(EventPayload), EVENT_URI);
  }
  else
  {
    // You only send a battery packet.
    numPacketsInFlight = 1;
  }

  BatteryPayload battery = createBatteryPayload(deviceId);
  send(&socket, &aMessageInfo, (void *) &battery, sizeof(BatteryPayload), BATTERY_URI);
  return;
}

/**
 * This function comes from the OpenThread SED example program given in ESP-IDF:
 * https://github.com/espressif/esp-idf/blob/master/examples/openthread/ot_sleepy_device/deep_sleep/main/esp_ot_sleepy_device.c#L73
 */
void otStateChangeCallback(otChangedFlags changed_flags, void* ctx)
{
    OT_UNUSED_VARIABLE(ctx);

    static otDeviceRole s_previous_role = OT_DEVICE_ROLE_DISABLED;
    otInstance* instance = esp_openthread_get_instance();
    if (!instance) {
        return;
    }

    otDeviceRole role = otThreadGetDeviceRole(instance);
    if (role == OT_DEVICE_ROLE_CHILD && s_previous_role != OT_DEVICE_ROLE_CHILD)
    {
      energyExperimentMain();
    }
    s_previous_role = role;
}

void app_main(void)
{
  wakeup = getTimevalNow();
  init();
  return;
}
