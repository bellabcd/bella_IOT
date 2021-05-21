/***************************************************************************//**
 * @file
 * @brief Callback implementation for ZigbeeMinimal sample application.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include "app/framework/include/af.h"

static uint8_t LedBlinkingCount=0;
static uint16_t button0Time = 0;
static uint16_t button0PressedTime=0;

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 *
 * @param status   Ver.: always
 */
bool emberAfStackStatusCallback(EmberStatus status)
{
  // This value is ignored by the framework.
  return false;
}

EmberEventControl ReportAttributeEventControl;
void ReportAttributeEventHandler(void)
{
     emberEventControlSetInactive(ReportAttributeEventControl);
     uint8_t data[4];
     uint8_t bufIndex = 0;
     EmberAfStatus status = emberAfAppendAttributeReportFields(1, ZCL_ON_OFF_CLUSTER_ID, ZCL_ON_OFF_ATTRIBUTE_ID, CLUSTER_MASK_SERVER, data, sizeof(data), &bufIndex);
     if(EMBER_ZCL_STATUS_SUCCESS == status)
     {
         emberAfFillCommandGlobalServerToClientReportAttributes(ZCL_ON_OFF_CLUSTER_ID, data, bufIndex);
         emberAfSetCommandEndpoints(emberAfPrimaryEndpoint(),1);
         emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);
     }
     emberEventControlSetDelayMS(ReportAttributeEventControl,60000);
}

/** @brief Complete
 *
 * This callback is fired when the Network Steering plugin is complete.
 *
 * @param status On success this will be set to EMBER_SUCCESS to indicate a
 * network was joined successfully. On failure this will be the status code of
 * the last join or scan attempt. Ver.: always
 * @param totalBeacons The total number of 802.15.4 beacons that were heard,
 * including beacons from different devices with the same PAN ID. Ver.: always
 * @param joinAttempts The number of join attempts that were made to get onto
 * an open Zigbee network. Ver.: always
 * @param finalState The finishing state of the network steering process. From
 * this, one is able to tell on which channel mask and with which key the
 * process was complete. Ver.: always
 */
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState)
{
  emberAfCorePrintln("%p network %p: 0x%X", "Join", "complete", status);
  if (emberAfNetworkState() == EMBER_JOINED_NETWORK)
     {
      emberAfCorePrintln("Smart Plug Joined Network");
      emberSendDeviceAnnouncement();
      emberAfCorePrintln("Smart Plug Send Device Announcement");
      emberEventControlSetDelayMS(ReportAttributeEventControl,60000);
     }
}

/** @brief On/off Cluster On
 *
 *
 *
 */
boolean emberAfOnOffClusterOnCallback(void)
{
  emberAfCorePrintln("On command is received");
  boolean OnOff=1;
  EmberAfStatus status=emberAfWriteServerAttribute(1,ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID,&OnOff,sizeof(OnOff));
  if (status==EMBER_ZCL_STATUS_SUCCESS)
      {
         halSetLed(1);
         GPIO_PinOutSet(gpioPortD, 4);
         uint8_t OnOffBuff[4]={0x00,0x00,0x10,OnOff};
         emberAfFillCommandGlobalServerToClientReportAttributes(ZCL_ON_OFF_CLUSTER_ID,(uint8_t *) OnOffBuff,4);
         emberAfSetCommandEndpoints(emberAfPrimaryEndpoint(),1);
         emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);
         emberEventControlSetDelayMS(ReportAttributeEventControl,60000);
      }
}

/** @brief On/off Cluster Off
 *
 *
 *
 */
boolean emberAfOnOffClusterOffCallback(void)
{
  emberAfCorePrintln("Off command is received");
  boolean OnOff=0;
  EmberAfStatus status=emberAfWriteServerAttribute(1,ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID,&OnOff,sizeof(OnOff));
  if (status==EMBER_ZCL_STATUS_SUCCESS)
      {
        halClearLed(1);
        GPIO_PinOutClear(gpioPortD, 4);
        uint8_t OnOffBuff[4]={0x00,0x00,0x10,OnOff};
        emberAfFillCommandGlobalServerToClientReportAttributes(ZCL_ON_OFF_CLUSTER_ID,(uint8_t *) OnOffBuff,4);
        emberAfSetCommandEndpoints(emberAfPrimaryEndpoint(),1);
        emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);
        emberEventControlSetDelayMS(ReportAttributeEventControl,60000);
      }
}

/** @brief On/off Cluster Toggle
 *
 *
 *
 */
boolean emberAfOnOffClusterToggleCallback(void)
{
  emberAfCorePrintln("Toggle command is received");
  boolean OnOff;
  EmberAfStatus status =emberAfReadServerAttribute(1, ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID,(uint8_t *) &OnOff, sizeof(OnOff));
  if (status == EMBER_ZCL_STATUS_SUCCESS)
  {
    OnOff=!OnOff;
    status=emberAfWriteServerAttribute(1,ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID,&OnOff,sizeof(OnOff));
    if (status==EMBER_ZCL_STATUS_SUCCESS)
        {
            halToggleLed(1);
            GPIO_PinOutToggle(gpioPortD, 4);
            uint8_t OnOffBuff[4]={0x00,0x00,0x10,OnOff};
            emberAfFillCommandGlobalServerToClientReportAttributes(ZCL_ON_OFF_CLUSTER_ID,(uint8_t *) OnOffBuff,4);
            emberAfSetCommandEndpoints(emberAfPrimaryEndpoint(),1);
            emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);
            emberEventControlSetDelayMS(ReportAttributeEventControl,60000);
        }
  }
}

