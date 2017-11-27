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
#include "esp8266.h"

/******************************************************************************/
/******************************************************************************/
/***                           Private structures                            **/
/******************************************************************************/
/******************************************************************************/
typedef struct {
    uint8_t Length;
    uint8_t Data[128];
} Received_t;
#define RECEIVED_ADD(c)                     do { Received.Data[Received.Length++] = (c); Received.Data[Received.Length] = 0; } while (0)
#define RECEIVED_RESET()                    do { Received.Length = 0; Received.Data[0] = 0; } while (0)
#define RECEIVED_LENGTH()                   Received.Length

typedef struct {
    evol const void* CPtr1;
    evol const void* CPtr2;
    evol const void* CPtr3;
    evol void* Ptr1;
    evol void* Ptr2;
    evol void** PPtr1;
    evol uint32_t UI;
} Pointers_t;

/******************************************************************************/
/******************************************************************************/
/***                           Private definitions                           **/
/******************************************************************************/
/******************************************************************************/
#define CHARISNUM(x)                        ((x) >= '0' && (x) <= '9')
#define CHARISHEXNUM(x)                     (((x) >= '0' && (x) <= '9') || ((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F'))
#define CHARTONUM(x)                        ((x) - '0')
#define CHARHEXTONUM(x)                     (((x) >= '0' && (x) <= '9') ? ((x) - '0') : (((x) >= 'a' && (x) <= 'z') ? ((x) - 'a' + 10) : (((x) >= 'A' && (x) <= 'Z') ? ((x) - 'A' + 10) : 0)))
#define ISVALIDASCII(x)                     (((x) >= 32 && (x) <= 126) || (x) == '\r' || (x) == '\n')
#define FROMMEM(x)                          ((const char *)(x))

/* LL drivers */
#define UART_SEND_STR(str)                  ESP_LL_SendData((ESP_LL_t *)&_ESP->LL, (const uint8_t *)(str), strlen((const char *)(str)))
#define UART_SEND(str, len)                 ESP_LL_SendData((ESP_LL_t *)&_ESP->LL, (const uint8_t *)(str), (len))
#define UART_SEND_CH(ch)                    ESP_LL_SendData((ESP_LL_t *)&_ESP->LL, (const uint8_t *)(ch), 1)

#define RESP_OK                             FROMMEM("OK\r\n")
#define RESP_ERROR                          FROMMEM("ERROR\r\n")
#define RESP_BUSY                           FROMMEM("busy p...\r\n")
#define RESP_READY                          FROMMEM("ready\r\n")
#define _CRLF                               FROMMEM("\r\n")

/* List of commands */
#define CMD_IDLE                            ((uint16_t)0x0000)

/* Basic set of commands */
#define CMD_BASIC                           ((uint16_t)0x1000)
#define CMD_BASIC_AT                        ((uint16_t)0x1001)
#define CMD_BASIC_RST                       ((uint16_t)0x1002)
#define CMD_BASIC_GMR                       ((uint16_t)0x1003)
#define CMD_BASIC_GSLP                      ((uint16_t)0x1004)
#define CMD_BASIC_ATE                       ((uint16_t)0x1005)
#define CMD_BASIC_RESTORE                   ((uint16_t)0x1006)
#define CMD_BASIC_UART                      ((uint16_t)0x1007)
#define CMD_BASIC_SLEEP                     ((uint16_t)0x1008)
#define CMD_BASIC_WAKEUPGPIO                ((uint16_t)0x1009)
#define CMD_BASIC_RFPOWER                   ((uint16_t)0x100A)
#define CMD_BASIC_RFVDD                     ((uint16_t)0x100B)
#define CMD_IS_ACTIVE_BASIC(p)              ((p)->ActiveCmd >= 0x1000 && (p)->ActiveCmd < 0x2000)

/* Wifi commands */
#define CMD_WIFI                            ((uint16_t)0x2000)
#define CMD_WIFI_CWMODE                     ((uint16_t)0x2001)
#define CMD_WIFI_CWJAP                      ((uint16_t)0x2002)
#define CMD_WIFI_CWLAPOPT                   ((uint16_t)0x2003)
#define CMD_WIFI_CWLAP                      ((uint16_t)0x2004)
#define CMD_WIFI_CWQAP                      ((uint16_t)0x2005)
#define CMD_WIFI_CWSAP                      ((uint16_t)0x2006)
#define CMD_WIFI_CWLIF                      ((uint16_t)0x2007)
#define CMD_WIFI_CWDHCP                     ((uint16_t)0x2008)
#define CMD_WIFI_CWDHCPS                    ((uint16_t)0x2009)
#define CMD_WIFI_CWAUTOCONN                 ((uint16_t)0x200A)
#define CMD_WIFI_CIPSTAMAC                  ((uint16_t)0x200B)
#define CMD_WIFI_CIPAPMAC                   ((uint16_t)0x200C)
#define CMD_WIFI_CIPSTA                     ((uint16_t)0x200D)
#define CMD_WIFI_CIPAP                      ((uint16_t)0x200E)
#define CMD_WIFI_CWSTARTSMART               ((uint16_t)0x200F)
#define CMD_WIFI_CWSTOPSMART                ((uint16_t)0x2010)
#define CMD_WIFI_CWSTARTDISCOVER            ((uint16_t)0x2011)
#define CMD_WIFI_CWSTOPDISCOVER             ((uint16_t)0x2012)
#define CMD_WIFI_WPS                        ((uint16_t)0x2013)
#define CMD_WIFI_MDNS                       ((uint16_t)0x2014)

#define CMD_WIFI_GETSTAMAC                  ((uint16_t)0x2101)
#define CMD_WIFI_GETAPMAC                   ((uint16_t)0x2102)
#define CMD_WIFI_GETSTAIP                   ((uint16_t)0x2103)
#define CMD_WIFI_GETAPIP                    ((uint16_t)0x2104)
#define CMD_WIFI_SETSTAMAC                  ((uint16_t)0x2105)
#define CMD_WIFI_SETAPMAC                   ((uint16_t)0x2106)
#define CMD_WIFI_LISTACCESSPOINTS           ((uint16_t)0x2107)
#define CMD_WIFI_GETCWJAP                   ((uint16_t)0x2108)
#define CMD_WIFI_GETCWSAP                   ((uint16_t)0x2109)
#define CMD_WIFI_SETCWSAP                   ((uint16_t)0x210A)
#define CMD_WIFI_SETSTAIP                   ((uint16_t)0x210B)
#define CMD_WIFI_SETAPIP                    ((uint16_t)0x210C)
#define CMD_IS_ACTIVE_WIFI(p)               ((p)->ActiveCmd >= 0x2000 && (p)->ActiveCmd < 0x3000)

#define CMD_TCPIP                           ((uint16_t)0x3000)
#define CMD_TCPIP_CIPSTATUS                 ((uint16_t)0x3001)
#define CMD_TCPIP_CIPDOMAIN                 ((uint16_t)0x3002)
#define CMD_TCPIP_CIPSTART                  ((uint16_t)0x3003)
#define CMD_TCPIP_CIPSSLSIZE                ((uint16_t)0x3004)
#define CMD_TCPIP_CIPSEND                   ((uint16_t)0x3005)
#define CMD_TCPIP_CIPSENDEX                 ((uint16_t)0x3006)
#define CMD_TCPIP_CIPSENDBUF                ((uint16_t)0x3007)
#define CMD_TCPIP_CIPBUFSTATUS              ((uint16_t)0x3008)
#define CMD_TCPIP_CIPCHECKSEQ               ((uint16_t)0x3009)
#define CMD_TCPIP_CIPBUFRESET               ((uint16_t)0x300A)
#define CMD_TCPIP_CIPCLOSE                  ((uint16_t)0x300B)
#define CMD_TCPIP_CIFSR                     ((uint16_t)0x300C)
#define CMD_TCPIP_CIPMUX                    ((uint16_t)0x300D)
#define CMD_TCPIP_CIPSERVER                 ((uint16_t)0x300E)
#define CMD_TCPIP_CIPMODE                   ((uint16_t)0x300F)
#define CMD_TCPIP_SAVETRANSLINK             ((uint16_t)0x3010)
#define CMD_TCPIP_CIPSTO                    ((uint16_t)0x3011)
#define CMD_TCPIP_PING                      ((uint16_t)0x3012)
#define CMD_TCPIP_CIUPDATE                  ((uint16_t)0x3013)
#define CMD_TCPIP_CIPDINFO                  ((uint16_t)0x3014)
#define CMD_TCPIP_IPD                       ((uint16_t)0x3015)

#define CMD_TCPIP_SERVERENABLE              ((uint16_t)0x3101)
#define CMD_TCPIP_SERVERDISABLE             ((uint16_t)0x3102)
#define CMD_IS_ACTIVE_TCPIP(p)              ((p)->ActiveCmd >= 0x3000 && (p)->ActiveCmd < 0x4000)
 
#define ESP_DEFAULT_BAUDRATE                115200              /* Default ESP8266 baudrate */
#define ESP_TIMEOUT                         30000               /* Timeout value in milliseconds */

/* Debug */
#define __DEBUG(fmt, ...)                   printf(fmt, ##__VA_ARGS__)

/* Delay milliseconds */
#define __DELAYMS(ESP, x)                   do { volatile uint32_t t = (ESP)->Time; while (((ESP)->Time - t) < (x)); } while (0)

/* Constants */
#define ESP_MAX_RFPWR                       82
#define ESP_MAX_SEND_DATA_LEN               2048

#if ESP_RTOS
#define __IS_BUSY(p)                        ((p)->ActiveCmd != CMD_IDLE || (p)->Flags.F.Call_Idle != 0)
#else
#define __IS_BUSY(p)                        ((p)->ActiveCmd != CMD_IDLE || (p)->Flags.F.Call_Idle != 0)
#endif
#define __IS_READY(p)                       (!__IS_BUSY(p))
#define __CHECK_BUSY(p)                     do { if (__IS_BUSY(p)) { __RETURN(ESP, espBUSY); } } while (0)
#define __CHECK_INPUTS(c)                   do { if (!(c)) { __RETURN(ESP, espPARERROR); } } while (0)

#define __CONN_RESET(c)                     do { memset((void *)c, 0x00, sizeof(ESP_CONN_t)); } while (0)

#if ESP_RTOS == 1
#define __IDLE(p)                           do {\
    if (ESP_SYS_Release((ESP_RTOS_SYNC_t *)&(p)->Sync)) {   \
    }                                           \
    (p)->ActiveCmd = CMD_IDLE;                  \
    __RESET_THREADS(p);                          \
    if (!(p)->Flags.F.IsBlocking) {             \
        (p)->Flags.F.Call_Idle = 1;             \
    }                                           \
    memset((void *)&Pointers, 0x00, sizeof(Pointers));  \
} while (0)
#else
#define __IDLE(p)                           do {\
    (p)->ActiveCmd = CMD_IDLE;                  \
    __RESET_THREADS(p);                         \
    if (!(p)->Flags.F.IsBlocking) {             \
        (p)->Flags.F.Call_Idle = 1;             \
    }                                           \
    memset((void *)&Pointers, 0x00, sizeof(Pointers));  \
} while (0)
#endif

