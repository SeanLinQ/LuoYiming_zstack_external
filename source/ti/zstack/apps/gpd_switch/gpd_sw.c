/**************************************************************************************************
  Filename:       gpd_sw.c
  Revised:        $Date: 2017-8-15 16:04:46 -0700 (Tue, 15 Aug 2017) $
  Revision:       $Revision: 40796 $


  Description:    Green power device - sample switch application.


  Copyright 2006-2014 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
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
**************************************************************************************************/

/*********************************************************************
  This application implements a Green Power Switch, based on Z-Stack 3.0. It can be configured
  to have all supported security levels, application ID's or keys from gpd_config.h file. Some of these configurations are
  set as compilation flags.

  Application-specific UI peripherals being used:

  - LEDs:
    LED Red: Blinks if a frame(s) got transmitted but not all of them. If none of the frames got able to be transmitted then a solid On is set.

    LED Green: If all duplicate frames are send (GPDF_FRAME_DUPLICATES), then Green LED becomes solid On.

  Application-specific switch button actions:

  Switch behaves depending if BATTERYLESS_DEVICE flag is enabled or not.

    |Switch pressed| Battery device | Batteryless device |
    |--------------|----------------|--------------------|
    | none         | No Action      |Sends Toggle Cmd    |
    | 1            | Sends On Cmd   |Sends On Cmd        |
    | 2            | Sends Off Cmd  |Sends Off Cmd       |

*********************************************************************/

/*********************************************************************
 * INCLUDES
 */
#include "zcomdef.h"
#include "rom_jt_154.h"
#include "gpd_sw.h"

#include "ti_drivers_config.h"
#include "cui.h"
#include <ti/sysbios/knl/Semaphore.h>
#include "api_mac.h"
#include "mac_util.h"
#include "string.h"
#include "gpd.h"
#include "gpd_memory.h"

/*********************************************************************
 * MACROS
 */

#define CONFIG_FH_ENABLE             false
#define CONFIG_TRANSMIT_POWER        0

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Passed in function pointers to the NV driver
static NVINTF_nvFuncts_t *pfnZdlNV = NULL;
static uint8_t duplicates;
static ApiMac_mcpsDataReq_t gpdfDuplicate;

extern uint8_t _macTaskId;

/*********************************************************************
 * GLOBAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
// Semaphore used to post events to the application thread
static Semaphore_Handle appSemHandle;

// Key press parameters
static uint8_t keys = 0xFF;

// Task pending events
static uint16_t events = 0;
static CUI_clientHandle_t gCuiHandle;
/*********************************************************************
 * LOCAL FUNCTIONS
 */
static void     gpdSampleSw_initialization(void);
static void     gpdSampleSw_changeKeyCallback(uint32_t _btn, Button_EventMask _buttonEvents);
static void     gpdSampleSw_processKey(uint32_t keysPressed);
static uint16_t gpdSampleSw_process_loop( void );
static void     gpdSampleSw_Init(void);
static void     dataCnfCB(ApiMac_mcpsDataCnf_t *pDataCnf);
static uint8_t  Initialize_CUI(void);
/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * STATUS STRINGS
 */

/*********************************************************************
 * REFERENCED EXTERNALS
 */

/******************************************************************************
 Callback tables
 *****************************************************************************/

/*! API MAC Callback table */
static ApiMac_callbacks_t Gpd_macCallbacks =
    {
      /*! Associate Indicated callback */
      NULL,
      /*! Associate Confirmation callback */
      NULL,
      /*! Disassociate Indication callback */
      NULL,
      /*! Disassociate Confirmation callback */
      NULL,
      /*! Beacon Notify Indication callback */
      NULL,
      /*! Orphan Indication callback */
      NULL,
      /*! Scan Confirmation callback */
      NULL,
      /*! Start Confirmation callback */
      NULL,
      /*! Sync Loss Indication callback */
      NULL,
      /*! Poll Confirm callback */
      NULL,
      /*! Comm Status Indication callback */
      NULL,
      /*! Poll Indication Callback */
      NULL,
      /*! Data Confirmation callback */
      dataCnfCB,
      /*! Data Indication callback */
      NULL,
      /*! Purge Confirm callback */
      NULL,
      /*! WiSUN Async Indication callback */
      NULL,
      /*! WiSUN Async Confirmation callback */
      NULL,
      /*! Unprocessed message callback */
      NULL
    };

