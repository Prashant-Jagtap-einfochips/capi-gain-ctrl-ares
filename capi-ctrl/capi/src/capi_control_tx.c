/* =========================================================================
  Copyright (c) 2019-2021 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
 * =========================================================================*/

/**
 * @file capi_gain.cpp
 * C source file to implement the Gain module
 */

/* =========================================================================
 * Edit History:
 * when         who         what, where, why
 * ----------   -------     ------------------------------------------------

 * =========================================================================*/

/*------------------------------------------------------------------------
 * Include files
 * -----------------------------------------------------------------------*/
#include "capi_control_tx.h"
#include "capi_control_tx_utils.h"


//IMCL
 #include "capi_cmn_imcl_utils.h"
 #include "capi_intf_extn_imcl.h"
 #include "capi_cmn_ctrl_port_list.h"
 #include "capi_cmn.h"
 #include "imcl_control_tx_api.h"


// todo: try multichannel test cases (maybe 16 channels. It should work fine.)

/*------------------------------------------------------------------------
 * Static function declarations
 * -----------------------------------------------------------------------*/
static capi_err_t capi_control_tx_process(capi_t *_pif, capi_stream_data_t *input[], capi_stream_data_t *output[]);

static capi_err_t capi_control_tx_end(capi_t *_pif);

static capi_err_t capi_control_tx_set_param(capi_t *                _pif,
                                      uint32_t                param_id,
                                      const capi_port_info_t *port_info_ptr,
                                      capi_buf_t *            params_ptr);

static capi_err_t capi_control_tx_get_param(capi_t *                _pif,
                                      uint32_t                param_id,
                                      const capi_port_info_t *port_info_ptr,
                                      capi_buf_t *            params_ptr);

static capi_err_t capi_control_tx_set_properties(capi_t *_pif, capi_proplist_t *props_ptr);

static capi_err_t capi_control_tx_get_properties(capi_t *_pif, capi_proplist_t *props_ptr);

/* Function table for gain module*/
static capi_vtbl_t vtbl = { capi_control_tx_process,        capi_control_tx_end,           capi_control_tx_set_param, capi_control_tx_get_param,
                            capi_control_tx_set_properties, capi_control_tx_get_properties };

/* -------------------------------------------------------------------------
 * Function name: capi_gain_get_static_properties
 * Used to query the static properties of the gain module that are independent of the
   instance. This function is used to query the memory requirements of the module
   in order to create an instance.
 * -------------------------------------------------------------------------*/
capi_err_t capi_control_tx_get_static_properties(capi_proplist_t *init_set_properties, capi_proplist_t *static_properties)
{
   capi_err_t capi_result = CAPI_EOK;
   if (NULL != static_properties)
   {
      capi_result = capi_control_tx_process_get_properties((capi_gain_t *)NULL, static_properties);
      if (CAPI_FAILED(capi_result))
      {
         AR_MSG(DBG_ERROR_PRIO, "CAPI GAIN: get static properties failed!");
         return capi_result;
      }
      else
      {
         AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN: get static properties successful");
      }
   }
   else
   {
      AR_MSG(DBG_ERROR_PRIO, "CAPI GAIN: Get static properties received NULL bad pointer");
   }

   return capi_result;
}

/*------------------------------------------------------------------------
  Function name: capi_control_tx_init
  Instantiates the gain module to set up the virtual function table, and also
  allocates any memory required by the module. Default states within the module are also
  initialized here
 * -----------------------------------------------------------------------*/
