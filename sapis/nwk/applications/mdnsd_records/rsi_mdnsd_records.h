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
#ifndef RSI_MDNSD_RECORDS_H
#define RSI_MDNSD_RECORDS_H



typedef struct rsi_mdns_txt_rec_g
{
    uint8_t  *buffer;       // Pointer to data
    uint16_t buflen;        // Length of buffer
    uint16_t datalen;       // Length currently in use
    uint16_t malloced;  	// Non-zero if buffer was allocated via malloc()
} rsi_mdns_txt_rec_t;


void rsi_mdns_txt_rec_create( rsi_mdns_txt_rec_t *txtRecord, uint16_t bufferLen, void *buffer);
int8_t rsi_mdns_txt_rec_setvalue( rsi_mdns_txt_rec_t *txtRecord, const char *key, uint8_t valueSize, const void *value);
uint8_t *rsi_mdns_txt_rec_search(uint16_t txtLen, const void *txtRecord, const char *key, uint32_t *keylen);
int8_t  rsi_mdns_txt_rec_removevalue(rsi_mdns_txt_rec_t *txtRecord, const char *key);
const void * rsi_mdns_txt_get_bytes_ptr(rsi_mdns_txt_rec_t *txtRecord);

#endif