/*******************************************************************************
 * @fn          sampleApp_task
 *
 * @brief       Application task entry point for the Z-Stack
 *              Sample Application
 *
 * @param       pfnNV - pointer to the NV functions
 *
 * @return      none
 */
void sampleApp_task(NVINTF_nvFuncts_t *pfnNV)
{
#ifdef NV_RESTORE
   // Save and register the function pointers to the NV drivers
   pfnZdlNV = pfnNV;
#endif
  // Initialize application
  gpdSampleSw_initialization();

  // No return from task process
  gpdSampleSw_process_loop();

#ifndef NV_RESTORE
  //To avoid warnings.
  (void)pfnZdlNV;
#endif
}

/*******************************************************************************
 * @fn          gpdSampleSw_initialization
 *
 * @brief       Initialize the application
 *
 * @param       none
 *
 * @return      none
 */
static void gpdSampleSw_initialization(void)
{
    /* Initialize keys */
    keys = Initialize_CUI();

    //Initialize stack
    gpdSampleSw_Init();

    memset(&duplicates, frameDuplicates, sizeof(uint8_t));

    gpdDuplicateFrameInit(&gpdfDuplicate);

#ifdef BATTERYLESS_DEVICE
    /* Start the device */
    Util_setEvent(&events, SAMPLEAPP_KEY_EVT);

    Semaphore_post(appSemHandle);
#endif
}

/*********************************************************************
 * @fn          gpdSampleSw_Init
 *
 * @brief       Initialization function for the gpd layer.
 *
 * @param       none
 *
 * @return      none
 */
static void gpdSampleSw_Init(void)
{
    /* Initialize the MAC */
    appSemHandle = ApiMac_init(_macTaskId, CONFIG_FH_ENABLE);

    /* Register the MAC Callbacks */
    ApiMac_registerCallbacks(&Gpd_macCallbacks);

#if defined(CONFIG_PA_TYPE) && (CONFIG_PA_TYPE==APIMAC_HIGH_PA)
    /* Set the PA type */
    ApiMac_mlmeSetReqUint8(ApiMac_attribute_paType,
                           (uint8_t)CONFIG_PA_TYPE);
#endif

    /* Set the transmit power */
    ApiMac_mlmeSetReqUint8(ApiMac_attribute_phyTransmitPowerSigned,
                           (uint8_t)CONFIG_TRANSMIT_POWER);
#ifdef NV_RESTORE
    gp_appNvInit(pfnZdlNV);
#endif
}

/*********************************************************************
 * @fn          gpdSampleSw_process_loop
 *
 * @brief       Event Loop Processor for zclGeneral.
 *
 * @param       none
 *
 * @return      none
 */
static uint16_t gpdSampleSw_process_loop( void )
{
    /* Forever loop */
    for(;;)
    {
        if(events & SAMPLEAPP_KEY_EVT)
        {
            // Process Key Presses
            gpdSampleSw_processKey(keys);
            keys = 0xFF;
            events &= ~SAMPLEAPP_KEY_EVT;
        }

        ApiMac_processIncoming();
    }
}

/*********************************************************************
 * @fn      gpdSampleSw_changeKeyCallback
 *
 * @brief   Key event handler function
 *
 * @param   keysPressed - keys to be process in application context
 *
 * @return  none
 */
static void gpdSampleSw_changeKeyCallback(uint32_t _btn, Button_EventMask _buttonEvents)
{
    if (_buttonEvents & Button_EV_CLICKED)
    {
        keys = _btn;
        /* Start the device */
        Util_setEvent(&events, SAMPLEAPP_KEY_EVT);

        Semaphore_post(appSemHandle);
    }
}