capi_err_t capi_control_tx_init(capi_t *_pif, capi_proplist_t *init_set_properties)
{
   capi_err_t capi_result = CAPI_EOK;

   if (NULL == _pif || NULL == init_set_properties)
   {
      AR_MSG(DBG_ERROR_PRIO, "CAPI GAIN: Init received NULL pointers, failing");
      return CAPI_EBADPARAM;
   }

   /* Cast to gain module capi struct*/
   capi_gain_t *me_ptr = (capi_gain_t *)_pif;

   memset(me_ptr, 0, sizeof(capi_gain_t));

   /* Assign function table defined above*/
   me_ptr->vtbl.vtbl_ptr = &vtbl;

   /* Initialize the operating media format of the gain module*/
   capi_gain_module_init_media_fmt_v2(&me_ptr->operating_media_fmt);

   /* Initialize gain configuration to defaults*/
   uint16_t apply_gain          = 0x2000;
   me_ptr->gain_config.gain_q12 = apply_gain >> 1;
   me_ptr->gain_config.gain_q13 = apply_gain;
   
    //IMCL
   me_ptr->is_ctrl_port_received = GAIN_CTRL_PORT_INFO_NOT_RCVD;
   //Initialize the control port list
   capi_cmn_ctrl_port_list_init(&me_ptr->ctrl_port_info);

   if (NULL != init_set_properties)
   {
      capi_result = capi_control_tx_process_set_properties(me_ptr, init_set_properties);
      if (CAPI_FAILED(capi_result))
      {
         AR_MSG(DBG_ERROR_PRIO, "CAPI GAIN:  Initialization Set Property Failed");
         return capi_result;
      }
   }

   AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN: Initialization completed !!");
   return capi_result;
}

/* -------------------------------------------------------------------------
 * Function name: capi_control_tx_process
 * Gain module Data Process function to process an input buffer
 * and fill an output buffer.
 * -------------------------------------------------------------------------*/
static capi_err_t capi_control_tx_process(capi_t *_pif, capi_stream_data_t *input[], capi_stream_data_t *output[])
{

   capi_err_t       capi_result = CAPI_EOK;

   memcpy(output[0]->buf_ptr[0].data_ptr, input[0]->buf_ptr[0].data_ptr, input[0]->buf_ptr->actual_data_len);
   if (2 == output[0]->bufs_num)
   {
      memcpy(output[0]->buf_ptr[1].data_ptr, input[0]->buf_ptr[1].data_ptr, input[0]->buf_ptr->actual_data_len);
   }
   // Update actual data length for output buffer
   output[0]->buf_ptr[0].actual_data_len = input[0]->buf_ptr->actual_data_len;
   if (2 == output[0]->bufs_num)
   {
      output[0]->buf_ptr[1].actual_data_len = input[0]->buf_ptr->actual_data_len;
   }

   // Copy flags from input to output
   output[0]->flags = input[0]->flags;

   return capi_result;

#if 0
   AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN: INSIDE PROCESS");

   capi_err_t   capi_result = CAPI_EOK;
   capi_gain_t *me_ptr      = (capi_gain_t *)_pif;

   if ((!input[0]) && (!output[0]))
   {
      AR_MSG(DBG_ERROR_PRIO, "CAPI GAIN: Process: No input or output buffer provided");
      return CAPI_EFAILED;
   }

   /* Call library function to process data*/
   uint32_t samples_to_process;
   int32_t  byte_sample_convert = (BIT_WIDTH_16 == me_ptr->operating_media_fmt.format.bits_per_sample) ? 1 : 2;

   int32_t inp_samples = input[0]->buf_ptr[0].actual_data_len >> byte_sample_convert;
   int32_t out_samples = output[0]->buf_ptr[0].max_data_len >> byte_sample_convert;

   /* Samples to process is the minimum of the input and output lengths available*/
   samples_to_process = (inp_samples < out_samples) ? inp_samples : out_samples;

   for (uint32_t ch = 0; ch < me_ptr->operating_media_fmt.format.num_channels; ch++)
   {
      if (BIT_WIDTH_16 == me_ptr->operating_media_fmt.format.bits_per_sample)
      {
         example_apply_gain_16((int16_t *)output[0]->buf_ptr[ch].data_ptr,
                               (int16_t *)input[0]->buf_ptr[ch].data_ptr,
                               me_ptr->gain_config.gain_q12,
                               samples_to_process);
         output[0]->buf_ptr[ch].actual_data_len = samples_to_process << 1;
         input[0]->buf_ptr[ch].actual_data_len  = samples_to_process << 1;
      }
      else
      {
         if ((uint16_t)(me_ptr->gain_config.gain_q12 >> 12) > 0)
         {
            // for gain greater than 1
            example_apply_gain_32_G1((int32_t *)output[0]->buf_ptr[ch].data_ptr,
                                     (int32_t *)input[0]->buf_ptr[ch].data_ptr,
                                     me_ptr->gain_config.gain_q12,
                                     samples_to_process,
                                     me_ptr->operating_media_fmt.format.q_factor);
         }
         else
         {
            // for gain less than 1
            example_apply_gain_32_L1((int32_t *)output[0]->buf_ptr[ch].data_ptr,
                                     (int32_t *)input[0]->buf_ptr[ch].data_ptr,
                                     me_ptr->gain_config.gain_q12,
                                     samples_to_process,
                                     me_ptr->operating_media_fmt.format.q_factor);
         }
         output[0]->buf_ptr[ch].actual_data_len = samples_to_process << 2;
         input[0]->buf_ptr[ch].actual_data_len  = samples_to_process << 2;
      }
   }

   if (samples_to_process)
   {
      output[0]->flags = input[0]->flags; // Copying flags from input to output

      if (input[0]->flags.is_timestamp_valid) // updating timestamps
      {
         output[0]->timestamp = input[0]->timestamp - me_ptr->events_info.delay_in_us;
      }
   }

   return capi_result;
#endif   
   
}

