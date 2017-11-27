/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \website https://majerle.eu/projects/esp8266-at-commands-parser-for-embedded-systems
 * \license MIT
 * \brief   Low level, platform dependant, part for communicate with ESP module and platform.
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
#ifndef ESP_LL_H
#define ESP_LL_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdlib.h"
    
/**
 * \defgroup LOWLEVEL
 * \brief    Low level, platform dependant, part for communication with ESP module and platform.
 * \{
 */

/**
 * \brief  Low level structure for driver
 * \note   For now it has basic settings only without hardware flow control.
 */
typedef struct _ESP_LL_t {
    uint32_t Baudrate;          /*!< Baudrate to be used for UART */
} ESP_LL_t;
    
/* Include library */
#include "esp8266.h"
#include "esp8266_config.h"
    
#define ESP_RTS_SET         1   /*!< RTS should be set high (RTS set, device should stop sending data, in most cases this is set to high) */
#define ESP_RTS_CLR         0   /*!< RTS should be set low */
#define ESP_RESET_SET       1   /*!< Reset pin should be set (device should go to reset state) */
#define ESP_RESET_CLR       0   /*!< Reset pin should be cleared */
    
/**
 * \brief  Initializes Low-Level driver to communicate with SIM module
 * \param  *LL: Pointer to \ref ESP_LL_t structure with settings
 * \retval Success status:
 *            - 0: Successful
 *            - > 0: Error
 */
uint8_t ESP_LL_Init(ESP_LL_t* LL);

/**
 * \brief  Sends data to ESP module from ESP stack
 * \note   Send can be implemented using DMA or IRQ easily,
 *            without waiting for finish.
 *            However, when multiple calls are executed,
 *            you must take care to send previous data first.
 *            Using DMA or IRQ, you can use cyclic buffers for implementation.
 *
 * \param  *LL: Pointer to \ref ESP_LL_t structure with settings
 * \param  *data: Data to be sent to module
 * \param  count: Number of bytes to be sent to module
 * \retval Success status:
 *            - 0: Successful
 *            - > 0: Error
 */
uint8_t ESP_LL_SendData(ESP_LL_t* LL, const uint8_t* data, uint16_t count);

/**
 * \brief  Set reset pin high or low
 * \param  *LL: Pointer to \ref ESP_LL_t structure with settings
 * \param  state: State for reset pin, it can be high or low. Check \ref ESP_RESET_SET and \ref ESP_RESET_CLR
 * \retval Success status:
 *            - 0: Successful
 *            - > 0: Error
 */
uint8_t ESP_LL_SetReset(ESP_LL_t* LL, uint8_t state);

/**
 * \brief  Initializes Low-Level driver to communicate with ESP module
 * \param  *LL: Pointer to \ref ESP_LL_t structure with settings
 * \param  state: State for RTS pin, it can be high or low. Check \ref ESP_RTS_SET and \ref ESP_RTS_CLR
 * \retval Success status:
 *            - 0: Successful
 *            - > 0: Error
 */
uint8_t ESP_LL_SetRTS(ESP_LL_t* LL, uint8_t state);

/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
