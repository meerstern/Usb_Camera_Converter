/*******************************************************************************
  System Configuration Header

  File Name:
    configuration.h

  Summary:
    Build-time configuration header for the system defined by this project.

  Description:
    An MPLAB Project may have multiple configurations.  This file defines the
    build-time options for a single configuration.

  Remarks:
    This configuration header must not define any prototypes or data
    definitions (or include any files that do).  It only provides macro
    definitions for build-time configuration options

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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
/*  This section Includes other configuration headers necessary to completely
    define this configuration.
*/

#include "user.h"
#include "toolchain_specifics.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: System Configuration
// *****************************************************************************
// *****************************************************************************



// *****************************************************************************
// *****************************************************************************
// Section: System Service Configuration
// *****************************************************************************
// *****************************************************************************
#define SYS_DEBUG_INFO                              0
#define SYS_DEBUG_ENABLE
#define SYS_DEBUG_GLOBAL_ERROR_LEVEL       SYS_ERROR_WARNING//SYS_ERROR_DEBUG//SYS_ERROR_WARNING
#define SYS_DEBUG_BUFFER_DMA_READY
#define SYS_DEBUG_USE_CONSOLE

//#define ENABLE_LCD
//#define ENABLE_JPGDECODE

/* TIME System Service Configuration Options */
#define SYS_TIME_INDEX_0                            (0)
#define SYS_TIME_MAX_TIMERS                         (5)
#define SYS_TIME_HW_COUNTER_WIDTH                   (32)
#define SYS_TIME_HW_COUNTER_PERIOD                  (4294967295U)
#define SYS_TIME_HW_COUNTER_HALF_PERIOD             (SYS_TIME_HW_COUNTER_PERIOD>>1)
#define SYS_TIME_CPU_CLOCK_FREQUENCY                (200000000)
#define SYS_TIME_COMPARE_UPDATE_EXECUTION_CYCLES    (620)


/* File System Service Configuration */

#define SYS_FS_MEDIA_NUMBER               1
#define SYS_FS_VOLUME_NUMBER              1

#define SYS_FS_AUTOMOUNT_ENABLE           false
#define SYS_FS_MAX_FILES                  1
#define SYS_FS_MAX_FILE_SYSTEM_TYPE       1
#define SYS_FS_MEDIA_MAX_BLOCK_SIZE       512
#define SYS_FS_MEDIA_MANAGER_BUFFER_SIZE  2048
#define SYS_FS_USE_LFN                    1
#define SYS_FS_FILE_NAME_LEN              255
#define SYS_FS_CWD_STRING_LEN             1024


#define SYS_FS_FAT_VERSION                "v0.14a"
#define SYS_FS_FAT_READONLY               false
#define SYS_FS_FAT_CODE_PAGE              437
#define SYS_FS_FAT_MAX_SS                 SYS_FS_MEDIA_MAX_BLOCK_SIZE
#define SYS_FS_FAT_ALIGNED_BUFFER_LEN     512




#define SYS_CONSOLE_DEVICE_MAX_INSTANCES   			1
#define SYS_CONSOLE_UART_MAX_INSTANCES 	   			1
#define SYS_CONSOLE_USB_CDC_MAX_INSTANCES 	   		0
#define SYS_CONSOLE_PRINT_BUFFER_SIZE        		1040


#define SYS_CONSOLE_INDEX_0                       0






// *****************************************************************************
// *****************************************************************************
// Section: Driver Configuration
// *****************************************************************************
// *****************************************************************************
/* SDSPI Driver Instance 0 Configuration Options */
#define DRV_SDSPI_INDEX_0                       0
#define DRV_SDSPI_CLIENTS_NUMBER_IDX0           1
#define DRV_SDSPI_QUEUE_SIZE_IDX0               4
#define DRV_SDSPI_CHIP_SELECT_PIN_IDX0          SYS_PORT_PIN_RE8
#define DRV_SDSPI_SPEED_HZ_IDX0                 5000000
#define DRV_SDSPI_POLLING_INTERVAL_MS_IDX0      1000

