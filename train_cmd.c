#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
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

static void change_train_state ( TINY_TRAIN_STATE_PTR pT, void (*cb)(TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg), void *pr, void const *parg, BOOL mindles ) {  
  assert( pT );
  assert( cb );
  assert( pr );
  assert( parg );
  if( mindles )
    goto change_val;
  else {
    int r_mutex = -1;
    r_mutex = pthread_mutex_lock( &cbtc_ctrl_cmds_mutex );
    if( r_mutex ) {
      assert( FALSE );
    } else {
      assert( !mindles );
    change_val:      
      cb( pT, pr, parg );
      if( !mindles ) {
	assert( r_mutex == 0 );
	r_mutex = -1;
	r_mutex = pthread_mutex_unlock( &cbtc_ctrl_cmds_mutex );
	assert( !r_mutex );
      }
    }
  }
}

int change_train_state_rakeID ( TINY_TRAIN_STATE_PTR pT, const int rakeID, BOOL mindles ) {
  assert( pT );
  if( mindles )
    goto change_val;
  else {
    int r_mutex = -1;
    r_mutex = pthread_mutex_lock( &cbtc_ctrl_cmds_mutex );
    if( r_mutex ) {
      assert( FALSE );
    } else {
      assert( !mindles );
    change_val:
      pT->rakeID = rakeID;
      
      if( !mindles ) {
	assert( r_mutex == 0 );
	r_mutex = -1;
	r_mutex = pthread_mutex_unlock( &cbtc_ctrl_cmds_mutex );
	assert( !r_mutex );
      }
    }
  }
  return rakeID;
}

static void cb_trainID ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  pT->train_ID = *((TRAIN_ID_C_PTR)parg);
  *((TRAIN_ID_PTR *)pres) = &pT->train_ID;
}
TRAIN_ID_PTR change_train_state_trainID ( TINY_TRAIN_STATE_PTR pT, const TRAIN_ID train_ID, BOOL mindles ) {
  assert( pT );
  TRAIN_ID_PTR r = NULL;
  
  TRAIN_ID_C_PTR ptid = &train_ID;
  change_train_state( pT, cb_trainID, &r, ptid, mindles );
  
  return r;
}

static void cb_dest_blockID ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->dest_blockID = *((int *)parg);
  *((int *)pres) = pT->dest_blockID;
}
int change_train_state_dest_blockID ( TINY_TRAIN_STATE_PTR pT, const int dest_blockID, BOOL mindles ) {
  assert( pT );
  int r;
  
  int const *pdstblk = &dest_blockID;
  change_train_state( pT, cb_dest_blockID, &r, pdstblk, mindles );
  
  return r;
}

static void cb_crnt_blockID ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->crnt_blockID = *((int *)parg);
  *((int *)pres) = pT->crnt_blockID;
}
int change_train_state_crnt_blockID ( TINY_TRAIN_STATE_PTR pT, const int crnt_blockID, BOOL mindles ) {
  assert( pT );
  int r;
  
  int const *pcrntblk = &crnt_blockID;
  change_train_state( pT, cb_crnt_blockID, &r, pcrntblk, mindles );
  
  return r;
}

static void cb_keep_door_closed ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->keep_door_closed = *((BOOL *)parg);
  *((BOOL *)pres) = pT->keep_door_closed;
}
BOOL change_train_state_keep_door_closed ( TINY_TRAIN_STATE_PTR pT, const BOOL keep_door_closed, BOOL mindles ) {
  assert( pT );
  BOOL r;

  const BOOL *pkdc = &keep_door_closed;
  change_train_state( pT, cb_keep_door_closed, &r, pkdc, mindles );
  
  return r;
}

static void cb_out_of_service ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->out_of_service = *((BOOL *)parg);
  *((BOOL *)pres) = pT->out_of_service;
}
BOOL change_train_state_out_of_service ( TINY_TRAIN_STATE_PTR pT, const BOOL out_of_service, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pOoS = &out_of_service;
  change_train_state( pT, cb_out_of_service, &r, pOoS, mindles );
  
  return r;
}