#if ESP_RTOS
#define __ACTIVE_CMD(p, cmd)                do {\
    if (ESP_SYS_Request((ESP_RTOS_SYNC_t *)&(p)->Sync)) {   \
        return espTIMEOUT;                      \
    }                                           \
    if ((p)->ActiveCmd == CMD_IDLE) {           \
        (p)->ActiveCmdStart = (p)->Time;        \
    }                                           \
    (p)->ActiveCmd = (cmd);                     \
} while (0)
#else
#define __ACTIVE_CMD(p, cmd)                do {\
    if ((p)->ActiveCmd == CMD_IDLE) {           \
        (p)->ActiveCmdStart = (ESP)->Time;      \
    }                                           \
    (p)->ActiveCmd = (cmd);                     \
} while (0)
#endif

#define __CMD_SAVE(p)                       (p)->ActiveCmdSaved = (p)->ActiveCmd
#define __CMD_RESTORE(p)                    (p)->ActiveCmd = (p)->ActiveCmdSaved

#define __RETURN(p, v)                      do { (p)->RetVal = (v); return (v); } while (0)
#define __RETURN_BLOCKING(p, b, mt)         do {\
    ESP_Result_t res;                           \
    (p)->ActiveCmdTimeout = mt;                 \
    if (!(b)) {                                 \
        (p)->Flags.F.IsBlocking = 0;            \
        __RETURN(p, espOK);                     \
    }                                           \
    (p)->Flags.F.IsBlocking = 1;                \
    res = ESP_WaitReady(p, mt);                 \
    if (res == espTIMEOUT) {                    \
        return espTIMEOUT;                      \
    }                                           \
    res = (p)->ActiveResult;                    \
    (p)->ActiveResult = espOK;                  \
    return res;                                 \
} while (0)

#define __RST_EVENTS_RESP(p)                do { (p)->Events.Value = 0; (p)->ActiveCmdStart = (p)->Time; } while (0)

#define ESP_CALL_CALLBACK(p, e)             (p)->Callback(e, (ESP_EventParams_t *)&(p)->CallbackParams);

#if ESP_USE_CTS
#define ESP_SET_RTS(p, s)                   do {\
    if (RTSStatus != (s)) {                     \
        RTSStatus = (s);                        \
        ESP_LL_SetRTS((ESP_LL_t *)&(p)->LL, (s));   \
    }                                           \
} while (0)
#else
#define ESP_SET_RTS(p, s)                   (void)0
#endif
     
/* Check device CIPSTATUS */
#define __CHECK_CIPSTATUS(p)                do {\
    __RST_EVENTS_RESP((ESP));                   \
    UART_SEND_STR(FROMMEM("AT+CIPSTATUS"));     \
    UART_SEND_STR(FROMMEM(_CRLF));              \
    StartCommand((ESP), CMD_TCPIP_CIPSTATUS, NULL); \
    PT_WAIT_UNTIL(pt, (ESP)->Events.F.RespOk || \
                    (ESP)->Events.F.RespError); \
} while (0)

/******************************************************************************/
/******************************************************************************/
/***                            Private variables                            **/
/******************************************************************************/
/******************************************************************************/
#if ESP_USE_CTS
static 
uint8_t RTSStatus;                                          /* RTS pin status */
#endif

/* Buffers */
static BUFFER_t Buffer;                                     /* Buffer structure */
static uint8_t Buffer_Data[ESP_BUFFER_SIZE + 1];            /* Buffer data array */
static Received_t Received;                                 /* Received data structure */
static Pointers_t Pointers;                                 /* Pointers object */
static ESP_t* _ESP;
#if ESP_CONN_SINGLEBUFFER
static uint8_t IPD_Data[ESP_CONNBUFFER_SIZE + 1];           /* Data buffer for incoming connection */
#endif

static
struct pt pt_BASIC, pt_WIFI, pt_TCPIP;

#define __RESET_THREADS(GSM)                  do {          \
PT_INIT(&pt_BASIC); PT_INIT(&pt_WIFI); PT_INIT(&pt_TCPIP);  \
} while (0);

/******************************************************************************/
/******************************************************************************/
/***                            Private functions                            **/
/******************************************************************************/
/******************************************************************************/
/* Default callback for events */
estatic
int ESP_CallbackDefault(ESP_Event_t evt, ESP_EventParams_t* params) {
    return 0;
}

/* Returns number from hex value */
estatic
uint8_t Hex2Num(char a) {
    if (a >= '0' && a <= '9') {                             /* Char is num */
        return a - '0';
    } else if (a >= 'a' && a <= 'f') {                      /* Char is lowercase character A - Z (hex) */
        return (a - 'a') + 10;
    } else if (a >= 'A' && a <= 'F') {                      /* Char is uppercase character A - Z (hex) */
        return (a - 'A') + 10;
    }
    
    return 0;
}

/* Parses and returns number from string */
estatic
int32_t ParseNumber(const char* ptr, uint8_t* cnt) {
    uint8_t minus = 0, i = 0;
    int32_t sum = 0;
    
    if (*ptr == '-') {                                		/* Check for minus character */
        minus = 1;
        ptr++;
        i++;
    }
    while (CHARISNUM(*ptr)) {                        		/* Parse number */
        sum = 10 * sum + CHARTONUM(*ptr);
        ptr++;
        i++;
    }
    if (cnt) {                                		        /* Save number of characters used for number */
        *cnt = i;
    }
    if (minus) {                                    		/* Minus detected */
        return 0 - sum;
    }
    return sum;                                       		/* Return number */
}

/* Parses and returns HEX number from string */
estatic
uint32_t ParseHexNumber(const char* ptr, uint8_t* cnt) {
    uint32_t sum = 0;
    uint8_t i = 0;
    
    while (CHARISHEXNUM(*ptr)) {                    		/* Parse number */
        sum <<= 4;
        sum += Hex2Num(*ptr);
        ptr++;
        i++;
    }
    
    if (cnt) {                               		        /* Save number of characters used for number */
        *cnt = i;
    }
    return sum;                                        		/* Return number */
}

/* Parse MAC number in string format xx:xx:xx:xx:xx:xx */
estatic
void ParseMAC(evol ESP_t* ESP, const char* str, uint8_t* mac, uint8_t* cnt) {
    uint8_t i = 6;
    while (i--) {
        *mac++ = ParseHexNumber(str, NULL);
        str += 3;
    }
    if (cnt) {
        *cnt = 17;
    }
}

/* Parse IP number in string format xxx.xxx.xxx.xxx */
estatic
void ParseIP(evol ESP_t* ESP, const char* str, uint8_t* ip, uint8_t* cnt) {
    uint8_t i = 4;
    uint8_t c = 0;
    if (cnt) {
        *cnt = 0;
    }
    while (i--) {
        *ip++ = ParseNumber(str, &c);
        str += c + 1;
        if (cnt) {
            *cnt += c;
            if (i) {
                *cnt += 1;
            }
        }
    }
}

/* Parse +CWLAP statement */
estatic
void ParseCWLAP(evol ESP_t* ESP, const char* str, ESP_AP_t* AP) {
    uint8_t cnt;
    
    if (*str == '(') {                                      /* Remove opening bracket */
        str++;
    }
    
    memset((void *)AP, 0x00, sizeof(ESP_AP_t));             /* Reset structure first */
    
    AP->Ecn = ParseNumber(str, &cnt);                       /* Parse ECN value */
    str += cnt + 1;
    
    if (*str == '"') {                                      /* Remove opening " */
        str++;
    }
    
    cnt = 0;                                                /* Parse SSID */
    while (*str) {
        if (*str == '"' && *(str + 1) == ',') {
            break;
        }
        if (cnt < sizeof(AP->SSID) - 1) {
            AP->SSID[cnt] = *str;
        }
        
        cnt++;
        str++;
    }
    
    str += 2;                                               /* Parse RSSI */
    AP->RSSI = ParseNumber(str, &cnt);
    
    str += cnt + 1;
    if (*str == '"') {
        str++;
    }
    ParseMAC(ESP, str, AP->MAC, NULL);                      /* Parse MAC */
    
    str += 19;                                              /* Ignore mac, " and comma */
    
    AP->Channel = ParseNumber(str, &cnt);                   /* Parse channel for wifi */
    str += cnt + 1;
    
    AP->Offset = ParseNumber(str, &cnt);                    /* Parse offset */
    str += cnt + 1;
    
    AP->Calibration = ParseNumber(str, &cnt);               /* Parse calibration number */
    str += cnt + 1;
}

/* Parse +CWJAP statement */
estatic
void ParseCWJAP(evol ESP_t* ESP, const char* ptr, ESP_ConnectedAP_t* AP) {
    uint8_t i, cnt;
    
    while (*ptr && *ptr != '"') {                    		/* Find first " character */
        ptr++;
    }
    if (!*ptr) {                                    		/* Check if zero detected */
        return;
    }
    ptr++;                                            		/* Remove first " for SSID */
    i = 0;                                            		/* Parse SSID part */
    while (*ptr && (*ptr != '"' || *(ptr + 1) != ',' || *(ptr + 2) != '"')) {
        AP->SSID[i++] = *ptr++;
    }
    AP->SSID[i++] = 0;
    ptr += 3;                                        		/* Increase pointer by 3, ignore "," part */
    ParseMAC(ESP, ptr, AP->MAC, NULL);    	                /* Get MAC */
    ptr += 19;                                    		    /* Increase counter by elements in MAC address and ", part */
    AP->Channel = ParseNumber(ptr, &cnt);	                /* Get channel */
    ptr += cnt + 1;                                    		/* Increase position */
    AP->RSSI = ParseNumber(ptr, &cnt);    /* Get RSSI */
}

/* Parse CWLIF statement with IP and MAC */
estatic
void ParseCWLIF(evol ESP_t* ESP, const char* str, ESP_ConnectedStation_t* station) {
    uint8_t cnt;
    
    ParseIP(ESP, str, station->IP, &cnt);                   /* Parse IP address */
    str += cnt + 1;
    ParseMAC(ESP, str, station->MAC, &cnt);                 /* Parse MAC */
}

/* Parse incoming IPD statement */
estatic
void ParseIPD(evol ESP_t* ESP, const char* str, ESP_IPD_t* IPD) {
    uint8_t cnt;
    
    memset((void *) IPD, 0x00, sizeof(ESP_IPD_t));          /* Reset structure */
    
#if !ESP_SINGLE_CONN
    IPD->Conn = (ESP_CONN_t *)&ESP->Conn[ParseNumber(str, &cnt)];   /* Get connection */
    str += cnt + 1;
#else
    IPD->Conn = (ESP_CONN_t *)&ESP->Conn[0];                /* Get connection */
#endif /* ESP_SINGLE_CONN */
    IPD->BytesRemaining = ParseNumber(str, &cnt);           /* Set bytes remaining to read */
}

