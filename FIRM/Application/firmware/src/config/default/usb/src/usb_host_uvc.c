/*******************************************************************************
 USB HOST Video Class Driver

 Company:
    Microchip Technology Inc.

 File Name:
    usb_host_uvc.c

 Summary:
    USB Host Video class driver.

 Description:
    USB Host Video class driver.
 *******************************************************************************/

// DOM-IGNORE-BEGIN
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "usb/usb_host_client_driver.h"
#include "usb/usb_host.h"
#include "usb/src/usb_host_local.h"
#include "usb/usb_uvc.h"
#include "usb/usb_host_uvc.h"
#include "usb/usb_host_uvc_local.h"

// *****************************************************************************
// *****************************************************************************
// Section: File Scope or Global Data Types
// *****************************************************************************
// *****************************************************************************
extern APP_DATA appData;
static uint32_t getMaxInterval(uint8_t selectedIndex);
int     _USB_HOST_UVC_InterfaceHandleToVideoInstance(   USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle);
bool    _USB_HOST_UVC_IntrfcHndlToStrmIntrfcPtr(
        USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle, 
        USB_HOST_UVC_STREAMING_INTERFACE** , 
        uint8_t *, uint8_t *);
// *****************************************************************************
/* Video Host Instance structure

 Summary:
    Defines the Video instance(s).

 Description:
    This data type defines the Video instance(s). The number of instances is
    defined by the application using USB_HOST_UVC_INSTANCES_NUMBER.

 Remarks:
 This structure is private to the Video.
 */
USB_HOST_UVC_INSTANCE gUSBHostVideoInstance[USB_HOST_UVC_INSTANCES_NUMBER];

// *****************************************************************************
/* Video Host Common Object 

 Summary:
    Defines the Video Host common object which is common to all instances of 
    Video Host. 

 Description:
    Defines the Video Host common object which is common to all instances of 
    Video Host. 

 Remarks:
    This structure is private to the Video Host Client driver.
 */
USB_HOST_UVC_COMMON_OBJ                 gUSBHostUVCCommonObj; 
USB_UVC_VIDEO_PROBE_AND_COMMIT_CONTROLS Parameters;
// *****************************************************************************
/* Video Host driver Interface structure

 Summary:
 Defines the Video Host driver structure. 

 Description:
 Defines the Video Host driver structure which is used by Host layer to
 communicate with the Video client driver. This structure should be 
 passed as the parameter in the TPL. 

 Remarks:
 None. 
 */
const USB_HOST_CLIENT_DRIVER gUSBHostUVCDriver = 
{
    .initialize             = _USB_HOST_UVC_Initialize,
    .deinitialize           = _USB_HOST_UVC_Deinitialize,
    .reinitialize           = _USB_HOST_UVC_Reinitialize,
    .interfaceAssign        = _USB_HOST_UVC_InterfaceAssign,
    .interfaceRelease       = _USB_HOST_UVC_InterfaceRelease,
    .interfaceEventHandler  = _USB_HOST_UVC_InterfaceEventHandler,
    .interfaceTasks         = _USB_HOST_UVC_InterfaceTasks,
    .deviceEventHandler     = NULL,
    .deviceAssign           = NULL,
    .deviceEventHandler     = NULL,
    .deviceRelease          = NULL
  };

// *****************************************************************************
// *****************************************************************************
// USB Host Video Local Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    void USB_HOST_UVC_Initialize(void *init)

  Summary:
    This function is called when the Host Layer is initializing.

  Description:
    This function is called when the Host Layer is initializing.

  Remarks:
    This is a local function and should not be called directly by the
    application.
*/
void _USB_HOST_UVC_Initialize(void *init)
{
 
}

void _USB_HOST_UVC_Deinitialize(void)
{
 
}

void _USB_HOST_UVC_Reinitialize(void * init)
{
 
}

// *****************************************************************************
/* Function:
 void _USB_HOST_UVC_ControlInterfaceInitialize
      (USB_HOST_UVC_INSTANCE * videoInstance)

 Summary:
    This function initializes Video Control Interface and Opens Control Pipe. 

 Description:
    This function initializes Video Control Interface and Opens Control Pipe.

 Remarks:
    This is a local function and should not be called directly by the
    application.
*/
bool _USB_HOST_UVC_ControlInterfaceInitialize(USB_HOST_UVC_INSTANCE *videoInstance)
{
    uint8_t *descriptor;
    USB_UVC_VC_INTERFACE_DESCRIPTOR *videoControlDesc;
    USB_UVC_INPUT_TERMINAL_DESCRIPTOR *videoInputTerminalDesc;
    USB_UVC_OUTPUT_TERMINAL_DESCRIPTOR *videoOutputTerminalDesc __attribute__((unused));
    USB_UVC_PROCESSING_UNIT_DESCRIPTOR *videoProcessingUnitDesc;
    int strmIndex; 
    bool error = true; 
    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle = videoInstance->deviceObjHandle; 
 
       
    /* Open a control pipe to the device. */
    videoInstance->controlPipeHandle = USB_HOST_DeviceControlPipeOpen(deviceObjHandle);
   
    if(videoInstance->controlPipeHandle != USB_HOST_CONTROL_PIPE_HANDLE_INVALID)
    {
        /* Control Pipe Opened successfully */ 
        error = false; 
  
        /* Get pointer to Video Class Specific Descriptor */
        descriptor = videoInstance->pVideoControlDescriptor; 
        videoControlDesc = (USB_UVC_VC_INTERFACE_DESCRIPTOR *)(descriptor+((USB_INTERFACE_DESCRIPTOR *)descriptor)->bLength);

        /* Get Number of Video Streaming interfaces */
        videoInstance->nVSInterfaces = videoControlDesc->bInCollection;    

        videoInputTerminalDesc = (USB_UVC_INPUT_TERMINAL_DESCRIPTOR *)((uint8_t*)videoControlDesc + videoControlDesc->bLength);
        videoProcessingUnitDesc = (USB_UVC_PROCESSING_UNIT_DESCRIPTOR *)((uint8_t*)videoInputTerminalDesc + videoInputTerminalDesc->bLength);
        videoOutputTerminalDesc = (USB_UVC_OUTPUT_TERMINAL_DESCRIPTOR *)((uint8_t*)videoProcessingUnitDesc + videoProcessingUnitDesc->bLength);

        
        /* Reset Video Streaming Interfaces Counter. This variable will be 
           incremented when a Video Streaming interface is received. */
        videoInstance->countVSInterfaces = 0; 

        /* Check if the Attached Video Device exposes more streaming 
           interfaces than user defined */  
        if(videoInstance->nVSInterfaces > USB_HOST_UVC_STREAMING_INTERFACES_NUMBER)
        {
            videoInstance->nVSInterfaces = USB_HOST_UVC_STREAMING_INTERFACES_NUMBER; 
            SYS_DEBUG_MESSAGE ( SYS_DEBUG_INFO ,
                "USB_HOST_UVC_InterfaceAssign():Attached Video Device has more streaming interfaces than user expected" );
        }
 
        for(strmIndex=0; strmIndex< videoInstance->nVSInterfaces; strmIndex++)
        {
            /* Save Video Streaming Interface Number */
            videoInstance->streamInf[strmIndex].interfaceId = *((uint8_t *)&videoControlDesc->bInCollection + strmIndex + 1);
            /* Set Alternate setting to Zero */
            videoInstance->streamInf[strmIndex].activeInterfaceSetting = 0;
        } 
    }
    else
    {
        /* Control pipe could not be opened */
        error = true;
    }
 
    return error; 
}


// *****************************************************************************
/* Function:
 void _USB_HOST_UVC_StreamingInterfaceInitialize
      (USB_HOST_UVC_INSTANCE * videoInstance)

 Summary:
    This function initializes Video Streaming Interface and Opens Control Pipe. 

 Description:
    This function initializes Video Streaming Interface and Opens Control Pipe.

 Remarks:
    This is a local function and should not be called directly by the
    application.
*/
bool _USB_HOST_UVC_StreamingInterfaceInitialize
(
    USB_HOST_UVC_INSTANCE *videoInstance, 
    USB_INTERFACE_DESCRIPTOR *descriptor
)
{
    USB_HOST_ENDPOINT_DESCRIPTOR_QUERY endpointQuery;
    USB_HOST_INTERFACE_DESCRIPTOR_QUERY interfaceQuery;
    USB_INTERFACE_DESCRIPTOR *interfaceDescriptor; 
    USB_HOST_UVC_STREAM_SETTING *videoStream;
    USB_UVC_VS_INPUT_HEADER_DESCRIPTOR *videoVsDescriptor;
    USB_UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR *videoFormatDesc;
    USB_UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR *videoFrameDesc;
    USB_UVC_VS_STILL_IMAGE_FRAME_DESCRIPTOR *stillImageDesc;
    USB_UVC_VS_COLOR_FORMAT_DESCRIPTOR *colorFormatDesc __attribute__((unused));
    USB_ENDPOINT_DESCRIPTOR *endpointDescriptor;
    bool error = true;
    
    USB_UVC_VS_INPUT_HEADER_DESCRIPTOR * videoInterfaceHeaderDescriptor;
 
    //SYS_DEBUG_PRINT ( SYS_DEBUG_INFO,"StreamInterface Init: %u\r\n",videoInstance->countVSInterfaces);
        
    /* Reset number of Interface Settings */
    videoInstance->streamInf[videoInstance->countVSInterfaces].nInterfaceSetting = 0; 

    /* Save Interface Id */
    videoInstance->streamInf[videoInstance->countVSInterfaces].interfaceId = descriptor->bInterfaceNumber; 
 
    /* Fill in Interface Query structure */  
    interfaceQuery.bInterfaceNumber = descriptor->bInterfaceNumber; 
    interfaceQuery.bInterfaceClass = USB_UVC_CLASS_CODE; 
    interfaceQuery.bInterfaceSubClass = USB_UVC_SUBCLASS_VIDEOSTREAMING;
    interfaceQuery.flags =    USB_HOST_INTERFACE_QUERY_BY_NUMBER | 
                              USB_HOST_INTERFACE_QUERY_BY_CLASS | 
                              USB_HOST_INTERFACE_QUERY_BY_SUBCLASS |
                              USB_HOST_INTERFACE_QUERY_ALT_SETTING; 
 
    /* Start searching for Alternate Setting 0  */  
    interfaceQuery.bAlternateSetting = 0;
    
    do
    {
        USB_HOST_DeviceInterfaceQueryContextClear(&interfaceQuery); 
        interfaceDescriptor = USB_HOST_DeviceGeneralInterfaceDescriptorQuery(descriptor, &interfaceQuery); 
        
        if(interfaceDescriptor != NULL)
        {
            /* Get pointer to the current Video Stream */
            videoStream = (USB_HOST_UVC_STREAM_SETTING *)&videoInstance->streamInf[videoInstance->countVSInterfaces].videoStreamSetting[interfaceQuery.bAlternateSetting];
            
            /* We found a new Alternate setting. Increment number of Alternate settings */
            videoInstance->streamInf[videoInstance->countVSInterfaces].nInterfaceSetting++;
            
            /* Save alternate setting number */
            videoStream->interfaceAlternateSetting = interfaceQuery.bAlternateSetting;
            
            /* Save Number of endpoints */
            videoStream->nEndpoints = interfaceDescriptor->bNumEndPoints;             
            
            if(((USB_UVC_VS_INPUT_HEADER_DESCRIPTOR *)((uint8_t *)interfaceDescriptor + interfaceDescriptor->bLength))->bDescriptorType == USB_VIDEO_CLASS_SPECIFIC_DESCRIPTOR)
            {
                /* Get AS Class Specific Descriptor */
                videoVsDescriptor = (USB_UVC_VS_INPUT_HEADER_DESCRIPTOR *)((uint8_t *)interfaceDescriptor + interfaceDescriptor->bLength);

                /* Save ID of the terminal this AS interface is connected to */
                videoStream->bTerminalLink = videoVsDescriptor->bTerminalLink;

                while(videoVsDescriptor->bNumFormats--)
                {
        
                    /* Get to Video Format Descriptor */
                    videoFormatDesc = (USB_UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR *)((uint8_t*)videoVsDescriptor + videoVsDescriptor->bLength);
                    videoStream->bBitsPerPixel[0]=videoFormatDesc->bBitsPerPixel;    
                    videoFrameDesc = (USB_UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR *)((uint8_t*)videoFormatDesc + videoFormatDesc->bLength);
                                
                    do
                    {
                        altFrame:
 
                        /* Save Video Format Details */
                        videoStream->bSubframeSize[0] = videoFrameDesc->dwMaxVideoFrameBufferSize; 
                        videoStream->wWidth[0]=videoFrameDesc->wWidth[0];
                        videoStream->wHeight[0]=videoFrameDesc->wHeight[0];

                        videoFrameDesc = (USB_UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR *)((uint8_t*)videoFrameDesc + videoFrameDesc->bLength);
                    }
                    while(((int8_t)--videoFormatDesc->bNumFrameDescriptors)>0);

                    /* Get to Still Image Descriptor */
                    stillImageDesc = (USB_UVC_VS_STILL_IMAGE_FRAME_DESCRIPTOR *)videoFrameDesc;
                    if(stillImageDesc->bDescriptorSubtype==USB_UVC_VS_FRAME_UNCOMPRESSED)
                    {
                        videoFormatDesc->bNumFrameDescriptors=1;     
                        goto altFrame;
                    }
     
                    if(stillImageDesc->bDescriptorSubtype==USB_UVC_VS_STILL_IMAGE_FRAME)
                    {
                        colorFormatDesc = (USB_UVC_VS_COLOR_FORMAT_DESCRIPTOR *)((uint8_t*)stillImageDesc + stillImageDesc->bLength);
                       
                    }
                    else
                    {
                        /* Get to Color Format Descriptor*/
                        colorFormatDesc = (USB_UVC_VS_COLOR_FORMAT_DESCRIPTOR *)videoFrameDesc;
                    }
                    
                } // bNumFormats
            }
            
            if(videoStream->nEndpoints == 1)
            {
                /* Clear Endpoint Query Context */
                USB_HOST_DeviceEndpointQueryContextClear(&endpointQuery);
                /* Fill endpoint Query */
                endpointQuery.direction = USB_DATA_DIRECTION_DEVICE_TO_HOST; 
                endpointQuery.transferType = USB_TRANSFER_TYPE_ISOCHRONOUS; 
                endpointQuery.flags = USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE | USB_HOST_ENDPOINT_QUERY_BY_DIRECTION; 
                /* Submit Endpoint query */
                endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointQuery);
       
                if(endpointDescriptor != NULL)
                {   
                    /* We have received a valid descriptor. Copy that to our structure */
                    memcpy(&videoStream->isoDataEndpointDesc, endpointDescriptor, sizeof(USB_ENDPOINT_DESCRIPTOR)); 
                    
                    /* Save stream direction */
                    videoStream->direction = USB_HOST_UVC_DIRECTION_IN;
                    
                    /* Change Video Stream state to ready */
                    videoInstance->streamInf[videoInstance->countVSInterfaces].state = USB_HOST_UVC_STREAM_STATE_READY;
                           
                    appData.packetSize = videoStream->isoDataEndpointDesc.packetSize;
                    error = false; 
                    
                    SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\nIF:%X ALT:%X ID:%d\r\n",
                        videoInstance->countVSInterfaces,appData.packetSize,interfaceQuery.bAlternateSetting);
                    
                }
                else
                {
                    USB_HOST_DeviceEndpointQueryContextClear(&endpointQuery);
                    endpointQuery.direction = USB_DATA_DIRECTION_DEVICE_TO_HOST; 
                    endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointQuery); 

                    if(endpointDescriptor != NULL)
                    {
                        /* We have received a valid descriptor. Copy that to our structure */
                        memcpy(&videoStream->isoDataEndpointDesc, endpointDescriptor, sizeof(USB_ENDPOINT_DESCRIPTOR)); 
                        
                        /* Save stream direction */
                        videoStream->direction = USB_HOST_UVC_DIRECTION_IN;
                        
                        /* Change Video Stream state to ready */
                        videoInstance->streamInf[videoInstance->countVSInterfaces].state = USB_HOST_UVC_STREAM_STATE_READY; 

                        appData.packetSize=videoStream->isoDataEndpointDesc.packetSize;
                        error = false;
                        
                        SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\nSetIF:%X ALT:%X %d\r\n",
                        videoInstance->countVSInterfaces,appData.packetSize,interfaceQuery.bAlternateSetting);
                        
                    }
                    else 
                    {
                        /* Error has occurred */
                        error = true; 
                        videoInstance->streamInf[videoInstance->countVSInterfaces].state = USB_HOST_UVC_STREAM_STATE_ERROR; 
                    }
                }
            }
        } 
        /* We need to query for next alternate setting */
        interfaceQuery.bAlternateSetting++; 
    }
    while(interfaceDescriptor);

    /*** Add Setting Data for comitts ***/
   
    /* Fill in Interface Query structure */  
    interfaceQuery.bInterfaceNumber = descriptor->bInterfaceNumber; 
    interfaceQuery.bInterfaceClass = USB_UVC_CLASS_CODE; 
    interfaceQuery.bInterfaceSubClass = USB_UVC_SUBCLASS_VIDEOSTREAMING; 
    interfaceQuery.flags =   USB_HOST_INTERFACE_QUERY_BY_NUMBER 
                           | USB_HOST_INTERFACE_QUERY_BY_CLASS
                           | USB_HOST_INTERFACE_QUERY_BY_SUBCLASS 
                           | USB_HOST_INTERFACE_QUERY_ALT_SETTING; 
    
    /* Start searching for Alternate Setting 0  */  
    interfaceQuery.bAlternateSetting = 0; 
    
    do
    {
        USB_HOST_DeviceInterfaceQueryContextClear(&interfaceQuery); 
        interfaceDescriptor = USB_HOST_DeviceGeneralInterfaceDescriptorQuery(descriptor, &interfaceQuery); 
        
        if (interfaceDescriptor != NULL)
        {
            SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\nIF Type:%X, Num:%X, Alt:%X",
                    interfaceDescriptor->bDescriptorType,
                    interfaceDescriptor->bInterfaceNumber,
                    interfaceDescriptor->bAlternateSetting);  
            
            if(interfaceDescriptor->bNumEndPoints != 0)
            {
                USB_HOST_DeviceEndpointQueryContextClear(&endpointQuery);
                /* Fill endpoint Query */
                endpointQuery.direction = USB_DATA_DIRECTION_DEVICE_TO_HOST; 
                endpointQuery.transferType = USB_TRANSFER_TYPE_ISOCHRONOUS; 
                endpointQuery.flags = USB_HOST_ENDPOINT_QUERY_BY_TRANSFER_TYPE | USB_HOST_ENDPOINT_QUERY_BY_DIRECTION; 

                /* Submit Endpoint query */
                endpointDescriptor = USB_HOST_DeviceEndpointDescriptorQuery(interfaceDescriptor, &endpointQuery); 
                if (endpointDescriptor != NULL)
                {
                    SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\t EP Addr:%X, Size:%X",
                        endpointDescriptor->bEndpointAddress,
                        endpointDescriptor->wMaxPacketSize);
                    
                    if(uvcAlternateCount<sizeof(uvcAlternalte)/sizeof(uvcAlternalte[0]))
                    {
                        uvcAlternalte[uvcAlternateCount].index=interfaceDescriptor->bAlternateSetting;
                        uvcAlternalte[uvcAlternateCount].size=endpointDescriptor->wMaxPacketSize;
                        uvcAlternateCount++;
                    } 
                }
            }
        } 
        /* We need to query for next alternate setting */
        interfaceQuery.bAlternateSetting++; 
    }
    while (interfaceDescriptor != NULL);
       
    /* Fill in Interface Query structure */  
    interfaceQuery.bInterfaceNumber = descriptor->bInterfaceNumber; 
    interfaceQuery.bInterfaceClass = USB_UVC_CLASS_CODE; 
    interfaceQuery.bInterfaceSubClass = USB_UVC_SUBCLASS_VIDEOSTREAMING; 
    interfaceQuery.flags =   USB_HOST_INTERFACE_QUERY_BY_NUMBER 
                           | USB_HOST_INTERFACE_QUERY_BY_CLASS
                           | USB_HOST_INTERFACE_QUERY_BY_SUBCLASS; 

    do
    {
        USB_HOST_DeviceInterfaceQueryContextClear(&interfaceQuery); 
        interfaceDescriptor = USB_HOST_DeviceGeneralInterfaceDescriptorQuery(descriptor, &interfaceQuery); 
        if (interfaceDescriptor != NULL)
        {
            SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\nIF Type:%X, Num:%X, Alt:%X",
                    interfaceDescriptor->bDescriptorType,
                    interfaceDescriptor->bInterfaceNumber,
                    interfaceDescriptor->bAlternateSetting); 
            
         
            videoInterfaceHeaderDescriptor = (USB_UVC_VS_INPUT_HEADER_DESCRIPTOR*)((uint8_t*)interfaceDescriptor + interfaceDescriptor->bLength  );

            uint8_t * desc=(uint8_t*)videoInterfaceHeaderDescriptor;
            uint16_t offset=0;
            uint8_t len=0;
            uint8_t type=0;
            uint8_t subtype=0;
            uint8_t format=0;
            uint8_t formatidx=0;
            
            do
            {
                len = *(desc + offset);
                type= *(desc + offset +1);
                subtype = *(desc + offset+2);
                
                if(type == USB_UVC_VIDEO_STREAMING_INTERFACE)
                {
                   if(subtype == USB_UVC_UNCOMPRESSED_FORMAT_TYPE)
                   {
                       USB_UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR * ucf_desc =(USB_UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR*)(desc+offset);
                       SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\nUncompressed Format Type:%X, Index:%X",
                            ucf_desc->bDescriptorSubtype,
                            ucf_desc->bFormatIndex);
                       format = ucf_desc->bDescriptorSubtype;
                       formatidx = ucf_desc->bFormatIndex;
                   }
                   else if(subtype == USB_UVC_UNCOMPRESSED_FRAME_TYPE)
                   {
                        USB_UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR * ucf_desc =(USB_UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR*)(desc+offset);

                        uint16_t w=ucf_desc->wWidth[0]+256*ucf_desc->wWidth[1];
                        uint16_t h=ucf_desc->wHeight[0]+256*ucf_desc->wHeight[1];

                        SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\nUncompressed Frame Type:%d, Index:%d, Width:%d, Height:%d",
                            ucf_desc->bDescriptorSubtype,
                            ucf_desc->bFrameIndex,
                            w, h); 
                       
                        if(uvcSettingsCount<sizeof(uvcSettings)/sizeof(uvcSettings[0]))
                        {
                            int m=0;
                            for( m=0; m< ucf_desc->bFrameIntervalType;m++)
                            {
                                uvcSettings[uvcSettingsCount].interval[m]=ucf_desc->dwFrameIntervalType[m];
                            }
                            uvcSettings[uvcSettingsCount].intervalCount=ucf_desc->bFrameIntervalType;
                            uvcSettings[uvcSettingsCount].format=format;
                            uvcSettings[uvcSettingsCount].formatidx=formatidx;
                            uvcSettings[uvcSettingsCount].frame=ucf_desc->bFrameIndex;
                            uvcSettings[uvcSettingsCount].height=h;
                            uvcSettings[uvcSettingsCount].width=w;
                            uvcSettingsCount++;
                        }   

                   }
                   else if(subtype == USB_UVC_MJPEG_FORMAT_TYPE)
                   {
                       USB_VIDEO_VS_MPEG_FORMAT_TYPE_DESCRIPTOR * ucf_desc =(USB_VIDEO_VS_MPEG_FORMAT_TYPE_DESCRIPTOR*)(desc+offset);
                       SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\nMJPEG Format Type:%X, Index:%X",
                       ucf_desc->bDescriptorSubtype,
                       ucf_desc->bFormatIndex);  
                       
                       format = ucf_desc->bDescriptorSubtype;
                       formatidx = ucf_desc->bFormatIndex;
                   }
                   else if(subtype == USB_UVC_MJPEG_FRAME_TYPE)
                   {
                        USB_VIDEO_VS_MPEG_FRAME_TYPE_DESCRIPTOR * ucf_desc =(USB_VIDEO_VS_MPEG_FRAME_TYPE_DESCRIPTOR*)(desc+offset);
                       
                        uint16_t w=ucf_desc->wWidth[0]+256*ucf_desc->wWidth[1];
                        uint16_t h=ucf_desc->wHeight[0]+256*ucf_desc->wHeight[1];
                        SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\nMJPEG Frame Type:(%d)%d, Index:%d, Width:%d, Height:%d",
                            format,
                            ucf_desc->bDescriptorSubtype,
                            ucf_desc->bFrameIndex,
                            w, h);                        
                       
                        if(uvcSettingsCount<sizeof(uvcSettings)/sizeof(uvcSettings[0]))
                        {
                            int m=0;
                            for( m=0; m< ucf_desc->bFrameIntervalType;m++)
                            {
                                uvcSettings[uvcSettingsCount].interval[m]=ucf_desc->dwFrameIntervalType[m];
                            }
                            uvcSettings[uvcSettingsCount].intervalCount=ucf_desc->bFrameIntervalType;
                            uvcSettings[uvcSettingsCount].format=format;
                            uvcSettings[uvcSettingsCount].formatidx=formatidx;
                            uvcSettings[uvcSettingsCount].frame=ucf_desc->bFrameIndex;
                            uvcSettings[uvcSettingsCount].height=h;
                            uvcSettings[uvcSettingsCount].width=w;
                            uvcSettingsCount++;
                        }  
                        
                        
                       
                   }          
                }             
                
                offset=offset+len;
            }
            while(type==USB_UVC_VIDEO_STREAMING_INTERFACE); 
        } 
        /* We need to query for next alternate setting */
        break;
    }
    while (interfaceDescriptor != NULL);
    
     /*** Add Setting Data for comitts End ***/
    
    /* Increment Video Streaming Interface Counter */ 
    videoInstance->countVSInterfaces++; 
    return error; 
}