static void cb_crnt_station_plcode ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->crnt_station_plcode = *((CRNT_ST_PLCODE *)parg);
  *((CRNT_ST_PLCODE *)pres) = pT->crnt_station_plcode;
}
CRNT_ST_PLCODE change_train_state_crnt_station_plcode ( TINY_TRAIN_STATE_PTR pT, const CRNT_ST_PLCODE plcode, BOOL mindles ) {
  assert( pT );
  CRNT_ST_PLCODE r;
  
  const CRNT_ST_PLCODE *pcode = &plcode;
  change_train_state( pT, cb_crnt_station_plcode, &r, pcode, mindles );
  
  return r;
}

static void cb_nexr_station_plcode ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->next_station_plcode = *((NEXT_ST_PLCODE *)parg);
  *((NEXT_ST_PLCODE *)pres) = pT->next_station_plcode;
}
NEXT_ST_PLCODE change_train_state_next_station_plcode ( TINY_TRAIN_STATE_PTR pT, const NEXT_ST_PLCODE plcode, BOOL mindles ) {
  assert( pT );
  NEXT_ST_PLCODE r;
  
  const NEXT_ST_PLCODE *pcode = &plcode;
  change_train_state( pT, cb_nexr_station_plcode, &r, pcode, mindles );
  
  return r;
}

static void cb_dst_station_plcode ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->dst_station_plcode = *((DST_ST_PLCODE *)parg);
  *((DST_ST_PLCODE *)pres) = pT->dst_station_plcode;
}
DST_ST_PLCODE change_train_state_dst_station_plcode ( TINY_TRAIN_STATE_PTR pT, const DST_ST_PLCODE plcode, BOOL mindles ) {
  assert( pT );
  DST_ST_PLCODE r;
  
  const DST_ST_PLCODE *pcode = &plcode;
  change_train_state( pT, cb_dst_station_plcode, &r, pcode, mindles );
  
  return r;
}

static void cb_destination_number ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->destination_number = *((int *)parg);
  *((int *)pres) = pT->destination_number;
}
int change_train_state_destination_number ( TINY_TRAIN_STATE_PTR pT, const int destination_number, BOOL mindles ) {
  assert( pT );
  int r;
  
  const int *pdstnum = &destination_number;
  change_train_state( pT, cb_destination_number, &r, pdstnum, mindles );
  
  return r;
}

static void cb_next_station_number ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->next_station_number = *((int *)parg);
  *((int *)pres) = pT->next_station_number;
}
int change_train_state_next_station_number ( TINY_TRAIN_STATE_PTR pT, const int next_station_number, BOOL mindles ) {
  assert( pT );
  int r;
  
  const int *pnextnum = &next_station_number;
  change_train_state( pT, cb_next_station_number, &r, pnextnum, mindles );
  
  return r;
}

static void cb_crnt_station_number ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->crnt_station_number = *((int *)parg);
  *((int *)pres) = pT->crnt_station_number;
}
int change_train_state_crnt_station_number ( TINY_TRAIN_STATE_PTR pT, const int crnt_station_number, BOOL mindles ) {
  assert( pT );
  int r;
  
  const int *pcrntnum = &crnt_station_number;
  change_train_state( pT, cb_crnt_station_number, &r, pcrntnum, mindles );
  
  return r;
}

static void cb_ATO_dept_cmd ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->ATO_dept_cmd = *((BOOL *)parg);  
  *((BOOL *)pres) = pT->ATO_dept_cmd;
}
BOOL change_train_state_ATO_dept_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL ATO_dept_cmd, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pdep = &ATO_dept_cmd;
  change_train_state( pT, cb_ATO_dept_cmd, &r, pdep, mindles );
  
  return r;
}