#define DRV_SDSPI_DMA_MODE
#define DRV_SDSPI_XMIT_DMA_CH_IDX0              SYS_DMA_CHANNEL_0
#define DRV_SDSPI_RCV_DMA_CH_IDX0               SYS_DMA_CHANNEL_1



/* SPI Driver Instance 0 Configuration Options */
#define DRV_SPI_INDEX_0                       0
#define DRV_SPI_CLIENTS_NUMBER_IDX0           1
#define DRV_SPI_QUEUE_SIZE_IDX0               4

/* SDSPI Driver Common Configuration Options */
#define DRV_SDSPI_INSTANCES_NUMBER              1

/* SPI Driver Common Configuration Options */
#define DRV_SPI_INSTANCES_NUMBER              1



// *****************************************************************************
// *****************************************************************************
// Section: Middleware & Other Library Configuration
// *****************************************************************************
// *****************************************************************************
/*** USB Driver Configuration ***/

/* Maximum USB driver instances */
#define DRV_USBHS_INSTANCES_NUMBER                        1

/* Interrupt mode enabled */
#define DRV_USBHS_INTERRUPT_MODE                          true


/* Disable Device Support */
#define DRV_USBHS_DEVICE_SUPPORT                          false
	
/* Enable Host Support */
#define DRV_USBHS_HOST_SUPPORT                            true

/* Number of NAKs to wait before returning transfer failure */ 
#define DRV_USBHS_HOST_NAK_LIMIT                          2000 

/* Maximum Number of pipes */
#define DRV_USBHS_HOST_PIPES_NUMBER                       10 

/* Attach Debounce duration in milli Seconds */ 
#define DRV_USBHS_HOST_ATTACH_DEBOUNCE_DURATION           500

/* Reset duration in milli Seconds */ 
#define DRV_USBHS_HOST_RESET_DURATION                     100



/* Alignment for buffers that are submitted to USB Driver*/ 
#define USB_ALIGN  CACHE_ALIGN

// *****************************************************************************
// *****************************************************************************
// Section: USB Host Layer Configuration
// *****************************************************************************
// **************************************************************************

/* Number of Endpoints used */
#define DRV_USBHS_ENDPOINTS_NUMBER 							1

#define USB_HOST_DEVICES_NUMBER                             2 

/* Target peripheral list entries */
#define  USB_HOST_TPL_ENTRIES                               2 

/* Maximum number of configurations supported per device */
#define USB_HOST_DEVICE_INTERFACES_NUMBER                   5    

#define USB_HOST_CONTROLLERS_NUMBER                         1

#define USB_HOST_TRANSFERS_NUMBER                           10

/* Provides Host pipes number */
#define USB_HOST_PIPES_NUMBER                               10


/* Number of HUB Function driver instances in the application */
#define USB_HOST_HUB_SUPPORT                  true
#define USB_HOST_HUB_INSTANCES_NUMBER         1
#define USB_HOST_HUB_PORTS_NUMBER			  4


/* Number of Audio v1.0 Client driver instances in the application */
#define USB_HOST_AUDIO_V1_INSTANCES_NUMBER         1

/* Maximum number of Streaming interfaces provides by any Device that will be
 be connected to this Audio Host */
#define USB_HOST_AUDIO_V1_STREAMING_INTERFACES_NUMBER 2

/* Maximum number of Streaming interface alternate settings provided by any 
   Device that will be connected to this Audio Host. (This number includes 
   alternate setting 0) */
#define USB_HOST_AUDIO_V1_STREAMING_INTERFACE_ALTERNATE_SETTINGS_NUMBER 2

/* Maximum number of discrete Sampling frequencies supported by the Attached Audio Device */ 
#define USB_HOST_AUDIO_V1_SAMPLING_FREQUENCIES_NUMBER 3




// *****************************************************************************
// *****************************************************************************
// Section: Application Configuration
// *****************************************************************************
// *****************************************************************************


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // CONFIGURATION_H
/*******************************************************************************
 End of File
*/
