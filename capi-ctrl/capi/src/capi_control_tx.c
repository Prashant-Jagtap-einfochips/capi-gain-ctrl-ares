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
#include "control_tx_module_api.h"

//IMCL
#include "capi_cmn_imcl_utils.h"
#include "capi_intf_extn_imcl.h"
#include "capi_cmn_ctrl_port_list.h"
#include "capi_cmn.h"
#include "imcl_control_tx_api.h"
 
#include "HAP_perf.h"


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
         AR_MSG(DBG_ERROR_PRIO, "CAPI CONTROL: get static properties failed!");
         return capi_result;
      }
      else
      {
         AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL: get static properties successful");
      }
   }
   else
   {
      AR_MSG(DBG_ERROR_PRIO, "CAPI CONTROL: Get static properties received NULL bad pointer");
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
      AR_MSG(DBG_ERROR_PRIO, "CAPI CONTROL: Init received NULL pointers, failing");
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
   
   /* Disable mute parameter by default */
   me_ptr->mute = 0;
   
    //IMCL
   me_ptr->is_ctrl_port_received = GAIN_CTRL_PORT_INFO_NOT_RCVD;
   //Initialize the control port list
   capi_cmn_ctrl_port_list_init(&me_ptr->ctrl_port_info);

   if (NULL != init_set_properties)
   {
      capi_result = capi_control_tx_process_set_properties(me_ptr, init_set_properties);
      if (CAPI_FAILED(capi_result))
      {
         AR_MSG(DBG_ERROR_PRIO, "CAPI CONTROL:  Initialization Set Property Failed");
         return capi_result;
      }
   }

   AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL: Initialization completed !!");
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

   AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL: End done");
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
      AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL: End done");
      return CAPI_EBADPARAM;
   }

   capi_gain_t *me_ptr = (capi_gain_t *)(_pif);

   switch (param_id)
   {
      /* parameter to enable the gain module */
      case PARAM_MOD_CONTROL_ENABLE:
      {
         if (params_ptr->actual_data_len >= sizeof(control_module_enable_t))
         {
            control_module_enable_t *gain_module_enable_ptr = (control_module_enable_t *)(params_ptr->data_ptr);
            me_ptr->gain_config.enable                       = gain_module_enable_ptr->enable;
            if (me_ptr->gain_config.enable)
            {
               AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL: PARAM_ID_MODULE_ENABLE, %lu", me_ptr->gain_config.enable);
            }
            else
            {
               AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL: PARAM_ID_MODULE_ENABLE, %lu", me_ptr->gain_config.enable);
            }
            /* Raise process check event based on enable value set here*/
            capi_result |= capi_gain_raise_process_event(me_ptr, me_ptr->gain_config.enable);

            AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL: PARAM_ID_MODULE_ENABLE, %lu", me_ptr->gain_config.enable);
         }
         else
         {
            AR_MSG(DBG_ERROR_PRIO,
                   "CAPI CONTROL: PARAM_ID_MODULE_ENABLE, Bad param size %lu",
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
#if 0
            uint32_t enable = TRUE;

            if (Q13_UNITY_GAIN == me_ptr->gain_config.gain_q12 << 1)
            {
               enable = FALSE;
            }
            enable = enable && ((me_ptr->gain_config.enable == 0) ? 0 : 1);
            /* Raise process check event*/
            capi_result |= capi_gain_raise_process_event(me_ptr, enable);
#endif
            AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL: PARAM_ID_GAIN %d", me_ptr->gain_config.gain_q13);
            
            
            
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

         	if (0 != control_port_id)
			{
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
            AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL: PARAM_ID_GAIN %d", me_ptr->gain_config.gain_q13);
            CAPI_SET_ERROR(capi_result, CAPI_ENEEDMORE);
         }
         break;
      }
	  
      case GAIN_PARAM_COEFF_ARR:
      {
         if (params_ptr->actual_data_len >= sizeof(control_tx_coeff_arr_t))
         {
		    control_tx_coeff_arr_t *cfg_ptr = (control_tx_coeff_arr_t *)(params_ptr->data_ptr);
		    memcpy(me_ptr->coeff_val, cfg_ptr->coeff_val, sizeof(cfg_ptr->coeff_val));             
             
		    ///////////// Calling IMCL stuff here
               
		    capi_err_t result = CAPI_EOK;
		    capi_buf_t buf;
		    uint32_t control_port_id = 0;
		    imcl_port_state_t port_state = CTRL_PORT_CLOSE;
   		    buf.actual_data_len = sizeof(gain_imcl_header_t) + sizeof(capi_gain_coeff_arr_payload_t);
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
                  capi_gain_coeff_arr_payload_t *data_over_imc_payload = (capi_gain_coeff_arr_payload_t*)(out_cfg_ptr + 1);
			      
                  out_cfg_ptr->opcode = PARAM_ID_GAIN_COEFF_IMC_PAYLOAD;
                  out_cfg_ptr->actual_data_len = sizeof(capi_gain_coeff_arr_payload_t);
                  memcpy(data_over_imc_payload->coeff_val, me_ptr->coeff_val, sizeof(me_ptr->coeff_val));
                  // send data to peer/destination module
                  if (CAPI_SUCCEEDED(capi_cmn_imcl_send_to_peer(&me_ptr->cb_info, &buf, control_port_id, flags)))
                  {
                      //AR_MSG(DBG_HIGH_PRIO,"Enable %d and factor %d sent to control port 0x%x", data_over_imc_payload->coeff_val[0], control_port_id);
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
            AR_MSG(DBG_ERROR_PRIO, "CAPI CONTROL: <<set_param>> Bad param size %lu", params_ptr->actual_data_len);
            return CAPI_ENEEDMORE;
         }
         break;
      }
      
      case PARAM_ID_GAIN_MODULE_MUTE:
      {
         AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL: PARAM_ID_GAIN_MODULE_MUTE entered");
         if (params_ptr->actual_data_len >= sizeof(control_tx_mute_t))
         {
		    control_tx_mute_t *cfg_ptr = (control_tx_mute_t *)(params_ptr->data_ptr);
		    me_ptr->mute = cfg_ptr->mute;
			
		    ///////////// Calling IMCL stuff here
              
		    capi_err_t result = CAPI_EOK;
		    capi_buf_t buf;
		    uint32_t control_port_id = 0;
		    imcl_port_state_t port_state = CTRL_PORT_CLOSE;
   		    buf.actual_data_len = sizeof(gain_imcl_header_t) + sizeof(capi_gain_mute_payload_t);
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
               if (CTRL_PORT_PEER_CONNECTED == port_state)
               {
                  /***** Send mute parameter here *****/
                  unsigned long long func_start_time = HAP_perf_get_time_us();
                  AR_MSG(DBG_HIGH_PRIO,"CAPI CONTROL: func_start_time = %lluus\n", func_start_time);
                  	            			
                  // Get one time buf from the queue
                  result |= capi_cmn_imcl_get_one_time_buf(&me_ptr->cb_info, control_port_id, buf.actual_data_len, &buf);
			      
                  AR_MSG_ISLAND(DBG_ERROR_PRIO,"buf.actual_data_len=0x%x", buf.actual_data_len);
			      
                  if (CAPI_FAILED(result) || NULL == buf.data_ptr)
                  {
                     AR_MSG(DBG_ERROR_PRIO,"Getting one time buffer failed");
                     return result;
                  }
                  gain_imcl_header_t *out_cfg_ptr = (gain_imcl_header_t *)buf.data_ptr;
                  control_tx_mute_t *data_over_imc_payload = (control_tx_mute_t*)(out_cfg_ptr + 1);
			      
                  out_cfg_ptr->opcode = PARAM_ID_MUTE_IMC_PAYLOAD;
                  out_cfg_ptr->actual_data_len = sizeof(capi_gain_mute_payload_t);
                  data_over_imc_payload->mute = me_ptr->mute;
                  // send data to peer/destination module
                  unsigned long long func_start_send_mute_time = HAP_perf_get_time_us();
                  AR_MSG(DBG_HIGH_PRIO,"CAPI CONTROL: func_start_send_mute_time = %lluus\n", func_start_send_mute_time);
                  	
                  if (CAPI_SUCCEEDED(capi_cmn_imcl_send_to_peer(&me_ptr->cb_info, &buf, control_port_id, flags)))
                  {
                     //AR_MSG(DBG_HIGH_PRIO,"Enable %d and factor %d sent to control port 0x%x", data_over_imc_payload->mute, control_port_id);
                  }
                  unsigned long long func_end_mute_time = HAP_perf_get_time_us();
                  AR_MSG(DBG_HIGH_PRIO,"CAPI CONTROL: func_end_mute_time = %lluus\n", func_end_mute_time);
			      
                  /***** Send coeffs here *****/
               	
                  if (data_over_imc_payload->mute == 1) {
				     capi_buf_t buf2;            			
            		 buf2.actual_data_len = sizeof(gain_imcl_header_t) + sizeof(capi_gain_coeff_arr_payload_t);
				     buf2.data_ptr = NULL;
				     buf2.max_data_len = 0;
               		 // Get one time buf from the queue
               		 result |= capi_cmn_imcl_get_one_time_buf(&me_ptr->cb_info, control_port_id, buf2.actual_data_len, &buf2);
					 
               		 AR_MSG_ISLAND(DBG_ERROR_PRIO,"buf2.actual_data_len=0x%x", buf2.actual_data_len);
					 
               		 if (CAPI_FAILED(result) || NULL == buf2.data_ptr)
               		 {
                  	    AR_MSG(DBG_ERROR_PRIO,"Getting one time buffer failed");
                  	    return result;
               		 }
               		 out_cfg_ptr = (gain_imcl_header_t *)buf2.data_ptr;
               		 capi_gain_coeff_arr_payload_t *data_over_imc_payload_2 = (capi_gain_coeff_arr_payload_t*)(out_cfg_ptr + 1);
					 
               		 out_cfg_ptr->opcode = PARAM_ID_GAIN_COEFF_IMC_PAYLOAD;
               		 out_cfg_ptr->actual_data_len = sizeof(capi_gain_coeff_arr_payload_t);
            		 	
            		 unsigned long long tx_start_send_coeff_time = HAP_perf_get_time_us();
             		 AR_MSG(DBG_HIGH_PRIO,"CAPI CONTROL: tx_start_send_coeff_time = %lluus\n", tx_start_send_coeff_time);
               	     // send data to peer/destination module
	               	 memset(data_over_imc_payload_2->coeff_val, 0x7F, sizeof(me_ptr->coeff_val));
               		 if (CAPI_SUCCEEDED(capi_cmn_imcl_send_to_peer(&me_ptr->cb_info, &buf2, control_port_id, flags)))
               		 {
               		 	// do nothing
               		 }

#if 0
               		 for (int count = 0; count < 175; count++) {
	               	 	memset(data_over_imc_payload_2->coeff_val, count, sizeof(me_ptr->coeff_val));
               		 	// send data to peer/destination module
               		 	if (CAPI_SUCCEEDED(capi_cmn_imcl_send_to_peer(&me_ptr->cb_info, &buf2, control_port_id, flags)))
               		 	{
                  	 		   // do nothing
               		 	}
               		 }
#endif

            		 unsigned long long func_end_coeff_time = HAP_perf_get_time_us();
            		 AR_MSG(DBG_HIGH_PRIO,"CAPI CONTROL: tx_end_coeff_time = %lluus\n", func_end_coeff_time);
                  }

#if 0
            	  unsigned long long func_coeff_time = HAP_perf_get_time_us();

				  /***** Togle mute param and send it again TODO:fix this ****/
				  capi_buf_t buf3;            			
            	  buf3.actual_data_len = sizeof(gain_imcl_header_t) + sizeof(capi_gain_mute_payload_t);
				  buf3.data_ptr = NULL;
				  buf3.max_data_len = 0;
               	  if (CAPI_FAILED(result) || NULL == buf3.data_ptr)
               	  {
                     AR_MSG_ISLAND(DBG_ERROR_PRIO,"Getting one time buffer failed");
                     return result;
               	  }
               	  out_cfg_ptr = (gain_imcl_header_t *)buf3.data_ptr;
               	  control_tx_mute_t *data_over_imc_payload_3 = (control_tx_mute_t*)(out_cfg_ptr + 1);
               	  
               	  out_cfg_ptr->opcode = PARAM_ID_MUTE_IMC_PAYLOAD;
               	  out_cfg_ptr->actual_data_len = sizeof(capi_gain_mute_payload_t);
               	  if (data_over_imc_payload->mute == 1) {
               	  	data_over_imc_payload_3->mute = 0;
               	  } else if (data_over_imc_payload->mute == 0) {
               	  	data_over_imc_payload_3->mute = 1;
               	  }
               	  // send data to peer/destination module
               	  if (CAPI_SUCCEEDED(capi_cmn_imcl_send_to_peer(&me_ptr->cb_info, &buf3, control_port_id, flags)))
               	  {
                     //AR_MSG(DBG_HIGH_PRIO,"Enable %d and factor %d sent to control port 0x%x", data_over_imc_payload->mute, control_port_id);
               	  }
                  unsigned long long func_end_time = HAP_perf_get_time_us();
                  AR_MSG(DBG_HIGH_PRIO,"CAPI CONTROL: func_end_time = %lluus\n", func_end_time);
#endif
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
            AR_MSG(DBG_ERROR_PRIO, "CAPI CONTROL: <<set_param>> Bad param size %lu", params_ptr->actual_data_len);
            return CAPI_ENEEDMORE;
         }
         break;
      }

      case INTF_EXTN_PARAM_ID_IMCL_PORT_OPERATION:
      {
         uint32_t supported_intent[1] = {INTENT_ID_GAIN_CONTROL};
         capi_result = capi_cmn_ctrl_port_operation_handler(
            &me_ptr->ctrl_port_info, params_ptr, 
            (POSAL_HEAP_ID)me_ptr->heap_info.heap_id, 0, 1, supported_intent);

#if 0
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
#endif       	
         break;
      }
      
      default:
      {
         AR_MSG(DBG_ERROR_PRIO, "CAPI CONTROL: Set, unsupported param ID 0x%x", (int)param_id);
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
      AR_MSG(DBG_ERROR_PRIO, "CAPI CONTROL: Set, unsupported param ID 0x%x", (int)param_id);
      return CAPI_EBADPARAM;
   }

   capi_gain_t *me_ptr = (capi_gain_t *)_pif;

   switch (param_id)
   {
      case PARAM_MOD_CONTROL_ENABLE:
      {
         AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL: GET PARAM");
         if (params_ptr->max_data_len >= sizeof(control_module_enable_t))
         {
            control_module_enable_t *enable_ptr = (control_module_enable_t *)(params_ptr->data_ptr);
            enable_ptr->enable                   = me_ptr->gain_config.enable;

            /* Populate actual data length*/
            params_ptr->actual_data_len = (uint32_t)sizeof(control_module_enable_t);
         }
         else
         {
            AR_MSG(DBG_ERROR_PRIO, "CAPI CONTROL: Get Enable Param, Bad payload size %d", params_ptr->max_data_len);
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
            AR_MSG(DBG_HIGH_PRIO, "CAPI CONTROL:GET PARAM: gain = 0x%lx", gain_cfg_ptr->gain);
            /* Populate actual data length*/
            params_ptr->actual_data_len = sizeof(param_id_module_gain_cfg_t);
         }
         else
         {
            AR_MSG(DBG_ERROR_PRIO, "CAPI CONTROL: Get, Bad param size %lu", params_ptr->max_data_len);
            capi_result = CAPI_ENEEDMORE;
         }
         break;
      }
      case GAIN_PARAM_COEFF_ARR:
      {
         if (params_ptr->max_data_len >= sizeof(control_tx_coeff_arr_t))
      	 {
		    control_tx_coeff_arr_t *gain_cfg_ptr = (control_tx_coeff_arr_t *)(params_ptr->data_ptr);
		    memcpy(gain_cfg_ptr->coeff_val, me_ptr->coeff_val, sizeof(me_ptr->coeff_val));
		    params_ptr->actual_data_len    = sizeof(control_tx_coeff_arr_t);
	     }
	     else
         {
            AR_MSG(DBG_ERROR_PRIO,
                   "CAPI CONTROL: <<get_param>> Bad param size %lu  Param id = %lu",
                   params_ptr->max_data_len,
                   param_id);
            return CAPI_ENEEDMORE;
         }
	     break;
      }
      case PARAM_ID_GAIN_MODULE_MUTE:
      {
      	 if (params_ptr->max_data_len >= sizeof(control_tx_mute_t))
      	 {
		    control_tx_mute_t *cfg_ptr = (control_tx_mute_t *)(params_ptr->data_ptr);
		    cfg_ptr->mute = me_ptr->mute;
		    params_ptr->actual_data_len    = sizeof(control_tx_mute_t);
	     }
	     else
         {
            AR_MSG(DBG_ERROR_PRIO,
                   "CAPI CONTROL: <<get_param>> Bad param size %lu  Param id = %lu",
                   params_ptr->max_data_len,
                   param_id);
            return CAPI_ENEEDMORE;
         }
	     break;
      }
      default:
      {
         AR_MSG(DBG_ERROR_PRIO, "CAPI CONTROL: Get, unsupported param ID 0x%x", param_id);
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