/*************************************************************************/
/* Function:
 void USB_HOST_UVC_InterfaceAssign
 (
    USB_HOST_DEVICE_INTERFACE_HANDLE * interfaces,
    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
    size_t nInterfaces,
    uint8_t * descriptor
  )

 Summary:
    Initializes Video Instance. 

 Description:
    This function is called by USB Host whenever an Video device is attached
    to the USB Bus. 

 Parameters:
    interfaces     
    deviceObjHandle
    nInterfaces
    descriptor

 Returns:
    None
 
 Remarks:
    This is a local function and should be called directly by clients. 
*/
void _USB_HOST_UVC_InterfaceAssign
(
    USB_HOST_DEVICE_INTERFACE_HANDLE *interfaces,
    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
    size_t nInterfaces, 
    uint8_t *descriptor
)
{
    int driverIndex, iterator;
    USB_INTERFACE_DESCRIPTOR * interfaceDescriptor;
    USB_HOST_INTERFACE_DESCRIPTOR_QUERY interfaceQuery;
    USB_HOST_UVC_INSTANCE * videoInstanceInfo = NULL;
    bool error = true;
        
    /* This function is being called because an USB Video device was attached 
       and the driver has matched.*/ 
    interfaceDescriptor = (USB_INTERFACE_DESCRIPTOR*)descriptor; 
        
    if(nInterfaces == 1)
    { 
        /* If we have a valid interface descriptor find out its type */
        if(interfaceDescriptor != NULL)
        {
      
            if((interfaceDescriptor->bInterfaceClass == USB_UVC_CLASS_CODE)
                && (interfaceDescriptor->bInterfaceSubClass == USB_UVC_SUBCLASS_VIDEOCONTROL)
                && (interfaceDescriptor->bInterfaceProtocol == USB_UVC_PR_PROTOCOL_UNDEFINED))
            {
                /* Check if there is a free Video object.*/
                for(driverIndex = 0; driverIndex < USB_HOST_UVC_INSTANCES_NUMBER; driverIndex++)
                {
                    if(!gUSBHostVideoInstance[driverIndex].assigned)
                    {
                        /* Found a free instance */
                        gUSBHostVideoInstance[driverIndex].assigned = true;
                        videoInstanceInfo = &gUSBHostVideoInstance[driverIndex];
                        videoInstanceInfo->index = driverIndex; 
                        break;
                    }
                }
                if(videoInstanceInfo != NULL)
                {
                    /* Save the Device handle */
                    videoInstanceInfo->deviceObjHandle = deviceObjHandle;
                    /* Save the Control interface handle */ 
                    videoInstanceInfo->acInterfaceHandle = interfaces[0];
                    /* Save the Video Control descriptor pointer */
                    videoInstanceInfo->pVideoControlDescriptor = descriptor; 
                    /* Initialize Video Control Interface */
                    error = _USB_HOST_UVC_ControlInterfaceInitialize(videoInstanceInfo); 
                    /* Save the pointer to Video Instance for further processing */
                    gUSBHostUVCCommonObj.prevVideoInstance = videoInstanceInfo; 
                }
            }
            else if((interfaceDescriptor->bInterfaceClass == USB_UVC_CLASS_CODE) &&
                    (interfaceDescriptor->bInterfaceSubClass == USB_UVC_SUBCLASS_VIDEOSTREAMING) &&
                    (interfaceDescriptor->bInterfaceProtocol == USB_UVC_PR_PROTOCOL_UNDEFINED)) 
            {
                /* Get pointer to Video Instance */
                videoInstanceInfo = gUSBHostUVCCommonObj.prevVideoInstance;
   
                if((videoInstanceInfo != NULL) 
                        && (videoInstanceInfo->countVSInterfaces < USB_HOST_UVC_STREAMING_INTERFACES_NUMBER))
                { 
                    /* Save Streaming Interface Handle */
                    videoInstanceInfo->streamInf[videoInstanceInfo->countVSInterfaces].vsInterfaceHandle = interfaces[0];
                    error = _USB_HOST_UVC_StreamingInterfaceInitialize(videoInstanceInfo, (USB_INTERFACE_DESCRIPTOR *)descriptor);
                }
            }
        }
    }
    else if(nInterfaces > 1)
    {
        /* This means that this is an IAD. We first assign an Video instance
         * to this device */
        for(driverIndex=0; driverIndex < USB_HOST_UVC_INSTANCES_NUMBER; driverIndex++)
        {
            if(!gUSBHostVideoInstance[driverIndex].assigned)
            {
                /* Found a free instance */
                gUSBHostVideoInstance[driverIndex].assigned = true;
                videoInstanceInfo = &gUSBHostVideoInstance[driverIndex];
                videoInstanceInfo->index = driverIndex; 
                break;
            }
        }
        
        if(videoInstanceInfo != NULL)
        {
            /* Save the Device Object handle */
            videoInstanceInfo->deviceObjHandle = deviceObjHandle;
  
            /* An instance is assigned. The descriptor will be a pointer to the
             * IAD. Lets get the first interface descriptor in the IAD group and
             * see which interface this is. */ 
            USB_HOST_DeviceInterfaceQueryContextClear(&interfaceQuery);
            interfaceQuery.flags = USB_HOST_INTERFACE_QUERY_BY_NUMBER;
            interfaceQuery.bInterfaceNumber = ((USB_INTERFACE_ASSOCIATION_DESCRIPTOR *)descriptor)->bFirstInterface; 
  
            /* We know that we need two interfaces */
            for(iterator=0; iterator < nInterfaces; iterator++)
            {    
                /* We need to search for Video interface descriptors */
                interfaceDescriptor = USB_HOST_DeviceGeneralInterfaceDescriptorQuery(
                    (USB_INTERFACE_ASSOCIATION_DESCRIPTOR *)descriptor, &interfaceQuery);
    
                /* If we have a valid interface descriptor find out its type */
                if(interfaceDescriptor != NULL)
                {
                    if((interfaceDescriptor->bInterfaceClass == USB_UVC_CLASS_CODE) &&
                       (interfaceDescriptor->bInterfaceSubClass == USB_UVC_SUBCLASS_VIDEOCONTROL) &&
                       (interfaceDescriptor->bInterfaceProtocol == USB_UVC_PR_PROTOCOL_UNDEFINED))
                    {            
                        /* Save the Device handle */
                        videoInstanceInfo->deviceObjHandle = deviceObjHandle;
                        /* Save the Control interface handle */ 
                        videoInstanceInfo->acInterfaceHandle = interfaces[0];
                        /* Save the Video Control descriptor pointer */
                        videoInstanceInfo->pVideoControlDescriptor = (uint8_t *)interfaceDescriptor;
                        /* Initialize Video Control Interface */
                        error = _USB_HOST_UVC_ControlInterfaceInitialize(videoInstanceInfo);
                        //SYS_DEBUG_PRINT ( SYS_DEBUG_INFO," FOUND Interface control: %u\r\n",error);
                    }
                    else if((interfaceDescriptor->bInterfaceClass == USB_UVC_CLASS_CODE) &&
                            (interfaceDescriptor->bInterfaceSubClass == USB_UVC_SUBCLASS_VIDEOSTREAMING) &&
                            (interfaceDescriptor->bInterfaceProtocol == USB_UVC_PR_PROTOCOL_UNDEFINED)) 
                    {
                        if(videoInstanceInfo->countVSInterfaces < USB_HOST_UVC_STREAMING_INTERFACES_NUMBER)
                        {
                            /* Save Streaming Interface Handle */
                            videoInstanceInfo->streamInf[videoInstanceInfo->countVSInterfaces].vsInterfaceHandle = interfaces[videoInstanceInfo->countVSInterfaces + 1];
                            error = _USB_HOST_UVC_StreamingInterfaceInitialize(videoInstanceInfo, interfaceDescriptor);              
         
                        }
                        else
                        {
                            /* This means user has not allocated enough data 
                             * memory for this interface. Release this interface */
                            error = false; 
                            USB_HOST_DeviceInterfaceRelease(interfaces[iterator]);
                        }
                    }
                } 
                interfaceQuery.bInterfaceNumber++; 
            }   
        }
    }
    else
    {
        error = true; 
    } 
 
    /* Sent attach event to the application */
    if((gUSBHostUVCCommonObj.attachEventHandler != NULL) 
            && (videoInstanceInfo != NULL) 
            && (error == false))
    {
        /* We must receive all streaming interfaces before sending the 
           attach event to the application */
        if(videoInstanceInfo->countVSInterfaces == videoInstanceInfo->nVSInterfaces)
        {
            gUSBHostUVCCommonObj.attachEventHandler
            (
                (USB_HOST_UVC_OBJ)videoInstanceInfo, 
                USB_HOST_UVC_EVENT_ATTACH, 
                gUSBHostUVCCommonObj.context
            );
        }
    }
    else
    {
        /* Let the host know that this interface cannot be processed */
        SYS_DEBUG_MESSAGE(SYS_ERROR_INFO, "Could not find Video object");
        /* Release all the interfaces in this group */
        for(iterator=0; iterator < nInterfaces; iterator++)
        {
            USB_HOST_DeviceInterfaceRelease(interfaces[iterator]);
        }
    }
    return;
  }
 