static void cb_depcond_release ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->depcond_release = *((BOOL *)parg);  
  *((BOOL *)pres) = pT->depcond_release;
}
BOOL change_train_state_depcond_release ( TINY_TRAIN_STATE_PTR pT, const BOOL depcond_release, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pcond = &depcond_release;
  change_train_state( pT, cb_depcond_release, &r, pcond, mindles );
  
  return r;
}

static void cb_skip_next_stop ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->skip_next_stop = *((BOOL *)parg);
  *((BOOL *)pres) = pT->skip_next_stop;
}
BOOL change_train_state_skip_next_stop ( TINY_TRAIN_STATE_PTR pT, const BOOL skip_next_stop, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pSS = &skip_next_stop;
  change_train_state( pT, cb_skip_next_stop, &r, pSS, mindles );
  
  return r;
}

static void cb_origin_station ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->origin_station = *((BOOL *)parg);
  *((BOOL *)pres) = pT->origin_station;
}
BOOL change_train_state_origin_station ( TINY_TRAIN_STATE_PTR pT, const BOOL origin_station, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *porig = &origin_station;
  change_train_state( pT, cb_origin_station, &r, porig, mindles );
  
  return r;
}

static void cb_next_st_dooropen_side ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->next_st_dooropen_side = *((NEXT_ST_DOOROPEN_SIDE *)parg);
  *((NEXT_ST_DOOROPEN_SIDE *)pres) = pT->next_st_dooropen_side;
}
NEXT_ST_DOOROPEN_SIDE change_train_state_next_st_dooropen_side ( TINY_TRAIN_STATE_PTR pT, const NEXT_ST_DOOROPEN_SIDE next_st_dooropen_side, BOOL mindles ) {
  assert( pT );
  NEXT_ST_DOOROPEN_SIDE r;
  
  const NEXT_ST_DOOROPEN_SIDE *popside = &next_st_dooropen_side;
  change_train_state( pT, cb_next_st_dooropen_side, &r, popside, mindles );
  
  return r;
}

static void cb_operation_mode ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->operaton_mode = *((TRAIN_OPERATION_MODE *)parg);
  *((TRAIN_OPERATION_MODE *)pres) = pT->operaton_mode;
}
TRAIN_OPERATION_MODE change_train_state_operation_mode ( TINY_TRAIN_STATE_PTR pT, const TRAIN_OPERATION_MODE operation_mode, BOOL mindles ) {
  assert( pT );
  TRAIN_OPERATION_MODE r;
  
  const TRAIN_OPERATION_MODE *pope= &operation_mode;
  change_train_state( pT, cb_operation_mode, &r, pope, mindles );
  
  return r;
}

static void cb_leave_now ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->leave_now = *((BOOL *)parg);
  *((BOOL *)pres) = pT->leave_now;
}
BOOL change_train_state_leave_now ( TINY_TRAIN_STATE_PTR pT, const BOOL leave_now, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pnow= &leave_now;
  change_train_state( pT, cb_leave_now, &r, pnow, mindles );
  
  return r;
}

static void cb_perf_regime ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->perf_regime = *((TRAIN_PERF_REGIME *)parg);
  *(TRAIN_PERF_REGIME *)pres = pT->perf_regime;
}
TRAIN_PERF_REGIME change_train_state_perf_regime ( TINY_TRAIN_STATE_PTR pT, const TRAIN_PERF_REGIME perf_regime, BOOL mindles ) {
  assert( pT );
  TRAIN_PERF_REGIME r;
  
  TRAIN_PERF_REGIME const *pperf = &perf_regime;
  change_train_state( pT, cb_perf_regime, &r, pperf, mindles );
  
  return r;
}

static void cb_coasting_cmd ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->coasting_cmd = *((BOOL *)parg);
  *((BOOL *)pres) = pT->coasting_cmd;
}
BOOL change_train_state_coasting_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL coasting_cmd, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pcmd = &coasting_cmd;
  change_train_state( pT, cb_coasting_cmd, &r, pcmd, mindles );
  
  return r;
}

