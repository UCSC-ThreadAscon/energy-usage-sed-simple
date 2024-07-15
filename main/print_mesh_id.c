#include "main.h"

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