/*------------------------------------------------------------------------
 * Function name: capi_gain_end
 * Gain End function, returns the library to the uninitialized
 * state and frees all the memory that was allocated. This function also
 * frees the virtual function table.
 * -----------------------------------------------------------------------*/
static capi_err_t capi_control_tx_end(capi_t *_pif)
{

   capi_err_t capi_result = CAPI_EOK;
   if (NULL == _pif)
   {
      CAPI_SET_ERROR(capi_result, CAPI_EBADPARAM);
      return capi_result;
   }

   capi_gain_t *me_ptr   = (capi_gain_t *)_pif;
   me_ptr->vtbl.vtbl_ptr = NULL;

   AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN: End done");
   return capi_result;
}

/* -------------------------------------------------------------------------
 * Function name: capi_gain_set_param
 * Sets either a parameter value or a parameter structure containing
 * multiple parameters. In the event of a failure, the appropriate error
 * code is returned.
 * The actual_data_len field of the parameter pointer is to be at least the size
  of the parameter structure. Every case statement should have this check
 * -------------------------------------------------------------------------*/
static capi_err_t capi_control_tx_set_param(capi_t *                _pif,
                                      uint32_t                param_id,
                                      const capi_port_info_t *port_info_ptr,
                                      capi_buf_t *            params_ptr)
{
   capi_err_t capi_result = CAPI_EOK;
   if (NULL == _pif || NULL == params_ptr)
   {
      AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN: End done");
      return CAPI_EBADPARAM;
   }

   capi_gain_t *me_ptr = (capi_gain_t *)(_pif);

   switch (param_id)
   {
      /* parameter to enable the gain module */
      case PARAM_ID_MODULE_ENABLE:
      {
         if (params_ptr->actual_data_len >= sizeof(param_id_module_enable_t))
         {
            param_id_module_enable_t *gain_module_enable_ptr = (param_id_module_enable_t *)(params_ptr->data_ptr);
            me_ptr->gain_config.enable                       = gain_module_enable_ptr->enable;
            if (me_ptr->gain_config.enable)
            {
               AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN: PARAM_ID_MODULE_ENABLE, %lu", me_ptr->gain_config.enable);
            }
            else
            {
               AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN: PARAM_ID_MODULE_ENABLE, %lu", me_ptr->gain_config.enable);
            }
            /* Raise process check event based on enable value set here*/
            capi_result |= capi_gain_raise_process_event(me_ptr, me_ptr->gain_config.enable);

            AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN: PARAM_ID_MODULE_ENABLE, %lu", me_ptr->gain_config.enable);
         }
         else
         {
            AR_MSG(DBG_ERROR_PRIO,
                   "CAPI GAIN: PARAM_ID_MODULE_ENABLE, Bad param size %lu",
                   params_ptr->actual_data_len);

            capi_result |= CAPI_ENEEDMORE;
         }
         break;
      }
      case PARAM_ID_GAIN_MODULE_GAIN:
      {
         if (params_ptr->actual_data_len >= sizeof(param_id_module_gain_cfg_t))
         {
            param_id_module_gain_cfg_t *gain_cfg_ptr = (param_id_module_gain_cfg_t *)(params_ptr->data_ptr);
            me_ptr->gain_config.gain_q13             = gain_cfg_ptr->gain;
            me_ptr->gain_config.gain_q12             = gain_cfg_ptr->gain >> 1;

            /* Determine if gain module should be enabled
             * Module is enabled if PARAM_ID_MODULE_ENABLE was set to enable AND
             * if the gain value is not unity in Q13 format */
            uint32_t enable = TRUE;

#if 0
            if (Q13_UNITY_GAIN == me_ptr->gain_config.gain_q12 << 1)
            {
               enable = FALSE;
            }
            enable = enable && ((me_ptr->gain_config.enable == 0) ? 0 : 1);
#endif
            /* Raise process check event*/
            capi_result |= capi_gain_raise_process_event(me_ptr, enable);
            AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN: PARAM_ID_GAIN %d", me_ptr->gain_config.gain_q13);
            
            
            
            ///////////// Calling IMCL stuff here
               
                capi_err_t result = CAPI_EOK;
         	capi_buf_t buf;
         	uint32_t control_port_id = 0;
         	imcl_port_state_t port_state = CTRL_PORT_CLOSE;
   
         	buf.actual_data_len = sizeof(gain_imcl_header_t) + sizeof(capi_gain_control_data_payload_t);
         	buf.data_ptr = NULL;
         	buf.max_data_len = 0;

         	imcl_outgoing_data_flag_t flags;
         	flags.should_send = TRUE;
         	flags.is_trigger = FALSE;
   
         	// Get the first control port id for the intent #INTENT_ID_GAIN_CONTROL
         	capi_cmn_ctrl_port_list_get_next_port_data(&me_ptr->ctrl_port_info,
                                              INTENT_ID_GAIN_CONTROL,
                                              control_port_id, // initially, an invalid port id
                                              &me_ptr->ctrl_port_ptr);

         	if (me_ptr->ctrl_port_ptr)
         	{
            		control_port_id = me_ptr->ctrl_port_ptr->port_info.port_id;
            		port_state = me_ptr->ctrl_port_ptr->state;
         	}
         	else
         	{
            		AR_MSG_ISLAND(DBG_ERROR_PRIO,"Port data ptr doesnt exist. ctrl port id=0x%x port state = 0x%x",control_port_id, port_state);
         	}

         	if (0 != control_port_id) {
            		me_ptr->is_ctrl_port_received = GAIN_CTRL_PORT_INFO_RCVD;
            		if (CTRL_PORT_PEER_CONNECTED == port_state)
            		{
               		// Get one time buf from the queue
               		result |= capi_cmn_imcl_get_one_time_buf(&me_ptr->cb_info, control_port_id, buf.actual_data_len, &buf);
      
               		AR_MSG_ISLAND(DBG_ERROR_PRIO,"buf.actual_data_len=0x%x", buf.actual_data_len);

               		if (CAPI_FAILED(result) || NULL == buf.data_ptr)
               		{
                  			AR_MSG(DBG_ERROR_PRIO,"Getting one time buffer failed");
                  			return result;
               		}
               		gain_imcl_header_t *out_cfg_ptr = (gain_imcl_header_t *)buf.data_ptr;
               		capi_gain_control_data_payload_t *data_over_imc_payload = (capi_gain_control_data_payload_t*)(out_cfg_ptr + 1);

               		out_cfg_ptr->opcode = PARAM_ID_GAIN_CONTROL_IMC_PAYLOAD;
               		out_cfg_ptr->actual_data_len = sizeof(capi_gain_control_data_payload_t);
               		data_over_imc_payload->gain = me_ptr->gain_config.gain_q13;
               		// send data to peer/destination module
               		if (CAPI_SUCCEEDED(capi_cmn_imcl_send_to_peer(&me_ptr->cb_info, &buf, control_port_id, flags)))
               		{
                  			AR_MSG(DBG_HIGH_PRIO,"Enable %d and dec factor %d sent to control port 0x%x", data_over_imc_payload->gain, control_port_id);
               		}
            		}
            		else
            		{
               		AR_MSG(DBG_ERROR_PRIO,"Control port is not connected");
            		}              
         	}
         	else {
			AR_MSG(DBG_ERROR_PRIO,"Control port id is not proper");
	 	}
            	/////////////////////////// IMCL ends here    
            
            
         }
         else
         {
            AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN: PARAM_ID_GAIN %d", me_ptr->gain_config.gain_q13);
            CAPI_SET_ERROR(capi_result, CAPI_ENEEDMORE);
         }
         break;
      }
      
      case INTF_EXTN_PARAM_ID_IMCL_PORT_OPERATION:
      {
         uint32_t supported_intent[1] = {INTENT_ID_GAIN_CONTROL};
         capi_result = capi_cmn_ctrl_port_operation_handler(
            &me_ptr->ctrl_port_info, params_ptr, 
            (POSAL_HEAP_ID)me_ptr->heap_info.heap_id, 0, 1, supported_intent);

               ///////////// Calling IMCL stuff here
               
               capi_err_t result = CAPI_EOK;
         	capi_buf_t buf;
         	uint32_t control_port_id = 0;
         	imcl_port_state_t port_state = CTRL_PORT_CLOSE;
   
         	buf.actual_data_len = sizeof(gain_imcl_header_t) + sizeof(capi_gain_control_data_payload_t);
         	buf.data_ptr = NULL;
         	buf.max_data_len = 0;

         	imcl_outgoing_data_flag_t flags;
         	flags.should_send = TRUE;
         	flags.is_trigger = FALSE;
   
         	// Get the first control port id for the intent #INTENT_ID_GAIN_CONTROL
         	capi_cmn_ctrl_port_list_get_next_port_data(&me_ptr->ctrl_port_info,
                                              INTENT_ID_GAIN_CONTROL,
                                              control_port_id, // initially, an invalid port id
                                              &me_ptr->ctrl_port_ptr);

         	if (me_ptr->ctrl_port_ptr)
         	{
            		control_port_id = me_ptr->ctrl_port_ptr->port_info.port_id;
            		port_state = me_ptr->ctrl_port_ptr->state;
         	}
         	else
         	{
            		AR_MSG_ISLAND(DBG_ERROR_PRIO,"Port data ptr doesnt exist. ctrl port id=0x%x port state = 0x%x",control_port_id, port_state);
         	}

         	if (0 != control_port_id) {
            		me_ptr->is_ctrl_port_received = GAIN_CTRL_PORT_INFO_RCVD;
            		if (CTRL_PORT_PEER_CONNECTED == port_state)
            		{
               		// Get one time buf from the queue
               		result |= capi_cmn_imcl_get_one_time_buf(&me_ptr->cb_info, control_port_id, buf.actual_data_len, &buf);
      
               		AR_MSG_ISLAND(DBG_ERROR_PRIO,"buf.actual_data_len=0x%x", buf.actual_data_len);

               		if (CAPI_FAILED(result) || NULL == buf.data_ptr)
               		{
                  			AR_MSG(DBG_ERROR_PRIO,"Getting one time buffer failed");
                  			return result;
               		}
               		gain_imcl_header_t *out_cfg_ptr = (gain_imcl_header_t *)buf.data_ptr;
               		capi_gain_control_data_payload_t *data_over_imc_payload = (capi_gain_control_data_payload_t*)(out_cfg_ptr + 1);

               		out_cfg_ptr->opcode = PARAM_ID_GAIN_CONTROL_IMC_PAYLOAD;
               		out_cfg_ptr->actual_data_len = sizeof(capi_gain_control_data_payload_t);
               		data_over_imc_payload->gain = me_ptr->gain_config.gain_q13;
               		// send data to peer/destination module
               		if (CAPI_SUCCEEDED(capi_cmn_imcl_send_to_peer(&me_ptr->cb_info, &buf, control_port_id, flags)))
               		{
                  			AR_MSG(DBG_HIGH_PRIO,"Enable %d and dec factor %d sent to control port 0x%x", data_over_imc_payload->gain, control_port_id);
               		}
            		}
            		else
            		{
               		AR_MSG(DBG_ERROR_PRIO,"Control port is not connected");
            		}              
         	}
         	else {
			AR_MSG(DBG_ERROR_PRIO,"Control port id is not proper");
	 	}
            	/////////////////////////// IMCL ends here            	

      }
      
      
      default:
      {
         AR_MSG(DBG_ERROR_PRIO, "CAPI GAIN: Set, unsupported param ID 0x%x", (int)param_id);
         CAPI_SET_ERROR(capi_result, CAPI_EUNSUPPORTED);
         break;
      }
   }
   return capi_result;
}

