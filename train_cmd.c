#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <assert.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"

/* equivalent to
   #define TRAIN_CMD_TRAINID( Ce, _trainID ) ((Ce).trainID = (uint16_t)((sp2_dst_platformID((_trainID).dest) << 8) + (journeyID2_serviceID((_trainID).jid)))) */
unsigned short TRAIN_CMD_TRAINID ( TRAIN_COMMAND_ENTRY_PTR pCe, TRAIN_ID_C_PTR ptrainID ) {
  assert( pCe );
  assert( ptrainID );
  
  const uint16_t train_id = (uint16_t)((sp2_dst_platformID(ptrainID->dest) << 8) + (journeyID2_serviceID(ptrainID->jid)));
  pCe->trainID = htons( train_id );
  return (unsigned short)ntohs( pCe->trainID );
}
