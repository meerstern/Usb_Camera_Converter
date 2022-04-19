/*******************************************************************************
  USB Video class definitions

  Company:
    Microchip Technology Inc.

  File Name:
    usb_uvc.h

  Summary:
    USB Video class definitions.

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

#ifndef _USB_UVC_H_
#define _USB_UVC_H_

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif
// DOM-IGNORE-END 
        
// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
// *****************************************************************************
// Section: Constants
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Video Interface Class codes

  Summary:
    Identifies the Class Codes for Video interface. 

  Description:
    This constant identifies the value of the Video Interface class code. 

  Remarks:
    None.
*/
#define USB_UVC_CLASS_CODE                  0x0E  
#define USB_UVC_VIDEO_STREAMING_INTERFACE   0x24  
#define USB_VIDEO_CLASS_SPECIFIC_DESCRIPTOR 0x24
#define USB_VIDEO_CLASS_SPECIFIC_INTERRUPT_ENDPOINT 0x25
// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Video Interface subclass codes

  Summary:
    Identifies the subclass codes for Video interface. 

  Description:
    This enumeration identifies the possible subclass codes for 
    Video interface. 

  Remarks:
    The "ISC" in the enumeration member names is an acronym for Interface 
    Subclass Code.
*/
        
typedef enum
{
    USB_UVC_SUBCLASS_UNDEFINED  = 0x00,
    USB_UVC_SUBCLASS_VIDEOCONTROL = 0x01,
    USB_UVC_SUBCLASS_VIDEOSTREAMING = 0x02,
    USB_UVC_SUBCLASS_INTERFACE_COLLECTION = 0x03,            
    USB_VIDEO_SUBCLASS_VIDEOCONTROL      = 0x01
            
} USB_UVC_SUBCLASS_CODE; 
        
// *****************************************************************************
/* Video Interface Protocol codes

  Summary:
    Identifies the protocol codes for Video interface. 

  Description:
    This enumeration identifies the possible protocol codes for 
    Video interface

  Remarks:
    As per USB Device class definition for Video Device, the
    protocol code should always be 0.
*/        
typedef enum 
{
    USB_UVC_PR_PROTOCOL_UNDEFINED    = 0x0
} USB_UVC_PROTOCOL_CODE;     
       
// *****************************************************************************
/* Video Class Specific AC Interface Descriptor Subtypes

  Summary:
    Identifies the Video Class Specific AC Interface Descriptor Subtypes.

  Description:
    This enumeration identifies the possible Video Class Specific AC Interface
    Descriptor Subtypes.

  Remarks:
    The "CS" in the enumeration member names is an acronym for Class Specific.
    The "AC" in the enumeration member names is an acronym for Video Control.
*/
typedef enum
{
    USB_UVC_DESCRIPTOR_UNDEFINED        = 0x00,
    USB_UVC_HEADER                      = 0x01,
    USB_UVC_INPUT_TERMINAL              = 0x02,
    USB_UVC_OUTPUT_TERMINAL             = 0x03,
    USB_UVC_MIXER_UNIT                  = 0x04,
    USB_UVC_SELECTOR_UNIT               = 0x05,
    USB_UVC_FEATURE_UNIT                = 0x06,
    USB_UVC_PROCESSING_UNIT             = 0x05,
    USB_UVC_EXTENSION_UNIT              = 0x06,

    USB_UVC_VS_STILL_IMAGE_FRAME        = 0x03,
    USB_UVC_VS_FORMAT_UNCOMPRESSED      = 0x04,
    USB_UVC_VS_FRAME_UNCOMPRESSED       = 0x05,
    USB_UVC_VS_COLOR_MATCHING           = 0x0D,

    USB_UVC_CS_ENDPOINT                 = 0x25,
    USB_UVC_CS_INTERFACE                = 0x24,
           
} USB_UVC_INTERFACE_DESCRIPTOR_SUBTYPE, USB_UVC_ENTITY_TYPE; 

