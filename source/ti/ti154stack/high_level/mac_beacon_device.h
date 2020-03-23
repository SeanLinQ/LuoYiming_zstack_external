/******************************************************************************

 @file  mac_beacon_device.h

 @brief Interface to device only procedures for beacon enabled networks.

 Group: WCS, LPC
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2006-2019, Texas Instruments Incorporated
 All rights reserved.

 IMPORTANT: Your use of this Software is limited to those specific rights
 granted under the terms of a software license agreement between the user
 who downloaded the software, his/her employer (which must be your employer)
 and Texas Instruments Incorporated (the "License"). You may not use this
 Software unless you agree to abide by the terms of the License. The License
 limits your use, and you acknowledge, that the Software may not be modified,
 copied or distributed unless embedded on a Texas Instruments microcontroller
 or used solely and exclusively in conjunction with a Texas Instruments radio
 frequency transceiver, which is integrated into your product. Other than for
 the foregoing purpose, you may not use, reproduce, copy, prepare derivative
 works of, modify, distribute, perform, display or sell this Software and/or
 its documentation for any purpose.

 YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
 PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
 INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
 NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
 TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
 NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
 LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
 INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
 OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
 OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
 (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

 Should you have any questions regarding your right to use this Software,
 contact Texas Instruments Incorporated at www.TI.com.

 ******************************************************************************
 
 
 *****************************************************************************/

#ifndef MAC_BEACON_DEVICE_H
#define MAC_BEACON_DEVICE_H

/* ------------------------------------------------------------------------------------------------
 *                                               Includes
 * ------------------------------------------------------------------------------------------------
 */
#include "mac_high_level.h"

/* ------------------------------------------------------------------------------------------------
 *                                           Function Prototypes
 * ------------------------------------------------------------------------------------------------
 */

#define MAC_BEACON_TO_MULTIPLE  2

typedef struct
{
  macTimer_t      trackTimer;           /* beacon tracking timer */
  macTimer_t      timeoutTimer;         /* beacon timeout timer */
  macTimer_t      broacastPendTimer;    /* broadcast pending timer */
  uint8           lostCount;            /* lost beacon count */
} macBeaconDevice_t;


/* Action Functions */
MAC_INTERNAL_API void macAutoPoll(macEvent_t *pEvent);
MAC_INTERNAL_API void macApiSyncReq(macEvent_t *pEvent);
MAC_INTERNAL_API void macBeaconStartFrameResponseTimer(macEvent_t *pEvent);
MAC_INTERNAL_API void macStartBroadcastPendTimer(macEvent_t *pEvent);
MAC_INTERNAL_API void macBeaconStopTrack(void);
MAC_INTERNAL_API void macRxBeaconCritical(macRx_t *pBuf);
#endif /* MAC_BEACON_DEVICE_H */
