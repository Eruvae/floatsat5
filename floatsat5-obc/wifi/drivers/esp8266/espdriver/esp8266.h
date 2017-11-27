/**
 * \author  Tilen Majerle
 * \email   tilen@majerle.eu
 * \website https://majerle.eu/projects/esp8266-at-commands-parser-for-embedded-systems
 * \version v2.2.0
 * \license MIT
 * \brief   Library for ESP8266 module using AT commands for embedded systems
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
#ifndef ESP_H
#define ESP_H 220

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
 
/**
 * \defgroup ESP
 * \brief           High level, application part of module
 * \{
 *
 * Application layer for user use.
 *
 */

/* Standard C libraries */
#include "string.h"
#include "stdio.h"
#include "stdint.h"

/* Low level based implementation */
#include "esp8266_ll.h"

/* Include configuration */
#include "esp8266_config.h"

/* Buffer implementation */
#include "buffer.h"

/* Buffer implementation */
#include "pt/pt.h"

/* When RTOS is enabled */
#if ESP_RTOS
#include "esp8266_sys.h"
#endif

/* Check values */
#if !defined(ESP_CONF_H) || ESP_CONF_H != ESP_H
//#error Wrong configuration file!
#endif

/**
 * \defgroup      ESP_Macros
 * \brief         Library defines
 * \{
 */

/* Backward ocmpatibility definitions */
#if !defined(ESP_SINGLE_CONN)       
#define ESP_SINGLE_CONN             0   /*!< Single connection mode */
#endif

/* This settings should not be modified */
#if ESP_SINGLE_CONN
#define ESP_MAX_CONNECTIONS         1   /*!< Number of maximum active connections on ESP */
#else
#define ESP_MAX_CONNECTIONS         5   /*!< Number of maximum active connections on ESP */
#endif /* ESP_SINGLE_CONN */

#define evol                        volatile
#define estatic                     static

/* Check debug */
#ifndef ESP_ECHO
#define ESP_ECHO                    1
#endif

/* Public defines */
#define ESP_MIN_BAUDRATE            (110UL)             /*!< Minimum baud for UART communication */
#define ESP_MAX_BAUDRATE            (4608000UL)         /*!< Maximum baud for UART communication */

/**
 * \}
 */
 
/**
 * \defgroup      ESP_Typedefs
 * \brief         Library Typedefs
 * \{
 */

/**
 * \brief         ESP8266 library possible return statements on function calls
 */
typedef enum _ESP_Result_t {
	espOK = 0x00,                                       /*!< Everything is OK */
	espERROR,                                           /*!< An error occurred */
    espLLERROR,                                         /*!< Low-level error */
    espSYSERROR,                                        /*!< System call error */
    espPARERROR,                                        /*!< Parameter error */
	espDEVICENOTCONNECTED,                              /*!< Device is not connected to UART */
	espTIMEOUT,                                         /*!< Timeout was detected when sending command to ESP module */
	espNOHEAP,                                          /*!< Heap memory is not available */
	espWIFINOTCONNECTED,                                /*!< Wifi is not connected to network */
	espBUSY,                                            /*!< Device is busy, new command is not possible */
	espINVALIDPARAMETERS,                               /*!< Parameters for functions are invalid */
    espSENDERROR,                                       /*!< Error trying to send data on connection */
    espSSLERROR,                                        /*!< Connection SSL error, when there is already a connection with SSL */
    
    espAPNOTFOUND,                                      /*!< AP was not found to connect to */
    espWRONGPASSWORD                                    /*!< Password is wrong */
} ESP_Result_t;

/**
 * \brief         ESP8266 modes of operation enumeration
 */
typedef enum _ESP_Mode_t {
	ESP_Mode_STA = 0x01,                                /*!< ESP in station mode */
	ESP_Mode_AP = 0x02,                                 /*!< ESP as software Access Point mode */
	ESP_Mode_STA_AP = 0x03                              /*!< ESP in both modes */
} ESP_Mode_t;