// *****************************************************************************
/* Video Class Specific Terminal Types. 

  Summary:
    Identifies the Video Class Specific Video Class Specific Terminal Types.

  Description:
    This enumeration identifies the possible Video Class Specific Terminal Types.
    
  Remarks:
    The "CS" in the enumeration member names is an acronym for Class Specific. 
*/
//typedef enum 
//{
//    /* USB Terminal Types */ 
//    USB_UVC_TERMINAL_TYPE_USB_UNDEFINED = 0x0100,
//    USB_UVC_TERMINAL_TYPE_USB_STREAMING = 0x0101,
//    USB_UVC_TERMINAL_TYPE_USB_VENDOR_SPECIFIC = 0x01FF,
//         
//    /* Input Terminal types */ 
//    USB_UVC_TERMINAL_TYPE_INPUT_UNDEFINED = 0x0200, 
//    USB_UVC_TERMINAL_TYPE_INPUT_MICROPHONE = 0x0201,
//    USB_UVC_TERMINAL_TYPE_INPUT_MICROPHONE_DESKTOP = 0x0202,
//    USB_UVC_TERMINAL_TYPE_INPUT_MICROPHONE_PERSONAL = 0x0203,
//    USB_UVC_TERMINAL_TYPE_INPUT_MICROPHONE_OMNI = 0x0204,
//    USB_UVC_TERMINAL_TYPE_INPUT_MICROPHONE_ARRAY = 0x0205,
//    USB_UVC_TERMINAL_TYPE_INPUT_MICROPHONE_ARRAY_PROCESSING = 0x0206,
//       
//    /* Output Terminal types */         
//    USB_UVC_TERMINAL_TYPE_OUTPUT_UNDEFINED = 0x0300,
//    USB_UVC_TERMINAL_TYPE_OUTPUT_SPEAKER = 0x0301,
//    USB_UVC_TERMINAL_TYPE_OUTPUT_HEADPHONES = 0x0302,
//    USB_UVC_TERMINAL_TYPE_OUTPUT_HMD = 0x0303,
//    USB_UVC_TERMINAL_TYPE_OUTPUT_SPEAKER_DESKTOP = 0x0304,
//    USB_UVC_TERMINAL_TYPE_OUTPUT_SPEAKER_ROOM = 0x0305,
//    USB_UVC_TERMINAL_TYPE_OUTPUT_SPEAKER_COMM = 0x0306,
//    USB_UVC_TERMINAL_TYPE_OUTPUT_SPEAKER_LFE = 0x0307
//            
//} USB_UVC_TERMINAL_TYPE; 

// *****************************************************************************
/* Video Class Specific Request Codes.

  Summary:
    Identifies the Video Class Specific Request Codes.

  Description:
    This enumeration identifies the possible Video Class Specific Request codes.

  Remarks:
    The "CS" in the enumeration member names is an acronym for Class Specific.
*/
typedef enum
{
    USB_UVC_CS_REQUEST_CODE_UNDEFINED = 0x00,
    USB_UVC_CS_SET_CUR   = 0x01,
    USB_UVC_CS_GET_CUR   = 0x81,
    USB_UVC_CS_SET_MIN   = 0x02,
    USB_UVC_CS_GET_MIN   = 0x82,
    USB_UVC_CS_SET_MAX   = 0x03,
    USB_UVC_CS_GET_MAX   = 0x83,
    USB_UVC_CS_SET_RES   = 0x04,
    USB_UVC_CS_GET_RES   = 0x84,
    USB_UVC_CS_SET_MEM   = 0x05,
    USB_UVC_CS_GET_MEM   = 0x85,
    USB_UVC_CS_GET_INFO  = 0x86,
    USB_UVC_CS_GET_DEF   = 0x87,
    USB_UVC_CS_GET_STAT  = 0xFF
} USB_UVC_CS_REQUEST_CODE;

// *****************************************************************************
/* Video Feature Unit Control Selector.

  Summary:
    Identifies the Video Feature Unit Control Selector.

  Description:
    This enumeration identifies the possible Video Feature Unit Control
    Selectors.

  Remarks:
    None.
*/
typedef enum
{
    USB_UVC_FU_CONTROL_UNDEFINED        = 0x00,
    USB_UVC_MUTE_CONTROL                = 0x01,
    USB_UVC_VOLUME_CONTROL              = 0x02,
    USB_UVC_BASS_CONTROL                = 0x03,
    USB_UVC_MID_CONTROL                 = 0x04,
    USB_UVC_TREBLE_CONTROL              = 0x05,
    USB_UVC_GRAPHIC_EQUALIZER_CONTROL   = 0x06,
    USB_UVC_AUTOMATIC_GAIN_CONTROL      = 0x07,
    USB_UVC_DELAY_CONTROL               = 0x08,
    USB_UVC_BASS_BOOST_CONTROL          = 0x09,
    USB_UVC_LOUDNESS_CONTROL            = 0x0A,
            
    USB_UVC_PROBE_CONTROL=1,
    USB_UVC_COMMIT_CONTROL=2,

} USB_UVC_FEATURE_UNIT_CONTROL_SELECTORS;    

