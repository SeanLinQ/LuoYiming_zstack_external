/******************************************************************************

 @file fh_api.h

 @brief TIMAC 2.0 API

 Group: WCS LPC
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2016-2019, Texas Instruments Incorporated
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

#ifndef FH_API_H
#define FH_API_H

/******************************************************************************
 Includes
 *****************************************************************************/

#include "mac_high_level.h"
#include "hal_types.h"

/*!
 @mainpage TIMAC 2.0 FH API

 Overview
 ============================
 This document describes the application programming interface for the FH module
 The API provides an interface to the management and data services for HMAC and LMAC

 Callback Functions
 ============================
 These functions must be implemented by the application and are used to pass
 events and data from the MAC to the application.


 API Interfaces
 ============================
 - FHAPI_reset()
 - FHAPI_start()
 - FHAPI_startBS()
 - FHAPI_sendData()
 - FHAPI_stopAsync()
 - FHAPI_getTxParams()
 - FHAPI_completeTxCb()
 - FHAPI_completeRxCb()
 - FHAPI_setStateCb()
 - FHAPI_SFDRxCb()

 <BR><BR>
 */

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/*! FH Flow Control Header IE bitmap  */
#define FH_WISUN_HIE_FC_IE_BITMAP       0x00000001
/*! FH Unicast Time Header IE bitmap  */
#define FH_WISUN_HIE_UT_IE_BITMAP       0x00000002
/*! FH RSL Header IE bitmap  */
#define FH_WISUN_HIE_RSL_IE_BITMAP      0x00000004
/*! FH Broadcast time Header IE bitmap  */
#define FH_WISUN_HIE_BT_IE_BITMAP       0x00000008
/*! FH Unicast Schedule Payload IE bitmap  */
#define FH_WISUN_PIE_US_IE_BITMAP       0x00010000
/*! FH broadcast Schedule Payload IE bitmap  */
#define FH_WISUN_PIE_BS_IE_BITMAP       0x00020000
/*! FH PAN Payload IE bitmap  */
#define FH_WISUN_PIE_PAN_IE_BITMAP      0x00040000
/*! FH Network name Payload IE bitmap  */
#define FH_WISUN_PIE_NETNAME_IE_BITMAP  0x00080000
/*! FH PAN Version Payload IE bitmap  */
#define FH_WISUN_PIE_PANVER_IE_BITMAP   0x00100000
/*! FH GTK Hash Payload IE bitmap  */
#define FH_WISUN_PIE_GTKHASH_IE_BITMAP  0x00200000

/*Num times to check if a packet is detected or not in a certain time */
#define FH_MAXPDB                                2
/******************************************************************************
 Typedefs
 *****************************************************************************/

/*! FH module status types */
typedef enum __fhapi_status_
{
    /*! Success */
    FHAPI_STATUS_SUCCESS                       = 0x00,
    /*! FH general error */
    FHAPI_STATUS_ERR                           = 0x61,
    /*! IE is not supported in FH */
    FHAPI_STATUS_ERR_NOT_SUPPORTED_IE          = 0x62,
    /*! There is no ASYNC message in MAC TX queue */
    FHAPI_STATUS_ERR_NOT_IN_ASYNC              = 0x63,
    /*! destination address is not in FH neighbor table */
    FHAPI_STATUS_ERR_NO_ENTRY_IN_THE_NEIGHBOR  = 0x64,
    /*! fh is not in UC or BC dwell time slot */
    FHAPI_STATUS_ERR_OUT_SLOT                  = 0x65,
    /*! address is invalid */
    FHAPI_STATUS_ERR_INVALID_ADDRESS           = 0x66,
    /*! IE format is wrong */
    FHAPI_STATUS_ERR_INVALID_FORMAT            = 0x67,
    /*! PIB is not supported in FH module */
    FHAPI_STATUS_ERR_NOT_SUPPORTED_PIB         = 0x68,
    /* PIB is read only in FH module */
    FHAPI_STATUS_ERR_READ_ONLY_PIB             = 0x69,
    /*! parameter is invalid in FH PIB API */
    FHAPI_STATUS_ERR_INVALID_PARAM_PIB         = 0x6A,
    /*! invalid FH frame type */
    FHAPI_STATUS_ERR_INVALID_FRAME_TYPE        = 0x6B,
    /*! expired FH node */
    FHAPI_STATUS_ERR_EXPIRED_NODE              = 0x6C,
    /*! patch is not needed */
    FHAPI_STATUS_ERR_NO_PATCH                  = 0x6D,
    /*! last status value of FH module */
    FHAPI_STATUS_MAX
} FHAPI_status;

