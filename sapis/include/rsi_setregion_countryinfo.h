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

#ifndef _SETREGION_COUNTRYINFO_H_
#define _SETREGION_COUNTRYINFO_H_

#include <rsi_driver.h>

#define       REGION_US          1
#define       REGION_EU          2
#define       REGION_JP          3
#define       REGION_INDIA       4
#define       REGION_SINGAPORE   5
#define       REGION_KOREA       6


void extract_setregionap_country_info(rsi_req_set_region_ap_t *);
#endif