static void cb_TH_cmd ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->TH_cmd = *((BOOL *)parg);
  *((BOOL *)pres) = pT->TH_cmd;
}
BOOL change_train_state_TH_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL TH_cmd, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pTH = &TH_cmd;
  change_train_state( pT, cb_TH_cmd, &r, pTH, mindles );
  
  return r;
}

static void cb_maximum_speed_cmd ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->maximum_speed_cmd = *((int *)parg);
  *((int *)pres) = pT->maximum_speed_cmd;
}
int change_train_state_maximum_speed_cmd ( TINY_TRAIN_STATE_PTR pT, const int maximum_speed_cmd, BOOL mindles ) {
  assert( pT );
  int r;
  
  int const *pmax = &maximum_speed_cmd;
  change_train_state( pT, cb_maximum_speed_cmd, &r, pmax, mindles );
  
  return r;
}

static void cb_turnback_siding ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->turnback_siding = *((BOOL *)parg);
  *((BOOL *)pres) = pT->turnback_siding;
}
BOOL change_train_state_turnback_siding ( TINY_TRAIN_STATE_PTR pT, const BOOL turnback_siding, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pT_S = &turnback_siding;
  change_train_state( pT, cb_turnback_siding, &r, pT_S, mindles );
  
  return r;
}

static void cb_passenger_address ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->passenger_address = *((int *)parg);
  *((int *)pres) = pT->passenger_address;
}
int change_train_state_passenger_address ( TINY_TRAIN_STATE_PTR pT, const int passenger_address, BOOL mindles ) {
  assert( pT );
  int r;
  
  const int *ppa = &passenger_address;
  change_train_state( pT, cb_passenger_address, &r, ppa, mindles );
  
  return r;
}

static void cb_dep_dir ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->dep_dir = *((TRAIN_MOVE_DIR *)parg);
  *((TRAIN_MOVE_DIR *)pres) = pT->dep_dir;
}
TRAIN_MOVE_DIR change_train_state_dep_dir ( TINY_TRAIN_STATE_PTR pT, const TRAIN_MOVE_DIR dep_dir, BOOL mindles ) {
  assert( pT );
  TRAIN_MOVE_DIR r;
  
  const TRAIN_MOVE_DIR *pdir = &dep_dir;
  change_train_state( pT, cb_dep_dir, &r, pdir, mindles );
  
  return r;
}

static void cb_regulation_speed  ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->regulation_speed = *((int *)parg);
  *((int *)pres) = pT->regulation_speed;
}
int change_train_state_regulation_speed ( TINY_TRAIN_STATE_PTR pT, const int regulation_speed, BOOL mindles ) {
  assert( pT );
  int r;
  
  int const *pspeed = &regulation_speed;
  change_train_state( pT, cb_regulation_speed, &r, pspeed, mindles );
  
  return r;
}

static void cb_dwell_time ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->dwell_time = *((int *)parg);
  *((int *)pres) = pT->dwell_time;
}
int change_train_state_dwell_time ( TINY_TRAIN_STATE_PTR pT, const int dwell_time, BOOL mindles ) {
  assert( pT );
  int r;
  
  int const *pdw = &dwell_time;
  change_train_state( pT, cb_dwell_time, &r, pdw, mindles );
  
  return r;
}

static void cb_ordering_wakeup ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->ordering_wakeup = *((BOOL *)parg);
  *((BOOL *)pres) = pT->ordering_wakeup;
}
BOOL change_train_state_ordering_wakeup ( TINY_TRAIN_STATE_PTR pT, const BOOL ordering_wakeup, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pwake = &ordering_wakeup;
  change_train_state( pT, cb_ordering_wakeup, &r, pwake, mindles );
  
  return r;
}