/**
 * \brief         Security settings for wifi network
 */
typedef enum _ESP_Ecn_t {
	ESP_Ecn_OPEN = 0x00,                                /*!< Wifi is open */
	ESP_Ecn_WEP = 0x01,                                 /*!< Wired Equivalent Privacy option for wifi security. \note  This mode can't be used when setting up ESP8266 wifi */
	ESP_Ecn_WPA_PSK = 0x02,                             /*!< Wi-Fi Protected Access */
	ESP_Ecn_WPA2_PSK = 0x03,                            /*!< Wi-Fi Protected Access 2 */
	ESP_Ecn_WPA_WPA2_PSK = 0x04,                        /*!< Wi-Fi Protected Access with both modes */
} ESP_Ecn_t;

/**
 * \brief         Firmware update statuses
 */
typedef enum _ESP_FirmwareUpdate_t {
	ESP_FirmwareUpdate_ServerFound = 0x01,              /*!< Server for update has been found */
	ESP_FirmwareUpdate_Connected = 0x02,                /*!< We are connected to server for firmware */
	ESP_FirmwareUpdate_GotEdition = 0x03,               /*!< We have firmware edition to download */
	ESP_FirmwareUpdate_StartUpdate = 0x04,              /*!< Update has started */
} ESP_FirmwareUpdate_t;

/**
 * \brief         Sleep mode enumeration
 */
typedef enum _ESP_SleepMode_t {
	ESP_SleepMode_Disable = 0x00,                       /*!< Sleep mode disabled */
	ESP_SleepMode_Light = 0x01,                         /*!< Light sleep mode */
	ESP_SleepMode_Modem = 0x02                          /*!< Model sleep mode */
} ESP_SleepMode_t;

/**
 * \brief         Connection type
 */
typedef enum _ESP_CONN_Type_t {
	ESP_CONN_Type_TCP = 0x00,                           /*!< Connection type is TCP */
	ESP_CONN_Type_UDP = 0x01,                           /*!< Connection type is UDP */
	ESP_CONN_Type_SSL = 0x02                            /*!< Connection type is SSL */
} ESP_CONN_Type_t;

/**
 * \brief         Connection structure
 */
typedef struct _ESP_CONN_t {
	uint8_t Number;                                     /*!< Connection number */
	uint16_t RemotePort;                                /*!< Remote PORT number */
	uint8_t RemoteIP[4];                                /*!< IP address of device */
    uint16_t LocalPort;                                 /*!< Local PORT number */
	ESP_CONN_Type_t Type;                               /*!< Connection type. Parameter is valid only if connection is made as client */
#if ESP_CONN_SINGLEBUFFER
    uint8_t* Data;                                      /*!< Pointer to data array */
#else
    uint8_t Data[ESP_CONNBUFFER_SIZE + 1];              /*!< Received data on connection */
#endif
    uint16_t DataLength;                                /*!< Number of bytes received in connection packet */
    
    uint32_t TotalBytesReceived;                        /*!< Number of total bytes so far received on connection */
    uint32_t DataStartTime;                             /*!< Current time in units of milliseconds when first data packet was received on connection */
	union {
		struct {
			uint8_t Active:1;                           /*!< Status if connection is active */
			uint8_t Client:1;                           /*!< Set to 1 if connection was made as client */
            uint8_t SSL:1;                              /*!< Connection has been made as SSL */
        } F;
		uint8_t Value;                                  /*!< Value of entire union */
	} Flags;                                            /*!< Connection flags management */
    union {
        struct {
            uint8_t Connect:1;                          /*!< Connection was just connected, client or server */
            uint8_t Closed:1;                           /*!< Connection was just disconnected, client or server */
            uint8_t DataSent:1;                         /*!< Data were sent successfully */
            uint8_t DataError:1;                        /*!< Error trying to send data */
            uint8_t CallLastPartOfPacketReceived:1;     /*!< Data are processed synchronously. When there is last part of packet received and command is not idle, we must save notification for callback */
        } F;
        uint8_t Value;
    } Callback;                                         /*!< Flags for callback management */
} ESP_CONN_t;

