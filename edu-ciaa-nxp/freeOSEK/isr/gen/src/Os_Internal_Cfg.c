/********************************************************
 * DO NOT CHANGE THIS FILE, IT IS GENERATED AUTOMATICALY*
 ********************************************************/

/* Copyright 2008, 2009, 2014, 2015 Mariano Cerdeiro
 * Copyright 2014, ACSE & CADIEEL
 *      ACSE: http://www.sase.com.ar/asociacion-civil-sistemas-embebidos/ciaa/
 *      CADIEEL: http://www.cadieel.org.ar
 *
 * This file is part of CIAA Firmware.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/** \brief FreeOSEK Os Generated Internal Configuration Implementation File
 **
 ** \file Os_Internal_Cfg.c
 **
 **/

/** \addtogroup FreeOSEK
 ** @{ */
/** \addtogroup FreeOSEK_Os
 ** @{ */
/** \addtogroup FreeOSEK_Os_Internal
 ** @{ */

/*
 * Initials     Name
 * ---------------------------
 * MaCe         Mariano Cerdeiro
 * JuCe         Juan Cecconi
 */

/*
 * modification history (new versions first)
 * -----------------------------------------------------------
 * 20150619 v0.1.4 MaCe fix issue #279
 * 20141125 v0.1.3 JuCe additional stack for x86 ARCH
 * 20090719 v0.1.2 MaCe rename file to Os_
 * 20090128 v0.1.1 MaCe add OSEK_MEMMAP check
 * 20080713 v0.1.0 MaCe initial version
 */

/*==================[inclusions]=============================================*/
#include "Os_Internal.h"

/*==================[macros and definitions]=================================*/

/*==================[internal data declaration]==============================*/

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/
/** \brief adcStartAsyncWrapperTask stack */
#if ( x86 == ARCH )
uint8 StackTaskadcStartAsyncWrapperTask[512 + TASK_STACK_ADDITIONAL_SIZE];
#else
uint8 StackTaskadcStartAsyncWrapperTask[512];
#endif
/** \brief tickIncrementTask stack */
#if ( x86 == ARCH )
uint8 StackTasktickIncrementTask[512 + TASK_STACK_ADDITIONAL_SIZE];
#else
uint8 StackTasktickIncrementTask[512];
#endif
/** \brief FSMTask stack */
#if ( x86 == ARCH )
uint8 StackTaskFSMTask[512 + TASK_STACK_ADDITIONAL_SIZE];
#else
uint8 StackTaskFSMTask[512];
#endif

/** \brief adcStartAsyncWrapperTask context */
TaskContextType ContextTaskadcStartAsyncWrapperTask;
/** \brief tickIncrementTask context */
TaskContextType ContextTasktickIncrementTask;
/** \brief FSMTask context */
TaskContextType ContextTaskFSMTask;

/** \brief Ready List for Priority 1 */
TaskType ReadyList1[1];

/** \brief Ready List for Priority 0 */
TaskType ReadyList0[2];

const AlarmType OSEK_ALARMLIST_HardwareCounter[2] = {
   ActivateAdcStartAsyncWrapperTask, /* this alarm has to be incremented with this counter */
   ActivateTickIncrementerTask, /* this alarm has to be incremented with this counter */
};


/*==================[external data definition]===============================*/
/* FreeOSEK to configured priority table
 *
 * This table show the relationship between the user selected
 * priorities and the OpenOSE priorities:
 *
 * User P.         Osek P.
 * 2               1
 * 1               0
 */

const TaskConstType TasksConst[TASKS_COUNT] = {
   /* Task adcStartAsyncWrapperTask */
   {
       OSEK_TASK_adcStartAsyncWrapperTask,   /* task entry point */
       &ContextTaskadcStartAsyncWrapperTask, /* pointer to task context */
       StackTaskadcStartAsyncWrapperTask, /* pointer stack memory */
       sizeof(StackTaskadcStartAsyncWrapperTask), /* stack size */
       1, /* task priority */
       1, /* task max activations */
       {
         1, /* extended task */
         1, /* preemtive task */
         0
      }, /* task const flags */
      0 , /* events mask */
      0 ,/* resources mask */
      0 /* core */
   },
   /* Task tickIncrementTask */
   {
       OSEK_TASK_tickIncrementTask,   /* task entry point */
       &ContextTasktickIncrementTask, /* pointer to task context */
       StackTasktickIncrementTask, /* pointer stack memory */
       sizeof(StackTasktickIncrementTask), /* stack size */
       0, /* task priority */
       1, /* task max activations */
       {
         1, /* extended task */
         1, /* preemtive task */
         0
      }, /* task const flags */
      0 , /* events mask */
      0 | ( 1 << TickCount ) ,/* resources mask */
      0 /* core */
   },
   /* Task FSMTask */
   {
       OSEK_TASK_FSMTask,   /* task entry point */
       &ContextTaskFSMTask, /* pointer to task context */
       StackTaskFSMTask, /* pointer stack memory */
       sizeof(StackTaskFSMTask), /* stack size */
       0, /* task priority */
       1, /* task max activations */
       {
         1, /* extended task */
         1, /* preemtive task */
         0
      }, /* task const flags */
      0 | ADCRead , /* events mask */
      0 | ( 1 << TickCount ) ,/* resources mask */
      0 /* core */
   }
};