static void gpdSampleSw_processKey(uint32_t keysPressed)
{
#ifdef BATTERYLESS_DEVICE
    //Button 1
    if(keysPressed == CONFIG_BTN_LEFT)
    {
        gpdfReq.gpdCmdID = GP_COMMAND_ON;
    }
    //Button 2
    else if (keysPressed == CONFIG_BTN_RIGHT)
    {
        gpdfReq.gpdCmdID = GP_COMMAND_OFF;
    }
    //No button pressed
    else
    {
        gpdfReq.gpdCmdID = GP_COMMAND_TOGGLE;
    }
#else
    //Button 1
    if(keysPressed == CONFIG_BTN_LEFT)
    {
        gpdfReq.gpdCmdID = GP_COMMAND_ON;
    }
    //Button 2
    if(keysPressed == CONFIG_BTN_RIGHT)
    {
        gpdfReq.gpdCmdID = GP_COMMAND_OFF;
    }
    //clear the states
    CUI_ledOff(gCuiHandle, CONFIG_LED_RED);
    CUI_ledOff(gCuiHandle, CONFIG_LED_GREEN);
#endif

    //Send the frame
    GreenPowerDataFrameSend(&gpdfReq ,gpdChannel, TRUE);
}

/*********************************************************************
 * @fn         dataCnfCB
 *
 * @brief      MAC Data Confirm callback.
 *
 * @param      pDataCnf - pointer to the data confirm information
 */
static void dataCnfCB(ApiMac_mcpsDataCnf_t *pDataCnf)
{
#ifdef BATTERYLESS_DEVICE
    if(pDataCnf->status == ApiMac_status_channelAccessFailure)
    {
        if(duplicates < frameDuplicates)
        {
            //Keep blinking to indicate fail to send all the duplicates
            CUI_ledBlink(gCuiHandle, CONFIG_LED_RED, CUI_BLINK_CONTINUOUS);
        }
        else
        {
            //Fail and not a single frame got send
            CUI_ledOn(gCuiHandle, CONFIG_LED_RED, NULL);
        }
        while(1)
        {
            static uint8_t finish = 0;

            //End of operation.
            //Power consumption should be measured until this point since the start of the device operation to see if power budget is achieved.
            if(!finish)
            {
                //NOTE: Consider free the msg and initialize the duplicate msg pointer and turn off the radio if your product operation does not end here.
                ApiMac_mlmeSetReqBool(ApiMac_attribute_RxOnWhenIdle, FALSE);
                OsalPort_free(gpdfDuplicate.msdu.p);
                duplicates = frameDuplicates;
                finish = TRUE;
            }

        }
    }
    else if(pDataCnf->status == ApiMac_status_success)
    {
      if(duplicates > 0)
      {
        uint8_t secNum;
        ApiMac_mlmeGetReqUint8(ApiMac_attribute_dsn, (uint8_t*)&secNum);
        secNum--;
        ApiMac_mlmeSetReqUint8(ApiMac_attribute_dsn, secNum);
        ApiMac_mcpsDataReq(&gpdfDuplicate);
        duplicates--;
      }
      else
      {
        //Turn on Green to indicate end of operation with Success!
          CUI_ledOn(gCuiHandle, CONFIG_LED_GREEN, NULL);

        while(1)
        {
            static uint8_t finish = 0;
            //End of operation.
            //Power consumption should be measured until this point since the start of the device operation to see if power budget is achieved.

            if(!finish)
            {
                //NOTE: Consider free the msg and initialize the duplicate msg pointer and turn off the radio if your product operation does not end here.
                ApiMac_mlmeSetReqBool(ApiMac_attribute_RxOnWhenIdle, FALSE);
                OsalPort_free(gpdfDuplicate.msdu.p);
                duplicates = frameDuplicates;
                finish = TRUE;
            }
        }
      }
    }
#else
    if(pDataCnf->status == ApiMac_status_channelAccessFailure)
    {
        // turn off receiver
        ApiMac_mlmeSetReqBool(ApiMac_attribute_RxOnWhenIdle, FALSE);

        if(duplicates < frameDuplicates)
        {
            //Keep blinking to indicate fail to send all the duplicates
            CUI_ledBlink(gCuiHandle, CONFIG_LED_RED, CUI_BLINK_CONTINUOUS);
        }
        else
        {
            //Fail and not a single frame got send
            CUI_ledOn(gCuiHandle, CONFIG_LED_RED, NULL);
        }
        OsalPort_free(gpdfDuplicate.msdu.p);
        duplicates = frameDuplicates;
    }
    else if(pDataCnf->status == ApiMac_status_success)
    {
        if(duplicates > 0)
        {
            uint8_t secNum;
            ApiMac_mlmeGetReqUint8(ApiMac_attribute_dsn, (uint8_t*)&secNum);
            secNum--;
            ApiMac_mlmeSetReqUint8(ApiMac_attribute_dsn, secNum);
            ApiMac_mcpsDataReq(&gpdfDuplicate);
            duplicates--;
        }
        else
        {
            // turn off receiver
            ApiMac_mlmeSetReqBool(ApiMac_attribute_RxOnWhenIdle, FALSE);

            //Solid Green due to successful transmission
            CUI_ledOn(gCuiHandle, CONFIG_LED_GREEN, NULL);

            OsalPort_free(gpdfDuplicate.msdu.p);
            duplicates = frameDuplicates;
        }
    }
#endif
}