// *****************************************************************************
/* Function:
 USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE _USB_HOST_UVC_InterfaceEventHandler
  (
    USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle,
    USB_HOST_DEVICE_INTERFACE_EVENT event,
    void * eventData,
    uintptr_t context
  )

 Summary:
    This function is called when the Host Layer generates interface level
    events. 

 Description:
    This function is called when the Host Layer generates interface level
    events. 

 Remarks:
    This is a local function and should not be called directly by the
    application.
*/ 
USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE _USB_HOST_UVC_InterfaceEventHandler
(
    USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle,
    USB_HOST_DEVICE_INTERFACE_EVENT event, 
    void *eventData, 
    uintptr_t context
)
{
    USB_HOST_UVC_STREAM_SETTING *videoStrm;
    USB_HOST_UVC_STREAMING_INTERFACE *vsInterface;
    USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE_DATA *dataTransferEvent;
    USB_HOST_DEVICE_INTERFACE_EVENT_SET_INTERFACE_COMPLETE_DATA *DataIntrfcSetEvent;
    USB_HOST_UVC_STREAM_EVENT videoStreamEvent;
    USB_HOST_UVC_STREAM_EVENT_WRITE_COMPLETE_DATA videoStrmXfrCmpltData;
    uint8_t alternateSetting; 
    USB_HOST_UVC_STREAM_HANDLE strmHandle; 
    uint8_t videoIndex, asIntrfcIndex; 
    uint8_t versionFlag; 

    /* Find out to which Video Stream this interface belongs */
    if(_USB_HOST_UVC_IntrfcHndlToStrmIntrfcPtr
            (
                interfaceHandle, 
                &vsInterface,
                &videoIndex, 
                &asIntrfcIndex
            ) == false)
    {
        /* A matching interface could not be found. This is an error condition */
        return USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE_NONE; 
    }
 
    /* NULL check */
    if(vsInterface== NULL)
    {
        /* A matching interface could not be found. This is an error condition */
        return USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE_NONE;
    }
  
    switch(event)
    {
        case USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE:
            /* This means a data transfer has completed */
            /* Get the Video Stream Event, we saved this at the time of 
                submit request */
            videoStreamEvent = (USB_HOST_UVC_STREAM_EVENT)(context) & 0x000000FF;
            versionFlag = (uint8_t)(context >> 8); 

            /* Get event Data*/
            dataTransferEvent = (USB_HOST_DEVICE_INTERFACE_EVENT_TRANSFER_COMPLETE_DATA *)(eventData);
            videoStrmXfrCmpltData.transferHandle = dataTransferEvent->transferHandle;
            videoStrmXfrCmpltData.result = dataTransferEvent->result;
            videoStrmXfrCmpltData.length = dataTransferEvent->length;
            alternateSetting = vsInterface->activeInterfaceSetting; 
            if(alternateSetting == 0)
            {
                /* We should not have received an Video transfer complete event 
                    when alternate setting is Zero. This is an error condition */
                SYS_DEBUG_MESSAGE(SYS_DEBUG_INFO, "USB_HOST_UVC_InterfaceEventHandler: Data transfer event received but Alternate Setting is Zero"); 
                return USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE_NONE; 
            }

            /* Create Stream Handle */
            strmHandle = (USB_HOST_UVC_STREAM_HANDLE)   (uint32_t)videoIndex |
                                                        (uint32_t)asIntrfcIndex << 8 |
                                                        (uint32_t)alternateSetting << 16;


            if(versionFlag == USB_HOST_UVC_API_VERSION_FLAG_V1)
            {
                /* Send event to the Video Stream Handler */

                if(vsInterface->streamEventHandler != NULL)
                {
                    vsInterface->streamEventHandler(strmHandle, videoStreamEvent, 
                                        &videoStrmXfrCmpltData, vsInterface->context);
                }
            }
            else if(versionFlag == USB_HOST_UVC_API_VERSION_FLAG_V1_DEPRECIATED)
            {
                /* Get handle to the current active stream */
                videoStrm = &vsInterface->videoStreamSetting[alternateSetting];
                if(videoStrm->streamEventHandler != NULL)
                {
                    videoStrm->streamEventHandler(strmHandle, videoStreamEvent, 
                                        &videoStrmXfrCmpltData, videoStrm->context);
                }
            }
            break;
  
        case USB_HOST_DEVICE_INTERFACE_EVENT_SET_INTERFACE_COMPLETE:
            DataIntrfcSetEvent = (USB_HOST_DEVICE_INTERFACE_EVENT_SET_INTERFACE_COMPLETE_DATA*)(eventData);

            /* Save event and event data in the Video Streaming interface structure */
            vsInterface->stateData.eventData.requestHandle = DataIntrfcSetEvent->requestHandle; 
            vsInterface->stateData.eventData.result = DataIntrfcSetEvent->result;
            vsInterface->stateData.context = context; 
            vsInterface->stateData.event = event; 

            /* Change Video Stream state to Pipe Action Pending. The Pipe open
               or close action will be performed in the Interface Event Handler
               routine. */
            vsInterface->state = USB_HOST_UVC_STREAM_STATE_PIPE_ACTION_PENDING;  
            break; 
  
         default:
            break;
    }
 
    return(USB_HOST_DEVICE_INTERFACE_EVENT_RESPONSE_NONE);
}

// *****************************************************************************
/* Function:
 void _USB_HOST_UVC_InterfaceTasks
  (
    USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle
  )

 Summary:
    This function is called by the Host Layer to update the state of this
    driver.

 Description:
    This function is called by the Host Layer to update the state of this
    driver.

 Remarks:
    This is a local function and should not be called directly by the
    application.
*/
void _USB_HOST_UVC_InterfaceTasks(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle)
{
    USB_HOST_UVC_STREAMING_INTERFACE *vsInterface;
    uint8_t videoIndex, vsIntrfcIndex;
    USB_ENDPOINT_ADDRESS endpointAddress; 
    USB_HOST_RESULT result;
    USB_HOST_UVC_STREAM_SETTING *videoStrmSetting;
    uint8_t alternateSetting; 
    uint8_t videoStreamEventLocal;
    USB_HOST_UVC_STREAM_HANDLE strmHandle; 
    USB_HOST_UVC_STREAM_EVENT videoStreamEvent;
    bool changeAlternateSetting = false; 
 
    /* Find out to which Video Stream this Interface belongs */
    if(_USB_HOST_UVC_IntrfcHndlToStrmIntrfcPtr
            (
                interfaceHandle, 
                &vsInterface,
                &videoIndex,
                &vsIntrfcIndex
            ) == false)
    {
        /* A matching interface could not be found. This is an error condition */
        return; 
    }
 
    /* NULL check */
    if(vsInterface== NULL)
    {
        /* A matching interface could not be found. This is an error condition */
        return;
    }
    
    switch(vsInterface->state) 
    {
        case USB_HOST_UVC_STREAM_STATE_PIPE_ACTION_PENDING: 
  
            vsInterface->state = USB_HOST_UVC_STREAM_STATE_READY; 
            /* Get the requested alternate setting, we saved this at the time of
               request submit */
            alternateSetting = (uint8_t)vsInterface->stateData.context;
            /* Get the event */
            videoStreamEventLocal = (uint8_t)(vsInterface->stateData.context >> 8); 
            /* Get pointer to Video Interface Setting  */
            videoStrmSetting = &vsInterface->videoStreamSetting[alternateSetting]; 
    
            if(vsInterface->stateData.eventData.result == USB_HOST_RESULT_SUCCESS)
            {
                if(alternateSetting !=NULL )
                {
                    /* Alternate Setting is a Non Zero Value. That means we have 
                       to Open pipe for all of the Endpoints present in this 
                       Alternate Setting. */
                    if(videoStrmSetting->nEndpoints != NULL)
                    {
                        /* Open pipe for Video Data Endpoint*/
                        if(vsInterface->isIsoDataPipeSet == false)
                        {
                            endpointAddress = videoStrmSetting->isoDataEndpointDesc.bEndpointAddress; 
                            vsInterface->isoDataPipeHandle = USB_HOST_DevicePipeOpen(vsInterface->vsInterfaceHandle, endpointAddress);
                            if(vsInterface->isoDataPipeHandle != USB_HOST_PIPE_HANDLE_INVALID)
                            {
                                vsInterface->isIsoDataPipeSet = true; 
                                changeAlternateSetting = true; 
                                vsInterface->state = USB_HOST_UVC_STREAM_STATE_PIPE_OPEN_SUCCESS; 
                            }
                            else
                            {
                                vsInterface->state = USB_HOST_UVC_STREAM_PIPE_OPEN_FAILED; 
                                vsInterface->isIsoDataPipeSet = false;
                                changeAlternateSetting = false; 
                                SYS_DEBUG_MESSAGE ( SYS_DEBUG_INFO , "USB_HOST_UVC_InterfaceEventHandler():Pipe Open Failed\r\n" );
                            }
                        }
                    }
                }
            }
            else
            {
                if(vsInterface->isIsoDataPipeSet == true)
                {
                    result = USB_HOST_DevicePipeClose(vsInterface->isoDataPipeHandle); 
                    if(result != USB_HOST_RESULT_SUCCESS)
                    {
                        changeAlternateSetting = false; 
                        vsInterface->state = USB_HOST_UVC_STREAM_PIPE_CLOSE_FAILED; 
                        SYS_DEBUG_MESSAGE ( SYS_DEBUG_INFO , "USB_HOST_UVC_InterfaceEventHandler():Pipe Close Failed\r\n" );
                    }
                    else
                    {
                        changeAlternateSetting = true; 
                        vsInterface->state = USB_HOST_UVC_STREAM_STATE_PIPE_CLOSE_SUCCESS; 
                    }
                    vsInterface->isIsoDataPipeSet = false; 
                }
            }

            if(changeAlternateSetting == true)
            {
                /* Change in the alternate setting in the Video Instance */
                vsInterface->activeInterfaceSetting = alternateSetting;
            }

            switch(videoStreamEventLocal)
            {
                case USB_HOST_UVC_API_VERSION_FLAG_STREAM_INTERFACE_SET:
                    videoStreamEvent = USB_HOST_UVC_STREAM_EVENT_INTERFACE_SET_COMPLETE;
                    /* Stream Handle */
                    strmHandle = (USB_HOST_UVC_STREAM_HANDLE)(uint32_t)videoIndex |
                                    ((uint32_t)vsIntrfcIndex << 8);
                    if(vsInterface->streamEventHandler != NULL)
                    {
                        vsInterface->streamEventHandler
                        (
                            strmHandle, 
                            videoStreamEvent, 
                            &vsInterface->stateData.eventData, 
                            vsInterface->context
                        );
                    }
                    break;
                case USB_HOST_UVC_API_VERSION_FLAG_STREAM_DISABLE:
                    videoStreamEvent = USB_HOST_UVC_0_STREAM_EVENT_DISABLE_COMPLETE; 
                    /* Stream Handle */ 
                    strmHandle = (USB_HOST_UVC_STREAM_HANDLE)(uint32_t)videoIndex |
                                    (uint32_t)vsIntrfcIndex << 8 |
                                    (uint32_t)alternateSetting << 16;
                    if(videoStrmSetting->streamEventHandler != NULL)
                    {
                        videoStrmSetting->streamEventHandler
                        (
                            strmHandle, 
                            videoStreamEvent, 
                            &vsInterface->stateData.eventData, 
                            videoStrmSetting->context
                        );
                    }
                    break;
                case USB_HOST_UVC_API_VERSION_FLAG_STREAM_ENABLE:
                    videoStreamEvent = USB_HOST_UVC_0_STREAM_EVENT_ENABLE_COMPLETE;
                    /* Stream Handle */
                     strmHandle = (USB_HOST_UVC_STREAM_HANDLE)(uint32_t)videoIndex |
                                    (uint32_t)vsIntrfcIndex << 8 |
                                    (uint32_t)alternateSetting << 16;
                    if(videoStrmSetting->streamEventHandler)
                    {
                        videoStrmSetting->streamEventHandler
                        (
                            strmHandle, 
                            videoStreamEvent, 
                            &vsInterface->stateData.eventData, 
                            videoStrmSetting->context
                        );
                    }
                    break;
            }
            break;
   
        default:
            break; 
    }
}

// ****************************************************************************
/* Function:
 bool _USB_HOST_UVC_IntrfcHndlToStrmIntrfcPtr
 (
    USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle, 
    USB_HOST_VIDEO_STREAMING_INTERFACE** strmIntfcPtr, 
    uint8_t *videoIndex, 
    uint8_t *asIntrfcIndex
 )

 Summary:
    This function converts an interface handle to following details. 
        1) Pointer to the Video streaming interface. 
        2) Index of the Video Instance
        3) Index of the Video Streaming interface array. 

 Description:
    This function converts an interface handle to following details. 
        1) Pointer to the Video streaming interface. 
        2) Index of the Video Instance
        3) Index of the Video Streaming interface array. 
 
 Precondition:
    None.

 Parameters:
    interfaceHandle - Interface Handle
 
    strmIntfcPtr - Pointer to a pointer of Video Streaming Interfacce. This is 
    an OUT parameter. 
 
    videoIndex - Pointer to Video Instance Array Index. This is an OUT parameter. 
 
    asIntrfcIndex - Pointer to Video Streaming interface Array Index. This is an 
    an OUT parameter. 

Returns:
     true - Video Streaming interface was found. 

    false - Video Streaming interface was not found. 
 */ 
bool _USB_HOST_UVC_IntrfcHndlToStrmIntrfcPtr
(
    USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle, 
    USB_HOST_UVC_STREAMING_INTERFACE** strmIntfcPtr, 
    uint8_t *videoIndex,
    uint8_t *vsIntrfcIndex
)
{
    int count;
    int vsIntrfcCount; 
    USB_HOST_UVC_INSTANCE *videoInstanceInfo; 

    for(count=0; count < USB_HOST_UVC_INSTANCES_NUMBER; count++)
    {
        videoInstanceInfo = &gUSBHostVideoInstance[count]; 
        for(vsIntrfcCount =0; vsIntrfcCount < videoInstanceInfo->nVSInterfaces; vsIntrfcCount++)
        {
            *strmIntfcPtr = &videoInstanceInfo->streamInf[vsIntrfcCount];
            if((*strmIntfcPtr)->vsInterfaceHandle == interfaceHandle)
            {
                *videoIndex = count;
                *vsIntrfcIndex = vsIntrfcCount;
                return true; 
            } 
        }
    }
    return false;
}

// *****************************************************************************
/* Function:
 void _USB_HOST_UVC_InterfaceRelease
  ( 
    USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle 
  )

 Summary:
    This function is called when the Host Layer detaches this driver from an
    interface.

 Description:
    This function is called when the Host Layer detaches this driver from an
    interface.

 Remarks:
    This is a local function and should not be called directly by the
    application.
*/
void _USB_HOST_UVC_InterfaceRelease(USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle) 
{
    USB_HOST_UVC_INSTANCE *videoInstanceInfo; 
    uint8_t videoIndex, vsIntrfcIndex; 
    USB_UVC_SUBCLASS_CODE videoInterface = USB_UVC_SUBCLASS_UNDEFINED; 
    USB_HOST_UVC_STREAMING_INTERFACE* vsInterface; 
    int strmIndex; 
 
    /* Get the instance associated with this interface */
    videoIndex = _USB_HOST_UVC_InterfaceHandleToVideoInstance(interfaceHandle); 
    if(videoIndex < USB_HOST_UVC_INSTANCES_NUMBER)
    {
        videoInterface = USB_UVC_SUBCLASS_VIDEOCONTROL;
    }
    else
    {
        /* Find out to which Video Stream this interface belongs */
        if(_USB_HOST_UVC_IntrfcHndlToStrmIntrfcPtr
                (
                    interfaceHandle, 
                    &vsInterface,
                    &videoIndex, 
                    &vsIntrfcIndex
                ) == true)
        {
            videoInterface = USB_UVC_SUBCLASS_VIDEOSTREAMING;
        }
    }
 
    switch(videoInterface)
    {
        case USB_UVC_SUBCLASS_VIDEOCONTROL:
            videoInstanceInfo = &gUSBHostVideoInstance[videoIndex]; 
  
            /* Close Control Pipe */ 
            for(strmIndex = 0; strmIndex< videoInstanceInfo->nVSInterfaces; strmIndex++)
            {
                vsInterface = &videoInstanceInfo->streamInf[strmIndex]; 
                /* Save Video Streaming Interface Number */
                vsInterface->interfaceId = 0;
                /* Set Alternate setting to Zero */
                vsInterface->activeInterfaceSetting = 0;
                if(vsInterface->isIsoDataPipeSet == true)
                {
                    /* Close pipe */
                    USB_HOST_DevicePipeClose(vsInterface->isoDataPipeHandle); 
                }
            }
   
            /* Notify client about detach event */
            if(gUSBHostUVCCommonObj.attachEventHandler != NULL)
            {
                gUSBHostUVCCommonObj.attachEventHandler
                (
                    (USB_HOST_UVC_OBJ)videoInstanceInfo, 
                    USB_HOST_UVC_EVENT_DETACH,
                    gUSBHostUVCCommonObj.context
                ); 
            }

            videoInstanceInfo->assigned = false;   
            memset(videoInstanceInfo, 0, sizeof(USB_HOST_UVC_INSTANCE)); 
            break; 
        case USB_UVC_SUBCLASS_VIDEOSTREAMING:
            break; 
            
        default:
            break; 
    }
}

// *****************************************************************************
/* Function:
 void _USB_HOST_UVC_ControlRequestCallback
 (
    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
    USB_HOST_REQUEST_HANDLE requestHandle,
    USB_HOST_RESULT result,
    size_t size,
    uintptr_t context
 )

 Summary:
    This function is called by USB Host Layer when an Control Transfer is complete. 

 Description:
    This function is called by USB Host Layer when an Control Transfer is complete.
 
 Precondition:
     None.

 Parameters:
    deviceObjHandle - Device handle 

    requestHandle - A pointer to event handler function. If NULL, then events
    will not be generated.
    result - Control transfer result 
    size - Control transfer size 
    context - Application specific context that is returned in the event handler.

 Returns:

 Remarks:
    None.
*/
void _USB_HOST_UVC_ControlRequestCallback
(
    USB_HOST_DEVICE_OBJ_HANDLE deviceObjHandle,
    USB_HOST_REQUEST_HANDLE requestHandle, 
    USB_HOST_RESULT result,
    size_t size, 
    uintptr_t context
)
{
    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)context; 
 
    if(videoInstanceInfo->videoControlObj.callback != NULL)
    {
        videoInstanceInfo->videoControlObj.callback
        (
            videoInstanceInfo->deviceObjHandle,
            requestHandle, 
            result, 
            size, 
            videoInstanceInfo->videoControlObj.context
        ); 
    }    
    videoInstanceInfo->videoControlObj.inUse = false; 
}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT _USB_HOST_UVC_ControlRequest
    (
        USB_HOST_UVC_OBJ videoObj,
        USB_HOST_UVC_REQUEST_HANDLE *requestHandle,
        USB_SETUP_PACKET *setupPacket,
        void *data,
        USB_HOST_UVC_CONTROL_CALLBACK callback, 
        uintptr_t context 
    );

 Summary:
    Schedules an Video control transfer.

 Description:
    This function schedules an Video control transfer. videoObj is an 
    Object of Video class driver to which the video control transfer is to
    be scheduled. setupPacket points to the setup command to be sent in the 
    Setup Stage of the control transfer. The size and the direction of the data
    stage is indicated by the setup packet. In case of control transfers where
    there is no data stage, data is ignored and can be NULL. In all other cases,
    data should point to the data to data be transferred in the data stage of 
    the control transfer. 
 
    If the transfer was scheduled successfully, requestHandle will contain a
    transfer handle that uniquely identifies this transfer. If the transfer
    could not be scheduled successfully, requestHandle will contain
    USB_HOST_UVC_REQUEST_HANDLE_INVALID.

    When the control transfer completes, the Video client driver will call
    the specified callback function. The context parameter specified here will 
    be returned in the callback.

 Precondition:
    Video Device should have attached. 

 Parameters:
    videoObj - Video client driver object 

    requestHandle - output parameter that will contain the handle to this
    transfer.

    setupPacket - Pointer to the setup packet to sent to the device in the setup
    stage of the control transfer.

    data - For control transfer with a data stage, this should point to data to
    be sent to the device (for a control write transfer) or point to the buffer
    that will receive data from the device (for a control read transfer). For
    control transfers that do not require a data stage, this parameter is
    ignored and can be NULL.

    callback - pointer to the callback function that will be called when the
    control transfer completes. If the callback function is NULL, there will be
    no notification of when the control transfer will complete.

    context - user defined context that is returned with the callback function.

 Returns:
    USB_HOST_UVC_RESULT_SUCCESS - the transfer was scheduled successfully.
    requestHandle will contain a valid transfer handle.
    USB_HOST_UVC_RESULT_FAILURE - an unknown failure occurred. requestHandle will
    contain USB_HOST_UVC_REQUEST_HANDLE_INVALID.
    USB_HOST_UVC_RESULT_PARAMETER_INVALID - The data pointer or requestHandle pointer
    is NULL.

 Example:
 <code>
 </code>

 Remarks:
    This is a local function and should not be accessed by applications. 