/**
 * \brief         IPD network data structure
 */
typedef struct _ESP_IPD_t {
	uint8_t InIPD;                                      /*!< Set to 1 when ESP is in IPD mode with data */
	ESP_CONN_t* Conn;                                   /*!< Connection number where IPD is active */
    uint16_t BytesRemaining;                            /*!< Remaining bytes to read from entire IPD statement */
    uint16_t BytesRead;                                 /*!< Bytes read in current packet */
} ESP_IPD_t;

/**
 * \brief         Connected AP structure
 */
typedef struct _ESP_ConnectedAP_t {
	char SSID[20 + 1];                                  /*!< SSID network name */
	uint8_t MAC[6];                                     /*!< MAC address of network */
	uint8_t Channel;                                    /*!< Network channel */
	int16_t RSSI;                                       /*!< Signal strength */
} ESP_ConnectedAP_t;

/**
 * \brief         AP station structure to use when searching for network
 */
typedef struct _ESP_AP_t {
	uint8_t Ecn;                                        /*!< Security of Wi-Fi spot. This parameter has a value of \ref ESP_Ecn_t enumeration */
	char SSID[20 + 1];                                  /*!< Service Set Identifier value. Wi-Fi spot name */
	int16_t RSSI;                                       /*!< Signal strength of Wi-Fi spot */
	uint8_t MAC[6];                                     /*!< MAC address of spot */
	uint8_t Channel;                                    /*!< Wi-Fi channel */
	int8_t Offset;                                      /*!< Frequency offset from base 2.4GHz in kHz */
	uint8_t Calibration;                                /*!< Frequency offset calibration */
} ESP_AP_t;

/**
 * \brief         Structure for connected station to softAP to ESP module
 */
typedef struct _ESP_ConnectedStation_t {
	uint8_t IP[4];                                      /*!< IP address of connected station */
	uint8_t MAC[6];                                     /*!< MAC address of connected station */
} ESP_ConnectedStation_t;

/**
 * \brief         Access point configuration
 */
typedef struct _ESP_APConfig_t {
	char SSID[20];                                      /*!< Network public name for ESP AP mode */
	char Pass[20];                                      /*!< Network password for ESP AP mode */
	ESP_Ecn_t Ecn;                                      /*!< Security of Wi-Fi spot. This parameter can be a value of \ref ESP_Ecn_t enumeration */
	uint8_t Channel;                                    /*!< Channel Wi-Fi is operating at */
	uint8_t MaxConnections;                             /*!< Max number of stations that are allowed to connect to ESP AP, between 1 and 4 */
	uint8_t Hidden;                                     /*!< Set to 1 if network is hidden (not broadcast) or zero if noz */
} ESP_APConfig_t;

/**
 * \brief         DNS structure
 */
typedef struct _ESP_Domain_t {
    const char* Domain;                                 /*!< Domain name for IP */
    uint8_t IP[4];                                      /*!< IP for domain */
    uint8_t Successful;                                 /*!< Status flag */
} ESP_Domain_t;

/**
 * \brief         Event enumeration for callback
 */
typedef enum _ESP_Event_t {
    espEventIdle = 0x00,                                /*!< Stack went idle and is ready to accept new instruction */
    espEventDataReceived,                               /*!< Data were received on connection */
    espEventWifiConnected,                              /*!< Wifi has connected to network */
    espEventWifiDisconnected,                           /*!< Wifi has disconnected to network */
    espEventWifiGotIP,                                  /*!< Wifi got IP address */
    espEventWifiDhcpTimeout,                            /*!< Wifi DHCP timeout to assing IP address */
    espEventConnActive,                                 /*!< Connection is just active, either client or server mode */
    espEventConnClosed,                                 /*!< Connection is just closed, either client or server mode */
    espEventDataSent,                                   /*!< Data were sent on connection */
    espEventDataSentError,                              /*!< Error trying to sent data on connection */
} ESP_Event_t;