/*******************************************************************************
 * @fn          Initialize_CUI
 *
 * @brief       Initialize the Common User Interface
 *
 * @param       none
 *
 * @return      none
 */
static uint8_t Initialize_CUI(void)
{
    CUI_params_t params;
    CUI_paramsInit(&params);
    uint8_t key = 0;

    //Do not initialize the UART, GPD do not use APP menues
    params.manageUart = FALSE;

    CUI_retVal_t retVal = CUI_init(&params);
    if (CUI_SUCCESS != retVal)
        while(1){};

    CUI_clientParams_t clientParams;
    CUI_clientParamsInit(&clientParams);

    strncpy(clientParams.clientName, "GPD Switch", MAX_CLIENT_NAME_LEN);

    gCuiHandle = CUI_clientOpen(&clientParams);
    if (gCuiHandle == NULL)
        while(1){};


    /* Initialize btns */
    CUI_btnRequest_t btnRequest;
    btnRequest.index = CONFIG_BTN_RIGHT;
    btnRequest.appCB = NULL;

    //Initialize left button to poll it
    retVal = CUI_btnResourceRequest(gCuiHandle, &btnRequest);
    if (CUI_SUCCESS != retVal)
        while(1){};

    //Initialize right buttonto poll it
    btnRequest.index = CONFIG_BTN_LEFT;
    retVal = CUI_btnResourceRequest(gCuiHandle, &btnRequest);
    if (CUI_SUCCESS != retVal)
        while(1){};

#ifdef BATTERYLESS_DEVICE
    bool btnState = false;
    retVal = CUI_btnGetValue(gCuiHandle, CONFIG_BTN_RIGHT, &btnState);
    if(!btnState)
    {
        key = CONFIG_BTN_RIGHT;
    }
    else
    {
        retVal = CUI_btnGetValue(gCuiHandle, CONFIG_BTN_LEFT, &btnState);
        if(!btnState)
        {
            key = CONFIG_BTN_RIGHT;
        }
    }
#endif

    CUI_btnSetCb(gCuiHandle, CONFIG_BTN_RIGHT, gpdSampleSw_changeKeyCallback);
    CUI_btnSetCb(gCuiHandle, CONFIG_BTN_LEFT, gpdSampleSw_changeKeyCallback);

    //Request the LEDs for App
    /* Initialize the LEDS */
    CUI_ledRequest_t ledRequest;
    ledRequest.index = CONFIG_LED_RED;

    retVal = CUI_ledResourceRequest(gCuiHandle, &ledRequest);

    ledRequest.index = CONFIG_LED_GREEN;

    retVal = CUI_ledResourceRequest(gCuiHandle, &ledRequest);
    (void) retVal;

    return key;
}

