#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"

/* equivalent to
   #define TRAIN_CMD_TRAINID( Ce, _trainID ) ((Ce).trainID = (uint16_t)((sp2_dst_platformID((_trainID).dest) << 8) + (journeyID2_serviceID((_trainID).jid)))) */
unsigned short TRAIN_CMD_TRAINID ( TRAIN_COMMAND_ENTRY Ce, TRAIN_ID trainID ) {
  const uint16_t train_id = (uint16_t)((sp2_dst_platformID(trainID.dest) << 8) + (journeyID2_serviceID(trainID.jid)));
  Ce.trainID = train_id;
  return (unsigned short)train_id;
}
