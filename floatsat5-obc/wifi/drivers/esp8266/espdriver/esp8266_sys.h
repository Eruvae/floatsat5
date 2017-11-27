/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \website https://majerle.eu/projects/esp8266-at-commands-parser-for-embedded-systems
 * \license MIT
 * \brief   ESP System calls
 *  
\verbatim
   ----------------------------------------------------------------------
    Copyright (c) 2016 Tilen Majerle

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without restriction,
    including without limitation the rights to use, copy, modify, merge,
    publish, distribute, sublicense, and/or sell copies of the Software, 
    and to permit persons to whom the Software is furnished to do so, 
    subject to the following conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
    AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
   ----------------------------------------------------------------------
\endverbatim
 */
#ifndef ESP_SYS_H
#define ESP_SYS_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/* Include core libraries */
#include "stdint.h"
#include "stdlib.h"
    
/* Platform dependant includes add here before ESP.h library include */
//#include "cmsis_os.h"
    
/* Include library */
#include "esp8266.h"
#include "esp8266_config.h"

/******************************************************************************/
/******************************************************************************/
/***   Copy this file to project directory and rename it to "esp8266_sys.h"  **/
/******************************************************************************/
/******************************************************************************/

/**
 * \defgroup SYSCALL
 * \brief    ESP Syscall implementation for RTOS synchronization
 * \{
 */

/**
 * \brief  Creates a synchronization object
 * \param  *Sync: Pointer to sync object to create in system
 * \retval Successfull status:
 *            - 0: Successfully created object
 *            - > 0: Error trying to create object
 */
uint8_t ESP_SYS_Create(ESP_RTOS_SYNC_t* Sync);

/**
 * \brief  Deletes a synchronization object
 * \param  *Sync: Pointer to sync object to delete from system
 * \retval Successfull status:
 *            - 0: Successfully deleted object
 *            - > 0: Error trying to delete object
 */
uint8_t ESP_SYS_Delete(ESP_RTOS_SYNC_t* Sync);

/**
 * \brief  Requests grant for specific sync object
 * \param  *Sync: Pointer to sync object to request grant
 * \retval Successfull status:
 *            - 0: Successfully granted permission
 *            - > 0: Error trying to grant permission
 */
uint8_t ESP_SYS_Request(ESP_RTOS_SYNC_t* Sync);

/**
 * \brief  Releases grant for specific sync object
 * \param  *Sync: Pointer to sync object to release grant
 * \retval Successfull status:
 *            - 0: Successfully released permission
 *            - > 0: Error trying to release permissing
 */
uint8_t ESP_SYS_Release(ESP_RTOS_SYNC_t* Sync);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