static void cb_ordering_standby ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->ordering_standby = *((BOOL *)parg);
  *((BOOL *)pres) = pT->ordering_standby;;
}
BOOL change_train_state_ordering_standby ( TINY_TRAIN_STATE_PTR pT, const BOOL ordering_standby, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pstandby = &ordering_standby;
  change_train_state( pT, cb_ordering_standby, &r, pstandby, mindles );
  
  return r;
}

static void cb_ATB_cmd ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->ATB_cmd = *((BOOL *)parg);
  *((BOOL *)pres) = pT->ATB_cmd;
}
BOOL change_train_state_ATB_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL ATB_cmd, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *patb = &ATB_cmd;
  change_train_state( pT, cb_ATB_cmd, &r, patb, mindles );
  
  return r;
}

static void cb_ordering_emergency_stop ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->ordering_emergency_stop = *((BOOL *)parg);
  *((BOOL *)pres) = pT->ordering_emergency_stop;
}
BOOL change_train_state_ordering_emergency_stop ( TINY_TRAIN_STATE_PTR pT, const BOOL ordering_emergency_stop, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *poem = &ordering_emergency_stop;
  change_train_state( pT, cb_ordering_emergency_stop, &r, poem, mindles );
  
  return r;
}

static void cb_releasing_emergency_stop ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->releasing_emergency_stop = *((BOOL *)parg);
  *((BOOL *)pres) = pT->ordering_emergency_stop;
}
BOOL change_train_state_releasing_emergency_stop ( TINY_TRAIN_STATE_PTR pT, const BOOL releasing_emergency_stop, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *prem = &releasing_emergency_stop;
  change_train_state( pT, cb_releasing_emergency_stop, &r, prem, mindles );
  
  return r;
}

static void cb_train_remove ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->train_remove = *((BOOL *)parg);
  *((BOOL *)pres) = pT->train_remove;
}
BOOL change_train_state_train_remove ( TINY_TRAIN_STATE_PTR pT, const BOOL train_remove, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *ptr = &train_remove;
  change_train_state( pT, cb_train_remove, &r, ptr, mindles );
  
  return r;
}

static void cb_system_switch_cmd ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->system_switch_cmd = *((SYSTEM_SWITCHING_CMD *)parg);
  *((SYSTEM_SWITCHING_CMD *)pres) = pT->system_switch_cmd;
}
SYSTEM_SWITCHING_CMD change_train_state_system_switch_cmd ( TINY_TRAIN_STATE_PTR pT, const SYSTEM_SWITCHING_CMD system_switch_cmd, BOOL mindles ) {
  assert( pT );
  SYSTEM_SWITCHING_CMD r;
  
  const SYSTEM_SWITCHING_CMD *psw = &system_switch_cmd;
  change_train_state( pT, cb_system_switch_cmd, &r, psw, mindles );
  
  return r;
}

static void cb_ordering_reset_onboard ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->ordering_reset_onboard = *((BOOL *)parg);
  *((BOOL *)pres) = pT->ordering_reset_onboard;
}
BOOL change_train_state_ordering_reset_onboard ( TINY_TRAIN_STATE_PTR pT, const BOOL ordering_reset_onboard, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *preset = &ordering_reset_onboard;
  change_train_state( pT, cb_ordering_reset_onboard, &r, preset, mindles );
  
  return r;
}

static void cb_energy_saving ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->energy_saving = *((BOOL *)parg);
  *((BOOL *)pres) = pT->energy_saving;
}
BOOL change_train_state_energy_saving ( TINY_TRAIN_STATE_PTR pT, const BOOL energy_saving, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *psavin = &energy_saving;
  change_train_state( pT, cb_energy_saving, &r, psavin, mindles );
  
  return r;
}

static void cb_remote_door_opening ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->remote_door_opening = *((BOOL *)parg);
  *((BOOL *)pres) = pT->remote_door_opening;
}
BOOL change_train_state_remote_door_opening ( TINY_TRAIN_STATE_PTR pT, const BOOL remote_door_opening, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pdor = &remote_door_opening;
  change_train_state( pT, cb_remote_door_opening, &r, pdor, mindles );
  
  return r;
}