/**
 * \brief         Parameters for callback processing
 */
typedef struct _ESP_EventParams_t {
    const void* CP1;                                    /*!< Constant void pointer number 1 */
    const void* CP2;                                    /*!< Constant void pointer number 2 */
    uint32_t UI;                                        /*!< Unsigned integer value */
} ESP_EventParams_t;

/**
 * \brief         Callback function prototype
 */
typedef int (*ESP_EventCallback_t)(ESP_Event_t, ESP_EventParams_t *);

/**
 * \brief         Main ESP8266 working structure
 */
typedef struct _ESP_t {
    evol uint32_t Time;                                 /*!< Current time in units of milliseconds */
    evol ESP_Result_t RetVal;                           /*!< Return value */
    
    /*!< Low-Level management */
    ESP_LL_t LL;                                        /*!< Structure for Low-Level communication */
    
    /*!< Active command informations */
    evol uint16_t ActiveCmd;                            /*!< Current active command for execution */
    evol uint16_t ActiveCmdSaved;                       /*!< Value of saved active CMD when necessary to change active command while processing one. */
    const char* evol ActiveCmdResp;                     /*!< Pointer to active command response we are waiting for */
    evol uint32_t ActiveCmdStart;                       /*!< Time when new command started with execution */
    evol ESP_Result_t ActiveResult;                     /*!< Result to return from function */
    evol uint32_t ActiveCmdTimeout;                     /*!< Timeout in units of MS for active command to finish */

	uint32_t Timeout;                                   /*!< Timeout in milliseconds for command to return response */
    
    /*!< RTOS support */
#if ESP_RTOS
    ESP_RTOS_SYNC_t Sync;                               /*!< RTOS synchronization object */
#endif
    
    /*!< Station informations */
	uint8_t STAIP[4];                                   /*!< Assigned IP address for station for ESP module */
	uint8_t STAGateway[4];                              /*!< Gateway address for station ESP is using */
	uint8_t STANetmask[4];                              /*!< Netmask address for station ESP is using */
	uint8_t STAMAC[6];                                  /*!< MAC address for station of ESP module */
    
    /*!< Access Point informations */
	uint8_t APIP[4];                                    /*!< Assigned IP address for softAP for ESP module */
	uint8_t APGateway[4];                               /*!< Gateway address ESP for softAP is using */
	uint8_t APNetmask[4];                               /*!< Netmask address ESP for softAP is using */
	uint8_t APMAC[6];                                   /*!< MAC address for softAP of ESP module */
    ESP_APConfig_t APConf;                              /*!< Soft AP configuration */
    
    /* Connections structure */
	ESP_CONN_t Conn[ESP_MAX_CONNECTIONS];               /*!< Array of connections */
    uint8_t ActiveConns;                                /*!< Bit variable of active connections on ESP8266 from CIPSTATUS response */
    uint8_t ActiveConnsResp;                            /*!< List of active connections */
    
    /*!< Incoming data structure */
    ESP_IPD_t IPD;                                      /*!< IPD network data structure */

	union {
		struct {
            uint8_t IsBlocking:1;                       /*!< Status whether action was called as blocking */
            uint8_t Call_Idle:1;                        /*!< Status whether idle status event should be called before we can proceed with another action */
		} F;
		uint32_t Value;
	} Flags;                                            /*!< Flags for library purpose */
    
    /*!< Callback management */
    union {
        struct {
            uint8_t WifiConnected:1;                    /*!< Wifi just got connected */
            uint8_t WifiDisconnected:1;                 /*!< Wifi just got disconnected */
            uint8_t WifiGotIP:1;                        /*!< Wifi station just got IP address */
        } F;
        uint32_t Value;
    } CallbackFlags;                                    /*!< List of global flags for callback events */
    ESP_EventCallback_t Callback;                       /*!< Pointer to callback function */
    ESP_EventParams_t CallbackParams;                   /*!< Callback parameters */
    
    /*!< Events management with receive interaction */
    union {
        struct {
            uint8_t RespOk:1;                           /*!< OK message response */
            uint8_t RespError:1;                        /*!< Error message response */
            uint8_t RespBracket:1;                      /*!< Bracket received (SMS messages) */
            uint8_t RespReady:1;                        /*!< Ready statement was received */
            
            uint8_t RespConnectOk:1;                    /*!< n, CONNECT OK was returned from device */
            uint8_t RespConnectFail:1;                  /*!< n, CONNECT FAIL was returned from device */
            uint8_t RespConnectAlready:1;               /*!< n, ALREADY CONNECTED was returned from device */
            uint8_t RespCloseOk:1;                      /*!< n, CLOSE OK was returned from device */
            uint8_t RespSendOk:1;                       /*!< n, SEND OK was returned from device */
            uint8_t RespSendFail:1;                     /*!< n, SEND FAIL was returned from device */
            
            uint8_t RespWifiConnected:1;
            uint8_t RespWifiDisconnected:1;
            uint8_t RespWifiGotIp:1;
        } F;
        uint32_t Value;                                 /*!< Value containing all the flags in single memory */
    } Events;                                           /*!< Union holding all the required events for library internal processing */
} ESP_t;

