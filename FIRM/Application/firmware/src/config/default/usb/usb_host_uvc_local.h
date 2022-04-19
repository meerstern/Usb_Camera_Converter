 /*******************************************************************************
  USB Video class definitions

  Company:
    Microchip Technology Inc.

  File Name:
    usb_host_uvc_local.h

  Summary:
    USB Video class definitions

  Description:
    This file describes the Video class specific definitions.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *******************************************************************************/
// DOM-IGNORE-END

#ifndef _USB_HOST_UVC_LOCAL_H
#define _USB_HOST_UVC_LOCAL_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "usb/usb_host.h"
#include "usb/src/usb_host_local.h"
#include "usb/usb_host_client_driver.h"
#include "usb/usb_host_uvc.h"


// *****************************************************************************
/* USB Host Video Client Driver Stream States

  Summary:
    USB Host Video Client Driver Stream States

  Description:
    This enumeration lists all possible states USB HOST Video client driver
    states.  

  Remarks:
    None.
*/

typedef enum {
    /* Error state */
    USB_HOST_UVC_STREAM_STATE_ERROR = -1,

    /* The instance is not ready */
    USB_HOST_UVC_STREAM_STATE_NOT_READY = 0,

    /* The instance should set the configuration */
    USB_HOST_UVC_STREAM_STATE_SET_CONFIGURATION,

    /* Wait for configuration set */
    USB_HOST_UVC_STREAM_STATE_WAIT_FOR_CONFIGURATION_SET,

    /* Wait for interfaces to get ready */
    USB_HOST_UVC_STREAM_STATE_WAIT_FOR_INTERFACES,

    /* The instance is ready */
    USB_HOST_UVC_STREAM_STATE_READY,
            
    /* Pipe Open or Close is pending on this the stream */ 
    USB_HOST_UVC_STREAM_STATE_PIPE_ACTION_PENDING, 
    
    /* Pipe is opened on the stream. Data can be transferred on this stream now */ 
    USB_HOST_UVC_STREAM_STATE_PIPE_OPEN_SUCCESS, 
            
    // Pipe Open failed
    USB_HOST_UVC_STREAM_PIPE_OPEN_FAILED,         
            
    /*  Pipe is closed on the stream */        
    USB_HOST_UVC_STREAM_STATE_PIPE_CLOSE_SUCCESS,         
    
    /* Pipe Close failed */         
    USB_HOST_UVC_STREAM_PIPE_CLOSE_FAILED     
            
} USB_HOST_UVC_STREAM_STATE;

// *****************************************************************************
/* USB Host Video Client Driver API Version Flags

  Summary:
    USB Host Video Client Driver API Version Flags

  Description:
    This enumeration lists all possible values of USB HostVideo Client 
    Driver API Version Flags. Video Client driver supports multiple versions of 
    API. These flags are used to identify the API Version during a callback.  

  Remarks:
    None.
*/
typedef enum 
{
    USB_HOST_UVC_API_VERSION_FLAG_STREAM_INTERFACE_SET = 0, 
    USB_HOST_UVC_API_VERSION_FLAG_STREAM_DISABLE = 1, 
    USB_HOST_UVC_API_VERSION_FLAG_STREAM_ENABLE = 2,
    USB_HOST_UVC_API_VERSION_FLAG_V1 = 3, 
    USB_HOST_UVC_API_VERSION_FLAG_V1_DEPRECIATED = 4
} USB_HOST_UVC_API_VERSION_FLAGS; 





// *****************************************************************************
/*  USB Host Video Terminal descriptor header

  Summary:
    USB Host Video Terminal descriptor header

  Description:
    This structure has details about the the common members in the Video Input
    and Output Terminals. 

  Remarks:
   None.
*/
typedef struct __attribute__((packed)) {
    // Size of this descriptor
    uint8_t bLength;

    // Type of this descriptor
    uint8_t bDescriptorType;

    // Subtype of this descriptor
    uint8_t bDescriptorSubtype; 
    
    // Unique ID of the Terminal
    uint8_t bTerminalID; 
    
    // Characterizes the type of Terminal. See USB Video Terminal Types.
    uint8_t wTerminalType; 
    
    // Identifies the Terminal to which this Terminal is associated.
    uint8_t bAssocTerminal; 
} USB_HOST_UVC_TERMINAL_HEADER_DESCRIPTOR;










// *****************************************************************************
/*  USB Host Video Control descriptor header

  Summary:
    USB Host Video Control descriptor header

  Description:
    USB Host Video Control descriptor header

  Remarks:
   None.
*/
typedef struct __attribute__((packed))
{
    /* Size of this descriptor */
    uint8_t bLength;

    /* Type of this descriptor */
    uint8_t bDescriptorType;

    /* Subtype of this descriptor */
    uint8_t bDescriptorSubtype; 
    
    /* Entity ID */
    uint8_t entityID; 

} USB_HOST_UVC_CONTROL_ENTITY_DESCRIPTOR_HEADER;

// *****************************************************************************
/*  USB Host Video Probe and Commit Controls

  Summary:
    USB Host Video Probe and Commit Controls

  Description:
    This structure has details about the the common members in the Video 
    Controls. 

  Remarks:
   None.
*/
typedef struct __attribute__((packed)) 
{
    uint16_t bmHint;
    uint8_t bFormatIndex;
    uint8_t bFrameIndex;
    uint32_t dwFrameInterval;
    uint16_t wKeyFrameRate;
    uint16_t wPFrameRate;
    uint16_t wCompQuality;
    uint16_t wCompQualitySize;
    uint16_t wDelay;
    uint32_t dwMaxVideoFrameSize;
    uint32_t dwMaxPayloadTransferSize;

} USB_UVC_VIDEO_PROBE_AND_COMMIT_CONTROLS;










void _USB_HOST_UVC_Initialize(void *);
void _USB_HOST_UVC_Deinitialize(void);
void _USB_HOST_UVC_Reinitialize(void *);
void _USB_HOST_UVC_InterfaceAssign
(
    USB_HOST_DEVICE_INTERFACE_HANDLE *,
    USB_HOST_DEVICE_OBJ_HANDLE, size_t, 
    uint8_t *
);
void _USB_HOST_UVC_InterfaceRelease(USB_HOST_DEVICE_INTERFACE_HANDLE);
void _USB_HOST_UVC_InterfaceTasks(USB_HOST_DEVICE_INTERFACE_HANDLE);
USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE _USB_HOST_UVC_InterfaceEventHandler
(
    USB_HOST_DEVICE_INTERFACE_HANDLE, 
    USB_HOST_DEVICE_INTERFACE_EVENT,
    void *, 
    uintptr_t
);




#endif