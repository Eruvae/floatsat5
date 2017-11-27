/**	
 * |----------------------------------------------------------------------
 * | Copyright (c) 2016 Tilen Majerle
 * |  
 * | Permission is hereby granted, free of charge, to any person
 * | obtaining a copy of this software and associated documentation
 * | files (the "Software"), to deal in the Software without restriction,
 * | including without limitation the rights to use, copy, modify, merge,
 * | publish, distribute, sublicense, and/or sell copies of the Software, 
 * | and to permit persons to whom the Software is furnished to do so, 
 * | subject to the following conditions:
 * | 
 * | The above copyright notice and this permission notice shall be
 * | included in all copies or substantial portions of the Software.
 * | 
 * | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * | OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * | AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * | HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * | WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * | FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * | OTHER DEALINGS IN THE SOFTWARE.
 * |----------------------------------------------------------------------
 */
#include "esp8266_sys.h"

#include <rodos.h>
/******************************************************************************/
/******************************************************************************/
/***   Copy this file to project directory and rename it to "esp8266_sys.c"  **/
/******************************************************************************/
/******************************************************************************/

Semaphore sem;

uint8_t ESP_SYS_Create(ESP_RTOS_SYNC_t* Sync) {
	return 0;
}

uint8_t ESP_SYS_Delete(ESP_RTOS_SYNC_t* Sync) {
	return 0;
}

uint8_t ESP_SYS_Request(ESP_RTOS_SYNC_t* Sync) {
	sem.enter();
	return 0;
}

uint8_t ESP_SYS_Release(ESP_RTOS_SYNC_t* Sync) {
	sem.leave();
	return 0;
}
