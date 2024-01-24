/*========================================================================

  Copyright (c) 2018-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
========================================================================= */
/**
@file posal_heapmgr.h

@brief This file contains utilities for memory allocation and release. This
file provides memory allocation functions and macros for both C and C++.
*/

#ifndef POSAL_HEAPMGR_H
#define POSAL_HEAPMGR_H

/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "posal_types.h"
#include "posal_memory.h"

#ifdef __cplusplus
#include <new>
extern "C" {
#endif //__cplusplus

/** @addtogroup posal_memory
@{ */

//***************************************************************************
// Heap features
//***************************************************************************

/**
  Initializes the heap manager for a specified heap.

  @datatypes
  #POSAL_HEAP_ID

  @param[in] heap_id_ptr     Pointer to the heap ID.
  @param[in] heap_start_ptr  Pointer to the start address of the heap.
  @param[in] heap_size       Size of the heap.

  @return
  Status of the heap manager creation.

  @dependenciess
  Before calling this function, the object must be created and initialized.
*/
ar_result_t posal_memory_heapmgr_create(POSAL_HEAP_ID *heap_id_ptr,
                                        void *         heap_start_ptr,
                                        uint32_t       heap_size,
                                        bool_t         is_init_heap_needed);

/**
  De-initializes the heap manager of a specified heap ID.

  @datatypes
  #POSAL_HEAP_ID

  @param[in] heap_id  ID of the heap.

  @return
  Status of the heap manager deletion.

  @dependencies
  Before calling this function, the object must be created and initialized.
  @newpage
*/
ar_result_t posal_memory_heapmgr_destroy(POSAL_HEAP_ID heap_id);

/** @} */ /* end_addtogroup posal_memory */

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef POSAL_HEAPMGR_H