/**
 * \}
 */

/**
 * \defgroup      ESP_Functions
 * \brief         ESP Functions
 * \{
 */

/**
 * \brief         Initializes ESP stack and prepares device to working state
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     baudrate: Baudrate for UART to communicate with ESP8266 module
 * \param[in]     callback: Pointer to callback function stack will use to notify user about updates
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_Init(evol ESP_t* ESP, uint32_t baudrate, ESP_EventCallback_t callback);

/**
 * \brief         Deinitializes ESP stack
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_DeInit(evol ESP_t* ESP);

/**
 * \brief         Waits stack to be ready
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     timeout: Timeout in units of milliseconds to wait for stack to be ready
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_WaitReady(evol ESP_t* ESP, uint32_t timeout);

/**
 * \brief         Delay for amount of time
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     delay: Number of milliseconds to delay
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_Delay(evol ESP_t* ESP, uint32_t delay);

/**
 * \brief         Checks if stack is ready
 * \note          This checks only flag in library. When working with RTOS, this function may not return actual value
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_IsReady(evol ESP_t* ESP);

/**
 * \brief         Updates ESP stack
 * \note          When RTOS is used, this should be used in separate thread
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_Update(evol ESP_t* ESP);

/**
 * \brief         Process callback calls
 * \note          \li When in RTOS or ASYNC mode, user should use separate thread than one used for \ref ESP_Update
 *                \li When in non-RTOS mode function is called from \ref ESP_Update function, user should not worry about it.
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_ProcessCallbacks(evol ESP_t* ESP);

/**
 * \brief         Update time for stack from timer IRQ or any other time source
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     time_increase: Number of milliseconds to increase after last function call
 * \retval        Member of \ref ESP_Result_t enumeration
 */
void ESP_UpdateTime(evol ESP_t* ESP, uint32_t time_increase);

/**
 * \brief         Add new data to ESP receive buffer
 * \note          Must be called from UART RXNE interrupt or any other input source of data from ESP
 * \param[in]     *ch: Pointer to byte or array of bytes to add to stack's input buffer
 * \param[in]     count: Number of bytes to write to stack's input buffer
 * \retval        Number of bytes written to internal ESP buffer
 */
uint16_t ESP_DataReceived(uint8_t* ch, uint16_t count);

/**
 * \brief         Gets last return status from stack
 * \note          Use this function in callback function to detect returned status of last operation
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_GetLastReturnStatus(evol ESP_t* ESP);

/**
 * \defgroup      MISC_API
 * \brief         Miscellaneous functions
 * \{
 */