// *****************************************************************************
/* Video Endpoint Control Selector.

  Summary:
    Identifies the Video Endpoint Control Selector.

  Description:
    This enumeration identifies the possible Video Endpoint Control Selectors.

  Remarks:
    None.
*/
typedef enum
{
    USB_UVC_EP_CONTROL_UNDEFINED  = 0x00,
    USB_UVC_SAMPLING_FREQ_CONTROL = 0x01,
    USB_UVC_PITCH_CONTROL         = 0x02
} USB_UVC_ENDPOINT_CONTROL_SELECTORS;


// *****************************************************************************
/* USB Video Format Codes.

  Summary:
    Identifies the USB Video Format codes for USB Video Class.

  Description:
    This enumeration identifies the possible USB Video Format codes for USB
    Video Class. 
 
  Remarks:
    None.
*/

#define USB_UVC_UNCOMPRESSED_FORMAT_TYPE    0x04
#define USB_UVC_UNCOMPRESSED_FRAME_TYPE     0x05
#define USB_UVC_MJPEG_FORMAT_TYPE           0x06
#define USB_UVC_MJPEG_FRAME_TYPE            0x07 
typedef enum
{
  USB_UVC_FORMAT_TYPE_I_UNDEFINED = 0x0000,
  USB_UVC_FORMAT_PCM = 0x0001,
  USB_UVC_FORMAT_PCM8 = 0x0002
} USB_UVC_FORMAT_CODE, USB_UVC_FORMAT_TAG;

// *****************************************************************************
/* Video Feature Unit Descriptor Header Type

  Summary:
    Identifies the Video Feature Unit Descriptor Type.

  Description:
    This type identifies the Video Feature Unit Descriptor.  
 
  Remarks:
    Always needs to be packed.
*/
typedef struct __attribute__((packed))
{
    /* Size of this descriptor */
    uint8_t bLength;

    /* Interface descriptor type */
    uint8_t bDescriptorType;

    /* Interface Descriptor Subtype */
    uint8_t bDescriptorSubtype;

    /* Constant uniquely identifying the Unit within the video function. */
    uint8_t bUnitID;

    /* Source Unit or Terminal ID */
    uint8_t bSourceID;

    /* Size in Bytes of an element in the Control array */
    uint8_t bControlSize;

} USB_UVC_FEATURE_UNIT_DESCRIPTOR_HEADER;

// *****************************************************************************
/* USB Video Class Specific Video Streaming Interface Descriptor

  Summary:
    Identifies the USB Video Class Specific Video Streaming Interface Descriptor
    Type.

  Description:
    This type identifies the USB Video Class Specific Video Streaming Interface
    Descriptor.  

  Remarks:
    Always needs to be packed.
*/
typedef struct __attribute__((packed))
{
    /* Size of the descriptor in bytes */
    uint8_t bLength;

    /* CS_INTERFACE descriptor type */
    uint8_t bDescriptorType;

    /* VC_HEADER descriptor subtype */
    uint8_t bDescriptorSubtype;
    
    uint16_t bcdUVC;
    uint16_t wTotalLength;
    uint32_t dwClockFrequency;
    uint8_t bInCollection;
    uint8_t baInterfaceNr1;
    uint8_t baInterfaceNrN[];

} USB_UVC_VC_INTERFACE_DESCRIPTOR;


// *****************************************************************************
/* USB Video Feature Unit Control Set and Get Request.

  Summary:
    Identifies the type of the USB Video Feature Unit Control Set and Get Request.

  Description:
    This type identifies the type of the USB Video Feature Unit Control Set and
    Get Request.  The application can type cast the received audio class
    specific setup packet to this type in order to service the feature unit
    control Set and Get requests.

  Remarks:
    Always needs to be packed.
*/
typedef struct __attribute__((packed))
{
    /* Request type SET or GET */
    uint8_t bmRequestType;
    
    /* Identifies the attribute to be accessed */
    uint8_t bRequest;

    union {
        struct { 
            // Identifies the channel number
            uint8_t channelNumber;
            // Identifies the control selector
            uint8_t controlSelector;
        };
        uint16_t wValue;
    };
    union {
        struct {
            // Identifies the interface number
            uint8_t interfaceNumber;
            // Identifies the feature unit ID
            uint8_t featureUnitId;
        };
        uint16_t wIndex;
    };   

    // Length of the parameter block
    uint16_t wLength;

} USB_UVC_FEATURE_UNIT_CONTROL_REQUEST;


