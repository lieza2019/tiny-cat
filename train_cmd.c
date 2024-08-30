#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "generic.h"
#include "misc.h"
#include "sparcs.h"

char *TRAIN_CMD_TRAINID ( TRAIN_COMMAND_ENTRY Ce, char *pbuf, int size ) {
  snprintf( pbuf, size, "[%04d]", Ce.trainID );
  return pbuf;
}