/** \brief RemoteTaskCore Array */
const TaskCoreType RemoteTasksCore[REMOTE_TASKS_COUNT] = {};

/** \brief TaskVar Array */
TaskVariableType TasksVar[TASKS_COUNT];

/** \brief AutoStart Array */
const AutoStartType AutoStart[1]  = {
   /* Application Mode AppMode1 */
   {
      0, /* Total Auto Start Tasks in this Application Mode */
      NULL /* no tasks on this mode */
   }
};

const ReadyConstType ReadyConst[2] = { 
   {
      1, /* Length of this ready list */
      ReadyList1 /* Pointer to the Ready List */
   },
   {
      2, /* Length of this ready list */
      ReadyList0 /* Pointer to the Ready List */
   }
};

/** TODO replace next line with: 
 ** ReadyVarType ReadyVar[2] ; */
ReadyVarType ReadyVar[2];

/** \brief Resources Priorities */
const TaskPriorityType ResourcesPriority[1]  = {
   0
};
/** TODO replace next line with: 
 ** AlarmVarType AlarmsVar[2]; */
AlarmVarType AlarmsVar[2];

const AlarmConstType AlarmsConst[2]  = {
   {
      OSEK_COUNTER_HardwareCounter, /* Counter */
      ACTIVATETASK, /* Alarm action */
      {
         NULL, /* no callback */
         adcStartAsyncWrapperTask, /* TaskID */
         0, /* no event */
         0 /* no counter */
      },
   },
   {
      OSEK_COUNTER_HardwareCounter, /* Counter */
      ACTIVATETASK, /* Alarm action */
      {
         NULL, /* no callback */
         tickIncrementTask, /* TaskID */
         0, /* no event */
         0 /* no counter */
      },
   }
};

const AutoStartAlarmType AutoStartAlarm[ALARM_AUTOSTART_COUNT] = {
  {
      AppMode1, /* Application Mode */
      ActivateAdcStartAsyncWrapperTask, /* Alarms */
      1000, /* Alarm Time */
      5 /* Alarm Time */
   },
  {
      AppMode1, /* Application Mode */
      ActivateTickIncrementerTask, /* Alarms */
      500, /* Alarm Time */
      1 /* Alarm Time */
   }
};

CounterVarType CountersVar[1];

const CounterConstType CountersConst[1] = {
   {
      2, /* quantity of alarms for this counter */
      (AlarmType*)OSEK_ALARMLIST_HardwareCounter, /* alarms list */
      10000, /* max allowed value */
      1, /* min cycle */
      1 /* ticks per base */
   }
};


/** TODO replace the next line with
 ** uint8 ApplicationMode; */
uint8 ApplicationMode;

/** TODO replace the next line with
 ** uint8 ErrorHookRunning; */
uint8 ErrorHookRunning;

/*==================[internal functions definition]==========================*/

/*==================[external functions definition]==========================*/
void OSEK_ISR2_adcReadAsyncWrapperISR(void)
{
   /* store the calling context in a variable */
   ContextType actualContext = GetCallingContext();
   /* set isr 2 context */
   SetActualContext(CONTEXT_ISR2);

   /* trigger isr 2 */
   OSEK_ISR_adcReadAsyncWrapperISR();

   /* reset context */
   SetActualContext(actualContext);

#if (NON_PREEMPTIVE == OSEK_DISABLE)
   /* check if the actual task is preemptive */
   if ( ( CONTEXT_TASK == actualContext ) &&
        ( TasksConst[GetRunningTask()].ConstFlags.Preemtive ) )
   {
      /* this shall force a call to the scheduler */
      PostIsr2_Arch(isr);
   }
#endif /* #if (NON_PREEMPTIVE == OSEK_ENABLE) */
}


/** @} doxygen end group definition */
/** @} doxygen end group definition */
/** @} doxygen end group definition */
/*==================[end of file]============================================*/