// *****************************************************************************
/* USB Video Endpoint Control Set and Get Request.

  Summary:
    Identifies the type of the USB Video Endpoint Control Set and Get Request.

  Description:
    This type identifies the type of the USB Video Endpoint Control Set and
    Get Request.  The application can type cast the received audio class
    specific setup packet to this type in order to service the Endpoint
    control Set and Get requests. 
 
  Remarks:
    Always needs to be packed.
*/
typedef struct __attribute__((packed))
{
    /* Request type SET or GET */
    uint8_t bmRequestType;
    /* Identifies the attribute to be accessed */
    uint8_t bRequest;
    /* This field is always Zero */
    unsigned :8;
    /* Identifies the control selector */
    uint8_t controlSelector;
    /* Identifies the Endpoint number */
    uint8_t endpointNumber;    
    /* This field is always Zero */
    unsigned :8;
    /* Length of the parameter block */
    uint16_t wLength;
} USB_UVC_ENDPOINT_CONTROL_REQUEST;

typedef struct __attribute__((packed)) 
{
    /* Size of this descriptor */
    uint8_t bLength;
    /* Interface descriptor type */
    uint8_t bDescriptorType;
    /* Interface Descriptor Subtype */
    uint8_t bDescriptorSubtype;
    uint8_t bNumFormats;
    uint16_t wTotalLength;
    uint8_t bEndpointAddress;
    uint8_t bmInfo;     
    uint8_t bTerminalLink;
    uint8_t bStillCaptureMethod;
    uint8_t bTriggerSupport;
    uint8_t bTriggerUsage;
    uint8_t bControlSize;
    uint8_t bmaControls1[2];
    uint8_t bmaControlsP[2];
} USB_UVC_VS_INPUT_HEADER_DESCRIPTOR;



typedef struct __attribute__((packed)) 
{
    /* Size of this descriptor */
    uint8_t bLength;
    /* Interface descriptor type */
    uint8_t bDescriptorType;
    /* Interface Descriptor Subtype */
    uint8_t bDescriptorSubtype;
    /* Unique Terminal Identifier Constant */
    uint8_t bTerminalID;
    /* Terminal Type */
    uint16_t wTerminalType;
    /* ID of the associated Output Terminal */
    uint8_t bAssocTerminal;
    /* Input Terminal String Descriptor Index */
    uint8_t iTerminal;
    uint16_t wObjectiveFocalLengthMin;
    uint16_t wObjectiveFocalLengthMax;
    uint16_t wOcularFocalLength;
    uint8_t bControSize;
    uint8_t bmControls[];
} USB_UVC_INPUT_TERMINAL_DESCRIPTOR;

typedef struct __attribute__((packed))
{
    /* Size of this descriptor */
    uint8_t bLength;
    /* Interface descriptor type */
    uint8_t bDescriptorType;
    /* Interface Descriptor Subtype */
    uint8_t bDescriptorSubtype;
    /* Unique Terminal Identifier Constant */
    uint8_t bTerminalID;
    /* Terminal Type */
    uint16_t wTerminalType;
    /* ID of the associated Input Terminal */
    uint8_t bAssocTerminal;
    /* Source Unit or Terminal ID */
    uint8_t bSourceID;
    /* Output Terminal String Descriptor Index */
    uint8_t iTerminal;
} USB_UVC_OUTPUT_TERMINAL_DESCRIPTOR;

typedef struct __attribute__((packed))
{
    /* Size of this descriptor */
    uint8_t bLength;
    /* Interface descriptor type */
    uint8_t bDescriptorType;
    /* Interface Descriptor Subtype */
    uint8_t bDescriptorSubtype;
    /* Unique Terminal Identifier Constant */
    uint8_t bUnitID;
    /* Source Unit or Terminal ID */
    uint8_t bSourceID;    
    uint16_t wMaxMultiplier;
    uint8_t bmControls[2];    
    // Processing Unit String Descriptor Index
    uint8_t iProcessing;    
} USB_UVC_PROCESSING_UNIT_DESCRIPTOR;

typedef struct __attribute__((packed))
{
    /* Size of this descriptor */
    uint8_t bLength;
    /* Interface descriptor type */
    uint8_t bDescriptorType;
    /* Interface Descriptor Subtype */
    uint8_t bDescriptorSubtype;
    /* Unique Terminal Identifier Constant */
    uint8_t bUnitID;
    uint8_t guidExtensionCode[16];
    uint8_t bNumControls;    
    uint8_t bNrInPins;    
    /* Source Unit or Terminal ID */
    uint8_t baSourceID1;    
    uint8_t baSourceIDP;    
    uint8_t bControlSize;  
    uint8_t bmControls[2];
    /* Extension Unit String Descriptor Index */
    uint8_t iExtension;
} USB_UVC_EXTENSION_UNIT_DESCRIPTOR;