/* Parses +CWSAP statement */
estatic
void ParseCWSAP(evol ESP_t* ESP, const char* ptr, ESP_APConfig_t* AP) {
    uint8_t cnt, i;
    
    memset((void *)AP, 0x00, sizeof(ESP_APConfig_t));       /* Reset structure */
    
    i = 0;                                            		/* Copy till "," which indicates end of SSID string and start of password part */
    while (*ptr && (*ptr != '"' || *(ptr + 1) != ',' || *(ptr + 2) != '"')) {
        AP->SSID[i++] = *ptr++;
    }
    AP->SSID[i++] = 0;
    ptr += 3;                                        		/* Increase pointer by 3, ignore "," part */
    i = 0;                                            		/* Copy till ", which indicates end of password string and start of number */
    while (*ptr && (*ptr != '"' || *(ptr + 1) != ',')) {
        AP->Pass[i++] = *ptr++;
    }
    AP->Pass[i++] = 0;
    ptr += 2;                                        		/* Increase pointer by 2 */
    AP->Channel = ParseNumber(ptr, &cnt);    		        /* Get channel number */
    ptr += cnt + 1;                                    		/* Increase pointer and comma */
    AP->Ecn = (ESP_Ecn_t)ParseNumber(ptr, &cnt);            /* Get ECN value */
    ptr += cnt + 1;                                    		/* Increase pointer and comma */
    AP->MaxConnections = ParseNumber(ptr, &cnt);            /* Get max connections value */
    ptr += cnt + 1;                                    		/* Increase pointer and comma */
    AP->Hidden = ParseNumber(ptr, &cnt);    		        /* Get hidden value */
}

/* Parse CIPSTATUS value */
estatic
void ParseCIPSTATUS(evol ESP_t* ESP, uint8_t* value, const char* str) {
    uint8_t i, cnt;
    uint8_t connNumber = 0;
    
#if !ESP_SINGLE_CONN
    connNumber = CHARTONUM(*str);                           /* Get connection number */
#endif    /* ESP_SINGLE_CONN */
    *value |= 1 << connNumber;                              /* Set bit according to active connection */
    
    /* Parse connection parameters */
    str += 2;
    while (*str && *str != ',') {
        str++;
    }
    str++;
    
    /* Parse connection IP */
    str++;
    for (i = 0; i < 4; i++) {
        ESP->Conn[connNumber].RemoteIP[i] = ParseNumber(str, &cnt);  /* Get remote IP */
        str += cnt + 1;
    }
    str++;
    
    /* Parse Remove PORT */
    ESP->Conn[connNumber].RemotePort = ParseNumber(str, &cnt);
    str += cnt + 1;
    
    /* Parse local port */
    ESP->Conn[connNumber].LocalPort = ParseNumber(str, &cnt);
    str += cnt + 1;
    
    /* Parse client/server type */
    ESP->Conn[connNumber].Flags.F.Client = CHARTONUM(*str) == 0;
}

/* Starts command and sets pointer for return statement */
estatic 
ESP_Result_t StartCommand(evol ESP_t* ESP, uint16_t cmd, const char* cmdResp) {
    ESP->ActiveCmd = cmd;
    ESP->ActiveCmdResp = (char *)cmdResp;
    ESP->ActiveCmdStart = ESP->Time;
    ESP->ActiveResult = espOK;
    
    if (cmd == CMD_TCPIP_CIPSTATUS) {                       /* On CIPSTATUS command */
        ESP->ActiveConnsResp = 0;                           /* Reset active connections status */
    }
    
    return espOK;
}

/* Converts number to string */
estatic
void NumberToString(char* str, uint32_t number) {
    sprintf(str, "%d", number);
}

/* Converts number to hex for MAC */
estatic
void HexNumberToString(char* str, uint8_t number) {
    sprintf(str, "%02X", number);
}

/* Escapes string and sends directly to output stream */
estatic
void EscapeStringAndSend(const char* str) {
    char special = '\\';
    
    while (*str) {                                    		/* Go through string */
        if (*str == ',' || *str == '"' || *str == '\\') {	/* Check for special character */    
            UART_SEND_CH(&special);                         /* Send special character */
        }
        UART_SEND_CH(str++);                                /* Send character */
    }
}

/* Process received character */
estatic 
void ParseReceived(evol ESP_t* ESP, Received_t* Received) {
    char* str = (char *)Received->Data;
    
    uint8_t is_ok = 0, is_error = 0, len;
    len = strlen(str);                                      /* String length */
    
    if (*str == '\r' && *(str + 1) == '\n') {               /* Check empty line */
        return;
    }

    is_ok = strcmp(str, RESP_OK) == 0;                      /* Check if OK received */
    if (!is_ok) {
        is_error = strcmp(str, RESP_ERROR) == 0;            /* Check if error received */
        if (!is_error) {
            is_error = strcmp(str, RESP_BUSY) == 0;         /* Check if busy */
        }
    }
    
    if (!is_ok && !is_error) {
        if (strcmp(str, RESP_READY) == 0) {
            ESP->Events.F.RespReady = 1;                    /* Device is ready flag */
        }
    }
    
    /* Device info */
    if (ESP->ActiveCmd == CMD_BASIC_GMR) {
        if ((str[0] == 'A' || str[0] == 'a') && Pointers.CPtr1 && len > 13) {   /* "AT version:" received */
            strncpy((char *)Pointers.CPtr1, &str[11], len - 13);    /* Save AT version */
            ((char *)Pointers.CPtr1)[len - 13] = 0;
        }
        if ((str[0] == 'S' || str[0] == 's') && Pointers.CPtr2) {   /* "SDK version:" received */
            strncpy((char *)Pointers.CPtr2, &str[12], len - 14);    /* Save SDK version */
            ((char *)Pointers.CPtr2)[len - 14] = 0;                 /* End of strings */
        }
        if ((str[0] == 'C' || str[0] == 'c') && Pointers.CPtr3) {   /* "compile time:" received */
            strncpy((char *)Pointers.CPtr3, &str[13], len - 15);    /* Save compile time version */
            ((char *)Pointers.CPtr3)[len - 15] = 0;                 /* End of strings */
        }
    }
    
    if (ESP->ActiveCmd == CMD_WIFI_CIPSTAMAC && str[0] == '+' && strncmp(str, FROMMEM("+CIPSTAMAC"), 10) == 0) {    /* On CIPSTAMAC active command */
        ParseMAC(ESP, str + 16, (uint8_t *)&ESP->STAMAC, NULL); /* Parse MAC */
        if (Pointers.Ptr1) {
            memcpy((void *)Pointers.Ptr1, (void *)&ESP->STAMAC, 6);
        }
    }
    if (ESP->ActiveCmd == CMD_WIFI_CIPAPMAC && str[0] == '+' && strncmp(str, FROMMEM("+CIPAPMAC"), 9) == 0) {   /* On CIPSTAMAC active command */
        ParseMAC(ESP, str + 15, (uint8_t *)&ESP->APMAC, NULL);  /* Parse MAC */
        if (Pointers.Ptr1) {
            memcpy((void *)Pointers.Ptr1, (void *)&ESP->APMAC, 6);
        }
    }
    if (ESP->ActiveCmd == CMD_WIFI_CIPSTA && str[0] == '+' && strncmp(str, FROMMEM("+CIPSTA_"), 8) == 0) {  /* +CIPSTA received */
        if (str[12] == 'i') {                               /* +CIPSTA_CUR:ip received */
            ParseIP(ESP, str + 16, (uint8_t *)&ESP->STAIP, NULL);   /* Parse IP string */
            if (Pointers.Ptr1) {
                memcpy((void *)Pointers.Ptr1, (void *)&ESP->STAIP, 4);
            }
        } else if (str[12] == 'g') {
            ParseIP(ESP, str + 21, (uint8_t *)&ESP->STAGateway, NULL);  /* Parse IP string */
        } else if (str[12] == 'n') {
            ParseIP(ESP, str + 21, (uint8_t *)&ESP->STANetmask, NULL);  /* Parse IP string */
        }
    }
    if (ESP->ActiveCmd == CMD_WIFI_CIPAP && str[0] == '+' && strncmp(str, FROMMEM("+CIPAP_"), 7) == 0) {    /* +CIPAP received */
        if (str[11] == 'i') {                               /* +CIPSTA_CUR:ip received */
            ParseIP(ESP, str + 15, (uint8_t *)&ESP->APIP, NULL);    /* Parse IP string */
            if (Pointers.Ptr1) {
                memcpy((void *)Pointers.Ptr1, (void *)&ESP->STAIP, 4);
            }
        } else if (str[11] == 'g') {
            ParseIP(ESP, str + 20, (uint8_t *)&ESP->APGateway, NULL);   /* Parse IP string */
        } else if (str[11] == 'n') {
            ParseIP(ESP, str + 20, (uint8_t *)&ESP->APNetmask, NULL);   /* Parse IP string */
        }
    }
    
    if (ESP->ActiveCmd == CMD_WIFI_CWLIF && CHARISNUM(str[0])) {    /* IP of device connected to AP received */
        if (*(uint16_t *)Pointers.Ptr2 < Pointers.UI) {     /* Check if memory still available */
            ParseCWLIF(ESP, str, (ESP_ConnectedStation_t *)Pointers.Ptr1);  /* Parse CWLIF statement */
            Pointers.Ptr1 = ((ESP_ConnectedStation_t *)Pointers.Ptr1) + 1;
            *(uint16_t *)Pointers.Ptr2 = (*(uint16_t *)Pointers.Ptr2) + 1;  /* Increase number of parsed elements */
        }
    }
    
    /* We received string starting with + sign = some useful data! */
    if (*str == '+') {
        if (ESP->ActiveCmd == CMD_WIFI_CWLAP && strncmp(str, FROMMEM("+CWLAP"), 6) == 0) {  /* When active command is listing wifi stations */
            if (*(uint16_t *)Pointers.Ptr2 < Pointers.UI) {     /* Check if memory still available */
                ParseCWLAP(ESP, str + 7, (ESP_AP_t *)Pointers.Ptr1);  /* Parse CWLAP statement */
                Pointers.Ptr1 = ((ESP_AP_t *)Pointers.Ptr1) + 1;
                *(uint32_t *)Pointers.Ptr2 = (*(uint16_t *)Pointers.Ptr2) + 1;  /* Increase number of parsed elements */
            }
        }
    }
    
    if (ESP->ActiveCmd == CMD_WIFI_CWSAP) {
        if (strncmp(str, FROMMEM("+CWSAP"), 6) == 0) {      /* Check for response */
            ParseCWSAP(ESP, str + 12, (ESP_APConfig_t *)&ESP->APConf);   /* Parse config from AP */             
        }
    }
    
    /* Connecting to AP */
    if (ESP->ActiveCmd == CMD_WIFI_CWJAP) {                 /* Trying to connect to wifi network */
        if (strcmp(str, FROMMEM("FAIL\r\n")) == 0) {        /* Fail received */
            is_error = 1;
        } else if (strncmp(str, FROMMEM("+CWJAP_CUR"), 10) == 0) {  /* Received currently connected AP info */
            ParseCWJAP(ESP, str + 10, (ESP_ConnectedAP_t *)Pointers.Ptr1);  /* Parse and save */
        }
    }
    
    /* Pinging */
    if (ESP->ActiveCmd == CMD_TCPIP_PING && str[0] == '+' && CHARISNUM(str[1])) {
        *(uint32_t *)Pointers.Ptr1 = ParseNumber(str + 1, NULL);    /* Parse response time */
    }
    
    /* Wifi management informations */
    if (strcmp(str, FROMMEM("WIFI CONNECTED\r\n")) == 0) {  /* Just connected */
        if (ESP->ActiveCmd == CMD_WIFI_CWJAP) {             /* If trying to join AP */
            ESP->Events.F.RespWifiConnected = 1;
        }
        ESP->CallbackFlags.F.WifiConnected = 1;
    } else if (strcmp(str, FROMMEM("WIFI DISCONNECT\r\n")) == 0) {  /* Just disconnected */
        if (ESP->ActiveCmd == CMD_WIFI_CWJAP) {             /* If trying to join AP */
            ESP->Events.F.RespWifiDisconnected = 1;
        }
        ESP->CallbackFlags.F.WifiDisconnected = 1;
    } else if (strcmp(str, FROMMEM("WIFI GOT IP\r\n")) == 0) {  /* ESP got assigned IP address from DHCP */
        if (ESP->ActiveCmd == CMD_WIFI_CWJAP) {             /* If trying to join AP */
            ESP->Events.F.RespWifiGotIp = 1;
        }
        ESP->CallbackFlags.F.WifiGotIP = 1;
    }
    
    /* Connection management */
#if !ESP_SINGLE_CONN
    if (strncmp(&str[1], FROMMEM(",CONNECT"), 8) == 0) {
        ESP_CONN_t* conn = (ESP_CONN_t *)&ESP->Conn[CHARTONUM(str[0])]; /* Get connection from number */
        conn->Number = CHARTONUM(str[0]);                   /* Set connection number */
        conn->Flags.F.Active = 1;                           /* Connection is active */
        conn->Callback.F.Connect = 1;
    } else if (strncmp(&str[1], FROMMEM(",CLOSED"), 7) == 0) {
        ESP_CONN_t* conn = (ESP_CONN_t *)&ESP->Conn[CHARTONUM(str[0])]; /* Get connection from number */
        __CONN_RESET(conn);                                 /* Reset connection */
        conn->Callback.F.Closed = 1;
    }
#else
    if (strncmp(str, FROMMEM("CONNECT"), 7) == 0) {
        ESP_CONN_t* conn = (ESP_CONN_t *)&ESP->Conn[0];     /* Get connection from number */
        conn->Number = 0;                                   /* Set connection number */
        conn->Flags.F.Active = 1;                           /* Connection is active */
        conn->Callback.F.Connect = 1;
    } else if (strncmp(str, FROMMEM("CLOSED"), 6) == 0) {
        ESP_CONN_t* conn = (ESP_CONN_t *)&ESP->Conn[0];     /* Get connection from number */
        __CONN_RESET(conn);                                 /* Reset connection */
        conn->Callback.F.Closed = 1;
    }
#endif /* ESP_SINGLE_CONN */
    
    /* Manage connection status */
    if (ESP->ActiveCmd == CMD_TCPIP_CIPSTATUS) {
        if (strncmp(str, FROMMEM("+CIPSTATUS"), 10) == 0) { /* +CIPSTATUS received */
            ParseCIPSTATUS(ESP, (uint8_t *)&ESP->ActiveConnsResp, str + 11);    /* Parse CIPSTATUS response */
        } else if (is_ok) {                                 /* OK received */
            /* Check and merge all connections from ESP */
            uint8_t i = 0;
            for (i = 0; i < ESP_MAX_CONNECTIONS; i++) {
                ESP->Conn[i].Flags.F.Active = (ESP->ActiveConnsResp & (1 << i)) == (1 << i);
            }
            ESP->ActiveConns = ESP->ActiveConnsResp;        /* Copy current value */
        }
    }
    
    /* Manage send data */
    if (ESP->ActiveCmd == CMD_TCPIP_CIPSEND) {
        if (strncmp(str, FROMMEM("SEND OK"), 7) == 0) {     /* Data successfully sent */
            ESP->Events.F.RespSendOk = 1;
        } else if (strncmp(str, FROMMEM("SEND FAIL"), 9) == 0) {    /* Data sent error */
            ESP->Events.F.RespSendFail = 1;
        }
    }
    
    /* DNS function */
    if (ESP->ActiveCmd == CMD_TCPIP_CIPDOMAIN && strncmp(str, FROMMEM("+CIPDOMAIN"), 10) == 0) {
        ParseIP(ESP, &str[11], (uint8_t *)Pointers.Ptr1, NULL); /* Parse IP and save it to user location */
    }
    
    /* Manage receive data */
    if (strncmp(str, FROMMEM("+IPD"), 4) == 0) {                     /* Check for incoming data */
        ParseIPD(ESP, str + 5, (ESP_IPD_t *)&ESP->IPD);     /* Parse incoming data string */
        ESP->IPD.InIPD = 1;                                 /* Start with data reading */
        if (!ESP->IPD.Conn->TotalBytesReceived) {
            ESP->IPD.Conn->DataStartTime = ESP->Time;       /* Set time when first IPD received on connection */
        }
        ESP->IPD.Conn->TotalBytesReceived += ESP->IPD.BytesRemaining;   /* Increase total bytes received so far */
    }
    
    if (is_ok) {
        ESP->Events.F.RespOk = 1;
        ESP->Events.F.RespError = 0;
    } else if (is_error) {
        ESP->Events.F.RespOk = 0;
        ESP->Events.F.RespError = 1;
    }
}