*/
USB_HOST_UVC_RESULT _USB_HOST_UVC_ControlRequest
(
    USB_HOST_UVC_OBJ videoObj, 
    USB_HOST_UVC_REQUEST_HANDLE *transferHandle,
    USB_SETUP_PACKET *setupPacket, 
    void *data
)
{
    USB_HOST_UVC_RESULT videoResult = USB_HOST_UVC_RESULT_FAILURE; 
    USB_HOST_RESULT hostResult = USB_HOST_RESULT_FAILURE; 
    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)videoObj;
    OSAL_CRITSECT_DATA_TYPE IntState;
 
    /* Check if the handle to the video Instance in not NULL */
    if(videoInstanceInfo ==NULL)
    {
        SYS_DEBUG_MESSAGE ( SYS_DEBUG_INFO, "USB_HOST_VIDEO_ControlSend: Not a valid instance " );
        return USB_HOST_UVC_RESULT_OBJ_INVALID;
    }

    /* Check if this Video Instance is initialized */
    if(videoInstanceInfo->assigned == false)
    {
        SYS_DEBUG_MESSAGE ( SYS_DEBUG_INFO , "USB_HOST_VIDEO_ControlSend: Invalid parameters " );
        return USB_HOST_UVC_RESULT_FAILURE;
    }

    if(videoInstanceInfo->videoControlObj.inUse == true)
    {
        return USB_HOST_UVC_RESULT_BUSY; 
    }
    
    /* Prevent other tasks pre-empting this sequence of code */ 
    IntState = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
    memcpy(&videoInstanceInfo->setupPacket, setupPacket, sizeof(USB_SETUP_PACKET)); 
    videoInstanceInfo->videoControlObj.inUse = true; 
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, IntState);
 
    /* Submit Request*/
    hostResult = USB_HOST_DeviceControlTransfer
            (
                videoInstanceInfo->controlPipeHandle,
                transferHandle, 
                &videoInstanceInfo->setupPacket, 
                (void *)data, 
                _USB_HOST_UVC_ControlRequestCallback,
                (uintptr_t)videoInstanceInfo
            ); 
 
    switch(hostResult)
    {
        case USB_HOST_RESULT_PARAMETER_INVALID:
            videoResult = USB_HOST_UVC_RESULT_INVALID_PARAMETER; 
            break; 
        case USB_HOST_RESULT_REQUEST_BUSY:
            videoResult = USB_HOST_UVC_RESULT_BUSY; 
            break; 
        case USB_HOST_RESULT_SUCCESS:
            videoResult = USB_HOST_UVC_RESULT_SUCCESS; 
            break; 
        default: 
            videoResult = USB_HOST_UVC_RESULT_FAILURE; 
            break; 
    }
  
    return videoResult;
}

/* Function:
    int _USB_HOST_UVC_InterfaceHandleToVideoInstance
    ( 
       USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle
    )

 Summary:
    This function will return the Video instance object that is associated with
    this interface.

 Description:
    This function will return the Video V1.0 instance object that is associated with
    this interface.

 Remarks:
    This is a local function and should not be called directly by the
    application.
*/
int _USB_HOST_UVC_InterfaceHandleToVideoInstance
(
    USB_HOST_DEVICE_INTERFACE_HANDLE interfaceHandle
)
{
    int iterator;
    int videoIndex = -1;

    /* Find the Video Instance object that owns this interface */
    for(iterator=0; iterator < USB_HOST_UVC_INSTANCES_NUMBER; iterator++)
    {
        if(gUSBHostVideoInstance[iterator].assigned)
        {
            if(gUSBHostVideoInstance[iterator].acInterfaceHandle== interfaceHandle)
            {
                /* Found it */
                videoIndex = iterator;
                break;
            }
        }
    }
    return (videoIndex);
}

// *****************************************************************************
// *****************************************************************************
// Video Host Client Driver Public function
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_StreamWrite
    (
        USB_HOST_UVC_STREAM_HANDLE streamHandle,
        USB_HOST_UVC_STREAM_TRANSFER_HANDLE * transferHandle,
        void * source, 
        size_t length
    ); 

 Summary:
    Schedules an Video stream Write request for the specified video stream. 

 Description:
    This function schedules an Video stream Write request for the specified 
    video stream. An event 
    USB_HOST_UVC_STREAM_EVENT_WRITE_COMPLETE is generated when this 
    request is completed. 
    USB_HOST_UVC_STREAM_EVENT_WRITE_COMPLETE_DATA returns 
    the status and request handle of the request.
 
 Precondition:
    Video stream should have been opened and enabled. The direction of the 
    Video Stream should be USB_HOST_UVC_DIRECTION_IN. 

 Parameters:
    streamHandle - Handle to the Video Stream.

    transferHandle - Handle to the Stream Write transfer request 

    source  - Pointer to the buffer containing data to be written to the 
       device. 

    length  - Amount of data to written (in bytes).

 Returns:
    USB_HOST_UVC_STREAM_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_STREAM_RESULT_HANDLE_INVALID - The specified video 
    stream does not exist.
    USB_HOST_UVC_STREAM_RESULT_FAILURE - An unknown failure occurred.

 Example:
    <code>
    </code>

 Remarks:
    None.  

*/
//USB_HOST_UVC_RESULT _USB_HOST_UVC_StreamWrite
//(
//    USB_HOST_UVC_STREAM_HANDLE streamHandle,
//    USB_HOST_UVC_STREAM_TRANSFER_HANDLE *transferHandle,
//    void *source, 
//    size_t length, 
//    uint8_t apiVersionFlag
//)
//{
//    USB_HOST_UVC_INSTANCE *videoInstanceInfo; 
//    uint8_t videoInstanceIndex;
//    uint8_t vsInterfaceIndex; 
//    USB_HOST_PIPE_HANDLE videoDataPipeHandle; 
//    USB_HOST_UVC_RESULT videoResult = USB_HOST_UVC_RESULT_FAILURE;
//    USB_HOST_RESULT hostResult; 
// 
//    /* Find Video Stream from videoStreamObj */
//    if(streamHandle == USB_HOST_UVC_STREAM_HANDLE_INVALID)
//    {
//        return USB_HOST_UVC_RESULT_INVALID_PARAMETER; 
//    }
//    
//    
//    videoInstanceIndex = (uint8_t)streamHandle;
//    vsInterfaceIndex = (uint8_t)(streamHandle >> 8); 
//    videoInstanceInfo = &gUSBHostVideoInstance[videoInstanceIndex];
//    if(videoInstanceInfo == NULL)
//    {
//        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
//    }
//    
//    if((length != 0) && (source) == NULL)
//    {
//        /* Input parameters are not valid */
//        return USB_HOST_UVC_RESULT_INVALID_PARAMETER;
//    }
//    
//    /* The context for the transfer is the event that needs to
//     * be sent to the application. In this case the event to be
//     * sent to the application when the transfer completes is
//     * USB_HOST_UVC_EVENT_WRITE_COMPLETE */
//    if(videoInstanceInfo->streamInf[vsInterfaceIndex].isIsoDataPipeSet == true)
//    {
//        videoDataPipeHandle = videoInstanceInfo->streamInf[vsInterfaceIndex].isoDataPipeHandle; 
//        hostResult = USB_HOST_DeviceTransfer(videoDataPipeHandle, transferHandle, source, length, (uintptr_t)(
//        USB_HOST_UVC_STREAM_EVENT_WRITE_COMPLETE | (apiVersionFlag << 8)));
//    }
//    else
//    {
//        return USB_HOST_UVC_RESULT_FAILURE; 
//    }
//    
//    switch(hostResult) 
//    {
//        case USB_HOST_RESULT_SUCCESS:
//            videoResult = USB_HOST_UVC_RESULT_SUCCESS;
//            break;
//        case USB_HOST_RESULT_REQUEST_BUSY:
//            videoResult = USB_HOST_UVC_RESULT_BUSY; 
//            break; 
//        default:
//            videoResult = USB_HOST_UVC_RESULT_FAILURE; 
//            break; 
//    }
//    return videoResult; 
//}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT _USB_HOST_UVC_StreamRead
    (
        USB_HOST_UVC_STREAM_HANDLE streamHandle,
        USB_HOST_UVC_STREAM_TRANSFER_HANDLE * transferHandle,
        void * source, 
        size_t length
    ); 

 Summary:
    Schedules an Video stream Read request for the specified video stream. 

 Description:
    This function schedules an Video stream Read request for the specified 
    video stream. An event 
    USB_HOST_UVC_STREAM_EVENT_READ_COMPLETE is generated when this 
    request is completed. 
    USB_HOST_UVC_STREAM_EVENT_READ_COMPLETE_DATA returns 
    the status and request handle of the request.

 Precondition:
    Video stream should have been opened and enabled. The direction of the 
    Video Stream should be USB_HOST_UVC_DIRECTION_IN. 

 Parameters:
    streamHandle - Handle to the Video Stream.

    transferHandle - Handle to the Stream Read transfer request 

    source  - Pointer to the buffer containing data to be read from the 
       device. 

    length  - Amount of data to read (in bytes).

 Returns:
    USB_HOST_UVC_STREAM_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_STREAM_RESULT_HANDLE_INVALID - The specified video 
    stream does not exist.
    USB_HOST_UVC_STREAM_RESULT_FAILURE - An unknown failure occurred.

 Example:
    <code>
    </code>

 Remarks:
    None.

*/
USB_HOST_UVC_RESULT _USB_HOST_UVC_StreamRead
(
    USB_HOST_UVC_STREAM_HANDLE streamHandle,
    USB_HOST_UVC_STREAM_TRANSFER_HANDLE *transferHandle,
    void *source, 
    size_t length, 
    uint8_t apiVersionFlag
)
{
    USB_HOST_UVC_INSTANCE *videoInstanceInfo; 
    uint8_t videoInstanceIndex;
    uint8_t vsInterfaceIndex; 
    USB_HOST_PIPE_HANDLE videoDataPipeHandle; 
    USB_HOST_UVC_RESULT videoResult = USB_HOST_UVC_RESULT_FAILURE;
    USB_HOST_RESULT hostResult; 

    /* Find Video Stream from videoStreamObj */
    if(streamHandle == USB_HOST_UVC_STREAM_HANDLE_INVALID)
    {
        return USB_HOST_UVC_RESULT_INVALID_PARAMETER; 
    }
    videoInstanceIndex = (uint8_t)streamHandle;
    vsInterfaceIndex = (uint8_t)(streamHandle >> 8); 

    videoInstanceInfo = &gUSBHostVideoInstance[videoInstanceIndex];

    if((length != 0) && (source) == NULL)
    {
        /* Input parameters are not valid */
        return USB_HOST_UVC_RESULT_INVALID_PARAMETER;
    }
    /* The context for the transfer is the event that needs to
     * be sent to the application. In this case the event to be
     * sent to the application when the transfer completes is
     * USB_HOST_UVC_EVENT_READ_COMPLETE */
    if(videoInstanceInfo->streamInf[vsInterfaceIndex].isIsoDataPipeSet == true) 
    {
        videoDataPipeHandle = videoInstanceInfo->streamInf[vsInterfaceIndex].isoDataPipeHandle;  
        hostResult = USB_HOST_DeviceTransfer(videoDataPipeHandle, transferHandle, source, length, (uintptr_t)(
        USB_HOST_UVC_STREAM_EVENT_READ_COMPLETE) | (apiVersionFlag << 8));
    }
    else
    {
        return USB_HOST_UVC_RESULT_FAILURE; 
    }
  
    switch(hostResult)
    {
        case USB_HOST_RESULT_SUCCESS:
            videoResult = USB_HOST_UVC_RESULT_SUCCESS;
            break;
        case USB_HOST_RESULT_REQUEST_BUSY:
            videoResult = USB_HOST_UVC_RESULT_BUSY; 
            break; 
        default:
            videoResult = USB_HOST_UVC_RESULT_FAILURE; 
            break; 
    }
    return videoResult; 
}

// ****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_AttachEventHandlerSet
    (
        USB_HOST_UVC_ATTACH_EVENT_HANDLER eventHandler,
        uintptr_t context
    );

 Summary:
    This function will set an attach/detach event handler.

 Description:
    This function will set an attach event handler. The attach event handler
    will be called when a Video device has been attached or detached. The 
    context will be returned in the event handler. This function should be 
    called before the bus has been enabled.
 
 Precondition:
    None.

 Parameters:
    eventHandler - Pointer to the attach event handler
 
    context - An application defined context that will be returned in the event
       handler.

 Returns:
    USB_HOST_UVC_RESULT_SUCCESS - if the attach event handler was registered
    successfully. 

    USB_HOST_UVC_RESULT_FAILURE - if the number of registered event 
    handlers has exceeded USB_HOST_UVC_ATTACH_LISTENERS_NUMBER.

 Example:
    <code> 
    </code>

 Remarks:
    Function should be called before USB_HOST_BusEnable() function is called.

 */
//USB_HOST_UVC_RESULT USB_HOST_UVC_AttachEventHandlerSet
//(
//    USB_HOST_UVC_ATTACH_EVENT_HANDLER attachEventHandler, 
//    uintptr_t context
//)
//{
// 
//    /* validate callback handler */
//    if(NULL == attachEventHandler)
//    {
//      return USB_HOST_UVC_RESULT_INVALID_PARAMETER;
//    }
//    gUSBHostUVCCommonObj.attachEventHandler = attachEventHandler; 
//    gUSBHostUVCCommonObj.context = context; 
//    return USB_HOST_UVC_RESULT_SUCCESS; 
//}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_EntityRequestCallbackSet
    (
        USB_HOST_UVC_OBJ videoDeviceObj, 
        USB_HOST_UVC_ENTITY_REQUEST_CALLBACK appVideoEntityRequestCallback,
        uintptr_t context
    ); 

Summary:
    Registers an Video Entity request callback function with the Video
    Client Driver.

 Description:
    This function registers a callback function for the Video Control
    Entity requests. The Video Host Client Driver will invoke this
    callback function when an Video Entity control request is completed. 

 Precondition:
    None.

 Parameters:
    videoDeviceObj - Video device object.

    appVideoEntityRequestCallback - A pointer to event handler function. If NULL,
    then events will not be generated.
 
    context - Application specific context that is returned in the event handler.

 Returns:
    USB_HOST_UVC_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_RESULT_OBJ_INVALID - The specified video 
    Object does not exist.
    USB_HOST_UVC_RESULT_FAILURE - An unknown failure occurred.

 Example:
    <code>
    </code>

 Remarks:
    This is a local function and should not be called by applications. 
*/
USB_HOST_UVC_RESULT USB_HOST_UVC_EntityRequestCallbackSet
(
    USB_HOST_UVC_OBJ videoDeviceObj, 
    USB_HOST_UVC_ENTITY_REQUEST_CALLBACK appVideoEntityRequestCallback, 
    uintptr_t context
)
{
  USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)videoDeviceObj; 
 
    /* NULL check */
    if(videoInstanceInfo == NULL)
    {
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    if(appVideoEntityRequestCallback != NULL)
    {
        videoInstanceInfo->videoControlObj.callback = appVideoEntityRequestCallback; 
        videoInstanceInfo->videoControlObj.context = context; 
        return USB_HOST_UVC_RESULT_SUCCESS; 
    }
    return USB_HOST_UVC_RESULT_INVALID_PARAMETER; 
}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_StreamEventHandlerSet
    (
        USB_HOST_UVC_STREAM_HANDLE handle,
        USB_HOST_UVC_STREAM_EVENT_HANDLER appVideoHandler,
        uintptr_t context
    ); 

 Summary:
    Registers an event handler with the Video Client Driver Stream.

 Description:
    This function registers a client specific Video stream event handler.
    The Video Host Client Driver will call appVideoHandler function 
    specified as 2nd argument with relevant event and associate event data, in
    response to video stream data transfers that have been scheduled by the 
    client.
 
 Precondition:
    None.

 Parameters:
    handle - handle to the Video Stream.

    eventHandler - A pointer to event handler function. If NULL, then events
       will not be generated.
 
    context - Application specific context that is returned in the event handler.

 Returns:
    USB_HOST_UVC_STREAM_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_STREAM_RESULT_HANDLE_INVALID - The specified video 
    stream does not exist.
    USB_HOST_UVC_STREAM_RESULT_FAILURE - An unknown failure occurred.

 Example:
    <code>
    </code>

 Remarks:
    None.
