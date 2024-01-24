#ifndef _APM_GRAPH_PROPERTIES_H_
#define _APM_GRAPH_PROPERTIES_H_
/**
 * \file apm_graph_properties.h
 * \brief
 *    This file contains APM graph property definitions.
 *
 * \copyright
 *  Copyright (c) 2019-2022 Qualcomm Technologies, Inc.
 *  All Rights Reserved.
 *  Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
// clang-format off
/*
$Header: //source/qcom/qct/platform/adsp/proj/addons_audio/spf/incs/api/apm/apm_graph_properties.h#4 $
*/
// clang-format on

#include "ar_defs.h"

#ifdef __cplusplus
extern "C"
{
#endif /*__cplusplus*/


/* Legacy container capability IDs */
/* To be removed. */

/** @ingroup spf_apm_graph_props
    Legacy capability identifier for pre/postprocessing. */
#define APM_CONTAINER_CAP_ID_PP                   0x1

/** @ingroup spf_apm_graph_props
    Legacy capability identifier for compression/decompression. */
#define APM_CONTAINER_CAP_ID_CD                   0x2

/** @ingroup spf_apm_graph_props
    Legacy capability identifier for an endpoint. */
#define APM_CONTAINER_CAP_ID_EP                   0x3

/** @ingroup spf_apm_graph_props
    Capability identifier for an offload container. */
#define APM_CONTAINER_CAP_ID_OLC                  0x4


/** @ingroup spf_apm_graph_props
    Container type identifier for a specialized container. */
#define APM_CONTAINER_TYPE_ID_SC                   0x0B001000

/** @ingroup spf_apm_graph_props
    Container type identifier for a generic container. */
#define APM_CONTAINER_TYPE_ID_GC                   0x0B001001

/** @ingroup spf_apm_graph_props
    Container type identifier for an offload container. */
#define APM_CONTAINER_TYPE_ID_OLC                  0x0B001002

/** @ingroup spf_apm_graph_props
    Container type identifier for a wear container. */
#define APM_CONTAINER_TYPE_ID_WC                   0x0B001003


/////////////////DOMAIN IDs//////////////////////////

/*  THESE HAVE TO BE SAME AS GPR IDs
  refer gpr_ids_domains.h */

/** @ingroup spf_apm_graph_props
    Processor domain identifier is invalid or unknown. */
#define APM_PROC_DOMAIN_ID_INVALID                0x0

/** @ingroup spf_apm_graph_props
    Processor domain identifier for the modem DSP. */
#define APM_PROC_DOMAIN_ID_MDSP                   0x1

/** @ingroup spf_apm_graph_props
    Processor domain identifier for the audio DSP. */
#define APM_PROC_DOMAIN_ID_ADSP                   0x2

/** @ingroup spf_apm_graph_props
    Processor domain identifier for the sensors DSP. */
#define APM_PROC_DOMAIN_ID_SDSP                   0x4

/** @ingroup spf_apm_graph_props
    Processor domain identifier for the Hexagon Tensor Processor (previously
    called the compute DSP). */
#define APM_PROC_DOMAIN_ID_CDSP                   0x5

/** @ingroup spf_apm_graph_props
    Processor domain identifier for the Companion Chip DSP. */
#define APM_PROC_DOMAIN_ID_CC_DSP                 0x6

/** @ingroup spf_apm_graph_props
    Processor domain identifier for the GP_DSP 0. */
#define APM_PROC_DOMAIN_ID_GDSP_0                 0xA

/** @ingroup spf_apm_graph_props
    Processor domain identifier for the GP_DSP 1. */
#define APM_PROC_DOMAIN_ID_GDSP_1                 0xB

/////////////////Heap IDs//////////////////////////

/** @ingroup spf_apm_graph_props
     Default heap ID. */
#define APM_HEAP_ID_DEFAULT                    0x1

/** @ingroup spf_apm_graph_props
    Low Power Island (LPI) heap ID. */
#define APM_HEAP_ID_LOW_POWER                  0x2

////////////////////////////////////////////////////////

/** @ingroup spf_apm_graph_props
    Property identifier is invalid or unknown. */
#define APM_PROP_ID_INVALID                       0x0

/** @ingroup spf_apm_graph_props
    Property identifier for the <em>don't care</em> value. @newpage*/
#define APM_PROP_ID_DONT_CARE AR_NON_GUID(0xFFFFFFFF)


#ifdef __H2XML__
// Enums required for module H2XML generation.

enum processors {
   PROC_DOMAIN_ADSP = APM_PROC_DOMAIN_ID_ADSP,
   /*#< @h2xmle_name {aDSP} */

   PROC_DOMAIN_MDSP = APM_PROC_DOMAIN_ID_MDSP,
   /*#< @h2xmle_name {mDSP} */

   PROC_DOMAIN_SDSP = APM_PROC_DOMAIN_ID_SDSP,
   /*#< @h2xmle_name {sDSP} */

   PROC_DOMAIN_CDSP = APM_PROC_DOMAIN_ID_CDSP,
   /*#< @h2xmle_name {cDSP} */

   PROC_DOMAIN_CC_DSP = APM_PROC_DOMAIN_ID_CC_DSP,
   /*#< @h2xmle_name {CC_DSP} */

   PROC_DOMAIN_GDSP_0 = APM_PROC_DOMAIN_ID_GDSP_0,
   /*#< @h2xmle_name {GDSP_0} */

   PROC_DOMAIN_GDSP_1 = APM_PROC_DOMAIN_ID_GDSP_1
   /*#< @h2xmle_name {GDSP_1} */
};

enum containerCapOld {
   APM_CONTAINER_CAP_INVALID_OLD = APM_PROP_ID_INVALID,
   /*#< @h2xmle_name {INVALID} */

   APM_CONTAINER_CAP_PP = APM_CONTAINER_CAP_ID_PP,
   /*#< @h2xmle_name {PP} */

   APM_CONTAINER_CAP_CD = APM_CONTAINER_CAP_ID_CD,
   /*#< @h2xmle_name {CD} */

   APM_CONTAINER_CAP_EP = APM_CONTAINER_CAP_ID_EP,
   /*#< @h2xmle_name {EP} */

   APM_CONTAINER_CAP_OLC = APM_CONTAINER_CAP_ID_OLC,
   /*#< @h2xmle_name {OLC} */
};

enum containerCap {
   APM_CONTAINER_TYPE_INVALID = APM_PROP_ID_INVALID,
   /*#< @h2xmle_name {Invalid} */

   APM_CONTAINER_TYPE_SC  = APM_CONTAINER_TYPE_ID_SC,
   /*#< @h2xmle_name    {Specialized}
        @h2xmle_replace {APM_CONTAINER_CAP_ID_PP} */

   APM_CONTAINER_TYPE_GC  = APM_CONTAINER_TYPE_ID_GC,
   /*#< @h2xmle_name    {Generic}
        @h2xmle_replace {APM_CONTAINER_CAP_ID_CD, APM_CONTAINER_CAP_ID_EP} */

   APM_CONTAINER_TYPE_OLC = APM_CONTAINER_TYPE_ID_OLC,
   /*#< @h2xmle_name    {Offload}
        @h2xmle_replace {APM_CONTAINER_CAP_ID_OLC} */

   APM_CONTAINER_TYPE_WC  = APM_CONTAINER_TYPE_ID_WC,
   /*#< @h2xmle_name {Wear} */
};

#else
/** @ingroup spf_apm_graph_props
    Valid processor domain IDs.
*/
enum processors {
   PROC_DOMAIN_ADSP = APM_PROC_DOMAIN_ID_ADSP,
   /**< Audio DSP. */

   PROC_DOMAIN_MDSP = APM_PROC_DOMAIN_ID_MDSP,
   /**< Modem DSP. */

   PROC_DOMAIN_SDSP = APM_PROC_DOMAIN_ID_SDSP,
   /**< Sensors DSP. */

   PROC_DOMAIN_CDSP = APM_PROC_DOMAIN_ID_CDSP,
   /**< Hexagon Tensor Processor (previously called the compute DSP). */

   PROC_DOMAIN_CC_DSP = APM_PROC_DOMAIN_ID_CC_DSP,
   /**< Companion chip DSP */

   PROC_DOMAIN_GDSP_0 = APM_PROC_DOMAIN_ID_GDSP_0,
   /*#< GDSP_0 - General Purpose DSP 0 */

   PROC_DOMAIN_GDSP_1 = APM_PROC_DOMAIN_ID_GDSP_1
   /*#< GDSP_1 - General Purpose DSP 1 */
};

/** @ingroup spf_apm_graph_props
    Valid container capabilities.
*/
enum containerCapOld {
   APM_CONTAINER_CAP_INVALID_OLD = APM_PROP_ID_INVALID,
   /**< Invalid capability ID. */

   APM_CONTAINER_CAP_PP = APM_CONTAINER_CAP_ID_PP,
   /**< Pre/postprocessing. */

   APM_CONTAINER_CAP_CD = APM_CONTAINER_CAP_ID_CD,
   /**< Compression or decompression. */

   APM_CONTAINER_CAP_EP = APM_CONTAINER_CAP_ID_EP,
   /**< Endpoint. */

   APM_CONTAINER_CAP_OLC = APM_CONTAINER_CAP_ID_OLC,
   /**< Offload container. */
};

/** @ingroup spf_apm_graph_props
    Valid types of containers.
*/
enum containerCap {
   APM_CONTAINER_TYPE_INVALID = APM_PROP_ID_INVALID,
   /**< Invalid property ID. */

   APM_CONTAINER_TYPE_SC  = APM_CONTAINER_TYPE_ID_SC,
   /**< Specialized container. */

   APM_CONTAINER_TYPE_GC  = APM_CONTAINER_TYPE_ID_GC,
   /**< Generic container. */

   APM_CONTAINER_TYPE_OLC = APM_CONTAINER_TYPE_ID_OLC,
   /**< Offload container. */

   APM_CONTAINER_TYPE_WC  = APM_CONTAINER_TYPE_ID_WC,
   /**< Wear container. */
};

#endif /* __H2XML__*/


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _APM_GRAPH_PROPERTIES_H_ */