/*! FH module IE type */
typedef enum __ie_type
{
    /*! Flow Control IE */
    FH_IE_TYPE_FC_IE        = 0x3,
    /*! Unicast Time IE */
    FH_IE_TYPE_UT_IE        = 0x1,
    /*! RSL IE */
    FH_IE_TYPE_RSL_IE       = 0x4,
    /*! Broadcast Time IE */
    FH_IE_TYPE_BT_IE        = 0x2,
    /*! Unicast Schedule IE */
    FH_IE_TYPE_US_IE        = 0x1,
    /*! Broadcast Schedule IE */
    FH_IE_TYPE_BS_IE        = 0x2,
    /*! PAN IE */
    FH_IE_TYPE_PAN_IE       = 0x4,
    /*! Network Name IE */
    FH_IE_TYPE_NETNAME_IE   = 0x5,
    /*! PAN Version IE */
    FH_IE_TYPE_PANVER_IE    = 0x6,
    /*! GTK Hash IE */
    FH_IE_TYPE_GTKHASH_IE   = 0x7
} IE_TYPE_t;

/*! LMAC callback status */
typedef enum __lmac_status
{
    /*! LMAC TX Start */
    LMAC_TX_START               = 0,
    /*! LMAC received SFD */
    LMAC_RX_SFD                 = 1,
    /*! LMAC TX CCA Busy */
    LMAC_CCA_BUSY               = 2,
    /*! LMAC TX RSSI Busy */
    LMAC_RSSI_BUSY              = 3,
    /*! LMAC TX Push to Queue because LMAC in RX */
    LMAC_TX_PUSH_TO_QUEUE       = 4


} FH_LMAC_STATUS_t;

/******************************************************************************
 Global Externs
 *****************************************************************************/

/******************************************************************************
 Prototype
 *****************************************************************************/


/*!
 * @brief       resets timers and data structure of the FH module. This API must be
 *              called by all other APIs
 */
extern MAC_INTERNAL_API void FHAPI_reset(void);


/*!
 * @brief       starts the unicast timer and if coordinator broadcast timer as well
 */
extern MAC_INTERNAL_API void FHAPI_start(void);

/*!
 * @brief       starts the broadcast timer once a node joins to the network
 */
extern MAC_INTERNAL_API void FHAPI_startBS(void);


/*!
 * @brief       This function is called when a packet is to be transmitted using frequency
 *              hopping module. It will extract TX packets from MAC TX queue based on FH type
 *              (ASYNC, Broadcast or unicast). For ASYNC packet, it will call macTxFrame without
 *              CSMA/CA. All other type packets will be sent in CSMA/CA fashion
 */

extern MAC_INTERNAL_API void FHAPI_sendData(void);


/*!
 * @brief       stop an Async process already in place. If the FH module is not in the ASYNC
 *              mode, This function will check the MAC TX queue to see if there are any ASYNC
 *              request start messages. if there is any ASYNC start request message, it will be
 *              purged from MAC TX queue. if the FH is in the middle of ASYNC operation, it will
 *              wait to current PHY TX complete and stop ASYNC operation.
 *
 * @return      The status of the operation, as follows:<BR>
 *              [FHAPI_STATUS_SUCCESS](@ref FHAPI_STATUS_SUCCESS)
 *               - Operation successful<BR>
 *              [FHAPI_STATUS_ERR_NOT_IN_ASYNC] (@ref FHAPI_STATUS_ERR_NOT_IN_ASYNC)
 *               - there is no ASYNC request message in the MAC TX queue<BR>
 */
extern MAC_INTERNAL_API FHAPI_status FHAPI_stopAsync(void);