/* -------------------------------------------------------------------------
 * Function name: capi_control_tx_get_param
 * Gets either a parameter value or a parameter structure containing
 * multiple parameters. In the event of a failure, the appropriate error
 * code is returned.
 * The max_data_len field of the parameter pointer must be at least the size
 * of the parameter structure. Therefore, this check is made in every case statement
 * Before returning, the actual_data_len field must be filled with the number
  of bytes written into the buffer.
 * -------------------------------------------------------------------------*/
static capi_err_t capi_control_tx_get_param(capi_t *                _pif,
                                      uint32_t                param_id,
                                      const capi_port_info_t *port_info_ptr,
                                      capi_buf_t *            params_ptr)
{
   capi_err_t capi_result = CAPI_EOK;
   if (NULL == _pif || NULL == params_ptr)
   {
      AR_MSG(DBG_ERROR_PRIO, "CAPI GAIN: Set, unsupported param ID 0x%x", (int)param_id);
      return CAPI_EBADPARAM;
   }

   capi_gain_t *me_ptr = (capi_gain_t *)_pif;

   switch (param_id)
   {
      case PARAM_ID_MODULE_ENABLE:
      {
         AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN: GET PARAM");
         if (params_ptr->max_data_len >= sizeof(param_id_module_enable_t))
         {
            param_id_module_enable_t *enable_ptr = (param_id_module_enable_t *)(params_ptr->data_ptr);
            enable_ptr->enable                   = me_ptr->gain_config.enable;

            /* Populate actual data length*/
            params_ptr->actual_data_len = (uint32_t)sizeof(param_id_module_enable_t);
         }
         else
         {
            AR_MSG(DBG_ERROR_PRIO, "CAPI GAIN: Get Enable Param, Bad payload size %d", params_ptr->max_data_len);
            capi_result = CAPI_ENEEDMORE;
         }
         break;
      }
      case PARAM_ID_GAIN_MODULE_GAIN:
      {
         if (params_ptr->max_data_len >= sizeof(param_id_module_gain_cfg_t))
         {
            param_id_module_gain_cfg_t *gain_cfg_ptr = (param_id_module_gain_cfg_t *)(params_ptr->data_ptr);

            /* Fetch the gain values which were set to the lib*/
            gain_cfg_ptr->gain     = me_ptr->gain_config.gain_q13;
            gain_cfg_ptr->reserved = 0;
            AR_MSG(DBG_HIGH_PRIO, "CAPI GAIN:GET PARAM: gain = 0x%lx", gain_cfg_ptr->gain);
            /* Populate actual data length*/
            params_ptr->actual_data_len = sizeof(param_id_module_gain_cfg_t);
         }
         else
         {
            AR_MSG(DBG_ERROR_PRIO, "CAPI GAIN: Get, Bad param size %lu", params_ptr->max_data_len);
            capi_result = CAPI_ENEEDMORE;
         }
         break;
      }
      default:
      {
         AR_MSG(DBG_ERROR_PRIO, "CAPI GAIN: Get, unsupported param ID 0x%x", param_id);
         capi_result |= CAPI_EUNSUPPORTED;
         break;
      }
   }
   return capi_result;
}

/* -------------------------------------------------------------------------
 * Function name: capi_gain_set_properties
 * Function to set Sets a list of property values.
 * -------------------------------------------------------------------------*/
static capi_err_t capi_control_tx_set_properties(capi_t *_pif, capi_proplist_t *props_ptr)
{
   capi_gain_t *me_ptr = (capi_gain_t *)_pif;
   return capi_control_tx_process_set_properties(me_ptr, props_ptr);
}

/* -------------------------------------------------------------------------
 * Function name: capi_gain_get_properties
 * Function to get a list of property values.
 * -------------------------------------------------------------------------*/
static capi_err_t capi_control_tx_get_properties(capi_t *_pif, capi_proplist_t *props_ptr)
{
   capi_gain_t *me_ptr = (capi_gain_t *)_pif;
   return capi_control_tx_process_get_properties(me_ptr, props_ptr);
}