/******************************************************************************/
/******************************************************************************/
/***                              Protothreads                               **/
/******************************************************************************/
/******************************************************************************/
estatic
PT_THREAD(PT_Thread_BASIC(struct pt* pt, evol ESP_t* ESP)) {
    static volatile uint32_t time;
    char str[8];
    PT_BEGIN(pt);
    
    if (ESP->ActiveCmd == CMD_BASIC_AT) {                   /* Send AT and check for any response */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        
        UART_SEND_STR(FROMMEM("AT"));                       /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_BASIC_AT, NULL);              /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_BASIC_RST) {           /* Process device reset */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        
        /* Software reset first */
        UART_SEND_STR(FROMMEM("AT+RST"));                   /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_BASIC_RST, NULL);             /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespReady ||
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespReady ? espOK : espERROR; /* Check response */
        
        if (ESP->ActiveResult != espOK) {
            __RST_EVENTS_RESP(ESP);                         /* Reset all events */
            /* Try hardware reset */
            ESP_LL_SetReset((ESP_LL_t *)&ESP->LL, ESP_RESET_SET);   /* Set reset */
            time = 4000; while (time--);
            ESP_LL_SetReset((ESP_LL_t *)&ESP->LL, ESP_RESET_CLR);   /* Clear reset */
            
            PT_WAIT_UNTIL(pt, ESP->Events.F.RespReady ||
                                ESP->Events.F.RespError);   /* Wait for response */
            
            ESP->ActiveResult = ESP->Events.F.RespReady ? espOK : espERROR; /* Check response */
        }
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_BASIC_GMR) {           /* Get informations about AT software */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        
        UART_SEND_STR(FROMMEM("AT+GMR"));                   /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_BASIC_GMR, NULL);             /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_BASIC_UART) {          /* Set UART */
        NumberToString(str, Pointers.UI);                   /* Get baudrate as string */
        
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+UART_"));                 /* Send data */
        UART_SEND_STR(FROMMEM(Pointers.CPtr1));
        UART_SEND_STR(FROMMEM("="));
        UART_SEND_STR(FROMMEM(str));
#if ESP_USE_CTS
        UART_SEND_STR(FROMMEM(",8,1,0,2"));                 /* Enable hardware CTS pin on ESP device */
#else
        //TODO: changed default to flow control here
        UART_SEND_STR(FROMMEM(",8,1,0,3"));                 /* No flow control for ESP */
#endif
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_BASIC_UART, NULL);            /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk ||
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        if (ESP->ActiveResult == espOK) {
            BUFFER_Reset(&Buffer);                          /* Reset buffer */
            
            ESP->LL.Baudrate = Pointers.UI;
            ESP_LL_Init((ESP_LL_t *)&ESP->LL);              /* Init low-level layer again */
        }
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_BASIC_RFPOWER) {       /* Set RF power */
        NumberToString(str, Pointers.UI);                   /* Convert mode to string */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+RFPOWER="));              /* Send data */
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_BASIC_RFPOWER, NULL);         /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_BASIC_RESTORE) {       /* Restore device */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        
        UART_SEND_STR(FROMMEM("AT+RESTORE"));               /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_BASIC_RESTORE, NULL);         /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespReady || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespReady ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    }
    PT_END(pt);
}

