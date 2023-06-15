/*************************************************************************
 * 
 * Copyright (c) 2019 Redpine Signals Incorporated. All Rights Reserved.
 * 
 * NOTICE:  All  information  contained  herein is, and  remains  the  property of 
 * Redpine Signals Incorporated. The intellectual and technical concepts contained
 * herein  are  proprietary to  Redpine Signals Incorporated and may be covered by 
 * U.S. and Foreign Patents, patents in process, and are protected by trade secret
 * or copyright law. Dissemination  of this  information or reproduction  of  this
 * material is strictly forbidden unless prior written permission is obtained from
 * Redpine Signals Incorporated.
 */

/**
 * Includes
 */


#include "rsi_setregion_countryinfo.h"

#if RSI_SET_REGION_AP_FROM_USER

/*====================================================*/
/**
 * @fn             void extract_setregionap_country_info(rsi_usetregion_ap_t *ptr_setreg_ap)
 * @brief          Initializes the parameters used in set region ap command
 * @param[in]      rsi_usetregion_ap_t *ptr_setreg_ap, pointer to the global parameter structure
 * @param[out]     none
 * @return         none
 *                 
 * @description	
 * This function is used to initialize the global parameter structure with parameters
 * used in set region ap command based on the region.
 * 
 */
void extract_setregionap_country_info(rsi_req_set_region_ap_t *rsi_set_region_ap)
{
  //! The given domain rules are generic rules for some of the countries
  //! USER can configure the domain regulations for the respective countries
  uint8_t region_code;

  if(!strcmp(RSI_COUNTRY_CODE,"US "))
    region_code = REGION_US;
  else if(!strcmp(RSI_COUNTRY_CODE,"EU "))
    region_code = REGION_EU;
  else if(!strcmp(RSI_COUNTRY_CODE,"JP "))
    region_code = REGION_JP;
  else if(!strcmp(RSI_COUNTRY_CODE,"SG "))
    region_code = REGION_SINGAPORE;
  else if(!strcmp(RSI_COUNTRY_CODE,"KR "))
    region_code = REGION_KOREA;


  switch(region_code)
  {
    case REGION_US:
      {

        if(RSI_BAND == RSI_BAND_2P4GHZ)
        {
          rsi_set_region_ap->no_of_rules[0] = 1;
          rsi_set_region_ap->channel_info[0].first_channel= 1;
          rsi_set_region_ap->channel_info[0].no_of_channels= 11;
          rsi_set_region_ap->channel_info[0].max_tx_power= 27;
        }
        else
        {
          rsi_set_region_ap->no_of_rules[0] = 5;
          rsi_set_region_ap->channel_info[0].first_channel= 36;
          rsi_set_region_ap->channel_info[0].no_of_channels= 4;
          rsi_set_region_ap->channel_info[0].max_tx_power= 16;
          rsi_set_region_ap->channel_info[1].first_channel= 52;
          rsi_set_region_ap->channel_info[1].no_of_channels= 4;
          rsi_set_region_ap->channel_info[1].max_tx_power= 23;
          rsi_set_region_ap->channel_info[2].first_channel= 100;
          rsi_set_region_ap->channel_info[2].no_of_channels= 5;
          rsi_set_region_ap->channel_info[2].max_tx_power= 23;
          rsi_set_region_ap->channel_info[3].first_channel= 132;
          rsi_set_region_ap->channel_info[3].no_of_channels= 3;
          rsi_set_region_ap->channel_info[3].max_tx_power= 23;
          rsi_set_region_ap->channel_info[4].first_channel= 149;
          rsi_set_region_ap->channel_info[4].no_of_channels= 5;
          rsi_set_region_ap->channel_info[4].max_tx_power= 29;

        }
      }
      break;
    case REGION_EU:
      {
        if(RSI_BAND == RSI_BAND_2P4GHZ)
        {
          rsi_set_region_ap->no_of_rules[0] = 1;
          rsi_set_region_ap->channel_info[0].first_channel= 1;
          rsi_set_region_ap->channel_info[0].no_of_channels= 13;
          rsi_set_region_ap->channel_info[0].max_tx_power= 20;
        }
        else
        {
          rsi_set_region_ap->no_of_rules[0] = 3;
          rsi_set_region_ap->channel_info[0].first_channel= 36;
          rsi_set_region_ap->channel_info[0].no_of_channels= 4;
          rsi_set_region_ap->channel_info[0].max_tx_power= 23;
          rsi_set_region_ap->channel_info[1].first_channel= 52;
          rsi_set_region_ap->channel_info[1].no_of_channels= 4;
          rsi_set_region_ap->channel_info[1].max_tx_power= 23;
          rsi_set_region_ap->channel_info[2].first_channel= 100;
          rsi_set_region_ap->channel_info[2].no_of_channels= 11;
          rsi_set_region_ap->channel_info[2].max_tx_power= 30;

        }


      }
      break;
    case REGION_JP:
      {
        if(RSI_BAND == RSI_BAND_2P4GHZ)
        {
          rsi_set_region_ap->no_of_rules[0] = 1;
          rsi_set_region_ap->channel_info[0].first_channel= 1;
          rsi_set_region_ap->channel_info[0].no_of_channels= 14;
          rsi_set_region_ap->channel_info[0].max_tx_power= 20;
        }
        else
        {
          rsi_set_region_ap->no_of_rules[0] = 3;
          rsi_set_region_ap->channel_info[0].first_channel= 36;
          rsi_set_region_ap->channel_info[0].no_of_channels= 4;
          rsi_set_region_ap->channel_info[0].max_tx_power= 20;
          rsi_set_region_ap->channel_info[1].first_channel= 52;
          rsi_set_region_ap->channel_info[1].no_of_channels= 4;
          rsi_set_region_ap->channel_info[1].max_tx_power= 20;
          rsi_set_region_ap->channel_info[2].first_channel= 100;
          rsi_set_region_ap->channel_info[2].no_of_channels= 11;
          rsi_set_region_ap->channel_info[2].max_tx_power= 30;

        }

      }
      break;

    case REGION_SINGAPORE:

      {
        if(RSI_BAND == RSI_BAND_2P4GHZ)
        {
          rsi_set_region_ap->no_of_rules[0] = 1;
          rsi_set_region_ap->channel_info[0].first_channel= 1;
          rsi_set_region_ap->channel_info[0].no_of_channels= 13;
          rsi_set_region_ap->channel_info[0].max_tx_power= 27;
        }
        else
        {
          rsi_set_region_ap->no_of_rules[0] = 5;
          rsi_set_region_ap->channel_info[0].first_channel= 36;
          rsi_set_region_ap->channel_info[0].no_of_channels= 4;
          rsi_set_region_ap->channel_info[0].max_tx_power= 16;
          rsi_set_region_ap->channel_info[1].first_channel= 52;
          rsi_set_region_ap->channel_info[1].no_of_channels= 4;
          rsi_set_region_ap->channel_info[1].max_tx_power= 23;
          rsi_set_region_ap->channel_info[2].first_channel= 100;
          rsi_set_region_ap->channel_info[2].no_of_channels= 5;
          rsi_set_region_ap->channel_info[2].max_tx_power= 23;
          rsi_set_region_ap->channel_info[3].first_channel= 132;
          rsi_set_region_ap->channel_info[3].no_of_channels= 3;
          rsi_set_region_ap->channel_info[3].max_tx_power= 23;
          rsi_set_region_ap->channel_info[4].first_channel= 149;
          rsi_set_region_ap->channel_info[4].no_of_channels= 4;
          rsi_set_region_ap->channel_info[4].max_tx_power= 29;

        }
      }
      break;
    case REGION_KOREA:
      {
        if(RSI_BAND == RSI_BAND_2P4GHZ)
        {
          rsi_set_region_ap->no_of_rules[0] = 1;
          rsi_set_region_ap->channel_info[0].first_channel= 1;
          rsi_set_region_ap->channel_info[0].no_of_channels= 13;
          rsi_set_region_ap->channel_info[0].max_tx_power= 27;
        }
        else
        {
          rsi_set_region_ap->no_of_rules[0] = 1;
          rsi_set_region_ap->channel_info[0].first_channel= 149;
          rsi_set_region_ap->channel_info[0].no_of_channels= 4;
          rsi_set_region_ap->channel_info[0].max_tx_power= 29;

        }
      }

      break;
    default :
      {
        if(RSI_BAND == RSI_BAND_2P4GHZ)
        {
          rsi_set_region_ap->no_of_rules[0] = 1;
          rsi_set_region_ap->channel_info[0].first_channel= 1;
          rsi_set_region_ap->channel_info[0].no_of_channels= 11;
          rsi_set_region_ap->channel_info[0].max_tx_power= 27;
        }
        else
        {
          rsi_set_region_ap->no_of_rules[0] = 5;
          rsi_set_region_ap->channel_info[0].first_channel= 36;
          rsi_set_region_ap->channel_info[0].no_of_channels= 4;
          rsi_set_region_ap->channel_info[0].max_tx_power= 16;
          rsi_set_region_ap->channel_info[1].first_channel= 52;
          rsi_set_region_ap->channel_info[1].no_of_channels= 4;
          rsi_set_region_ap->channel_info[1].max_tx_power= 23;
          rsi_set_region_ap->channel_info[2].first_channel= 100;
          rsi_set_region_ap->channel_info[2].no_of_channels= 5;
          rsi_set_region_ap->channel_info[2].max_tx_power= 23;
          rsi_set_region_ap->channel_info[3].first_channel= 132;
          rsi_set_region_ap->channel_info[3].no_of_channels= 3;
          rsi_set_region_ap->channel_info[3].max_tx_power= 23;
          rsi_set_region_ap->channel_info[4].first_channel= 149;
          rsi_set_region_ap->channel_info[4].no_of_channels= 5;
          rsi_set_region_ap->channel_info[4].max_tx_power= 29;

        }
      }
      break;

  }



}
#endif