static void cb_remote_door_closing ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->remote_door_closing = *((BOOL *)parg);
  *((BOOL *)pres) = pT->remote_door_closing;
}
BOOL change_train_state_remote_door_closing ( TINY_TRAIN_STATE_PTR pT, const BOOL remote_door_closing, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pdor = &remote_door_closing;
  change_train_state( pT, cb_remote_door_closing, &r, pdor, mindles );
  
  return r;
}

static void cb_static_test_cmd ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->static_test_cmd = *((BOOL *)parg);
  *((BOOL *)pres) = pT->static_test_cmd;
}
BOOL change_train_state_static_test_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL static_test_cmd, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pstst = &static_test_cmd;
  change_train_state( pT, cb_static_test_cmd, &r, pstst, mindles );
  
  return r;
}

static void cb_dynamic_test_cmd ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->dynamic_test_cmd = *((BOOL *)parg);
  *((BOOL *)pres) = pT->dynamic_test_cmd;
}
BOOL change_train_state_dynamic_test_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL dynamic_test_cmd, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pdtst = &dynamic_test_cmd;
  change_train_state( pT, cb_dynamic_test_cmd, &r, pdtst, mindles );
  
  return r;
}

static void cb_inching_cmd ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->inching_cmd = *((BOOL *)parg);
  *((BOOL *)pres) = pT->inching_cmd;
}
BOOL change_train_state_inching_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL inching_cmd, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pinc = &inching_cmd;
  change_train_state( pT, cb_inching_cmd, &r, pinc, mindles );
  
  return r;
}

static void cb_back_inching_cmd ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->back_inching_cmd = *((BOOL *)parg);
  *((BOOL *)pres) = pT->back_inching_cmd;
}
BOOL change_train_state_back_inching_cmd ( TINY_TRAIN_STATE_PTR pT, const BOOL back_inching_cmd, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pbin = &back_inching_cmd;
  change_train_state( pT, cb_back_inching_cmd, &r, pbin, mindles );
  
  return r;
}

static void cb_em_door_release ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->em_door_release = *((EMERGE_DOOR_RELEASING *)parg);
  *((EMERGE_DOOR_RELEASING *)pres) = pT->em_door_release;
}
EMERGE_DOOR_RELEASING change_train_state_em_door_release ( TINY_TRAIN_STATE_PTR pT, const EMERGE_DOOR_RELEASING em_door_release, BOOL mindles ) {
  assert( pT );
  EMERGE_DOOR_RELEASING r;
  
  const EMERGE_DOOR_RELEASING *pemrel = &em_door_release;
  change_train_state( pT, cb_em_door_release, &r, pemrel, mindles );
  
  return r;
}

static void cb_back_vrs_reset ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->back_vrs_reset = *((BOOL *)parg);
  *((BOOL *)pres) = pT->back_vrs_reset;
}
BOOL change_train_state_back_vrs_reset ( TINY_TRAIN_STATE_PTR pT, const BOOL back_vrs_reset, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pvrst = &back_vrs_reset;
  change_train_state( pT, cb_back_vrs_reset, &r, pvrst, mindles );
  
  return r;
}

static void cb_forward_vrs_reset ( TINY_TRAIN_STATE_PTR pT, void *pres, void const *parg ) {
  assert( pT );
  assert( pres );
  assert( parg );
  
  pT->forward_vrs_reset = *((BOOL *)parg);
  *((BOOL *)pres) = pT->forward_vrs_reset;
}
BOOL change_train_state_forward_vrs_reset ( TINY_TRAIN_STATE_PTR pT, const BOOL forward_vrs_reset, BOOL mindles ) {
  assert( pT );
  BOOL r;
  
  const BOOL *pvrst = &forward_vrs_reset;
  change_train_state( pT, cb_forward_vrs_reset, &r, pvrst, mindles );
  
  return r;
}