estatic
PT_THREAD(PT_Thread_WIFI(struct pt* pt, evol ESP_t* ESP)) {
    char str[7], ch = ':', i = 6;
    uint8_t* ptr;
    PT_BEGIN(pt);
    
    if (ESP->ActiveCmd == CMD_WIFI_CWMODE) {                /* Set device mode */
        NumberToString(str, Pointers.UI);                   /* Convert mode to string */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CWMODE="));               /* Send data */
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CWMODE, NULL);           /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_GETSTAMAC) {      /* Get station MAC address */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPSTAMAC_CUR?"));        /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CIPSTAMAC, NULL);        /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_GETAPMAC) {       /* Get AP MAC address */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPAPMAC_CUR?"));         /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CIPAPMAC, NULL);         /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_GETSTAIP) {       /* Get station IP address */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPSTA_CUR?"));           /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CIPSTA, NULL);           /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_GETAPIP) {        /* Get AP IP address */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPAP_CUR?"));            /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CIPAP, NULL);
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_SETSTAMAC) {      /* Get AP IP address */
        ptr = (uint8_t *) Pointers.CPtr2;
        
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPSTAMAC_"));            /* Send data */
        UART_SEND_STR(FROMMEM(Pointers.CPtr1));             /* Default or current */
        UART_SEND_STR(FROMMEM("=\""));
        i = 6; ch = ':';
        while (i--) {
            HexNumberToString(str, *ptr++);                 /* Convert to hex number */
            UART_SEND_STR(FROMMEM(str));
            if (i) {
                UART_SEND_CH(FROMMEM(&ch));
            }
        }
        UART_SEND_STR(FROMMEM("\""));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CIPSTAMAC, NULL);        /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        if (ESP->ActiveResult == espOK) {                   /* Copy data as new MAC address */
            memcpy((void *)&ESP->STAMAC, ptr - 6, 6);
        }
            
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_SETAPMAC) {       /* Get AP IP address */
        ptr = (uint8_t *) Pointers.CPtr2;
        
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPAPMAC_"));             /* Send data */
        UART_SEND_STR(FROMMEM(Pointers.CPtr1));             /* Default or current */
        UART_SEND_STR(FROMMEM("=\""));
        i = 6; ch = ':';
        while (i--) {
            HexNumberToString(str, *ptr++);                 /* Convert to hex number */
            UART_SEND_STR(FROMMEM(str));
            if (i) {
                UART_SEND_CH(FROMMEM(&ch));
            }
        }
        UART_SEND_STR(FROMMEM("\""));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CIPAPMAC, NULL);         /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        if (ESP->ActiveResult == espOK) {                   /* Copy data as new MAC address */
            memcpy((void *)&ESP->APMAC, ptr - 6, 6);
        }
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_SETSTAIP) {       /* Set AP IP address */
        ptr = (uint8_t *) Pointers.CPtr2;
        
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPSTA_"));               /* Send data */
        UART_SEND_STR(FROMMEM(Pointers.CPtr1));             /* Default or current */
        UART_SEND_STR(FROMMEM("=\""));
        i = 4; ch = '.';
        while (i--) {
            NumberToString(str, *ptr++);                    /* Convert to hex number */
            UART_SEND_STR(FROMMEM(str));
            if (i) {
                UART_SEND_CH(FROMMEM(&ch));
            }
        }
        UART_SEND_STR(FROMMEM("\""));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CIPSTA, NULL);           /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        if (ESP->ActiveResult == espOK) {                   /* Copy data as new MAC address */
            memcpy((void *)&ESP->STAIP, ptr - 4, 4);
        }
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_SETAPIP) {        /* Set AP IP address */
        ptr = (uint8_t *) Pointers.CPtr2;
        
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPAP_"));                /* Send data */
        UART_SEND_STR(FROMMEM(Pointers.CPtr1));             /* Default or current */
        UART_SEND_STR(FROMMEM("=\""));
        i = 4; ch = '.';
        while (i--) {
            NumberToString(str, *ptr++);                    /* Convert to hex number */
            UART_SEND_STR(FROMMEM(str));
            if (i) {
                UART_SEND_CH(FROMMEM(&ch));
            }
        }
        UART_SEND_STR(FROMMEM("\""));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CIPSTA, NULL);           /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        if (ESP->ActiveResult == espOK) {                   /* Copy data as new MAC address */
            memcpy((void *)&ESP->STAIP, ptr - 4, 4);
        }
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_LISTACCESSPOINTS) {   /* List available access points */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CWLAPOPT=1,127"));        /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CWLAPOPT, NULL);
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        if (ESP->ActiveResult != espOK) {
            goto cmd_wifi_listaccesspoints_clean;           /* Clean thread and stop execution */  
        }
        
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CWLAP"));                 /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CWLAP, NULL);            /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */

cmd_wifi_listaccesspoints_clean:
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_CWJAP) {          /* Connect to network */
        ptr = (uint8_t *) Pointers.Ptr1;
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CWJAP_"));                /* Send data */
        UART_SEND_STR(FROMMEM(Pointers.CPtr1));
        UART_SEND_STR(FROMMEM("=\""));
        EscapeStringAndSend(FROMMEM(Pointers.CPtr2));
        UART_SEND_STR(FROMMEM("\",\""));
        EscapeStringAndSend(FROMMEM(Pointers.CPtr3));
        UART_SEND_STR(FROMMEM("\"")); 
        if (ptr) {                                          /* Send MAC address */
            UART_SEND_STR(FROMMEM(",\""));
            i = 6; ch = ':';
            while (i--) {
                HexNumberToString(str, *ptr++);             /* Convert to hex number */
                UART_SEND_STR(FROMMEM(str));
                if (i) {
                    UART_SEND_CH(FROMMEM(&ch));
                }
            }
            UART_SEND_STR(FROMMEM("\""));
        }
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CWJAP, NULL);            /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        if (ESP->ActiveResult == espOK) {
            ESP->ActiveCmd = CMD_WIFI_GETSTAIP;
            //__IDLE(ESP);                                    /* Go IDLE mode */
            //__ACTIVE_CMD(ESP, CMD_WIFI_GETSTAIP);           /* Set new active CMD to retrieve info about station data */
        } else {
            __IDLE(ESP);                                    /* Go IDLE mode */
        }
    } else if (ESP->ActiveCmd == CMD_WIFI_CWQAP) {          /* Disconnect from network */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CWQAP"));                 /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CWQAP, NULL);            /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_GETCWJAP) {       /* Get AP we are connected with */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CWJAP_CUR?"));            /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CWJAP, NULL);            /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_CWAUTOCONN) {     /* Set autoconnect status */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CWAUTOCONN="));           /* Send data */
        UART_SEND_STR(Pointers.UI ? FROMMEM("1") : FROMMEM("0"));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CWAUTOCONN, NULL);       /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_CWLIF) {          /* List connected stations */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CWLIF"));                 /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CWLIF, NULL);            /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_GETCWSAP) {       /* GET AP settings */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */

        UART_SEND_STR(FROMMEM("AT+CWSAP_CUR?"));            /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CWSAP, NULL);            /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_WIFI_SETCWSAP) {       /* Set AP settings */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        
        UART_SEND_STR(FROMMEM("AT+CWSAP_"));                /* Send data */
        UART_SEND_STR(FROMMEM(Pointers.CPtr1));             /* Send data */
        UART_SEND_STR(FROMMEM("=\""));
        EscapeStringAndSend(((ESP_APConfig_t *)Pointers.CPtr2)->SSID);
        UART_SEND_STR(FROMMEM("\",\""));
        EscapeStringAndSend(((ESP_APConfig_t *)Pointers.CPtr2)->Pass);
        UART_SEND_STR(FROMMEM("\","));
        NumberToString(str, ((ESP_APConfig_t *)Pointers.CPtr2)->Channel);
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(FROMMEM(","));
        NumberToString(str, ((ESP_APConfig_t *)Pointers.CPtr2)->Ecn);
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(FROMMEM(","));
        NumberToString(str, ((ESP_APConfig_t *)Pointers.CPtr2)->MaxConnections);
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(FROMMEM(","));
        NumberToString(str, ((ESP_APConfig_t *)Pointers.CPtr2)->Hidden);
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_CWSAP, NULL);            /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        if (ESP->ActiveResult != espOK) {
            __IDLE(ESP);                                    /* Go IDLE mode */
        } else {
            __IDLE(ESP);                                    /* Go IDLE mode */
            __ACTIVE_CMD(ESP, CMD_WIFI_GETCWSAP);           /* Get info and save to structure */
        }
    } else if (ESP->ActiveCmd == CMD_WIFI_WPS) {            /* Set WPS function */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+WPS="));                  /* Send data */
        UART_SEND_STR(Pointers.UI ? FROMMEM("1") : FROMMEM("0"));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_WIFI_WPS, NULL);              /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    }
    
    PT_END(pt);
}

estatic
PT_THREAD(PT_Thread_TCPIP(struct pt* pt, evol ESP_t* ESP)) {
    char str[7];
    static uint8_t i;
    static uint8_t tries;
    static uint32_t btw = 0;
    
    PT_BEGIN(pt);

    if (ESP->ActiveCmd == CMD_TCPIP_CIPMUX) {               /* Set device mode */
        NumberToString(str, Pointers.UI);                   /* Convert mode to string */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPMUX="));               /* Send data */
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_TCPIP_CIPMUX, NULL);          /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_TCPIP_CIPDINFO) {      /* Set info on +IPD statement */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPDINFO=1"));            /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_TCPIP_CIPDINFO, NULL);        /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_TCPIP_SERVERENABLE) {  /* Enable server mode */
        NumberToString(str, Pointers.UI);                   /* Convert mode to string */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPSERVER=1,"));          /* Send data */
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_TCPIP_CIPSERVER, NULL);       /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_TCPIP_SERVERDISABLE) { /* Disable server mode */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPSERVER=0"));           /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_TCPIP_CIPSERVER, NULL);       /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_TCPIP_CIPSTO) {        /* Set server timeout */
        NumberToString(str, Pointers.UI);                   /* Convert mode to string */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPSTO="));               /* Send data */
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_TCPIP_CIPSERVER, NULL);       /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_TCPIP_CIPSTART) {      /* Start a new connection */
        __CMD_SAVE(ESP);                                    /* Save current command */
   
        /* Check CIPSTATUS */
        __CHECK_CIPSTATUS(ESP);
        
        /* Check response */
        if (ESP->Events.F.RespError) {
            ESP->ActiveResult = espERROR;
            goto cmd_tcpip_cipstart_clean;
        }
        
        /* Find available connection */
        for (i = 0; i < ESP_MAX_CONNECTIONS; i++) {
            if (!ESP->Conn[i].Flags.F.Active || (ESP->ActiveConns & (1 << i)) == 0) {
                ESP->Conn[i].Number = i;
                *(ESP_CONN_t **)Pointers.PPtr1 = (ESP_CONN_t *)&ESP->Conn[i];
                break;
            }
        }
        
        /* Check valid connection */
        if (!(*(ESP_CONN_t **)Pointers.PPtr1) || i == ESP_MAX_CONNECTIONS) {
            ESP->ActiveResult = espERROR;
            goto cmd_tcpip_cipstart_clean;
        }
        
        /* Check if there is an active connection and is SSL */
        if (((ESP_CONN_Type_t)(Pointers.UI >> 16)) == ESP_CONN_Type_SSL) {
            for (i = 0; i < ESP_MAX_CONNECTIONS; i++) {
                if (ESP->Conn[i].Flags.F.Active && ESP->Conn[i].Flags.F.SSL) {
                    ESP->ActiveResult = espSSLERROR;
                    goto cmd_tcpip_cipstart_clean;
                }
            }
        }

        (*(ESP_CONN_t **)Pointers.PPtr1)->Flags.F.Client = 1;   /* Connection made as client */
        (*(ESP_CONN_t **)Pointers.PPtr1)->Flags.F.SSL = ((ESP_CONN_Type_t)(Pointers.UI >> 16)) == ESP_CONN_Type_SSL;  /* Connection type is SSL */
        
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPSTART="));             /* Send data */
#if !ESP_SINGLE_CONN
        NumberToString(str, (*(ESP_CONN_t **)Pointers.PPtr1)->Number);  /* Convert mode to string */
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(FROMMEM(","));
#endif /* ESP_SINGLE_CONN */
        if (((ESP_CONN_Type_t)(Pointers.UI >> 16)) == ESP_CONN_Type_TCP) {
            UART_SEND_STR(FROMMEM("\"TCP"));
        } else if (((ESP_CONN_Type_t)(Pointers.UI >> 16)) == ESP_CONN_Type_UDP) {
            UART_SEND_STR(FROMMEM("\"UDP"));
        } else if (((ESP_CONN_Type_t)(Pointers.UI >> 16)) == ESP_CONN_Type_SSL) {
            UART_SEND_STR(FROMMEM("\"SSL"));
        }
        UART_SEND_STR(FROMMEM("\",\""));
        UART_SEND_STR(FROMMEM(Pointers.CPtr1));
        UART_SEND_STR(FROMMEM("\","));
        NumberToString(str, Pointers.UI & 0xFFFF);
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(FROMMEM(","));
        UART_SEND_STR(FROMMEM(str));

        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_TCPIP_CIPSTART, NULL);        /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        if (ESP->ActiveResult != espOK) {                   /* Failed, reset connection */
            __CONN_RESET((*(ESP_CONN_t **)Pointers.PPtr1));
            goto cmd_tcpip_cipstart_clean;
        }
        
        /* Execute CIPSTATUS */
        __CHECK_CIPSTATUS(ESP);                             /* Check CIPSTATUS response and parse connection parameters */
        