EmberEventControl LedBlinkingEventControl;
void LedBlinkingEventHandler(void)
{
  emberEventControlSetInactive(LedBlinkingEventControl);
  LedBlinkingCount++;
  halToggleLed(1);
  if (LedBlinkingCount<=0x1A)
    {
      emberEventControlSetDelayMS(LedBlinkingEventControl,200);
    }
  else if (LedBlinkingCount<=0x23)
    {
      emberEventControlSetDelayMS(LedBlinkingEventControl,500);
    }
  else
    {
      LedBlinkingCount=0;
    }
}

/** @brief Called when Button 0 goes low
 *
 * This function is called when the GPIO tied to button zero goes low
 *
 */
void emberAfPluginButtonInterfaceButton0LowCallback(void)
{
  button0Time = halCommonGetInt16uMillisecondTick();
  emberEventControlSetDelayMS(LedBlinkingEventControl,5000);
}

EmberEventControl SoftwareResetEventControl;
void SoftwareResetEventHandler(void)
{
  emberEventControlSetInactive(SoftwareResetEventControl);
  NVIC_SystemReset();
}

/** @brief Called when Button 0 goes high
 *
 * This function is called when the GPIO tied to button zero goes high
 *
 */
void emberAfPluginButtonInterfaceButton0HighCallback(void)
{
  button0PressedTime=halCommonGetInt16uMillisecondTick()-button0Time;
  emberAfCorePrintln("button0 is pressed %u ms", button0PressedTime);
  boolean OnOff;
  EmberAfStatus status =emberAfReadServerAttribute(1, ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID,(uint8_t *) &OnOff, sizeof(OnOff));
  if (button0PressedTime <= 0x1388)//button0PressedTime <= 5s
    {
      emberEventControlSetInactive(LedBlinkingEventControl);
      if (status == EMBER_ZCL_STATUS_SUCCESS)
      {
          OnOff=!OnOff;
          status=emberAfWriteServerAttribute(1,ZCL_ON_OFF_CLUSTER_ID,ZCL_ON_OFF_ATTRIBUTE_ID,&OnOff,sizeof(OnOff));
          if (status==EMBER_ZCL_STATUS_SUCCESS)
          {
                emberEventControlSetInactive(LedBlinkingEventControl);
                halToggleLed(1);
                GPIO_PinOutToggle(gpioPortD, 4);
                uint8_t OnOffBuff[4]={0x00,0x00,0x10,OnOff};
                emberAfFillCommandGlobalServerToClientReportAttributes(ZCL_ON_OFF_CLUSTER_ID,(uint8_t *) OnOffBuff,4);
                emberAfSetCommandEndpoints(emberAfPrimaryEndpoint(),1);
                emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, 0x0000);
                emberEventControlSetDelayMS(ReportAttributeEventControl,60000);
          }
      }
    }
  else if (button0PressedTime > 0x1388 && button0PressedTime <= 0x2710)// 5s < button0PressedTime <= 10s
    {
      emberEventControlSetInactive(LedBlinkingEventControl);
      if (OnOff==0)
        {
          halClearLed(1);
        }
      else
        {
          halSetLed(1);
        }
    }
  else if (button0PressedTime > 0x2710 && button0PressedTime <= 0x3A98)// 10s < button0PressedTime <= 15s
    {
      emberEventControlSetInactive(LedBlinkingEventControl);
      emberClearBindingTable();
      emberLeaveNetwork();
      emberEventControlSetDelayMS(SoftwareResetEventControl,2000);
    }
}

EmberEventControl UsartEventControl;
void UsartEventHandler(void)
{
     emberEventControlSetInactive(UsartEventControl);
     uint16_t len = emberSerialReadAvailable(COM_USART1);
     if(len)
     {
           uint8_t data=0;
           EmberStatus status;
           uint16_t timeout = 2;
           uint16_t startTime = halCommonGetInt16uMillisecondTick();
           while (elapsedTimeInt16u(startTime, halCommonGetInt16uMillisecondTick()) < timeout)
             {
             status = emberSerialReadByte(COM_USART1, &data);
             if (status==EMBER_SUCCESS)
               {
                 emberSerialWriteByte(COM_USART1, data);
                 startTime = halCommonGetInt16uMillisecondTick();
               }
             }
     }
     emberEventControlSetDelayMS(UsartEventControl,5);
}

/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup. Any
 * code that you would normally put into the top of the application's main()
 * routine should be put into this function. This is called before the clusters,
 * plugins, and the network are initialized so some functionality is not yet
 * available.
        Note: No callback in the Application Framework is
 * associated with resource cleanup. If you are implementing your application on
 * a Unix host where resource cleanup is a consideration, we expect that you
 * will use the standard Posix system calls, including the use of atexit() and
 * handlers for signals such as SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If
 * you use the signal() function to register your signal handler, please mind
 * the returned value which may be an Application Framework function. If the
 * return value is non-null, please make sure that you call the returned
 * function from your handler to avoid negating the resource cleanup of the
 * Application Framework itself.
 *
 */
void emberAfMainInitCallback(void)
{
  emberAfCorePrintln("The Smart Plug is at your service");
  if (emberAfNetworkState() == EMBER_NO_NETWORK)
       {
          emberAfCorePrintln("The Smart Plug is NO_NETWORK");
          EmberStatus status = emberAfPluginNetworkSteeringStart();
          emberAfCorePrintln("%p network %p: 0x%X", "Join", "start", status);
       }
  emberSerialInit(COM_USART1, 115200, 0, 1);
  emberEventControlSetDelayMS(UsartEventControl,5);
}