typedef struct __attribute__((packed))
{
    /* Size of the descriptor in bytes */
    uint8_t bLength;
    /* CS_INTERFACE descriptor type */
    uint8_t bDescriptorType;
    /* VS_FORMAT_UNCOMPRESSED descriptor subtype */
    uint8_t bDescriptorSubtype;    
    uint8_t bFormatIndex;
    uint8_t bNumFrameDescriptors;    
    uint8_t guidFormat[16];    
    uint8_t bBitsPerPixel;
    uint8_t bDefaultFrameIndex;    
    uint8_t bAspectRatioX;
    uint8_t bAspectRatioY;
    uint8_t bmInterlaceFlags;    
    uint8_t bCopyProtect;    
} USB_UVC_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR;

typedef struct __attribute__((packed))
{
    /* Size of the descriptor in bytes */
    uint8_t bLength;
    /* CS_INTERFACE descriptor type */
    uint8_t bDescriptorType;
    /* VS_FRAME_UNCOMPRESSED descriptor subtype */
    uint8_t bDescriptorSubtype;    
    uint8_t bFrameIndex;
    uint8_t bmCapabilities;    
    uint8_t wWidth[2];
    uint8_t wHeight[2];    
    uint32_t dwMinBitRate;
    uint32_t dwMaxBitRate;    
    uint32_t dwMaxVideoFrameBufferSize;    
    uint32_t dwDefaultFrameInterval;    
    uint8_t  bFrameIntervalType;
    uint32_t dwFrameIntervalType[];    
} USB_UVC_VS_FRAME_UNCOMPRESSED_DESCRIPTOR;

typedef struct __attribute__ ((packed))
{
    uint8_t bLength;	
    uint8_t bDescriptorType;	
    uint8_t bDescriptorSubtype;    
    uint8_t bFormatIndex;    
    uint8_t bNumFrameDescriptors;
    uint8_t bmFlags;
    uint8_t bDefaultFrameIndex;
    uint8_t bAspectRatioX;
    uint8_t bAspectRatioY;
    uint8_t bmInterlaceFlags;
    uint8_t bCopyProtect;    
}USB_VIDEO_VS_MPEG_FORMAT_TYPE_DESCRIPTOR;

typedef struct __attribute__ ((packed))
{
    uint8_t bLength;	
    uint8_t bDescriptorType;	
    uint8_t bDescriptorSubtype;    
    uint8_t bFrameIndex;    
    uint8_t bmCapabilities;
    uint8_t wWidth[2];
    uint8_t wHeight[2];
    uint8_t dwMinBitRate[4];
    uint8_t dwMaxBitRate[4];
    uint8_t dwMaxVideoFrameBufferSize[4];
    uint8_t dwDefaultFrameInterval[4];
    uint8_t bFrameIntervalType;
    uint32_t dwFrameIntervalType[];    
}USB_VIDEO_VS_MPEG_FRAME_TYPE_DESCRIPTOR;


typedef struct __attribute__((packed))
{
    /* Size of the descriptor in bytes */
    uint8_t bLength;
    /* CS_INTERFACE descriptor type */
    uint8_t bDescriptorType;
    /* VS_FRAME_UNCOMPRESSED descriptor subtype */
    uint8_t bDescriptorSubtype;    
    uint8_t bEndpointAddress;
    uint8_t bNumImageSizePatterns;    
    uint16_t wWidth1;
    uint16_t wHeight1;    
    uint16_t wWidthN;
    uint16_t wHeightN;    
    uint8_t bNumCompressionPattern;    
    uint8_t bCompression1;    
    uint8_t bCompressionN;    
} USB_UVC_VS_STILL_IMAGE_FRAME_DESCRIPTOR;
    
typedef struct __attribute__((packed)) 
{
    /* Size of the descriptor in bytes */
    uint8_t bLength;
    /* CS_INTERFACE descriptor type */
    uint8_t bDescriptorType;
    /* VS_FRAME_UNCOMPRESSED descriptor subtype */
    uint8_t bDescriptorSubtype;    
    uint8_t bColorPrimaries;
    uint8_t bTransferCharacteristics;    
    uint8_t bMatrixCoefficients;    
} USB_UVC_VS_COLOR_FORMAT_DESCRIPTOR;



//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif
        
        
        
        