*/
USB_HOST_UVC_RESULT USB_HOST_UVC_StreamEventHandlerSet
(
    USB_HOST_UVC_STREAM_HANDLE streamHandle,
    USB_HOST_UVC_STREAM_EVENT_HANDLER streamEventHandler, 
    uintptr_t context
)
{ 
    uint8_t videoInstance; 
    uint8_t vSIntrfcIdx; 
    uint8_t vsIntrfcSettingIdx; 
    USB_HOST_UVC_INSTANCE *videoInstanceInfo; 
 
    videoInstance = (uint8_t)streamHandle; 
    vSIntrfcIdx = (uint8_t)(streamHandle >> 8); 
    vsIntrfcSettingIdx = (uint8_t)(streamHandle >> 16); 
 
    videoInstanceInfo = &gUSBHostVideoInstance[videoInstance];
 
    /* NULL check */ 
    if(videoInstanceInfo == NULL)
    {
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
  
    /* Check if there is any Video Streaming Interface present in the Video Device */
    if(videoInstanceInfo->nVSInterfaces == 0)
    {
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    
    /* Verify Video Streaming Interface Object */
    if(vSIntrfcIdx >= videoInstanceInfo->nVSInterfaces)
    {
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    
    /* Verify Video streaming interface Setting Index */
    if(vsIntrfcSettingIdx >= videoInstanceInfo->streamInf[vSIntrfcIdx].nInterfaceSetting)
    {
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
  
    /* Check if next Streaming Interface exists */
    if(vSIntrfcIdx == videoInstanceInfo->nVSInterfaces)
    {
        return USB_HOST_UVC_RESULT_END_OF_STREAMING_INTERFACE; 
    }
    
    if(streamEventHandler != NULL )
    {
        videoInstanceInfo->streamInf[vSIntrfcIdx].streamEventHandler = streamEventHandler;
        videoInstanceInfo->streamInf[vSIntrfcIdx].context = context; 
        return USB_HOST_UVC_RESULT_SUCCESS; 
    } 
    return USB_HOST_UVC_RESULT_INVALID_PARAMETER; 
}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceGetFirst
    (
        USB_HOST_UVC_OBJ videoObj, 
        USB_HOST_UVC_STREAMING_INTERFACE_OBJ* streamingInterfaceObj
    )

 Summary:
 
 Description:
 
 
 Precondition:
 
 Parameters:
 
 Returns:
 
 Example:
    <code>
    </code>

 Remarks:
    None.
*/
//USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceGetFirst
//(
//    USB_HOST_UVC_OBJ videoDeviceObj, 
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ *streamingInterfaceObj
//)
//{
// 
//    /* Get Pointer to Video Device Instance */
//    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)videoDeviceObj;
//
//    /* Null check */
//    if(videoInstanceInfo == NULL)
//    {
//        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
//    }
//    
//    /* If there is any Video Streaming Interface present in the Video Device */
//    if(videoInstanceInfo->nVSInterfaces == 0)
//    {
//        return USB_HOST_UVC_RESULT_END_OF_INTERFACE_SETTINGS; 
//    }
//    /* Get pointer to First Video Streaming Interface */
//    *streamingInterfaceObj = (USB_HOST_UVC_STREAMING_INTERFACE_OBJ)videoInstanceInfo->index; 
//
//    return USB_HOST_UVC_RESULT_SUCCESS;  
//}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceGetNext
    (
        USB_HOST_UVC_OBJ videoObj, 
        USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObjCurrent 
        USB_HOST_UVC_STREAMING_INTERFACE_OBJ* streamingInterfaceObjNext
    );

 Summary:
    This function will Get the next streaming interface in the attached 
    Video Device.

 Description:
    This function will Get the next streaming interface in the attached 
    Video Device. 
 
 Precondition:
    Video device should have been attached. 

 Parameters:
    videoObj - Video Device Object
    streamingInterfaceObjCurrent - Current Video Streaming Interface Object
    streamingInterfaceObj - Pointer to Video Streaming Interface Object 

 Returns:
    USB_HOST_UVC_RESULT_SUCCESS - The request completed successfully
    USB_HOST_UVC_RESULT_END_OF_STREAMING_INTERFACE - No more streaming 
    interfaces available 
    USB_HOST_UVC_RESULT_DEVICE_UNKNOWN - Device is not attached currently
    USB_HOST_UVC_RESULT_OBJ_INVALID - Video Device object is invalid
    USB_HOST_UVC_RESULT_FAILURE - An error has occured
  
 Example:
    <code> 
    </code>

 Remarks:

*/
//USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceGetNext
//(
//    USB_HOST_UVC_OBJ videoDeviceObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObjCurrent, 
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ* streamingInterfaceObjNext
//)
//{
// 
//    /* Get Pointer to Video Device Instance */
//    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)videoDeviceObj;
//    uint8_t vSIntrfcIdxCurrent; 
//    uint8_t vSIntrfcIdxNext; 
//
//    /* Get the Streaming Interface Index from the Streaming Interface Object */
//    vSIntrfcIdxCurrent = (uint8_t)(streamingInterfaceObjCurrent >> 8); 
//
//    /* If there is any Video Streaming Interface present in the Video Device */
//    if(videoInstanceInfo->nVSInterfaces == 0)
//    {
//        return USB_HOST_UVC_RESULT_END_OF_INTERFACE_SETTINGS; 
//    }
//    
//    /* Verify Video Streaming Interface Object */
//    if(vSIntrfcIdxCurrent >= videoInstanceInfo->nVSInterfaces)
//    {
//        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
//    }
//    
//    /* Get the Index of the next Streaming Interface */ 
//    vSIntrfcIdxNext = vSIntrfcIdxCurrent+1; 
//
//    /* Check if next Streaming Interface exists */
//    if(vSIntrfcIdxNext == videoInstanceInfo->nVSInterfaces)
//    {
//        return USB_HOST_UVC_RESULT_END_OF_STREAMING_INTERFACE; 
//    }
//    *streamingInterfaceObjNext = (USB_HOST_UVC_STREAMING_INTERFACE_OBJ)(videoInstanceInfo->index | (uint32_t)vSIntrfcIdxNext << 8); 
//
//    return USB_HOST_UVC_RESULT_SUCCESS; 
//}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceSettingGetFirst
    (
        USB_HOST_UVC_OBJ videoDeviceObj,
        USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
        USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ *interfaceSettingObj
    )

 Summary:
    Returns information about first video stream in the specified Video stream 
    group. 

 Description:
    This function returns information about first video stream in the specified
    Video stream group. The stream group index is parameter to this function 
    and it can be any value starting from Zero to number of stream groups minus
    one. Number of stream groups can be obtained by using 
    USB_HOST_UVC_NumberOfStreamGroupsGet() function. 
 
    streamInfo object is an out parameter to this function. 
 
 Precondition:
    Video device should have been attached to the Host. 

 Parameters:
    videoDeviceObj - Video client driver object

    streamGroupIndex - Stream Group Index. 
 
    streamInfo  - Pointer to streamInfo object 

 Returns:
    USB_HOST_UVC_STREAM_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_RESULT_OBJ_INVALID - The specified Video client
    driver object does not exist.
    USB_HOST_UVC_STREAM_RESULT_FAILURE - An unknown failure occurred.

 Example:
    <code>
    </code>

 Remarks:
    None.  
*/
//USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceSettingGetFirst
//(
//    USB_HOST_UVC_OBJ videoDeviceObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ *interfaceSettingObj
//)
//{
//    USB_HOST_UVC_INSTANCE* videoInstanceInfo = (USB_HOST_UVC_INSTANCE*)videoDeviceObj;
//    uint8_t vSIntrfcIdx;
//
//    /* NULL check */
//    if(videoInstanceInfo == NULL)
//    {
//        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
//    }
//    
//    if(interfaceSettingObj == NULL)
//    {
//        return USB_HOST_UVC_RESULT_INVALID_PARAMETER; 
//    }
//    
//    /* Get the Streaming Interface Index from the Streaming Interface Object */
//    vSIntrfcIdx = (uint8_t)(streamingInterfaceObj >> 8); 
//
//    /* Verify Video Streaming Interface Object */
//    if(vSIntrfcIdx >= videoInstanceInfo->nVSInterfaces)
//    {
//        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
//    }
//    /* Fill the Video streaming interface setting object */
//    *interfaceSettingObj = (USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ)(videoInstanceInfo->index | vSIntrfcIdx << 8);
//
//    /* Return success */
//    return USB_HOST_UVC_RESULT_SUCCESS;  
//}
 
// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceSettingGetNext
    (
        USB_HOST_UVC_OBJ videoDeviceObj,
        USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
        USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ interfaceSettingObjCurrent, 
        USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ *interfaceSettingObjNext
    )

 Summary:
    Returns information about next video stream in the specified Video stream 
    group.

 Description:
    This function returns information about next video stream in the specified
    Video stream group. USB_HOST_UVC_StreamGetFirst() function should 
    have been called at least once on the same Video stream group before calling 
    this function. Then calling this function repeatedly on the stream group 
    will return information about the next video stream in the stream group. 
    When there are no more video streams to report, the function returns 
    USB_HOST_UVC_RESULT_END_OF_STREAM_LIST. 
 
    Calling the USB_HOST_UVC_StreamGetFirst() function on the stream group
    index after the USB_HOST_UVC_StreamGetNext() function has been called 
    will cause Video client driver to reset the video stream group to point
    to the first stream in the stream group.
 
 Precondition:
    The USB_HOST_UVC_StreamGetFirst() function must have been called
    before calling this function.

 Parameters:
    videoDeviceObj - Video client driver object

    streamGroupIndex - Stream Group Index. 
 
    streamInfo  - Pointer to streamInfo object 

 Returns:
    USB_HOST_UVC_STREAM_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_RESULT_OBJ_INVALID - The specified Video client
    driver object does not exist.
    USB_HOST_UVC_STREAM_RESULT_FAILURE - An unknown failure occurred.
    USB_HOST_UVC_RESULT_END_OF_STREAM_LIST - There are no more video 
    streams in the stream group. 
 
 Example:
    <code>
    </code>

 Remarks:
    None.
*/ 
//USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceSettingGetNext
//(
//    USB_HOST_UVC_OBJ videoDeviceObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ interfaceSettingObjCurrent, 
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ *interfaceSettingObjNext
//)
//{
//    USB_HOST_UVC_INSTANCE* videoInstanceInfo = (USB_HOST_UVC_INSTANCE*)videoDeviceObj;
//    uint32_t vSIntrfcIdx;
//    uint32_t aSIntrfcSettingIdxCurrent, aSIntrfcSettingIdxNext;
//
//    /* NULL check */
//    if(videoInstanceInfo == NULL)
//    {
//        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
//    }
//    
//    if(interfaceSettingObjNext==NULL)
//    {
//        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
//    }
//    
//    /* Get the Streaming Interface Index from the Streaming Interface Object */
//    vSIntrfcIdx = (uint8_t)(interfaceSettingObjCurrent >> 8); 
//
//    /* Verify Video Streaming Interface Object */
//    if(vSIntrfcIdx >= videoInstanceInfo->nVSInterfaces)
//    {
//        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
//    }
//    
//    /* Get the Interface Setting Index */
//    aSIntrfcSettingIdxCurrent = (uint8_t)(interfaceSettingObjCurrent >> 16); 
//
//    /* Verify Video streaming interface Setting Index */ 
//    if(aSIntrfcSettingIdxCurrent >= videoInstanceInfo->streamInf[vSIntrfcIdx].nInterfaceSetting)
//    {
//        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
//    }
//    
//    aSIntrfcSettingIdxNext = aSIntrfcSettingIdxCurrent + 1; 
//    
//    if(aSIntrfcSettingIdxNext == videoInstanceInfo->streamInf[vSIntrfcIdx].nInterfaceSetting)
//    {
//        return USB_HOST_UVC_RESULT_END_OF_INTERFACE_SETTINGS; 
//    }
//    
//    /* Fill the Video streaming interface setting object */
//    *interfaceSettingObjNext = (USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ)(videoInstanceInfo->index | vSIntrfcIdx << 8 | aSIntrfcSettingIdxNext << 16 ); 
//
//    /* return success */
//    return USB_HOST_UVC_RESULT_SUCCESS; 
//}

// ****************************************************************************
/* Function:
    USB_HOST_UVC_STREAM_HANDLE USB_HOST_UVC_StreamOpen
    ( 
        USB_HOST_UVC_STREAM_OBJ videoStreamObj 
    );
   
 Summary:
    This function opens the specified Video Stream. 

 Description:
    This function will open the specified Video Stream. Once opened, the Video
    stream can be accessed via the handle which this function returns. The
    videoStreamObj parameter is the value returned in the
    USB_HOST_UVC_StreamGetFirst() or USB_HOST_UVC_StreamGetNext()
    functions.

 Precondition:
    Video stream object should be valid.

 Input:
    videoStreamObj - Video Stream object. 

 Return:
    Will return a valid handle if the video stream could be opened successfully,
    else will return USB_HOST_UVC_STREAM_RESULT_HANDLE_INVALID. The 
    function will return a valid handle if the stream is ready to be opened.

 Example:
    <code>
    </code>

 Remarks:
    None.            
*/
USB_HOST_UVC_STREAM_HANDLE USB_HOST_UVC_StreamOpen
(
    USB_HOST_UVC_STREAMING_INTERFACE_OBJ videostreamingInterfaceObj
)
{
    USB_HOST_UVC_STREAM_HANDLE streamHandle = USB_HOST_UVC_STREAM_HANDLE_INVALID; 

    /* The present implementation is a single client implementation only */
    streamHandle = (USB_HOST_UVC_STREAM_HANDLE)videostreamingInterfaceObj; 

    return streamHandle; 
}

// ****************************************************************************
/* Function:
    void USB_HOST_UVC_StreamClose
    ( 
        USB_HOST_UVC_STREAM_HANDLE videoSteamHandle
    );
   
 Summary:
    This function closes the Video Stream. 

 Description:
    This function will close the open Video Stream. This closes the association
    between the application entity that opened the video stream and the video 
    stream. The video stream handle becomes invalid.

 Precondition:
    None.

 Input:
    videoSteamHandle - handle to the video stream obtained from the
    USB_HOST_UVC_StreamOpen() function.

 Return:
    None.
 
 Example:
    <code>
    </code>

 Remarks:
    The device handle becomes invalid after calling this function.            
*/
//void USB_HOST_UVC_StreamClose
//(
//    USB_HOST_UVC_STREAM_HANDLE videoSteamHandle
//)
//{
//    USB_HOST_UVC_INSTANCE *videoInstanceInfo; 
//    USB_HOST_UVC_STREAMING_INTERFACE *vsInterface;
//    uint8_t videoInstanceIndex;
//    uint8_t vsInterfaceIndex; 
//
//    /* Find Video Stream from videoStreamObj */
//    if(videoSteamHandle != USB_HOST_UVC_STREAM_HANDLE_INVALID)
//    {
//        videoInstanceIndex = (uint8_t)videoSteamHandle;
//        vsInterfaceIndex = (uint8_t)(videoSteamHandle >> 8); 
//
//        videoInstanceInfo = &gUSBHostVideoInstance[videoInstanceIndex];
//        vsInterface = &videoInstanceInfo->streamInf[vsInterfaceIndex]; 
//
//        if(vsInterface->streamEventHandler != (USB_HOST_UVC_STREAM_EVENT_HANDLER)NULL)
//        {
//            vsInterface->streamEventHandler = NULL;
//            vsInterface->context = (uintptr_t)NULL; 
//        }
//
//    }
//}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceSet
    (
        USB_HOST_UVC_STREAM_HANDLE streamHandle, 
        USB_HOST_UVC_REQUEST_HANDLE * requestHandle
    ); 

 Summary:
    Schedules a SET_INTERFACE request to the specified video stream. 

 Description:
    This function schedules an Video stream enable request for the specified 
    video stream. An video stream must be enable before scheduling any data 
    transfer with the stream. An event 
    USB_HOST_UVC_STREAM_EVENT_ENABLE_COMPLETE is generated when this 
    request is completed. USB_HOST_UVC_STREAM_EVENT_ENABLE_COMPLETE_DATA 
    returns the status and request handle of the request. 

  
 Precondition:
    Video stream should have been opened. Only one video stream from an video
    stream group can be enabled at a time. 

 Parameters:
    streamHandle - Handle to the Video Stream.
    requestHandle - Handle to the Stream Enable request. 

 Returns:
    USB_HOST_UVC_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_RESULT_HANDLE_INVALID - The specified video 
    stream does not exist.
    USB_HOST_UVC_RESULT_FAILURE - An unknown failure occurred.

 Example:
    <code>
    </code>

 Remarks:
    None.
*/
USB_HOST_UVC_RESULT USB_HOST_UVC_StreamingInterfaceSet
(
    USB_HOST_UVC_STREAM_HANDLE streamHandle, 
    USB_HOST_UVC_REQUEST_HANDLE *requestHandle,
    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ interfaceSettingObj
)
{
    uint8_t videoInstance; 
    uint8_t vSIntrfcIdx; 
    uint8_t asIntrfcSettingIdx; 
    uint8_t alternateSetting; 
    USB_HOST_RESULT hostResult; 
    USB_HOST_UVC_RESULT result; 
    USB_HOST_UVC_INSTANCE* videoInstanceInfo; 

    /* Retrieve the Video Instance Array Index from the Stream Handle */
    videoInstance = (uint8_t)interfaceSettingObj; 
    /* Retrieve the Video Interface Array Index */
    vSIntrfcIdx = (uint8_t)(interfaceSettingObj >> 8);
    /* Retrieve the Video Interface Setting Array Index */
    asIntrfcSettingIdx = (uint8_t)(interfaceSettingObj >> 16); 
    /* Get pointer to Video Instance */
    videoInstanceInfo = &gUSBHostVideoInstance[videoInstance];

    /* Perform NULL check on pointer to Video Instance */
    if(videoInstanceInfo == NULL)
    {
        /* Pointer to Video Instance is invalid. Cannot proceed. */
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    
    /* Check if there is any Video Streaming Interface present in the Video Device */
    if(videoInstanceInfo->nVSInterfaces == 0)
    {
        /* The Video instance has no Streaming interfaces. Cannot proceed. */
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    
    /* Verify Video Streaming Interface Object */
    if(vSIntrfcIdx >= videoInstanceInfo->nVSInterfaces)
    {
        /* Video streaming interface array index is invalid. Cannot proceed. */
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    /* Verify Video streaming interface Setting Array Index */
    if(asIntrfcSettingIdx >= videoInstanceInfo->streamInf[vSIntrfcIdx].nInterfaceSetting)
    {
        /* Video streaming interface array index is invalid. Cannot proceed. */
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    
    /* Retrieve the alternate Setting Number from the Video Instance */
    alternateSetting = videoInstanceInfo->streamInf[vSIntrfcIdx].videoStreamSetting[asIntrfcSettingIdx].interfaceAlternateSetting; 
    
    static bool flag=false;
    if(flag==false)
    {
        alternateSetting=0;
        flag=true;
    }
    else
    {
        alternateSetting=uvcAlternalte[uvcAlternateInterfaceIndex].index;
    }
    
    SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "\r\n*alternateSetting:%d/%d\r\n",alternateSetting,videoInstanceInfo->streamInf[vSIntrfcIdx].nInterfaceSetting);
  
    /* Place an Set Interface Request to the USB Host Layer */
    hostResult = USB_HOST_DeviceInterfaceSet
                    (
                        videoInstanceInfo->streamInf[vSIntrfcIdx].vsInterfaceHandle, 
                        requestHandle, 
                        alternateSetting,
                        (uintptr_t)alternateSetting | (uintptr_t)USB_HOST_UVC_API_VERSION_FLAG_STREAM_INTERFACE_SET << 8
                    ); 
 
    switch(hostResult)
    {
        case USB_HOST_RESULT_SUCCESS:
            result = USB_HOST_UVC_RESULT_SUCCESS;
            break; 
        case USB_HOST_RESULT_REQUEST_BUSY:
            result = USB_HOST_UVC_RESULT_BUSY; 
            break; 
        default:
            result = USB_HOST_UVC_RESULT_FAILURE; 
            break; 
    }    
    return result; 
}
// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_ControlEntityGetFirst
    (
        USB_HOST_UVC_OBJ videoObj, 
        USB_HOST_UVC_CONTROL_ENTITY_OBJ * pEntityHandle
    ); 

 Summary:
    Retrieves handle to the first Video Control entity 

 Description:
    This function retrieves handle to first Video Control entity. 
 
 Precondition:
 

 Parameters:
    videoObj  - USB Host Video device object. 

    pEntityHandle - pointer to the Video control entity handle

 Returns:
    USB_HOST_UVC_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_RESULT_END_OF_CONTROL_ENTITY - No more video control
    entity. 
    USB_HOST_UVC_RESULT_OBJ_INVALID - The specified video 
    stream does not exist.
    USB_HOST_UVC_RESULT_FAILURE - An unknown failure occurred.

 Example:
 <code>
 </code>
 Remarks:
 None.

*/
//USB_HOST_UVC_RESULT USB_HOST_UVC_ControlEntityGetFirst
//(   
//    USB_HOST_UVC_OBJ videoDeviceObj, 
//    USB_HOST_UVC_CONTROL_ENTITY_OBJ *pEntityHandle
//)
//{
//    uint8_t *pDescriptor; 
//
//    /* Get Pointer to Video Device Instance */
//    USB_HOST_UVC_INSTANCE* videoInstanceInfo = (USB_HOST_UVC_INSTANCE*)videoDeviceObj;
//
//    /* Null check */
//    if(videoInstanceInfo == NULL)
//    {
//        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
//    }
//    
//    /* Get pointer to Video Control Descriptor */
//    pDescriptor = videoInstanceInfo->pVideoControlDescriptor; 
//
//    if(pDescriptor ==NULL)
//    {
//        return USB_HOST_UVC_RESULT_FAILURE; 
//    }
//    
//    /* First Descriptor in the Video Control is always the Header. Get the next one */
//    *pEntityHandle = (USB_HOST_UVC_CONTROL_ENTITY_OBJ)((uint8_t *)pDescriptor + ((USB_INTERFACE_DESCRIPTOR*)pDescriptor)->bLength);
//
//    /* Return Success */
//    return USB_HOST_UVC_RESULT_SUCCESS; 
//}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_REESULT USB_HOST_UVC_ControlEntityGetNext
    (
        USB_HOST_UVC_OBJ videoObj, 
        USB_HOST_UVC_CONTROL_ENTITY_OBJ entityHandleCurrent,
        USB_HOST_UVC_CONTROL_ENTITY_OBJ *pEntityHandle
    ); 

 Summary:
    Retrieves handle to the next Video Control entity.

 Description:
    This function retrieves handle to the next Video Control entity. 
 
 Precondition:
 
 Parameters:
    videoObj  - USB Host Video device object. 

    entityHandleCurrent - Handle to current Video control entity. 

    pEntityHandle - pointer to Video control entity Handle. 

 Returns:
    USB_HOST_UVC_RESULT_SUCCESS - The operation was successful

    USB_HOST_UVC_RESULT_END_OF_CONTROL_ENTITY - No more video control
    entity. 
    USB_HOST_UVC_RESULT_OBJ_INVALID - The specified video 
    stream does not exist.
    USB_HOST_UVC_RESULT_FAILURE - An unknown failure occurred.
 
 Example:
    <code>
    </code>

 Remarks:
    None.

*/
USB_HOST_UVC_RESULT USB_HOST_UVC_ControlEntityGetNext
(
    USB_HOST_UVC_OBJ videoDeviceObj, 
    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObjectCurrent,
    USB_HOST_UVC_CONTROL_ENTITY_OBJ *pEntityObject
)
{ 
    USB_UVC_VC_INTERFACE_DESCRIPTOR *pDescriptor = (USB_UVC_VC_INTERFACE_DESCRIPTOR *) entityObjectCurrent; 

    /* Get Pointer to Video Device Instance */
    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)videoDeviceObj;

    /* Null check */
    if(videoInstanceInfo == NULL)
    {
      return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    
    if(pDescriptor == NULL)
    {
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    *pEntityObject = (USB_HOST_UVC_CONTROL_ENTITY_OBJ)((uint8_t *)pDescriptor + ((USB_INTERFACE_DESCRIPTOR*)pDescriptor)->bLength);

    return USB_HOST_UVC_RESULT_SUCCESS;
}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_EntityObjectGet
    (
        USB_HOST_UVC_OBJ videoObj,
        uint8_t entityId,
        USB_HOST_UVC_CONTROL_ENTITY_OBJ* entityObj
    ); 

 Summary:
     Retrieves Entity Object for Entity ID.

 Description:
    This function retrieves Entity Object for Entity ID. 
 
 Parameters:
    videoObj  - USB Host Video device object. 
 
    entityId - Entity ID

    entityObject - Video control entity Object 

 Returns:
     USB_HOST_UVC_RESULT_SUCCESS - The operation was successful
 
    USB_HOST_UVC_RESULT_FAILURE - Entity Id could not e found. Or 
    An unknown failure occurred.
 
 Example:
    <code>
    </code>

 Remarks:
    None.

*/
USB_HOST_UVC_RESULT USB_HOST_UVC_EntityObjectGet
(
    USB_HOST_UVC_OBJ videoDeviceObj,
    uint8_t entityId, 
    USB_HOST_UVC_CONTROL_ENTITY_OBJ* entityObj
)
{
    uint8_t *pDescriptor; 
    uint16_t descriptorCount; 
    USB_HOST_UVC_RESULT videoResult = USB_HOST_UVC_RESULT_FAILURE; 

    /* Get Pointer to video Device Instance */
    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)videoDeviceObj;

    /* Null check */
    if(videoInstanceInfo == NULL)
    {
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    
    /* Get pointer to video Control Descriptor */
    pDescriptor = videoInstanceInfo->pVideoControlDescriptor;
    pDescriptor = pDescriptor + ((USB_INTERFACE_DESCRIPTOR *)pDescriptor)->bLength; 
    descriptorCount = ((USB_UVC_VC_INTERFACE_DESCRIPTOR *)pDescriptor)->wTotalLength; 
    pDescriptor = pDescriptor + ((USB_INTERFACE_DESCRIPTOR *)pDescriptor)->bLength;

    while(descriptorCount)
    {
        if(entityId == ((USB_HOST_UVC_CONTROL_ENTITY_DESCRIPTOR_HEADER*)pDescriptor)->entityID)
        {
            *entityObj = (USB_HOST_UVC_CONTROL_ENTITY_OBJ)pDescriptor;
            videoResult = USB_HOST_UVC_RESULT_SUCCESS; 
            break; 
        }

        pDescriptor = pDescriptor + ((USB_INTERFACE_DESCRIPTOR *)pDescriptor)->bLength; 
        descriptorCount = descriptorCount - ((USB_INTERFACE_DESCRIPTOR *)pDescriptor)->bLength; 
    }
    return videoResult; 
}

// *****************************************************************************
/* Function:
    USB_UVC_ENTITY_TYPE USB_HOST_UVC_EntityTypeGet(
        USB_HOST_UVC_OBJ videoObj, 
        USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
    ); 

 Summary:
    Returns Entity Type of the Video Control entity.

 Description:
    This function returns Entity Type of the Video Control entity. Prior to
    calling this function Entity Object should obtained by calling
    USB_HOST_UVC_ControlEntityGetFirst(),
    USB_HOST_UVC_ControlEntityGetNext() or
    USB_HOST_UVC_EntityObjectGet() function. 

 Parameters:
    videoObj  - USB Host Video device object. 

    entityObject - Video control entity Object 

 Returns:
    USB_UVC_ENTITY_TYPE 
 
 Example:
    <code>
    </code>

 Remarks:
    None.

*/
USB_UVC_ENTITY_TYPE USB_HOST_UVC_EntityTypeGet
(
    USB_HOST_UVC_OBJ videoDeviceObj, 
    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
)
{
    USB_UVC_ENTITY_TYPE entityType; 

    USB_UVC_VC_INTERFACE_DESCRIPTOR *pDescriptor = (USB_UVC_VC_INTERFACE_DESCRIPTOR *)entityObject; 

    if(pDescriptor == NULL)
    {
        return 0; 
    }
    entityType = pDescriptor->bDescriptorSubtype; 

    return entityType; 
}

// *****************************************************************************
/* Function:
    USB_UVC_TERMINAL_TYPE USB_HOST_UVC_TerminalTypeGet
    (
        USB_HOST_UVC_OBJ videoObj,
        USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
    ); 

 Summary:
    Returns Terminal Type of the Video Control entity.

 Description:
    This function returns Terminal type of the Video Control entity. Prior to
    calling this function Entity Object should obtained by calling
    USB_HOST_UVC_ControlEntityGetFirst(),
    USB_HOST_UVC_ControlEntityGetNext() or
    USB_HOST_UVC_EntityObjectGet() function. 
  
 Parameters:
    videoObj  - USB Host Video device object. 

    entityObject - Video control entity Object 

 Returns:
    Terminal Type 
 
 Example:
    <code>
    </code>

 Remarks:
    None.

*/
//USB_UVC_TERMINAL_TYPE USB_HOST_UVC_TerminalTypeGet
//(
//    USB_HOST_UVC_OBJ videoObj,
//    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
//)
//{
//    USB_UVC_TERMINAL_TYPE terminalType; 
//
//    USB_HOST_UVC_TERMINAL_HEADER_DESCRIPTOR *pDescriptor = (USB_HOST_UVC_TERMINAL_HEADER_DESCRIPTOR *) entityObject;
//
//    if(pDescriptor == NULL)
//    {
//        return 0; 
//    }
//    terminalType = pDescriptor->wTerminalType; 
//
//    return terminalType; 
//}

// *****************************************************************************
/* Function:
    uint8_t USB_HOST_UVC_TerminalIDGet
    (
        USB_HOST_UVC_OBJ videoObj,
        USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
    ); 

 Summary:
    Returns Terminal ID of the Video Control entity.

 Description:
    This function returns Terminal ID of the Video Control entity. Prior to
    calling this function Entity Object should obtained by calling
    USB_HOST_UVC_ControlEntityGetFirst(),
    USB_HOST_UVC_ControlEntityGetNext() or
    USB_HOST_UVC_EntityObjectGet() function. 
  
 Parameters:
    videoObj  - USB Host Video device object. 

    entityObject - Video control entity Object 

 Returns:
    The terminal ID.
 
 Example:
    <code>
    </code>

 Remarks:
    None.

*/
//uint8_t USB_HOST_UVC_TerminalIDGet
//(
//    USB_HOST_UVC_OBJ videoObj,
//    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
//)
//{
//    uint8_t terminalID; 
//
//    USB_HOST_UVC_TERMINAL_HEADER_DESCRIPTOR *pDescriptor = (USB_HOST_UVC_TERMINAL_HEADER_DESCRIPTOR *) entityObject;
//
//    if(pDescriptor == NULL)
//    {
//        return 0; 
//    }
//    terminalID = pDescriptor->bTerminalID; 
//
//    return terminalID;
//}

// *****************************************************************************
/* Function:
    uint8_t USB_HOST_UVC_TerminalAssociationGet
    (
        USB_HOST_UVC_OBJ videoObj,
        USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
    ); 

 Summary:
    Returns Associated Terminal of the Video Control Terminal.

 Description:
    This function returns ID Associated Terminal type of the Video Control
    Terminal. Prior to calling this function Entity Object should obtained by
    calling USB_HOST_UVC_ControlEntityGetFirst(),
    USB_HOST_UVC_ControlEntityGetNext() or
    USB_HOST_UVC_EntityObjectGet() function. 
  
 Parameters:
    videoObj  - USB Host Video device object. 

    entityObject - Video control entity Object 

 Returns:
    ID of the Associated Terminal 
 
 Example:
    <code>
    </code>

 Remarks:
    None.

*/
//uint8_t USB_HOST_UVC_TerminalAssociationGet
//(
//    USB_HOST_UVC_OBJ videoObj, 
//    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
//)
//{
//    uint8_t bAssocTerminal; 
//
//    USB_HOST_UVC_TERMINAL_HEADER_DESCRIPTOR *pDescriptor = (USB_HOST_UVC_TERMINAL_HEADER_DESCRIPTOR *)entityObject;
//
//    if(pDescriptor == NULL)
//    {
//        return 0; 
//    }
//    bAssocTerminal = pDescriptor->bAssocTerminal; 
//
//    return bAssocTerminal; 
//}


// *****************************************************************************
/* Function:
    uint8_t USB_HOST_UVC_TerminalSourceIDGet
    (
        USB_HOST_UVC_OBJ videoObj,
        USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
    ); 

 Summary:
    Returns ID of the Unit or Terminal to which this Terminal is connected.

 Description:
    This function returns ID of the Unit or Terminal to which this Terminal is
    connected. This function is only applicable to Output Terminal. Prior to
    calling this function Entity Object should obtained by calling
    USB_HOST_UVC_ControlEntityGetFirst(),
    USB_HOST_UVC_ControlEntityGetNext() or
    USB_HOST_UVC_EntityObjectGet() function. 
  
 Parameters:
    videoObj  - USB Host Video device object. 

    entityObject - Video control entity Object 

 Returns:
    ID of the Unit or Terminal to which this Terminal is connected. 
 
 Example:
    <code>
    </code>

 Remarks:
    None.

*/
//uint8_t USB_HOST_UVC_TerminalSourceIDGet
//(   
//    USB_HOST_UVC_OBJ videoObj, 
//    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
//)
//{
//    uint8_t bSourceID; 
//
//    USB_UVC_OUTPUT_TERMINAL_DESCRIPTOR *pDescriptor = (USB_UVC_OUTPUT_TERMINAL_DESCRIPTOR *)entityObject;
//
//    if(pDescriptor == NULL)
//    {
//        return 0; 
//    }
//    bSourceID = pDescriptor->bSourceID; 
//
//    return bSourceID; 
//}

// *****************************************************************************
/* Function:
    uint8_t USB_HOST_UVC_FeatureUnitIDGet
    (
        USB_HOST_UVC_OBJ videoObj,
        USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
    ); 

 Summary:
    Returns ID of the Feature Unit.

 Description:
    This function returns ID of the D of the Feature Unit. This function is
    only applicable to Feature Unit. Prior to calling this function Entity
    Object should obtained by calling
    USB_HOST_UVC_ControlEntityGetFirst(),
    USB_HOST_UVC_ControlEntityGetNext() or
    USB_HOST_UVC_EntityObjectGet() function. 
  
 Parameters:
    videoObj  - USB Host Video device object. 

    entityObject - Video control entity Object 

 Returns:
    ID of Feature Unit. 
 
 Example:
    <code>
    </code>

 Remarks:
    None.

*/
//uint8_t USB_HOST_UVC_FeatureUnitIDGet
//(
//    USB_HOST_UVC_OBJ videoObj,
//    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
//)
//{
//    uint8_t unitId; 
//
//    USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *pDescriptor = (USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *)entityObject;
//
//    if(pDescriptor == NULL)
//    {
//        return 0; 
//    }
//    unitId = pDescriptor->bUnitID; 
//
//    return unitId;
//}

// *****************************************************************************
/* Function:
    uint8_t USB_HOST_UVC_FeatureUnitSourceIDGet
    (
        USB_HOST_UVC_OBJ videoObj,
        USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
    );

 Summary:
    Returns ID of the Unit or Terminal to which this Feature Unit is connected.

 Description:
    This function returns ID of the Unit or Terminal to which this Feature Unit
    is connected. This function is only applicable to Feature Unit. Prior to
    calling this function Entity Object should obtained by calling
    USB_HOST_UVC_ControlEntityGetFirst(),
    USB_HOST_UVC_ControlEntityGetNext() or
    USB_HOST_UVC_EntityObjectGet() function. 
  
 Parameters:
    videoObj  - USB Host Video device object. 

    entityObject - Video control entity Object 

 Returns:
    ID of the Unit or Terminal to which this Feature Unit is connected. 
 
 Example:
    <code>
    </code>

 Remarks:
    None.

*/
//uint8_t USB_HOST_UVC_FeatureUnitSourceIDGet
//(
//    USB_HOST_UVC_OBJ videoObj,
//    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
//)
//{
//  uint8_t bSourceID; 
// 
//  USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *pDescriptor = (USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *)entityObject;
// 
//  if(pDescriptor == NULL)
//  {
//    return 0;
//  }
//  bSourceID = pDescriptor->bSourceID; 
// 
//  return bSourceID; 
//}

// *****************************************************************************
/* Function:
    uint8_t USB_HOST_UVC_FeatureUnitChannelNumbersGet
    (
        USB_HOST_UVC_OBJ videoObj,
        USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
    ); 

 Summary:
    Returns Number of channels.

 Description:
    This function returns number of channels. This function is only applicable
    to Feature Unit. Prior to calling this function Entity Object should
    obtained by calling USB_HOST_UVC_ControlEntityGetFirst(),
    USB_HOST_UVC_ControlEntityGetNext() or
    USB_HOST_UVC_EntityObjectGet() function. 
  
 Parameters:
    videoObj  - USB Host Video device object. 

    entityObject - Video control entity Object 

 Returns:
    Number of channels. 
 
 Example:
    <code>
    </code>

 Remarks:
    None.

*/
//uint8_t USB_HOST_UVC_FeatureUnitChannelNumbersGet
//(
//    USB_HOST_UVC_OBJ videoObj,
//    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject
//)
//{
//    uint8_t nChannels; 
//    USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *pDescriptor = (USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *)entityObject;
//
//    if(pDescriptor == NULL)
//    {
//        return 0; 
//    }
//    if(pDescriptor->bControlSize == 0)
//      return 0; 
//
//    nChannels = (pDescriptor->bLength - sizeof(USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER) - 1)/pDescriptor->bControlSize - 1; 
//
//    return nChannels; 
//}

// *****************************************************************************
/* Function:
    uint8_t USB_HOST_VIDEO_StreamingInterfaceTerminalLinkGet
    (
        USB_HOST_VIDEO_OBJ audioObj,
        USB_HOST_VIDEO_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
        USB_HOST_VIDEO_STREAMING_INTERFACE_SETTING_OBJ interfaceSettingObj
    );

   Summary:
    This function returns Terminal Link of the specified streaming interface
    setting. 
    
   Description:
    This function returns Terminal Link of the specified streaming interface
    setting. TODO Add more description. 
    
   Precondition:
    Video v1.0 device should have been attached. 

   Parameters:
    videoDeviceObj - Video Device Object

    streamingInterfaceObj - Video Streaming Interface Object

    interfaceSettingObj - Video Streaming Interface Setting Object 

   Returns:
     Terminal Link of the Video Streaming Interface setting 
     
   Example:
    <code> 
    </code>

   Remarks:

*/
//uint8_t USB_HOST_UVC_StreamingInterfaceTerminalLinkGet
//(
//    USB_HOST_UVC_OBJ videoDeviceObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ interfaceSettingObj
//)
//{
//    uint8_t videoInstance; 
//    uint8_t vSIntrfcIdx; 
//    uint8_t vsIntrfcSettingIdx; 
//    USB_HOST_UVC_INSTANCE* videoInstanceInfo; 
//
//    videoInstance = (uint8_t)interfaceSettingObj; 
//    vSIntrfcIdx = (uint8_t)(interfaceSettingObj >> 8); 
//    vsIntrfcSettingIdx = (uint8_t)(interfaceSettingObj >> 16);  
//
//    videoInstanceInfo = &gUSBHostVideoInstance[videoInstance];
//
//    if(videoInstanceInfo==NULL)
//    {
//        return 0; 
//    }
//    return (USB_UVC_FORMAT_TAG)videoInstanceInfo->streamInf[vSIntrfcIdx].videoStreamSetting[vsIntrfcSettingIdx].bTerminalLink;
//}

// *****************************************************************************
/* Function:
    USB_UVC_FORMAT_TAG USB_HOST_UVC_StreamingInterfaceFormatTagGet
    (
        USB_HOST_UVC_OBJ videoObj,
        USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
        USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ interfaceSettingObj
    );

 Summary:
    This function returns Format Tag of the specified streaming interface
    setting. 
 
 Description:
    This function returns Format Tag Link of the specified streaming interface
    setting. TODO Add more description. 
 
 Precondition:
    Video device should have been attached. 

 Parameters:
    videoObj - Video Device Object

    streamingInterfaceObj - Video Streaming Interface Object

    interfaceSettingObj - Video Streaming Interface Setting Object 

 Returns:
    Format Tag of the Video Streaming Interface setting 
  
 Example:
    <code> 
    </code>

 Remarks:

*/
//USB_UVC_FORMAT_TAG USB_HOST_UVC_StreamingInterfaceFormatTagGet
//(
//    USB_HOST_UVC_OBJ videoObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ interfaceSettingObj
//)
//{
//    uint8_t videoInstance; 
//    uint8_t vSIntrfcIdx; 
//    uint8_t vsIntrfcSettingIdx; 
//    USB_HOST_UVC_INSTANCE* videoInstanceInfo; 
//
//    videoInstance = (uint8_t)interfaceSettingObj; 
//    vSIntrfcIdx = (uint8_t)(interfaceSettingObj >> 8); 
//    vsIntrfcSettingIdx = (uint8_t)(interfaceSettingObj >> 16); 
//
//    videoInstanceInfo = &gUSBHostVideoInstance[videoInstance];
//
//    if(videoInstanceInfo == NULL)
//    {
//        return USB_UVC_FORMAT_TYPE_I_UNDEFINED; 
//    }
//    return (USB_UVC_FORMAT_TAG)videoInstanceInfo->streamInf[vSIntrfcIdx].videoStreamSetting[vsIntrfcSettingIdx].bBitsPerPixel;
//}

// *****************************************************************************
/* Function:
    uint8_t USB_HOST_UVC_StreamingInterfaceSubFrameSizeGet
    (
        USB_HOST_UVC_OBJ videoObj,
        USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
        USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ interfaceSettingObj
    );

 Summary:
    This function returns Subframe size of the specified streaming interface
    setting. 
 
 Description:
    This function returns Subframe Size of the specified streaming interface
    setting. TODO Add more description. 
 
 Precondition:
    Video device should have been attached. 

 Parameters:
    videoObj - Video Device Object

    streamingInterfaceObj - Video Streaming Interface Object

    interfaceSettingObj - Video Streaming Interface Setting Object 

 Returns:
    Subframe Size of the Video Streaming Interface setting 
  
 Example:
    <code> 
    </code>

 Remarks:

*/
//uint8_t USB_HOST_UVC_StreamingInterfaceSubFrameSizeGet
//(
//    USB_HOST_UVC_OBJ videoObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ interfaceSettingObj
//)
//{
//    uint8_t videoInstance; 
//    uint8_t vSIntrfcIdx; 
//    uint8_t vsIntrfcSettingIdx; 
//    USB_HOST_UVC_INSTANCE *videoInstanceInfo; 
//
//    videoInstance = (uint8_t)interfaceSettingObj; 
//    vSIntrfcIdx = (uint8_t)(interfaceSettingObj >> 8); 
//    vsIntrfcSettingIdx = (uint8_t)(interfaceSettingObj >> 16); 
//
//    videoInstanceInfo = &gUSBHostVideoInstance[videoInstance];
//    if(videoInstanceInfo == NULL)
//    {
//      return 0; 
//    }
//    return videoInstanceInfo->streamInf[vSIntrfcIdx].videoStreamSetting[vsIntrfcSettingIdx].bSubframeSize[0];
//}


// *****************************************************************************
/* Function:
    USB_HOST_UVC_STREAM_DIRECTION USB_HOST_UVC_StreamInterfaceDirectionGet
    (
        USB_HOST_UVC_OBJ videoObj, 
        USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
        USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ interfaceSettingObj
    );

 Summary:
    Returns Direction of the specified streaming interface
    setting. 
 
 Description:
    This function returns Direction of the specified streaming 
    interface setting. TODO Add more description. 
 
 Precondition:
    Video device should have been attached. 

 Parameters:
    videoObj - Video Device Object

    streamingInterfaceObj - Video Streaming Interface Object

    interfaceSettingObj - Video Streaming Interface Setting Object 

 Returns:
    USB_HOST_UVC_DIRECTION_OUT - Host to Device
    USB_HOST_UVC_DIRECTION_IN - Device to Host
  
 Example:
    <code> 
    </code>

 Remarks:

*/
//USB_HOST_UVC_STREAM_DIRECTION USB_HOST_UVC_StreamingInterfaceDirectionGet
//(
//    USB_HOST_UVC_OBJ videoObj, 
//    USB_HOST_UVC_STREAMING_INTERFACE_OBJ streamingInterfaceObj,
//    USB_HOST_UVC_STREAMING_INTERFACE_SETTING_OBJ interfaceSettingObj
//)
//{
//    USB_ENDPOINT_DESCRIPTOR *dataEndpointDescriptor; 
//    uint8_t videoInstance; 
//    uint8_t vSIntrfcIdx; 
//    uint8_t vsIntrfcSettingIdx; 
//    USB_HOST_UVC_STREAM_DIRECTION direction = USB_HOST_UVC_DIRECTION_IN; 
//    USB_HOST_UVC_INSTANCE *videoInstanceInfo; 
//
//    videoInstance = (uint8_t)interfaceSettingObj; 
//    vSIntrfcIdx = (uint8_t)(interfaceSettingObj >> 8); 
//    vsIntrfcSettingIdx = (uint8_t)(interfaceSettingObj >> 16); 
//
//    videoInstanceInfo = &gUSBHostVideoInstance[videoInstance];
//
//    if(videoInstanceInfo==NULL)
//    {
//        return direction; 
//    }
//    dataEndpointDescriptor = &videoInstanceInfo->streamInf[vSIntrfcIdx].videoStreamSetting[vsIntrfcSettingIdx].isoDataEndpointDesc; 
//    direction = dataEndpointDescriptor->dirn; 
//    return direction; 
//}

// *****************************************************************************
// Section: Depreciated APIs - Not recommended for new applications 
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
/* Function:
    uint8_t USB_HOST_UVC_NumberOfStreamGroupsGet
    (
        USB_HOST_UVC_OBJ videoObj
    );

 Summary:
    This function will Get number of stream groups present in the attached 
    Video Device. 

 Description:
    This function will Get number of stream groups present in the attached 
    Video Device. Video stream with in a video streams cannot be enabled
    at the same time. 
 
 Precondition:
    Video device should have been attached. 

 Parameters:
    videoObj - Video client driver object 

 Returns:
    Returned uint8_t indicates number of video stream groups present in the 
    attached Video device. 
 
 Example:
    <code> 
    </code>

 Remarks:

 */
//uint8_t USB_HOST_UVC_NumberOfStreamGroupsGet
//(
//    USB_HOST_UVC_OBJ videoObj
//)
//{
//    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)videoObj; 
//
//    return videoInstanceInfo->nVSInterfaces; 
//}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_STREAM_RESULT USB_HOST_UVC_StreamEnable
    (
        USB_HOST_UVC_STREAM_HANDLE streamHandle, 
        USB_HOST_UVC_REQUEST_HANDLE * requestHandle
    ); 

 Summary:
    Schedules an Video stream enable request for the specified video stream. 

 Description:
    This function schedules an Video stream enable request for the specified 
    video stream. An video stream must be enable before scheduling any data 
    transfer with the stream. An event 
    USB_HOST_UVC_STREAM_EVENT_ENABLE_COMPLETE is generated when this 
    request is completed. USB_HOST_UVC_STREAM_EVENT_ENABLE_COMPLETE_DATA 
    returns the status and request handle of the request. 

  
 Precondition:
    Video stream should have been opened. Only one video stream from an video
    stream group can be enabled at a time. 

 Parameters:
    streamHandle - Handle to the Video Stream.
    requestHandle - Handle to the Stream Enable request. 

 Returns:
    USB_HOST_UVC_STREAM_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_STREAM_RESULT_HANDLE_INVALID - The specified video 
    stream does not exist.
    USB_HOST_UVC_STREAM_RESULT_FAILURE - An unknown failure occurred.

 Example:
    <code>
    </code>

 Remarks:
    None.
*/
//USB_HOST_UVC_STREAM_RESULT USB_HOST_UVC_StreamEnable
//(
//    USB_HOST_UVC_STREAM_HANDLE streamHandle, 
//    USB_HOST_UVC_REQUEST_HANDLE *requestHandle
//)
//{
//    USB_HOST_UVC_INSTANCE *videoInstanceInfo; 
//    USB_HOST_UVC_STREAMING_INTERFACE *vsInterface;
//    uint8_t videoInstanceIndex;
//    uint8_t vsInterfaceIndex; 
//    uint8_t alternateSetting; 
//    USB_HOST_RESULT hostResult; 
//
//    if(streamHandle == USB_HOST_UVC_STREAM_HANDLE_INVALID)
//    {
//        return USB_HOST_UVC_STREAM_RESULT_PARAMETER_INVALID; 
//    }
//    /* Find Video Stream from videoStreamObj */
//    videoInstanceIndex = (uint8_t)streamHandle;
//    vsInterfaceIndex = (uint8_t)(streamHandle >> 8); 
//    alternateSetting = (uint8_t)(streamHandle >> 16);
//
//    videoInstanceInfo = &gUSBHostVideoInstance[videoInstanceIndex]; 
//    vsInterface = &videoInstanceInfo->streamInf[vsInterfaceIndex];
//
//    hostResult = USB_HOST_DeviceInterfaceSet
//                (
//                    vsInterface->vsInterfaceHandle, 
//                    requestHandle, 
//                    alternateSetting,
//                    (uintptr_t)alternateSetting | (uintptr_t)USB_HOST_UVC_API_VERSION_FLAG_STREAM_ENABLE << 8
//                );
//
//    return hostResult; 
//}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_STREAM_RESULT USB_HOST_UVC_StreamDisable
    (
        USB_HOST_UVC_STREAM_HANDLE streamHandle, 
        USB_HOST_UVC_REQUEST_HANDLE * requestHandle
    ); 

 Summary:
     Schedules an Video stream disable request for the specified video stream. 

 Description:
    This function schedules an Video stream disable request for the specified 
    video stream. An event 
    USB_HOST_UVC_STREAM_EVENT_DISABLE_COMPLETE is generated when this
    request is completed. USB_HOST_UVC_STREAM_EVENT_DISABLE_COMPLETE_DATA 
    returns the status and request handle of the request. 

  
 Precondition:
     Video stream should have been opened. 

 Parameters:
    streamHandle - Handle to the Video Stream.
    requestHandle - Handle to the Stream Disable request. 

 Returns:
    USB_HOST_UVC_STREAM_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_STREAM_RESULT_HANDLE_INVALID - The specified video 
    stream does not exist.
    USB_HOST_UVC_STREAM_RESULT_FAILURE - An unknown failure occurred.

 Example:
    <code>
    </code>

 Remarks:
    None.
*/
//USB_HOST_UVC_STREAM_RESULT USB_HOST_UVC_StreamDisable
//(
//    USB_HOST_UVC_STREAM_HANDLE streamHandle,
//    USB_HOST_UVC_REQUEST_HANDLE *requestHandle
//)
//{
//    USB_HOST_UVC_INSTANCE *videoInstanceInfo; 
//    USB_HOST_UVC_STREAMING_INTERFACE *vsInterface;
//    uint8_t videoInstanceIndex;
//    uint8_t vsInterfaceIndex; 
//    uint8_t alternateSetting; 
//    USB_HOST_RESULT hostResult; 
//
//    /* Check if the stream handle is invalid */
//    if(streamHandle == USB_HOST_UVC_STREAM_HANDLE_INVALID)
//    {
//        return USB_HOST_UVC_STREAM_RESULT_PARAMETER_INVALID; 
//    }
//    /* Get Video Instance Index, It is always the LSB */
//    videoInstanceIndex = (uint8_t)streamHandle;
//    /* Get the Streaming Interface Index. It always 2nd LSB */
//    vsInterfaceIndex = (uint8_t)(streamHandle >> 8); 
//    /* Get the Alternate Setting Number. It always 3rd LSB */
//    alternateSetting = (uint8_t)(streamHandle >> 16); 
//    /* Get pointer to Video Instance */
//    videoInstanceInfo = &gUSBHostVideoInstance[videoInstanceIndex]; 
//
//    /* NULL check */
//    if(videoInstanceInfo == NULL)
//    {
//        return USB_HOST_UVC_STREAM_RESULT_PARAMETER_INVALID; 
//    }
//    /* Get pointer to Video Streaming Interface */
//    vsInterface = &videoInstanceInfo->streamInf[vsInterfaceIndex]; 
//
//    /* NULL check */
//    if(vsInterface == NULL)
//    {
//        return USB_HOST_UVC_STREAM_RESULT_PARAMETER_INVALID; 
//    }
//    /* Submit Set Interface request to Host Layer */
//    hostResult = USB_HOST_DeviceInterfaceSet
//                (
//                    vsInterface->vsInterfaceHandle, 
//                    requestHandle, 
//                    0,
//                    (uintptr_t)alternateSetting | (uintptr_t)USB_HOST_UVC_API_VERSION_FLAG_STREAM_DISABLE << 8
//                ); 
//
//    return hostResult; 
//}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_ControlRequest
    (
        USB_HOST_UVC_OBJ videoObj,
        USB_HOST_UVC_REQUEST_HANDLE * requestHandle,
        USB_SETUP_PACKET *setupPacket,
        void * data,
        USB_HOST_UVC_CONTROL_CALLBACK callback, 
        uintptr_t context 
    );

 Summary:
     Schedules a Video control transfer.

 Description:
    This function schedules an Video control transfer. videoObj is an 
    Object of Video class driver to which the video control transfer is to
    be scheduled. setupPacket points to the setup command to be sent in the 
    Setup Stage of the control transfer. The size and the direction of the data
    stage is indicated by the setup packet. In case of control transfers where
    there is no data stage, data is ignored and can be NULL. In all other cases,
    data should point to the data to data be transferred in the data stage of 
    the control transfer. 
 
    If the transfer was scheduled successfully, requestHandle will contain a
    transfer handle that uniquely identifies this transfer. If the transfer
    could not be scheduled successfully, requestHandle will contain
    USB_HOST_UVC_REQUEST_HANDLE_INVALID.

    When the control transfer completes, the Video client driver will call
    the specified callback function. The context parameter specified here will 
    be returned in the callback.

 Precondition:
    Video Device should have attached. 

 Parameters:
    videoObj - Video client driver object 

    requestHandle - output parameter that will contain the handle to this
    transfer.

    setupPacket - Pointer to the setup packet to sent to the device in the setup
    stage of the control transfer.

    data - For control transfer with a data stage, this should point to data to
    be sent to the device (for a control write transfer) or point to the buffer
    that will receive data from the device (for a control read transfer). For
    control transfers that do not require a data stage, this parameter is
    ignored and can be NULL.

    callback - pointer to the callback function that will be called when the
    control transfer completes. If the callback function is NULL, there will be
    no notification of when the control transfer will complete.

    context - user defined context that is returned with the callback function.

 Returns:
    USB_HOST_UVC_RESULT_SUCCESS - the transfer was scheduled successfully.
    requestHandle will contain a valid transfer handle.
    USB_HOST_UVC_RESULT_FAILURE - an unknown failure occurred. requestHandle will
    contain USB_HOST_UVC_REQUEST_HANDLE_INVALID.
    USB_HOST_UVC_RESULT_PARAMETER_INVALID - The data pointer or requestHandle pointer
    is NULL.

 Example:
    <code>
    </code>

 Remarks:
    None.
*/
USB_HOST_UVC_RESULT USB_HOST_UVC_ControlRequest
(
    USB_HOST_UVC_OBJ videoObj,
    USB_HOST_UVC_REQUEST_HANDLE *transferHandle, 
    USB_SETUP_PACKET *setupPacket,
    void *data, 
    USB_HOST_UVC_CONTROL_CALLBACK callback, 
    uintptr_t context
)
{
    USB_HOST_UVC_RESULT result = USB_HOST_UVC_RESULT_FAILURE; 
    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)videoObj;
    OSAL_CRITSECT_DATA_TYPE IntState;

    /* Check if the handle to the video Instance in not NULL */
    if(videoInstanceInfo == NULL)
    {
        SYS_DEBUG_MESSAGE ( SYS_DEBUG_INFO, "USB_HOST_VIDEO_ControlSend: Not a valid instance " );
        return USB_HOST_UVC_RESULT_OBJ_INVALID;
    }

    /* Check if this Video Instance is initialized */
    if(videoInstanceInfo->assigned == false)
    {
        SYS_DEBUG_MESSAGE ( SYS_DEBUG_INFO , "USB_HOST_VIDEO_ControlSend: Invalid parameters " );
        return USB_HOST_UVC_RESULT_FAILURE;
    }

    if(videoInstanceInfo->videoControlObj.inUse == true)
    {
        return USB_HOST_UVC_RESULT_BUSY; 
    }
    /* Prevent other tasks pre-empting this sequence of code */ 
    IntState = OSAL_CRIT_Enter(OSAL_CRIT_TYPE_HIGH);
    memcpy(&videoInstanceInfo->setupPacket, setupPacket, sizeof(USB_SETUP_PACKET)); 
    videoInstanceInfo->videoControlObj.inUse = true; 
    videoInstanceInfo->videoControlObj.context = context; 
    videoInstanceInfo->videoControlObj.callback = (USB_HOST_UVC_ENTITY_REQUEST_CALLBACK)callback; 
    OSAL_CRIT_Leave(OSAL_CRIT_TYPE_HIGH, IntState);
  
    result = USB_HOST_DeviceControlTransfer
            (
                videoInstanceInfo->controlPipeHandle,
                transferHandle, 
                &videoInstanceInfo->setupPacket, 
                (void *)data, 
                _USB_HOST_UVC_ControlRequestCallback, 
                (uintptr_t)videoInstanceInfo
            );  
    return result;
}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_StreamGetFirst
    (
        USB_HOST_UVC_OBJ videoDeviceObj, 
        uint8_t streamGroupIndex, 
        USB_HOST_UVC_STREAM_INFO * streamInfo
        );

 Summary:
    Returns information about first video stream in the specified Video stream 
    group. 

 Description:
    This function returns information about first video stream in the specified
    Video stream group. The stream group index is parameter to this function 
    and it can be any value starting from Zero to number of stream groups minus
    one. Number of stream groups can be obtained by using 
    USB_HOST_UVC_NumberOfStreamGroupsGet() function. 
 
    streamInfo object is an out parameter to this function. 
 
 Precondition:
     Video device should have been attached to the Host. 

 Parameters:
    videoDeviceObj - Video client driver object

    streamGroupIndex - Stream Group Index. 
 
    streamInfo  - Pointer to streamInfo object 

 Returns:
    USB_HOST_UVC_STREAM_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_RESULT_OBJ_INVALID - The specified Video client
    driver object does not exist.
    USB_HOST_UVC_STREAM_RESULT_FAILURE - An unknown failure occurred.

 Example:
    <code>
    </code>

 Remarks:
    None.
*/
USB_HOST_UVC_RESULT USB_HOST_UVC_StreamGetFirst
(
    USB_HOST_UVC_OBJ videoDeviceObj, 
    uint8_t streamGroupIndex, 
    USB_HOST_UVC_STREAM_INFO *streamInfo
)
{
    USB_HOST_UVC_STREAMING_INTERFACE *vsInterface; 
    USB_HOST_UVC_STREAM_SETTING *videoStream; 
    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE*)videoDeviceObj;
    uint8_t alternateSetting; 
    //int count; 
 
    if(videoInstanceInfo == NULL)
    {
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    /* Get pointer to First Stream from the Stream Group */
    vsInterface = &videoInstanceInfo->streamInf[streamGroupIndex]; 

    if(vsInterface->nInterfaceSetting == 0)
    {
        return USB_HOST_UVC_RESULT_FAILURE; 
    }
    /* First Stream is always the alternate setting 1 */
    alternateSetting = 1; 
    videoStream = &vsInterface->videoStreamSetting[alternateSetting]; 
    /* Stream Object is made from 
    LSB = video instance index 
    Second byte = Stream Interface Index
    Third byte = Alternate Setting
    MSB = 0x01 */
    streamInfo->streamObj = (USB_HOST_UVC_STREAM_OBJ) (uint32_t)videoInstanceInfo->index |
                            ((uint32_t)streamGroupIndex) << 8 |
                            ((uint32_t)alternateSetting << 16); 
    //  streamInfo->format = videoStream->wFormatTag;
    streamInfo->streamDirection = videoStream->direction;
    streamInfo->subFrameSize = videoStream->bSubframeSize[0];

    return USB_HOST_UVC_RESULT_SUCCESS;  
}

// *****************************************************************************
/* Function:
    USB_HOST_UVC_RESULT USB_HOST_UVC_StreamGetNext
    (
        USB_HOST_UVC_OBJ videoDeviceObj, 
        uint8_t streamGroupIndex,
        USB_HOST_UVC_STREAM_INFO * streamInfo
    );

 Summary:
    Returns information about next video stream in the specified Video stream 
    group.

 Description:
    This function returns information about next video stream in the specified
    Video stream group. USB_HOST_UVC_StreamGetFirst() function should 
    have been called at least once on the same Video stream group before calling 
    this function. Then calling this function repeatedly on the stream group 
    will return information about the next video stream in the stream group. 
    When there are no more video streams to report, the function returns 
    USB_HOST_UVC_RESULT_END_OF_STREAM_LIST. 

    Calling the USB_HOST_UVC_StreamGetFirst() function on the stream group
    index after the USB_HOST_UVC_StreamGetNext() function has been called 
    will cause Video client driver to reset the video stream group to point
    to the first stream in the stream group.
 
 Precondition:
    The USB_HOST_UVC_StreamGetFirst() function must have been called
    before calling this function.

 Parameters:
    videoDeviceObj - Video client driver object

    streamGroupIndex - Stream Group Index. 
 
    streamInfo  - Pointer to streamInfo object 

 Returns:
    USB_HOST_UVC_STREAM_RESULT_SUCCESS - The operation was successful
    USB_HOST_UVC_RESULT_OBJ_INVALID - The specified Video client
    driver object does not exist.
    USB_HOST_UVC_STREAM_RESULT_FAILURE - An unknown failure occurred.
    USB_HOST_UVC_RESULT_END_OF_STREAM_LIST - There are no more video 
    streams in the stream group. 

 Example:
    <code>
    </code>

 Remarks:
    None.
*/ 
USB_HOST_UVC_RESULT USB_HOST_UVC_StreamGetNext
(
    USB_HOST_UVC_STREAM_OBJ videoStreamObj, 
    USB_HOST_UVC_STREAM_INFO *streamInfo
)
{
    USB_HOST_UVC_INSTANCE *videoInstanceInfo; 
    USB_HOST_UVC_STREAM_SETTING *videoStream; 
    USB_HOST_UVC_STREAMING_INTERFACE *vsInterface; 
    uint8_t videoInstanceIndex;
    uint8_t vsInterfaceIndex; 
    uint8_t alternateSetting; 
    uint8_t nextAlternateSetting; 
    uint8_t nextAlternateSettingIndex; 
    //int count; 
 
    if(videoStreamObj == NULL)
    {
      return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    videoInstanceIndex = (uint8_t)videoStreamObj;
    vsInterfaceIndex = (uint8_t)(videoStreamObj >> 8); 
    alternateSetting = (uint8_t)(videoStreamObj >> 16);

    if(alternateSetting == 0)
    {
        return USB_HOST_UVC_STREAM_HANDLE_INVALID; 
    }
    
    videoInstanceInfo = &gUSBHostVideoInstance[videoInstanceIndex]; 
    vsInterface = &videoInstanceInfo->streamInf[vsInterfaceIndex]; 

    if(vsInterface->nInterfaceSetting == 0)
    {
      return USB_HOST_UVC_RESULT_FAILURE; 
    }
    
    /* First Stream is always the alternate setting 1 */
    nextAlternateSetting = alternateSetting + 1;
    if(nextAlternateSetting >= vsInterface->nInterfaceSetting)
    {
      return USB_HOST_UVC_STREAM_RESULT_END_OF_DEVICE_LIST; 
    }
    nextAlternateSettingIndex = nextAlternateSetting-1; 
    videoStream = &vsInterface->videoStreamSetting[nextAlternateSettingIndex]; 
    /* Stream Object is made from 
    LSB = video instance index 
    Second byte = Stream Interface Index
    Third byte = Alternate Setting (not index)
    MSB = 0x01 */
    streamInfo->streamObj = (USB_HOST_UVC_STREAM_OBJ)(uint32_t)videoInstanceInfo->index |
                            ((uint32_t)vsInterfaceIndex) << 8 |
                            ((uint32_t)nextAlternateSetting << 16); 
    //  streamInfo->format = videoStream->wFormatTag; 
    streamInfo->streamDirection = videoStream->direction; 
    streamInfo->subFrameSize = videoStream->bSubframeSize[0]; 

    return USB_HOST_UVC_RESULT_SUCCESS; 
}




USB_HOST_UVC_STREAM_RESULT USB_HOST_UVC_SetVideoProbeControl
(
    USB_HOST_UVC_OBJ videoDeviceObj,
    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject, 
    USB_HOST_UVC_REQUEST_HANDLE *requestHandle, uint8_t channelNumber
)
{
    USB_UVC_FEATURE_UNIT_CONTROL_REQUEST setupPacket;
    USB_HOST_UVC_RESULT result; 
    USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *pDescriptor = (USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *)entityObject;
    USB_INTERFACE_DESCRIPTOR *videoControlInterfaceDescriptor; 
    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)videoDeviceObj; 

    Parameters.bmHint = 0x0001;
#ifdef ENABLE_BMP
    if(IsBitmapMode==true)
    {
        Parameters.bFormatIndex = uvcSettings[uvcSelectedSettingIndex].formatidx;
        Parameters.bFrameIndex = uvcSettings[uvcSelectedSettingIndex].frame;
        Parameters.dwFrameInterval = getMaxInterval(uvcSelectedSettingIndex);
    }
#endif
#ifdef ENABLE_JPG
    if(IsBitmapMode==false)
    {
        Parameters.bFormatIndex = uvcSettings[uvcSelectedSettingIndex].formatidx;
        Parameters.bFrameIndex =  uvcSettings[uvcSelectedSettingIndex].frame;
        Parameters.dwFrameInterval = getMaxInterval(uvcSelectedSettingIndex);
    }
#endif  
   
    Parameters.wKeyFrameRate = 0;
    Parameters.wPFrameRate = 0;
    Parameters.wCompQuality = 0;
    Parameters.wCompQualitySize = 0;
    Parameters.wDelay = 0;
    Parameters.dwMaxVideoFrameSize = 0;
    Parameters.dwMaxPayloadTransferSize = 38400;   

    if(videoInstanceInfo == NULL)
    {
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    /* Fill in Setup Packet */
    setupPacket.bmRequestType = USB_SETUP_DIRN_HOST_TO_DEVICE | USB_SETUP_TYPE_CLASS | USB_SETUP_RECIPIENT_INTERFACE;
    setupPacket.bRequest = USB_UVC_CS_SET_CUR;

    /* Fill in Interface Number */
    videoControlInterfaceDescriptor = (USB_INTERFACE_DESCRIPTOR*)videoInstanceInfo->pVideoControlDescriptor; 
    setupPacket.interfaceNumber = videoControlInterfaceDescriptor->bInterfaceNumber; 
    pDescriptor = (USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *)videoInstanceInfo->pVideoControlDescriptor;
    setupPacket.featureUnitId = pDescriptor->bUnitID; 
    setupPacket.wIndex = 1;

    setupPacket.channelNumber = channelNumber;
    setupPacket.controlSelector = USB_UVC_PROBE_CONTROL;
    //setupPacket.wValue = 0x0100;
    setupPacket.wLength = 26;

    result = _USB_HOST_UVC_ControlRequest
                (
                    videoDeviceObj, 
                    requestHandle,
                    (USB_SETUP_PACKET *)&setupPacket, 
                    &Parameters
                ); 
    return result;
}

USB_HOST_UVC_STREAM_RESULT USB_HOST_UVC_GetVideoControl
(
    USB_HOST_UVC_OBJ videoDeviceObj,
    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject, 
    USB_HOST_UVC_REQUEST_HANDLE *requestHandle, 
    uint8_t channelNumber
) 
{
    USB_UVC_FEATURE_UNIT_CONTROL_REQUEST setupPacket;
    USB_HOST_UVC_RESULT result; 
    USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *pDescriptor = (USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *)entityObject;
    USB_INTERFACE_DESCRIPTOR *videoControlInterfaceDescriptor; 
    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)videoDeviceObj; 

    if(videoInstanceInfo == NULL)
    {
        return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }

    /* Fill in Setup Packet */
    setupPacket.bmRequestType = USB_SETUP_DIRN_DEVICE_TO_HOST | USB_SETUP_TYPE_CLASS | USB_SETUP_RECIPIENT_INTERFACE;
    setupPacket.bRequest = USB_UVC_CS_GET_CUR;

    /* Fill in Interface Number */
    videoControlInterfaceDescriptor = (USB_INTERFACE_DESCRIPTOR *)videoInstanceInfo->pVideoControlDescriptor; 
    setupPacket.interfaceNumber = videoControlInterfaceDescriptor->bInterfaceNumber; 
    pDescriptor = (USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *)videoInstanceInfo->pVideoControlDescriptor;
    setupPacket.featureUnitId = pDescriptor->bUnitID; 
    setupPacket.wIndex = 1;

    setupPacket.channelNumber = channelNumber;
    setupPacket.controlSelector = USB_UVC_PROBE_CONTROL;
    //setupPacket.wValue = 0x0100;
    setupPacket.wLength = 26;

    result = _USB_HOST_UVC_ControlRequest
                (
                    videoDeviceObj, 
                    requestHandle,
                    (USB_SETUP_PACKET *)&setupPacket,
                    &Parameters
                ); 

    return result;
}

USB_HOST_UVC_STREAM_RESULT USB_HOST_UVC_SetVideoCommitControl
(
    USB_HOST_UVC_OBJ videoDeviceObj,
    USB_HOST_UVC_CONTROL_ENTITY_OBJ entityObject, 
    USB_HOST_UVC_REQUEST_HANDLE *requestHandle, uint8_t channelNumber
)
{
    USB_UVC_FEATURE_UNIT_CONTROL_REQUEST setupPacket;
    USB_HOST_UVC_RESULT result; 
    USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *pDescriptor = (USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *)entityObject;
    USB_INTERFACE_DESCRIPTOR *videoControlInterfaceDescriptor; 
    USB_HOST_UVC_INSTANCE *videoInstanceInfo = (USB_HOST_UVC_INSTANCE *)videoDeviceObj; 

    if(videoInstanceInfo == NULL)
    {
      return USB_HOST_UVC_RESULT_OBJ_INVALID; 
    }
    /* Fill in Setup Packet */
    setupPacket.bmRequestType = USB_SETUP_DIRN_HOST_TO_DEVICE | USB_SETUP_TYPE_CLASS | USB_SETUP_RECIPIENT_INTERFACE;
    setupPacket.bRequest = USB_UVC_CS_SET_CUR;

    /* Fill in Interface Number */ 
    videoControlInterfaceDescriptor = (USB_INTERFACE_DESCRIPTOR *)videoInstanceInfo->pVideoControlDescriptor; 
    setupPacket.interfaceNumber = videoControlInterfaceDescriptor->bInterfaceNumber; 
    pDescriptor = (USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER *)videoInstanceInfo->pVideoControlDescriptor;
    setupPacket.featureUnitId = pDescriptor->bUnitID; 
    setupPacket.wIndex = 1;

    setupPacket.channelNumber = channelNumber;
    setupPacket.controlSelector = USB_UVC_COMMIT_CONTROL;
    //setupPacket.wValue = 0x0200;
    setupPacket.wLength = 26;

    result = _USB_HOST_UVC_ControlRequest
                (
                    videoDeviceObj, 
                    requestHandle,
                    (USB_SETUP_PACKET *)&setupPacket,
                    &Parameters
                ); 

    return result;
}

static uint32_t getMaxInterval(uint8_t selectedIndex)
{
    uint32_t val=0;

    val=uvcSettings[selectedIndex].interval[uvcDataFrameIntervalIndex];

    return val;
}


/*************** End of File ************************************/