cmd_tcpip_cipstart_clean:
        __CMD_RESTORE(ESP);                                 /* Restore command */
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_TCPIP_CIPCLOSE) {      /* Close connection */
        __CMD_SAVE(ESP);                                    /* Save current command */
        
        NumberToString(str, Pointers.UI);                   /* Close specific connection */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPCLOSE="));             /* Send data */
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_TCPIP_CIPSERVER, NULL);       /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        /* Execute CIPSTATUS */
        __CHECK_CIPSTATUS(ESP);                             /* Check CIPSTATUS and ignore response */
        
        __CMD_RESTORE(ESP);                                 /* Restore command */
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_TCPIP_CIPSEND) {       /* Send data on connection */
        __CMD_SAVE(ESP);                                    /* Save command */
        
        if (Pointers.Ptr2 != NULL) {
            *(uint32_t *)Pointers.Ptr2 = 0;                 /* Set sent bytes to zero first */
        }
        
        tries = 3;                                          /* Give 3 tries to send each packet */
        do {
            btw = Pointers.UI > ESP_MAX_SEND_DATA_LEN ? ESP_MAX_SEND_DATA_LEN : Pointers.UI;    /* Set length to send */
            
            __RST_EVENTS_RESP(ESP);                         /* Reset events */
            UART_SEND_STR(FROMMEM("AT+CIPSEND="));          /* Send number to ESP */
#if !ESP_SINGLE_CONN
            NumberToString(str, ((ESP_CONN_t *)Pointers.Ptr1)->Number);
            UART_SEND_STR(FROMMEM(str));
            UART_SEND_STR(FROMMEM(","));
#endif /* ESP_SINGLE_CONN */
            NumberToString(str, btw);                       /* Get string from number */
            UART_SEND_STR(str);
            UART_SEND_STR(_CRLF);
            StartCommand(ESP, CMD_TCPIP_CIPSEND, NULL);     /* Start command */
            
            PT_WAIT_UNTIL(pt, ESP->Events.F.RespBracket ||
                                ESP->Events.F.RespError);   /* Wait for > character and timeout */
            
            if (ESP->Events.F.RespBracket) {                /* We received bracket */
                __RST_EVENTS_RESP(ESP);                     /* Reset events */
                UART_SEND((uint8_t *)Pointers.CPtr1, btw);  /* Send data */
                
                PT_WAIT_UNTIL(pt, ESP->Events.F.RespSendOk ||
                                    ESP->Events.F.RespSendFail ||
                                    ESP->Events.F.RespError);   /* Wait for OK or ERROR */
                
                ESP->ActiveResult = ESP->Events.F.RespSendOk ? espOK : espSENDERROR; /* Set result to return */
                
                if (ESP->ActiveResult == espOK) {
                    if (Pointers.Ptr2 != NULL) {
                        *(uint32_t *)Pointers.Ptr2 = *(uint32_t *)Pointers.Ptr2 + btw;  /* Increase number of sent bytes */
                    }
                }
            } else if (ESP->Events.F.RespError) {
                ESP->ActiveResult = espERROR;               /* Process error */
            }
            if (ESP->ActiveResult == espOK) {
                tries = 3;                                  /* Reset number of tries */
                
                Pointers.UI -= btw;                         /* Decrease number of sent bytes */
                Pointers.CPtr1 = (uint8_t *)Pointers.CPtr1 + btw;   /* Set new data memory location to send */
            } else if (ESP->Events.F.RespSendFail) {        /* Send failed */
                tries--;                                    /* We failed, decrease number of tries and start over */
            } else {                                        /* Error was received, link is probably not active */
                tries = 0;                                  /* Stop execution here */
            }
        } while (Pointers.UI && tries);                     /* Until anything to send or max tries reached */
        
        if (tries) {
            ((ESP_CONN_t *)Pointers.Ptr1)->Callback.F.DataSent = 1; /* Set flag for callback */
        } else {
            ((ESP_CONN_t *)Pointers.Ptr1)->Callback.F.DataError = 1;/* Set flag for callback */
        }
        
        __CMD_RESTORE(ESP);                                 /* Restore command */
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_TCPIP_CIPSSLSIZE) {    /* Set SSL buffer size */
        NumberToString(str, Pointers.UI);                   /* Close specific connection */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPSSLSIZE="));           /* Send data */
        UART_SEND_STR(FROMMEM(str));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_TCPIP_CIPSSLSIZE, NULL);      /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_TCPIP_CIPDOMAIN) {     /* Get IP address from domain name */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIPDOMAIN=\""));          /* Send data */
        UART_SEND_STR(FROMMEM(Pointers.CPtr1));
        UART_SEND_STR(FROMMEM("\""));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_TCPIP_CIPDOMAIN, NULL);       /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_TCPIP_PING) {          /* Ping domain or IP */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+PING=\""));               /* Send data */
        UART_SEND_STR(FROMMEM(Pointers.CPtr1));
        UART_SEND_STR(FROMMEM("\""));
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_TCPIP_PING, NULL);            /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    } else if (ESP->ActiveCmd == CMD_TCPIP_CIUPDATE) {      /* Update firmware from cloud */
        __RST_EVENTS_RESP(ESP);                             /* Reset all events */
        UART_SEND_STR(FROMMEM("AT+CIUPDATE"));              /* Send data */
        UART_SEND_STR(_CRLF);
        StartCommand(ESP, CMD_TCPIP_CIUPDATE, NULL);        /* Start command */
        
        PT_WAIT_UNTIL(pt, ESP->Events.F.RespOk || 
                            ESP->Events.F.RespError);       /* Wait for response */
        
        ESP->ActiveResult = ESP->Events.F.RespOk ? espOK : espERROR;    /* Check response */
        
        __IDLE(ESP);                                        /* Go IDLE mode */
    }
    
    PT_END(pt);
}