/*!
 * @brief       get TX channel and adjusted backOffDur. After this function call, pbackOffDur will
 *              be adjusted and fall within a slot
 *
 * @param pBackOffDur - pointer to the back off duration
 * @param chIdx - pointer to the computed TX channel.
 *
 * @return      The status of the operation, as follows:<BR>
 *              [FHAPI_STATUS_SUCCESS](@ref FHAPI_STATUS_SUCCESS)
 *               - Operation successful<BR>
 *              [FHAPI_STATUS_ERR_OUT_SLOT] (@ref FHAPI_STATUS_ERR_OUT_SLOT)
 *               - the back off will be out of slot (unicast or broadcast)<BR>
 *              [FHAPI_STATUS_ERR_NO_ENTRY_IN_THE_NEIGHBOR] (@ref FHAPI_STATUS_ERR_NO_ENTRY_IN_THE_NEIGHBOR)
 *               - the destination address is not in the FH neighbor table<BR>
 *              [FHAPI_STATUS_ERR_INVALID_FRAME_TYPE] (@ref FHAPI_STATUS_ERR_INVALID_FRAME_TYPE)
 *               - frame type in TX packet is invalid<BR>
 */
extern MAC_INTERNAL_API FHAPI_status FHAPI_getTxParams_old(uint32_t *pBackOffDur, uint8_t *chIdx);
extern MAC_INTERNAL_API FHAPI_status FHAPI_getTxParams(uint32_t *pBackOffDur, uint8_t *chIdx);
/*!
 * @brief       callback from LMAC to indicate transmission status
 *
 * @param status - status of PHY TX transmission status<BR>
 *                  [MAC_NO_TIME]
    *                - there is no time to transmit this packet<BR>
    *               [ other value]
    *                - LMAC TX is success<BR>
 */
extern MAC_INTERNAL_API void FHAPI_completeTxCb(uint8_t status);

/*!
 * @brief       callback from LMAC to indicate packet is received sucessfully
 *
 * @param pMsg - pointer of received packet<BR>
 *
 */
extern MAC_INTERNAL_API void FHAPI_completeRxCb(macRx_t *pMsg);

/*!
 * @brief       callback from LMAC to indicate radio state
 *
 * @param state - state of LMAC radio state <BR>
 *               [LMAC_TX_START]
 *               - LMAC TX is started <BR>
 *               [LMAC_RX_SFD]
 *               - LMAC detected SFD<BR>
 *               [LMAC_CCA_BUSY]
 *               - LMAC CCA busy<BR>
 *
 */
extern MAC_INTERNAL_API void FHAPI_setStateCb(FH_LMAC_STATUS_t state);


/*!
 * @brief       callback from LMAC to indicate SFD is detected
 *
 * @param status - the status of SDF trigger. The FH module will start the RX logic when
 *          	MAC_SFD_DETECTED status is received and terminate the RX logic
 *          	when a non-zero
 *               [MAC_SFD_DETECTED]
 *               - SFD is detected <BR>
 *               [MAC_SFD_FRAME_RECEIVED]
 *               - MAC RX packet is received successfully<BR>
 *               [MAC_SFD_FRAME_DISCARDED]
 *               - MAC frame is discarded<BR>
 *               [MAC_SFD_HALT_CLEANED_UP]
 *               - MAC frame is cleanup by HAL<BR>
 *               [MAC_SFD_CRC_ERROR]
 *               - received MAC framehas CRC error<BR>
 *
 */
extern MAC_INTERNAL_API void FHAPI_SFDRxCb(uint8 status);


/*!
 * @brief       get channel hopping function of the node transmitting to
 *
 * @return      TX channel is hopping or fixed
 */
extern MAC_INTERNAL_API uint8_t FHAPI_getTxChHoppingFunc(void);


/*!
 * @brief       get remaining time the node transmitting to will be on current slot
 *
 * @return      reamining dwell time
 */
extern MAC_INTERNAL_API uint32_t FHAPI_getRemDT(void);


/*!
 * Computes the number of ones in a exclude channel bit accounting
 *              for max possible channels
 *
 */
extern MAC_INTERNAL_API uint8_t FHAPI_getBitCount(void);
/**************************************************************************************************
 */
#endif