/**
 * \brief         Set RF power for hardware
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     pwr: Power in units of dBm
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_SetRFPower(evol ESP_t* ESP, float pwr, uint32_t blocking);

/**
 * \brief         Set current UART baudrate for ESP8266 device
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     baudrate: Baudrate in units of bauds for ESP8266 communication purpose
 * \param[in]     def: Status whether this IP should be stored to ESP flash or not
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_SetUART(evol ESP_t* ESP, uint32_t baudrate, uint32_t def, uint32_t blocking);

/**
 * \brief         Restore ESP8266 flash settings to default values
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_RestoreDefault(evol ESP_t* ESP, uint32_t blocking);

/**
 * \brief         Update ESP8266 firmware update with remote process
 * \note          ESP8266 must be connected to network and have access to internet
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_FirmwareUpdate(evol ESP_t* ESP, uint32_t blocking);

/**
 * \brief         Get AT software info on ESP8266 device
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[out]    *atv: Pointer to array of string to save AT version.
 *                   Length of array should be at least 30 bytes but mostly depends on actual ESP8266 AT software.
 *                   Use NULL if you don't need AT version
 * \param[out]    *sdkv: Pointer to array of string to save SDK version.
 *                   Length of array should be at least 30 bytes but mostly depends on actual ESP8266 AT software.
 *                   Use NULL if you don't need SDK version
 * \param[out]    *cmpt: Pointer to array of string to save compile time version.
 *                   Length of array should be at least 30 bytes but mostly depends on actual ESP8266 AT software.
 *                   Use NULL if you don't need compile time
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_GetSoftwareInfo(evol ESP_t* ESP, char* atv, char* sdkv, char* cmpt, uint32_t blocking);

/**
 * \}
 */

/**
 * \defgroup      STATION_API
 * \brief         Functions regarding ESP as station device
 * \{
 */

/**
 * \brief         Get IP address of station
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[out]    *ip: Pointer to 4 bytes long memory to store IP address to
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_STA_GetIP(evol ESP_t* ESP, uint8_t* ip, uint32_t blocking);

/**
 * \brief         Set IP address of station
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *ip: Pointer to 4 bytes long memory with IP address, MSB first
 * \param[in]     def: Status whether this IP should be stored to ESP flash or not
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_STA_SetIP(evol ESP_t* ESP, const uint8_t* ip, uint8_t def, uint32_t blocking);

/**
 * \brief         Get MAC address of station
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[out]    *mac: Pointer to 6 bytes long memory to store MAC address to
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_STA_GetMAC(evol ESP_t* ESP, uint8_t* mac, uint32_t blocking);

/**
 * \brief         Set MAC address of AP
 * \note          Bit 0 of first byte (mac[0]) can not be set to 1
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *mac: Pointer to 6 bytes long MAC address to use for AP
 * \param[in]     def: Status whether this options should be set in flash for future connections
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_STA_SetMAC(evol ESP_t* ESP, const uint8_t* mac, uint32_t def, uint32_t blocking);

/**
 * \brief         Connect to Wi-Fi network
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *ssid: Pointer to SSID name to connect to
 * \param[in]     *pass: Pointer to password to use for connection
 * \param[in]     *mac: Pointer to MAC address to use for connection when multiple networks with same SSID are present. Use NULL if not required
 * \param[in]     def: Status whether this options should be set in flash for future connections
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_STA_Connect(evol ESP_t* ESP, const char* ssid, const char* pass, const uint8_t* mac, uint32_t def, uint32_t blocking);

/**
 * \brief         Get network informations you are currently connected to
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[out]    *AP: Pointer to \ref ESP_ConnectedAP_t structure to fill response to
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_STA_GetConnected(evol ESP_t* ESP, ESP_ConnectedAP_t* AP,  uint32_t blocking);

/**
 * \brief         Disconnect from Wi-Fi network
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_STA_Disconnect(evol ESP_t* ESP, uint32_t blocking);

/**
 * \brief         Set autoconnect feature
 * \note          Setting will be saved to flash automatically
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     autoconn: Status whether ESP should autoconnect to saved network when network is available
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_STA_SetAutoConnect(evol ESP_t* ESP, uint8_t autoconn, uint32_t blocking);

/**
 * \brief         List access points ESP can connect to
 * \note          This operation can take a while since ESP will scan network for access points
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[out]    *APs: Pointer to array of \ref ESP_AP_t structures to fill access points to
 * \param[in]     atr: Size of array elements in APs pointer
 * \param[out]    *ar: Pointer to save number of found networks
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_STA_ListAccessPoints(evol ESP_t* ESP, ESP_AP_t* APs, uint16_t atr, uint16_t* ar, uint32_t blocking);

/**
 * \}
 */