/* Process all thread calls */
ESP_Result_t ProcessThreads(evol ESP_t* ESP) {
    if (CMD_IS_ACTIVE_BASIC(ESP)) {                         /* General related commands */
        PT_Thread_BASIC(&pt_BASIC, ESP);                       
    }
    if (CMD_IS_ACTIVE_WIFI(ESP)) {                          /* General related commands */
        PT_Thread_WIFI(&pt_WIFI, ESP);                       
    }
    if (CMD_IS_ACTIVE_TCPIP(ESP)) {                         /* On active PIN related command */
        PT_Thread_TCPIP(&pt_TCPIP, ESP);
    }
#if !ESP_RTOS && !ESP_ASYNC
    ESP_ProcessCallbacks(ESP);                              /* Process callbacks when not in RTOS or ASYNC mode */
#endif
    __RETURN(ESP, espOK);
}
/******************************************************************************/
/******************************************************************************/
/***                              Public API                                 **/
/******************************************************************************/
/******************************************************************************/
ESP_Result_t ESP_Init(evol ESP_t* ESP, uint32_t baudrate, ESP_EventCallback_t callback) {
    uint32_t i = 50;
    BUFFER_t* Buff = &Buffer;
    
    _ESP = (ESP_t *)ESP;
    memset((void *)ESP, 0x00, sizeof(ESP_t));               /* Clear structure first */
    
    ESP->Callback = callback;                               /* Set event callback */
    if (callback == NULL) {
        ESP->Callback = ESP_CallbackDefault;                /* Set default callback function */
    }
    
#if ESP_CONN_SINGLEBUFFER
    do {
        uint8_t i = 0;
        for (i = 0; i < ESP_MAX_CONNECTIONS; i++) {
            ESP->Conn[i].Data = IPD_Data;
        }
    } while (0);
#endif /*!< ESP_CONN_SINGLEBUFFER */
    
    ESP->Time = 0;                                          /* Reset time start time */
    BUFFER_Init(Buff, sizeof(Buffer_Data) - 1, Buffer_Data);    /* Init buffer for receive */
    
    /* Low-Level initialization */
    ESP->LL.Baudrate = baudrate;
    if (ESP_LL_Init((ESP_LL_t *)&ESP->LL)) {                /* Init low-level */
        __RETURN(ESP, espLLERROR);                          /* Return error */
    }
    
#if ESP_RTOS
    /* RTOS support */
    if (ESP_SYS_Create((ESP_RTOS_SYNC_t *)&ESP->Sync)) {    /* Init sync object */
        __RETURN(ESP, espSYSERROR);
    }
#endif /*!< ESP_RTOS */

    /* Init all threads */
    __RESET_THREADS(ESP);

    /* Send initialization commands */
    ESP->Flags.F.IsBlocking = 1;                            /* Process blocking calls */
    ESP->ActiveCmdTimeout = 5000;                           /* Give 1 second timeout */
    while (i) {
        __ACTIVE_CMD(ESP, CMD_BASIC_RST);                   /* Reset device */
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
    ESP->ActiveCmdTimeout = 100;                            /* Set response timeout */
    while (i) {
        __ACTIVE_CMD(ESP, CMD_BASIC_AT);                    /* Check AT response */
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
    while (i) {
        __ACTIVE_CMD(ESP, CMD_BASIC_GMR);                   /* Check AT software */
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
#if ESP_USE_CTS
    while (i) {
        Pointers.CPtr1 = FROMMEM("CUR");
        Pointers.UI = baudrate;
        __ACTIVE_CMD(ESP, CMD_BASIC_UART);                  /* Check AT response */
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
#endif /*!< ESP_USE_CTS */
    while (i) {
        Pointers.UI = 3;
        __ACTIVE_CMD(ESP, CMD_WIFI_CWMODE);                 /* Set device mode */
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
    while (i) {
        Pointers.UI = !ESP_SINGLE_CONN && 1;                /* Set up for single connection mode */
        __ACTIVE_CMD(ESP, CMD_TCPIP_CIPMUX);                /* Set device mux */
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
    while (i) {
        Pointers.UI = !ESP_SINGLE_CONN && 1;                /* In single connection mode, we don't need informations on IPD data = CLIENT ONLY */
        __ACTIVE_CMD(ESP, CMD_TCPIP_CIPDINFO);              /* Enable informations about connection on +IPD statement */
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
    while (i) {
        __ACTIVE_CMD(ESP, CMD_WIFI_GETSTAMAC);              /* Get station MAC address */
        Pointers.UI = 1;
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
    while (i) {
        __ACTIVE_CMD(ESP, CMD_WIFI_GETAPMAC);               /* Get AP MAC address */
        Pointers.UI = 1;
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
    while (i) {
        __ACTIVE_CMD(ESP, CMD_WIFI_GETSTAIP);               /* Get station IP */
        Pointers.UI = 1;
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
    while (i) {
        __ACTIVE_CMD(ESP, CMD_WIFI_GETAPIP);                /* Get AP IP */
        Pointers.UI = 1;
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
    while (i) {
        __ACTIVE_CMD(ESP, CMD_WIFI_GETCWSAP);               /* Get AP settings */
        ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
        __IDLE(ESP);
        if (ESP->ActiveResult == espOK) {
            break;
        }
        i--;
    }
    //while (i) {
    //    __ACTIVE_CMD(ESP, CMD_TCPIP_SERVERDISABLE);         /* Get AP settings */
    //    ESP_WaitReady(ESP, ESP->ActiveCmdTimeout);
    //    __IDLE(ESP);
    //    break;                                              /* Ignore response */
    //}
    __IDLE(ESP);                                            /* Process IDLE */
    ESP->Flags.F.IsBlocking = 0;                            /* Reset blocking calls */
    ESP->Flags.F.Call_Idle = 0;
    
    __RETURN(ESP, ESP->ActiveResult);                       /* Return active status */
}

ESP_Result_t ESP_DeInit(evol ESP_t* ESP) {
    BUFFER_Free(&Buffer);                                   /* Clear USART buffer */
    
    __RETURN(ESP, espOK);                                   /* Return OK from function */
}

ESP_Result_t ESP_Update(evol ESP_t* ESP) {
    char ch;
    static char prev1_ch = 0x00, prev2_ch = 0x00;
    BUFFER_t* Buff = &Buffer;
    uint16_t processedCount = 500;
    
    if (ESP->ActiveCmd != CMD_IDLE && ESP->Time - ESP->ActiveCmdStart > ESP->ActiveCmdTimeout) {
        ESP->Events.F.RespError = 1;                        /* Set active error and process */
    }
    
    while (
#if !GSM_RTOS && GSM_ASYNC
        processedCount-- &&
#else
        processedCount &&
#endif
        BUFFER_Read(Buff, (uint8_t *)&ch, 1)                /* Read single character from buffer */
    ) {
        ESP_SET_RTS(ESP, ESP_RTS_CLR);                      /* Clear RTS pin */
        if (ESP->IPD.InIPD && ESP->IPD.BytesRemaining) {    /* Read network data */
            if (ESP->ActiveCmd == CMD_IDLE) {
                __ACTIVE_CMD(ESP, CMD_TCPIP_IPD);           /* Set active command! */
                ESP->Flags.F.IsBlocking = 1;                /* Set as it was blocking call */
            }
            ESP->IPD.Conn->Data[ESP->IPD.BytesRead] = ch;   /* Add character to receive buffer */
            ESP->IPD.BytesRead++;                           /* Increase number of bytes read in this packet */
            ESP->IPD.BytesRemaining--;                      /* Decrease number of bytes remaining to read in entire IPD packet */
            
            if (!ESP->IPD.BytesRemaining) {                 /* We read all the data? */
                ESP->IPD.InIPD = 0;
                if (ESP->ActiveCmd == CMD_TCPIP_IPD) {      /* If we set as TCPIP then release it */
                    __IDLE(ESP);                            /* Go to IDLE mode */
                }
            }
            if (ESP->IPD.BytesRead >= (ESP_CONNBUFFER_SIZE - 1) || !ESP->IPD.BytesRemaining) { /* Receive buffer full or we read all the data? */
                ESP->CallbackParams.CP1 = ESP->IPD.Conn;
                ESP->CallbackParams.CP2 = ESP->IPD.Conn->Data;
                ESP->IPD.Conn->DataLength = ESP->IPD.BytesRead;
                ESP->CallbackParams.UI = ESP->IPD.BytesRead;
                if (ESP->IPD.BytesRemaining
#if ESP_CONN_SINGLEBUFFER
                    || (!ESP->IPD.BytesRemaining && ESP->ActiveCmd == CMD_IDLE) /*!< Do this only if low of RAM (Do not USE RTOS in this mode) */
#endif      
                ) {
                    ESP_CALL_CALLBACK(ESP, espEventDataReceived);   /* Process callback */
                    ESP->IPD.Conn->Callback.F.CallLastPartOfPacketReceived = 0;
                } else {
                    ESP->IPD.Conn->Callback.F.CallLastPartOfPacketReceived = 1;
                }
                ESP->IPD.BytesRead = 0;                     /* Reset buffer and prepare for new packet */
            }
        } else {
            if (ISVALIDASCII(ch)) {
                switch (ch) {
                    case '\n':
                        RECEIVED_ADD(ch);                   /* Add character */
                        ParseReceived(ESP, &Received);      /* Parse received string */
                        RECEIVED_RESET();
                        break;
                    default: 
                        if (ch == ' ' && prev1_ch == '>' && prev2_ch == '\n') { /* Check if bracket received */
                            ESP->Events.F.RespBracket = 1;  /* We receive bracket on command */
                        }
                        RECEIVED_ADD(ch);                   /* Add character to buffer */
                        
                        /*!< Check IPD statement */
                        if (ch == ':' && RECEIVED_LENGTH() > 4) {   /* Maybe +IPD was received* */
                            if (Received.Data[0] == '+' && strncmp(FROMMEM(Received.Data), FROMMEM("+IPD"), 4) == 0) {  /* Check for IPD statement */
                                ParseReceived(ESP, &Received);  /* Process parsing received data */
                                RECEIVED_RESET();       /* Reset received object! */
                            }
                        }
                    break;
                } 
            } else {
                RECEIVED_RESET();                           /* Reset invalid received character */
            }
        }
        prev2_ch = prev1_ch;                                /* Save previous character to prevprev character */
        prev1_ch = ch;                                      /* Save current character as previous */      
    }
    
    return ProcessThreads(ESP);                             /* Process stack */
}

ESP_Result_t ESP_ProcessCallbacks(evol ESP_t* ESP) {
    uint8_t i = 0;
    /* Process callbacks */
    if (ESP->ActiveCmd == CMD_IDLE && ESP->Flags.F.Call_Idle) { /* Process IDLE call */
        ESP->Flags.F.Call_Idle = 0;
        ESP_CALL_CALLBACK(ESP, espEventIdle);
    }
    if (ESP->ActiveCmd == CMD_IDLE && ESP->CallbackFlags.F.WifiConnected) { /* Wifi just connected */
        ESP->CallbackFlags.F.WifiConnected = 0;
        ESP_CALL_CALLBACK(ESP, espEventWifiConnected);
    }
    if (ESP->ActiveCmd == CMD_IDLE && ESP->CallbackFlags.F.WifiDisconnected) {  /* Wifi just connected */
        ESP->CallbackFlags.F.WifiDisconnected = 0;
        ESP_CALL_CALLBACK(ESP, espEventWifiDisconnected);
    }
    if (ESP->ActiveCmd == CMD_IDLE && ESP->CallbackFlags.F.WifiGotIP) { /* Wifi just connected */
        ESP->CallbackFlags.F.WifiGotIP = 0;
        ESP_CALL_CALLBACK(ESP, espEventWifiGotIP);
    }
    
    for (i = 0; i < sizeof(ESP->Conn) / sizeof(ESP->Conn[0]); i++) {
        ESP_CONN_t* c = (ESP_CONN_t *)&ESP->Conn[i];
        
        /* Maybe move this part directly to __IDLE() command */
        /* More test required first to see usability of this */
        if (__IS_READY(ESP) && c->Callback.F.CallLastPartOfPacketReceived) {    /* Notify user about last packet */
            c->Callback.F.CallLastPartOfPacketReceived = 0;
            ESP->CallbackParams.CP1 = c;
            ESP->CallbackParams.CP2 = c->Data;
            ESP->CallbackParams.UI = c->DataLength;
            ESP_CALL_CALLBACK(ESP, espEventDataReceived);
        }
        if (__IS_READY(ESP) && c->Callback.F.DataSent) {    /* Data sent ok */
            c->Callback.F.DataSent = 0;
            ESP->CallbackParams.CP1 = c;
            ESP_CALL_CALLBACK(ESP, espEventDataSent);
        }
        if (__IS_READY(ESP) && c->Callback.F.DataError) {   /* Data sent error */
            c->Callback.F.DataError = 0;
            ESP->CallbackParams.CP1 = c;
            ESP_CALL_CALLBACK(ESP, espEventDataSentError);
        }
        if (__IS_READY(ESP) && c->Callback.F.Connect) {     /* Connection just active */
            c->Callback.F.Connect = 0;
            ESP->CallbackParams.CP1 = c;
            ESP_CALL_CALLBACK(ESP, espEventConnActive); 
        }
        if (__IS_READY(ESP) && c->Callback.F.Closed) {      /* Connection just closed */
            c->Callback.F.Closed = 0;
            ESP->CallbackParams.CP1 = c;
            ESP_CALL_CALLBACK(ESP, espEventConnClosed); 
        }
    }
    __RETURN(ESP, espOK);
}

void ESP_UpdateTime(evol ESP_t* ESP, uint32_t time_increase) {
    ESP->Time += time_increase;                             /* Increase time */
}

ESP_Result_t ESP_GetLastReturnStatus(evol ESP_t* ESP) {
    ESP_Result_t tmp = ESP->ActiveResult;
    ESP->ActiveResult = espOK;
    
    return tmp;
}

uint16_t ESP_DataReceived(uint8_t* ch, uint16_t count) {
    uint32_t r;
    
    r = BUFFER_Write(&Buffer, ch, count);                   /* Writes data to USART buffer */
#if ESP_USE_CTS
    if (BUFFER_GetFree(&Buffer) <= 3) {
        ESP_SET_RTS(_ESP, ESP_RTS_SET);                     /* Set RTS pin */
    }
#endif
    return r;
}

/******************************************************************************/
/*                              Device ready status                           */
/******************************************************************************/
ESP_Result_t ESP_WaitReady(evol ESP_t* ESP, uint32_t timeout) {
    ESP->ActiveCmdTimeout = timeout;                        /* Set timeout value */
    do {
#if !ESP_RTOS && !ESP_ASYNC
        ESP_Update(ESP);                                    /* Update stack if we are in synchronous mode */
#else
        ESP_ProcessCallbacks(ESP);                          /* Process callbacks when not in synchronous mode */
#endif
    } while (__IS_BUSY(ESP));
    __RETURN(ESP, ESP->ActiveResult);                       /* Return active result from command */
}

ESP_Result_t ESP_Delay(evol ESP_t* ESP, uint32_t timeout) {
    evol uint32_t start = ESP->Time;
    do {
#if !ESP_RTOS && !ESP_ASYNC
        ESP_Update(ESP);
#endif
    } while (ESP->Time - start < timeout);
    __RETURN(ESP, espOK);
}

ESP_Result_t ESP_IsReady(evol ESP_t* ESP) {
    return ESP->ActiveCmd != CMD_IDLE ? espERROR : espOK;
}

/******************************************************************************/
/***                              Device settings                            **/
/******************************************************************************/
ESP_Result_t ESP_RestoreDefault(evol ESP_t* ESP, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_BASIC_RESTORE);                   /* Set active command */
    
    __RETURN_BLOCKING(ESP, blocking, 2000);                 /* Return with blocking support */
}

ESP_Result_t ESP_SetUART(evol ESP_t* ESP, uint32_t baudrate, uint32_t def, uint32_t blocking) {
    __CHECK_INPUTS(baudrate >= 110 && baudrate <= (40 * 115200));   /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_BASIC_UART);                      /* Set active command */
    
    Pointers.CPtr1 = def ? FROMMEM("DEF") : FROMMEM("CUR");
    Pointers.UI = baudrate;
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_SetRFPower(evol ESP_t* ESP, float pwr, uint32_t blocking) {
    __CHECK_INPUTS(pwr > 0 && (pwr / 0.25f) <= 82);         /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_BASIC_RFPOWER);                   /* Set active command */
    
    Pointers.UI = (uint8_t)(pwr / 0.25f);
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_FirmwareUpdate(evol ESP_t* ESP, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_TCPIP_CIUPDATE);                  /* Set active command */
    
    __RETURN_BLOCKING(ESP, blocking, 180000);               /* Return with blocking support */
}

ESP_Result_t ESP_GetSoftwareInfo(evol ESP_t* ESP, char* atv, char* sdkv, char* cmpt, uint32_t blocking) {
    __CHECK_INPUTS(atv || sdkv || cmpt);                    /* Check inputs, at least one must be valid to start with this command */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_BASIC_GMR);                       /* Set active command */
    
    /* Use const pointers because of missing structure data and to prevent RAM usage */
    Pointers.CPtr1 = (const void *)atv;
    Pointers.CPtr2 = (const void *)sdkv;
    Pointers.CPtr3 = (const void *)cmpt;
    
    __RETURN_BLOCKING(ESP, blocking, 180000);               /* Return with blocking support */
}

/******************************************************************************/
/***                         STATION AND AP settings                         **/
/******************************************************************************/
ESP_Result_t ESP_STA_GetIP(evol ESP_t* ESP, uint8_t* ip, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_GETSTAIP);                   /* Set active command */
    
    Pointers.Ptr1 = ip;                                     /* Save pointer to save IP to */
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_STA_SetIP(evol ESP_t* ESP, const uint8_t* ip, uint8_t def, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_SETSTAIP);                   /* Set active command */
    
    Pointers.CPtr1 = def ? FROMMEM("DEF") : FROMMEM("CUR");
    Pointers.CPtr2 = ip;
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_STA_GetMAC(evol ESP_t* ESP, uint8_t* mac, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_GETSTAMAC);                  /* Set active command */
    
    Pointers.Ptr1 = mac;                                    /* Save pointer to save MAC to */
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_STA_SetMAC(evol ESP_t* ESP, const uint8_t* mac, uint32_t def, uint32_t blocking) {
    __CHECK_INPUTS(mac && (*mac & 0x01) == 0);              /* Check inputs, bit 0 of first byte cannot be set to 1 on station */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_SETSTAMAC);                  /* Set active command */
    
    Pointers.CPtr1 = def ? FROMMEM("DEF") : FROMMEM("CUR");
    Pointers.CPtr2 = mac;
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_AP_GetIP(evol ESP_t* ESP, uint8_t* ip, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_GETAPIP);                    /* Set active command */
    
    Pointers.Ptr1 = ip;                                     /* Save pointer to save IP to */
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_AP_SetIP(evol ESP_t* ESP, const uint8_t* ip, uint8_t def, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_SETAPIP);                    /* Set active command */
    
    Pointers.CPtr1 = def ? FROMMEM("DEF") : FROMMEM("CUR");
    Pointers.CPtr2 = ip;
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_AP_GetMAC(evol ESP_t* ESP, uint8_t* mac, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_GETAPMAC);                   /* Set active command */
    
    Pointers.Ptr1 = mac;                                    /* Save pointer to save MAC to */
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_AP_SetMAC(evol ESP_t* ESP, const uint8_t* mac, uint32_t def, uint32_t blocking) {
    __CHECK_INPUTS(mac);                                    /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_SETAPMAC);                   /* Set active command */
    
    Pointers.CPtr1 = def ? FROMMEM("DEF") : FROMMEM("CUR");
    Pointers.CPtr2 = mac;
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_AP_GetConfig(evol ESP_t* ESP, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_GETCWSAP);                   /* Set active command */
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_AP_SetConfig(evol ESP_t* ESP, ESP_APConfig_t* conf, uint8_t def, uint32_t blocking) {
    __CHECK_INPUTS(conf);                                   /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_SETCWSAP);                   /* Set active command */
    
    Pointers.CPtr1 = def ? FROMMEM("DEF") : FROMMEM("CUR");
    Pointers.CPtr2 = conf;
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

#if !ESP_SINGLE_CONN
/******************************************************************************/
/***                            SERVER settings                              **/
/******************************************************************************/
ESP_Result_t ESP_SERVER_Enable(evol ESP_t* ESP, uint16_t port, uint32_t blocking) {
    __CHECK_INPUTS(port > 0);                               /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_TCPIP_SERVERENABLE);              /* Set active command */
    
    Pointers.UI = port;                                     /* port number */
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_SERVER_Disable(evol ESP_t* ESP, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_TCPIP_SERVERDISABLE);             /* Set active command */
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

ESP_Result_t ESP_SERVER_SetTimeout(evol ESP_t* ESP, uint16_t timeout, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_TCPIP_CIPSTO);                    /* Set active command */
    
    Pointers.UI = timeout;                                  /* Save timeout value */
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}
#endif /* ESP_SINGLE_CONN */

/******************************************************************************/
/***                            List wifi stations                           **/
/******************************************************************************/
ESP_Result_t ESP_STA_ListAccessPoints(evol ESP_t* ESP, ESP_AP_t* APs, uint16_t atr, uint16_t* ar, uint32_t blocking) {
    __CHECK_INPUTS(APs && atr && ar);                       /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_LISTACCESSPOINTS);           /* Set active command */
    
    *ar = 0;
    Pointers.Ptr1 = APs;
    Pointers.Ptr2 = ar;
    Pointers.UI = atr;
    
    __RETURN_BLOCKING(ESP, blocking, 10000);                /* Return with blocking support */
}

ESP_Result_t ESP_AP_ListConnectedStations(evol ESP_t* ESP, ESP_ConnectedStation_t* stations, uint16_t size, uint16_t* sr, uint32_t blocking) {
    __CHECK_INPUTS(stations && size && sr);                 /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_CWLIF);                      /* Set active command */
    
    *sr = 0;
    Pointers.Ptr1 = stations;
    Pointers.Ptr2 = sr;
    Pointers.UI = size;
    
    __RETURN_BLOCKING(ESP, blocking, 10000);                /* Return with blocking support */
}

/******************************************************************************/
/***                            Connect to network                           **/
/******************************************************************************/
ESP_Result_t ESP_STA_Connect(evol ESP_t* ESP, const char* ssid, const char* pass, const uint8_t* mac, uint32_t def, uint32_t blocking) {
    __CHECK_INPUTS(ssid && pass);                           /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_CWJAP);                      /* Set active command */
    
    Pointers.CPtr1 = def ? FROMMEM("DEF") : FROMMEM("CUR");
    Pointers.CPtr2 = ssid;
    Pointers.CPtr3 = pass;
    Pointers.Ptr1 = (void *)mac;
    
    __RETURN_BLOCKING(ESP, blocking, 30000);                /* Return with blocking support */
}

ESP_Result_t ESP_STA_GetConnected(evol ESP_t* ESP, ESP_ConnectedAP_t* AP, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_GETCWJAP);                   /* Set active command */
    
    Pointers.Ptr1 = AP;
    
    __RETURN_BLOCKING(ESP, blocking, 10000);                /* Return with blocking support */
}

ESP_Result_t ESP_STA_Disconnect(evol ESP_t* ESP, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_CWQAP);                      /* Set active command */
    
    __RETURN_BLOCKING(ESP, blocking, 10000);                /* Return with blocking support */
}

ESP_Result_t ESP_STA_SetAutoConnect(evol ESP_t* ESP, uint8_t autoconn, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_CWAUTOCONN);                 /* Set active command */
    
    Pointers.UI = autoconn ? 1 : 0;
    
    __RETURN_BLOCKING(ESP, blocking, 10000);                /* Return with blocking support */
}

/******************************************************************************/
/***                            Connection management                        **/
/******************************************************************************/
ESP_Result_t ESP_CONN_Start(evol ESP_t* ESP, ESP_CONN_t** conn, ESP_CONN_Type_t type, const char* domain, uint16_t port, uint32_t blocking) {
    __CHECK_INPUTS(conn && domain && port);                 /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_TCPIP_CIPSTART);                  /* Set active command */
    
    Pointers.PPtr1 = (evol void **)conn;
    Pointers.CPtr1 = domain;
    Pointers.UI = type << 16 | port;
    
    __RETURN_BLOCKING(ESP, blocking, 180000);               /* Return with blocking support */
}

