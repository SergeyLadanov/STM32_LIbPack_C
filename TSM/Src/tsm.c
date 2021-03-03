/*
 * tsm.c
 *
 *  Created on: Dec 8, 2020
 *      Author: serge
 */

#include "tsm.h"

static uint32_t TSM_Tick = 0;

//----------------------------------------------------------------
uint32_t TSM_GetTick(void)
{
	return TSM_Tick;
}
//-----------------------------------------------------------------
void TSM_IncTick(void)
{
	TSM_Tick++;
}
//------------------------------------------------------------------
void TSM_Handler(TSM_Obj *thp, uint32_t period, void *arg, uint8_t (*pf)(void *arg))
{
	uint32_t wait = period;
	TSM_StatusTypeDef status;

	  if (thp->Status == 0)
	  {
		  thp->TickStart = TSM_GetTick();
		  thp->Status = TSM_BUSY;
	  }
//---------------------------------------------------
	  if (wait < HAL_MAX_DELAY)
	  {
		  wait++;
	  }
//------------------------------------------------------
	  if (((TSM_GetTick() - thp->TickStart) >= wait) || (thp->Status == TSM_EXCEPT))
	  {
		  status = pf(arg);
		  if (status == TSM_OK)
		  {
			  thp->TickStart = TSM_GetTick();
			  thp->Status = TSM_BUSY;
		  }
		  else if (status == TSM_ERR)
		  {
			  thp->TickStart = TSM_GetTick();
			  thp->Status = TSM_ERR;
		  }
	  }
}
//--------------------------------------------------------
void TSM_Execute(TSM_Obj *thp)
{
	thp->Status = TSM_EXCEPT;
}