/**
 * \defgroup      AP_API
 * \brief         Functions regarding SoftAP (software Access Point) on ESP 
 * \{
 */

/**
 * \brief         Get IP address of AP
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[out]    *ip: Pointer to 4 bytes long memory to store IP address to
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_AP_GetIP(evol ESP_t* ESP, uint8_t* ip, uint32_t blocking);

/**
 * \brief         Set IP address of AP
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *ip: Pointer to 4 bytes long memory with IP address, MSB first
 * \param[in]     def: Status whether this IP should be stored to ESP flash or not
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_AP_SetIP(evol ESP_t* ESP, const uint8_t* ip, uint8_t def, uint32_t blocking);

/**
 * \brief         Get MAC address of AP
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[out]    *mac: Pointer to 6 bytes long memory to store MAC address to
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_AP_GetMAC(evol ESP_t* ESP, uint8_t* mac, uint32_t blocking);

/**
 * \brief         Set MAC address of AP
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *mac: Pointer to 6 bytes long MAC address to use for AP
 * \param[in]     def: Status whether this options should be set in flash for future connections
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_AP_SetMAC(evol ESP_t* ESP, const uint8_t* mac, uint32_t def, uint32_t blocking);

/**
 * \brief         List currently connected devices to ESP softAP
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[out]    *stations: Pointer to array of \ref ESP_ConnectedStation_t structures to fill connected stations to
 * \param[in]     size: Number of elements in stations array
 * \param[out]    *sr: Pointer to save number of stations connected to softAP
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_AP_ListConnectedStations(evol ESP_t* ESP, ESP_ConnectedStation_t* stations, uint16_t size, uint16_t* sr, uint32_t blocking);

/**
 * \brief         Get current configuration of softAP
 * \note          This function does not accept output parameters. Instead, parameter in \ref ESP_t structure is filled with data
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_AP_GetConfig(evol ESP_t* ESP, uint32_t blocking);

/**
 * \brief         Set softAP configuration
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *conf: Pointer to \ref ESP_APConfig_t structure with configuration
 * \param[in]     def: Status whether this settings should be saved in flash or not
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_AP_SetConfig(evol ESP_t* ESP, ESP_APConfig_t* conf, uint8_t def, uint32_t blocking);

/**
 * \}
 */

/**
 * \defgroup      SERVER_API
 * \brief         Functions regarding server functionality
 * \{
 */

/**
 * \brief         Enable server functionality
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param [in]    port: Port used for server listening
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_SERVER_Enable(evol ESP_t* ESP, uint16_t port, uint32_t blocking);

/**
 * \brief         Disable server functionality
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_SERVER_Disable(evol ESP_t* ESP, uint32_t blocking);

/**
 * \brief         Set timeout for server connection
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     timeout: Timeout in units of seconds
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_SERVER_SetTimeout(evol ESP_t* ESP, uint16_t timeout, uint32_t blocking);

/**
 * \}
 */

/**
 * \defgroup      CONN_API
 * \brief         Connection management with client and server connection interaction
 * \{
 */

