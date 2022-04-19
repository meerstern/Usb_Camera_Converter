
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>            
#include "definitions.h"
#include "app.h"
#include "app_user.h"

#ifdef ENABLE_LCD
    #include "lcd/st7735.h"
#endif 

extern APP_DATA USB_ALIGN             appData;
extern struct st7735 lcd1;
extern uint8_t lcdStart;
extern uint8_t lcdEnd;
// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/
USB_HOST_EVENT_RESPONSE APP_USBHostEventHandler( USB_HOST_EVENT event, void * eventData, uintptr_t context)
{
    /* This function is called by the USB Host whenever a USB Host Layer event
     * has occurred. In this example we only handle the device unsupported event
     * */

    switch (event)
    {
        case USB_HOST_EVENT_DEVICE_UNSUPPORTED:
            /* The attached device is not supported for some reason */
            break;            
        default:
            break;                    
    }
    
    return(USB_HOST_EVENT_RESPONSE_NONE);
}

void APP_USBHostUVCAttachEventListener(USB_HOST_UVC_OBJ uvcObj, uintptr_t context)
{
    appData.deviceIsAttached = true;
    appData.uvcObj = uvcObj; 
}

void App_USBHostUVCAttachHandler(USB_HOST_UVC_OBJ videoObj, USB_HOST_UVC_EVENT event, uintptr_t context)
{

    switch(event)
    {
        case USB_HOST_UVC_EVENT_ATTACH:
            if(appData.deviceIsAttached == false)
            {
                appData.deviceIsAttached = true; 
                appData.uvcObj = videoObj; 
#ifdef ENABLE_LCD
                // set position X, Y
                ST7735_SetPosition (lcdStart - 5, 50);  
                // draw string
                ST7735_DrawString (&lcd1, "USB device attached", WHITE, X2);
#endif
            }
            else
            {
                // This application supports only one Video Device . Handle Error Here.
            }
            break; 
        case USB_HOST_UVC_EVENT_DETACH: 
            if(appData.deviceIsAttached == true)
            {
                /* This means the device was detached. There is no event data
                 * associated with this event.*/
                appData.state = APP_STATE_WAIT_FOR_DEVICE_ATTACH;
                APP_VideoDataSetDefault();
                CORETIMER_DelayMs(300);
                //RCON_SoftwareReset();
                break;
            }
            break; 
    }
}

USB_HOST_UVC_RESULT USB_HOST_UVC_AttachEventHandlerSet( USB_HOST_UVC_ATTACH_EVENT_HANDLER attachEventHandler, uintptr_t context) 
{
  
    // validate callback handler
    if(!attachEventHandler)
    {
        return USB_HOST_UVC_RESULT_INVALID_PARAMETER;
    }
  
    gUSBHostUVCCommonObj.attachEventHandler = attachEventHandler; 
    gUSBHostUVCCommonObj.context = context; 
    return USB_HOST_UVC_RESULT_SUCCESS;  
}

void App_USBUvcControlRequestCallback(USB_HOST_UVC_OBJ uvcObj, USB_HOST_UVC_REQUEST_HANDLE requestHandle, USB_HOST_UVC_RESULT result, size_t size, uintptr_t context)
{
    if(requestHandle == appData.requestHandle)
    {
        appData.isControlRequestCompleted = true; 
    }
}

bool APP_UvcFeatureUnitObjectGet(uint8_t inputTerminalID, USB_HOST_UVC_CONTROL_ENTITY_OBJ *obj)
{
    
    USB_HOST_UVC_RESULT videoResult;
    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObj, entityObjFeatureUnit; 
    USB_UVC_ENTITY_TYPE entityType;
    bool status = false; 

    videoResult = USB_HOST_UVC_EntityObjectGet(appData.uvcObj,
                      inputTerminalID, &entityObj);
    if(videoResult != USB_HOST_UVC_RESULT_SUCCESS)
    {
        appData.state = APP_STATE_ERROR; 
        return false; 
    }

    // Get the First Video Control Entity Object
    videoResult = USB_HOST_UVC_ControlEntityGetNext(appData.uvcObj, entityObj, &entityObjFeatureUnit);
    if(videoResult != USB_HOST_UVC_RESULT_SUCCESS)
    {
        appData.state = APP_STATE_ERROR;
        return false; 
    }

    // Get the Entity Type
    entityType = USB_HOST_UVC_EntityTypeGet(appData.uvcObj,entityObjFeatureUnit); 
    if(entityType == USB_UVC_FEATURE_UNIT)
    {
        *obj = entityObj; 
        status = true; 
    }

    return status; 
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

void APP_VideoDataSetDefault(void) 
{
  
  appData.isVideoInStreamFound = false; 
  appData.isVideoReadCompleted = false;
  appData.isStreamInterfaceSetZeroComplete = false;
  appData.isStreamInterfaceSetOneComplete = false;
  appData.requestHandle = USB_HOST_UVC_REQUEST_HANDLE_INVALID;
  appData.transferHandleVideoRead = USB_HOST_UVC_STREAM_TRANSFER_HANDLE_INVALID;

}

USB_HOST_UVC_STREAM_EVENT_RESPONSE APP_USBHostUVCStreamEventHandler( USB_HOST_UVC_STREAM_HANDLE streamHandle, USB_HOST_UVC_STREAM_EVENT event, void *eventData, uintptr_t context)
{
    USB_HOST_UVC_STREAM_EVENT_WRITE_COMPLETE_DATA *readCompleteEventData;

    RUN_LED_Clear();
    
    switch(event)
    {
      case USB_HOST_UVC_STREAM_EVENT_INTERFACE_SET_COMPLETE:
        appData.isStreamInterfaceSetComplete = true; 
        break;

      case USB_HOST_UVC_STREAM_EVENT_WRITE_COMPLETE:

        /* This means the Write request completed. We can
         * find out if the request was successful. */
        break;
      case USB_HOST_UVC_STREAM_EVENT_READ_COMPLETE:

        readCompleteEventData =
          (USB_HOST_UVC_STREAM_EVENT_READ_COMPLETE_DATA *)eventData;
        if(appData.transferHandleVideoRead == readCompleteEventData->transferHandle)
        {
            appData.wLenVideoRead=readCompleteEventData->length;
            appData.isVideoReadCompleted = true;
         
        }
        break;
      default:
        break;
    }

    return USB_HOST_UVC_STREAM_EVENT_RESPONSE_NONE; 
}


void CheckTrigger(bool *status)
{
    static bool sw_status_old=false;
    static bool trg_status_old=false;
    
    bool sw_status=PUSH_SW_Get();
    bool trg_status=TRG_IO_Get();

    if(sw_status==0 && sw_status_old==1)
    {
        *status=true;
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "SW Pushed!\r\n"); 
    }
    sw_status_old=sw_status;
    
    
    if(trg_status==0 && trg_status_old==1)
    {
        *status=true;
        SYS_DEBUG_PRINT(SYS_ERROR_WARNING, "IO Triggered!\r\n"); 
    }
    trg_status_old=trg_status;

}