ESP_Result_t ESP_CONN_Send(evol ESP_t* ESP, ESP_CONN_t* conn, const uint8_t* data, uint32_t btw, uint32_t* bw, uint32_t blocking) {
    __CHECK_INPUTS(conn && data && btw);                    /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_TCPIP_CIPSEND);                   /* Set active command */
    
    Pointers.Ptr1 = conn;
    Pointers.Ptr2 = bw;
    Pointers.CPtr1 = data;
    Pointers.UI = btw;
    
    __RETURN_BLOCKING(ESP, blocking, 10000);                /* Return with blocking support */
}

ESP_Result_t ESP_CONN_Close(evol ESP_t* ESP, ESP_CONN_t* conn, uint32_t blocking) {
    __CHECK_INPUTS(conn);                                   /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_TCPIP_CIPCLOSE);                  /* Set active command */
 
    Pointers.UI = conn->Number;
    
    __RETURN_BLOCKING(ESP, blocking, 10000);                /* Return with blocking support */
}

ESP_Result_t ESP_CONN_CloseAll(evol ESP_t* ESP, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_TCPIP_CIPCLOSE);                  /* Set active command */

    Pointers.UI = ESP_MAX_CONNECTIONS;                      /* Close all connections */
    
    __RETURN_BLOCKING(ESP, blocking, 5000);                 /* Return with blocking support */
}

ESP_Result_t ESP_SetSSLBufferSize(evol ESP_t* ESP, uint32_t size, uint32_t blocking) {
    __CHECK_INPUTS(size >= 2048 && size <= 4096);           /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_TCPIP_CIPSSLSIZE);                /* Set active command */

    Pointers.UI = size;                                     /* Close all connections */
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}

/******************************************************************************/
/***                            Miscellanious                                **/
/******************************************************************************/
ESP_Result_t ESP_DOMAIN_GetIp(evol ESP_t* ESP, const char* domain, uint8_t* ip, uint32_t blocking) {
    __CHECK_INPUTS(ip);                                     /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_TCPIP_CIPDOMAIN);                 /* Set active command */

    Pointers.CPtr1 = domain;
    Pointers.Ptr1 = ip;
    
    __RETURN_BLOCKING(ESP, blocking, 10000);                /* Return with blocking support */
}

ESP_Result_t ESP_Ping(evol ESP_t* ESP, const char* addr, uint32_t* time, uint32_t blocking) {
    __CHECK_INPUTS(addr && time);                           /* Check inputs */
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_TCPIP_PING);                      /* Set active command */

    *time = 0;
    Pointers.CPtr1 = addr;
    Pointers.Ptr1 = time;
    
    __RETURN_BLOCKING(ESP, blocking, 10000);                /* Return with blocking support */
}

ESP_Result_t ESP_SetWPS(evol ESP_t* ESP, uint8_t wps, uint32_t blocking) {
    __CHECK_BUSY(ESP);                                      /* Check busy status */
    __ACTIVE_CMD(ESP, CMD_WIFI_WPS);                        /* Set active command */

    Pointers.UI = wps ? 1 : 0;
    
    __RETURN_BLOCKING(ESP, blocking, 1000);                 /* Return with blocking support */
}