/**
 * \brief         Start a new client connection
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[out]    **conn: Pointer to pointer to save stack connection to user
 * \param[in]     type: Connection type. This parameter can be a value of \ref ESP_CONN_Type_t enumeration
 * \param[in]     *domain: Pointer to domain name or IP address in string format to connect to
 * \param[in]     port: Port used for new connection
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_CONN_Start(evol ESP_t* ESP, ESP_CONN_t** conn, ESP_CONN_Type_t type, const char* domain, uint16_t port, uint32_t blocking);

/**
 * \brief         Send data to active connection
 * \note          This function can be used either when connection is acting like server or client
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *conn: Pointer to \ref ESP_CONN_t structure with active connection
 * \param[in]     *data: Pointer to data to be sent to connection
 * \param[in]     btw: Number of bytes to send
 * \param[out]    *bw: Pointer to variable to store number of bytes actually written to connection and successfully sent
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_CONN_Send(evol ESP_t* ESP, ESP_CONN_t* conn, const uint8_t* data, uint32_t btw, uint32_t* bw, uint32_t blocking);

/**
 * \brief         Close active connection
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *conn: Pointer to \ref ESP_CONN_t structure with active connection
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_CONN_Close(evol ESP_t* ESP, ESP_CONN_t* conn, uint32_t blocking);

/**
 * \brief         Close all active connections
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_CONN_CloseAll(evol ESP_t* ESP, uint32_t blocking);

/**
 * \brief         Status if desired connection is active as client
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *conn: Pointer to \ref ESP_CONN_t structure with connection
 * \retval        Boolean status if connection is active client
 * \note          In this revision function is declared as macro
 * \hideinitializer
 */
#define ESP_CONN_IsClient(ESP, conn)        ((conn)->Flags.F.Client)

/**
 * \brief         Status if desired connection is active as server
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *conn: Pointer to \ref ESP_CONN_t structure with connection
 * \retval        Boolean status if connection is active server
 * \note          In this revision function is declared as macro
 * \hideinitializer
 */
#define ESP_CONN_IsServer(ESP, conn)       (!ESP_CONN_IsClient(ESP, conn))

/**
 * \brief         Status if desired connection is active
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *conn: Pointer to \ref ESP_CONN_t structure with connection
 * \retval        Boolean status if connection is active
 * \note          In this revision function is declared as macro
 * \hideinitializer
 */
#define ESP_CONN_IsActive(ESP, conn)        ((conn)->Flags.F.Active)

/**
 * \brief         Set SSL buffer size for connection
 * \note          Valid number of bytes is between 2048 and 4096
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     size: Number of bytes set for buffer size
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_SetSSLBufferSize(evol ESP_t* ESP, uint32_t size, uint32_t blocking);

/**
 * \}
 */

/**
 * \defgroup      MISC_API
 * \brief         Miscellaneous functions
 * \{
 */

/**
 * \brief         Get IP address for specific domain name (DNS function)
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *domain: Pointer to domain name to get IP address
 * \param[out]    *ip: Pointer to 4 bytes long memory for saving received IP address
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_DOMAIN_GetIp(evol ESP_t* ESP, const char* domain, uint8_t* ip, uint32_t blocking);

/**
 * \brief         Set WPS function for ESP
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     wps: Value to enable (> 0) or disable (0) WPS feature
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_SetWPS(evol ESP_t* ESP, uint8_t wps, uint32_t blocking);

/**
 * \brief         Ping desired domain name or IP address in string form
 * \param[in,out] *ESP: Pointer to working \ref ESP_t structure
 * \param[in]     *addr: Domain name or IP in string format to ping
 * \param[out]    *time: Pointer to output variable to store time information to in units of milliseconds
 * \param[in]     blocking: Status whether this function should be blocking to check for response
 * \retval        Member of \ref ESP_Result_t enumeration
 */
ESP_Result_t ESP_Ping(evol ESP_t* ESP, const char* addr, uint32_t* time, uint32_t blocking);

/**
 * \}
 */
 
//TODO for transparent connection
ESP_Result_t ESP_SetMode(evol ESP_t* ESP, ESP_Mode_t Mode, uint32_t blocking);

/**
 * \}
 */
 
/**
 * \}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
