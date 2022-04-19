/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <sys/kmem.h>
#include "app.h"
#include "bootloader/bootloader.h"
#include "system/debug/sys_debug.h"
#include "peripheral/gpio/plib_gpio.h"
#include "peripheral/coretimer/plib_coretimer.h"
#include "device_cache.h"
// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary callback functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************


/* TODO:  Add any necessary local functions.
*/
#define BTL_TRIGGER_PATTERN (0x5048434DUL)

static uint32_t *ramStart = (uint32_t *)BTL_TRIGGER_RAM_START;

bool bootloader_Trigger(void)
{
    uint32_t i;
    for (i = 0; i < 2000; i++)
    {
        asm("nop");
    }
    // Check for Bootloader Trigger Pattern in first 16 Bytes of RAM to enter Bootloader.
    if (BTL_TRIGGER_PATTERN == ramStart[0] && BTL_TRIGGER_PATTERN == ramStart[1] &&
        BTL_TRIGGER_PATTERN == ramStart[2] && BTL_TRIGGER_PATTERN == ramStart[3])
    {
        ramStart[0] = 0;
        DCACHE_CLEAN_BY_ADDR(ramStart, 4);
        //SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "Soft Triggered\r\n");        
        return true;
    }

    // Check for Switch press to enter Bootloader
    if (TRG_Get() == 0)
    {
        //SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "Hard Triggered\r\n");
        return true;
    }
    //SYS_DEBUG_PRINT(SYS_ERROR_DEBUG, "Not Triggered Bootloader\r\n");
    return false;
}

void bootloader_TriggerReset(void)
{
    /* Perform system unlock sequence */
    SYSKEY = 0x00000000;
    SYSKEY = 0xAA996655;
    SYSKEY = 0x556699AA;

    RSWRSTSET = _RSWRST_SWRST_MASK;
    (void)RSWRST;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */
#define VERSION_MSG "\r\n\r\n\t*** PIC32MZ Bootloder v1.0.0 ***\r\n"
#define ENTER_MSG   ">>Enter Firm Update Mode...\r\n"

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    
    UART2_Write(VERSION_MSG,sizeof(VERSION_MSG));
    UART2_Write(ENTER_MSG,sizeof(ENTER_MSG));

    /* TODO: Initialize your application's state machine and other
     * parameters.
     */
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

//Progress Blink Count
uint32_t count=0;
uint32_t max_count=200000;

void APP_Tasks ( void )
{
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
        {
            bool appInitialized = true;

            if (appInitialized)
            {
                RUN_LED_Set();
                ERR_LED_Clear();
                appData.state = APP_STATE_SERVICE_TASKS;
            }
            break;
        }

        case APP_STATE_SERVICE_TASKS:
        {
            count++;
            if(count>max_count)
            {
                RUN_LED_Toggle();
                ERR_LED_Toggle();
                count=0;
            }

            break;
        }

        /* TODO: implement your application state machine.*/


        /* The default state should never be executed. */
        default:
        {
            /* TODO: Handle error in application's state machine. */
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
