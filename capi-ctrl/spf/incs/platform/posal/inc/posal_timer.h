/*========================================================================
Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================== */
/**
@file posal_timer.h
@brief This file contains utilities for timers. One-shot, period and sleep
timers are provided.
 */
#ifndef POSAL_TIMER_H
#define POSAL_TIMER_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "ar_error_codes.h"
#include "posal_channel.h"
#include "posal_signal.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/** @addtogroup posal_timer
@{ */

/****************************************************************************
 ** Timers
 *****************************************************************************/
/** Valid timer types. */
typedef enum {
   POSAL_TIMER_ONESHOT_DURATION = 0,       /**< Duration-based one-shot timer. */
   POSAL_TIMER_ONESHOT_ABSOLUTE,           /**< Absolute one-shot timer. */
   POSAL_TIMER_PERIODIC,                   /**< Periodic timer. */
   POSAL_TIMER_ONESHOT_ABSOLUTE_DEFERRABLE /**< Defferable Absolute one-shot timer. */
} posal_timer_duration_t;

/** Valid timer clock sources. */
typedef enum {
   POSAL_TIMER_USER /**< General purpose timer for users/clients. */
} posal_timer_src_t;

typedef void *posal_timer_t;

/* -----------------------------------------------------------------------
 **  Function Definitions.
 ** ----------------------------------------------------------------------- */
/**
  Creates a timer in the default non-deferrable timer group.

  @datatypes
  posal_timer_t \n
  #posal_timer_duration_t \n
  #posal_timer_src_t \n
  posal_signal_t

  @param[in] pTimer      Pointer to the POSAL timer object.
  @param[in] timerType   One of the following:
                          - #POSAL_TIMER_ONESHOT_DURATION
                          - #POSAL_TIMER_PERIODIC
                          - #POSAL_TIMER_ONESHOT_ABSOLUTE
                            @tablebulletend
  @param[in] clockSource Clock source is #POSAL_TIMER_USER.
  @param[in] pSignal     Pointer to the signal to be generated when the timer
                         expires.
  @param[in] heap_id     heap id needed for malloc.
  @detdesc
  The caller must allocate the memory for the timer structure and pass the
  pointer to this function.
  @par
  After calling this function, call the appropriate start functions based on
  the type of timer.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  This function must be called before arming the timer. @newpage
 */
int32_t posal_timer_create(posal_timer_t *        pp_timer,
                           posal_timer_duration_t timerType,
                           posal_timer_src_t      clockSource,
                           posal_signal_t         p_signal,
                           POSAL_HEAP_ID          heap_id);

/**
  Deletes an existing timer.

  @datatypes
  posal_timer_t

  @param[in] pTimer   Pointer to the POSAL timer object.

  @return
  Indication of success (0) or failure (nonzero).

  @dependencies
  The timer object must be created using posal_timer_create().
 */
ar_result_t posal_timer_destroy(posal_timer_t *pp_timer);

/**
  Gets the duration of the specified timer.

  @datatypes
  posal_timer_t

  @param[in] pTimer   Pointer to the POSAL timer object.

  @return
  Duration of the timer, in microseconds.

  @dependencies
  The timer object must be created using posal_timer_create().
  @newpage
 */
uint64_t posal_timer_get_duration(posal_timer_t p_timer);

/**
  Creates a synchronous sleep timer. Control returns to the callee
  after the timer expires.

  @param[in] llMicrosec  Duration the timer will sleep.

  @return
  Always returns 0. The return type is int for backwards compatibility.

  @dependencies
  None.
 */
int32_t posal_timer_sleep(int64_t llMicrosec);

/**
  Gets the wall clock time.

  @return
  Returns the wall clock time in microseconds.

  @dependencies
  None.
 */
uint64_t posal_timer_get_time(void);

/**
  Gets the wall clock time in milliseconds.

  @return
  Returns the wall clock time in milliseconds.

  @dependencies
  None. @newpage
 */
uint64_t posal_timer_get_time_in_msec(void);

/**
  Restarts the absolute one-shot timer.

  @datatypes
  posal_timer_t

  @param[in] pTimer  Pointer to the POSAL timer object.
  @param[in] time    Absolute time of the timer, in microseconds.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must be created using posal_timer_create().
 */
int32_t posal_timer_oneshot_start_absolute(posal_timer_t p_timer, int64_t time);

/**
  Restarts the duration-based one-shot timer.

  @datatypes
  posal_timer_t

  @param[in] pTimer    Pointer to the POSAL timer object.
  @param[in] duration  Duration of the timer, in microseconds.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must be created using posal_timer_create(). @newpage
 */
int32_t posal_timer_oneshot_start_duration(posal_timer_t p_timer, int64_t duration);

/**
  Starts the periodic timer.

  @datatypes
  posal_timer_t

  @param[in] pTimer    Pointer to the POSAL timer object.
  @param[in] duration  Duration of the timer, in microseconds.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must be created using posal_timer_create().
 */
int32_t posal_timer_periodic_start(posal_timer_t p_timer, int64_t duration);

/**
  Starts the periodic timer after the specified duration.

  @datatypes
  posal_timer_t

  @param[in] pTimer    Pointer to the POSAL timer object.
  @param[in] duration  Duration of the timer, in microseconds.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must be created using posal_timer_create().
 */
int32_t posal_timer_periodic_start_with_offset(posal_timer_t p_timer, int64_t periodic_duration, int64_t start_offset);

/**
  Stops the timer.

  @datatypes
  posal_timer_t

  @param[in] pTimer   Pointer to the POSAL timer object.

  @return
  An indication of success (0) or failure (nonzero).

  @dependencies
  The timer must be created using posal_timer_create(). @newpage
 */
int32_t posal_timer_stop(posal_timer_t p_timer);

/** @} */ /* end_addtogroup posal_timer */

#ifdef __cplusplus
}
#endif //__cplusplus
#endif // #ifndef POSAL_TIMER_H
