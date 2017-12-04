/*
 * test.cpp
 *
 *  Created on: 02.01.2017
 *      Author: tmikschl
 */





#include "espdriver/esp8266.h"
#include "ESP8266.h"

#define WIFI_DEBUG

evol ESP_t ESP;
ESP_CONN_t* conn;
ESP_Result_t espRes;

HAL_UART *uart_esp8266;
HAL_GPIO *gpio0_esp8266;
HAL_GPIO *enable_esp8266;

UDPMsg esp8266rxMsg;
ESP8266 *esp_singleton;

bool check = false;

ESP8266::UpdateThread::UpdateThread(ESP8266 *_esp8266) : Thread("ESP8266 Update Thread",1004) {
	esp8266 = _esp8266;
}

void ESP8266::UpdateThread::run() {
	this->suspendCallerUntil(END_OF_TIME);
	while(true) {
		/*if (ESP_IsReady(&ESP) == espOK && !esp8266->txFifo.isEmpty()) {
			uint32_t bw;
			espRes = ESP_CONN_Send(&ESP, conn, msg->data, msg->length, &bw, 0);

			return bw;

		}*/
		ESP_Update(&ESP);
		AT(NOW()+10*MILLISECONDS);
	}
}

int ESP_Callback(ESP_Event_t evt, ESP_EventParams_t* params) {
	ESP_CONN_t* conn;
    uint8_t* data;
    switch (evt) {                              /* Check events */
        case espEventIdle:
        	/*
			#ifdef WIFI_DEBUG
            	PRINTF("Stack is IDLE!\r\n");
			#endif
			*/
            break;
        case espEventConnActive: {
                    conn = (ESP_CONN_t *)params->CP1;   /* Get connection for event */
					#ifdef WIFI_DEBUG
                    	PRINTF("Connection %d just became active!\r\n", conn->Number);
					#endif
                    break;
                }
                case espEventConnClosed: {
                    conn = (ESP_CONN_t *)params->CP1;   /* Get connection for event */
					#ifdef WIFI_DEBUG
                    	PRINTF("Connection %d was just closed!\r\n", conn->Number);
					#endif
                    break;
                }
                case espEventDataReceived: {
					#ifdef WIFI_DEBUG
                	//	PRINTF("Recv %d\n",params->UI);   // Disabled by Faisal
					#endif
                    conn = (ESP_CONN_t *)params->CP1;   /* Get connection for event */
                    data = (uint8_t *)params->CP2;      /* Get data */

                    esp8266rxMsg.length = params->UI;
            		memcpy(&esp8266rxMsg.data,data,esp8266rxMsg.length);
            		esp_singleton->recvMessage();

                    break;
                }
                case espEventDataSent:
                    conn = (ESP_CONN_t *)params->CP1;   /* Get connection for event */
                    esp_singleton->writeFinished();
                    /*
					#ifdef WIFI_DEBUG
                    	PRINTF("Data sent conn: %d\r\n", conn->Number);
					#endif
					*/
                    //PRINTF("Close conn resp: %d\r\n", ESP_CONN_Close(&ESP, conn, 0));
                    break;
                case espEventDataSentError:
                    conn = (ESP_CONN_t *)params->CP1;   /* Get connection for event */
					#ifdef WIFI_DEBUG
                    	PRINTF("Data sent error conn: %d\r\n", conn->Number);
					#endif
                    //ESP_CONN_Close(&ESP, conn, 0);
                    break;
        default:
            break;
    }

    return 0;
}

/**
 * Converts IP from uint32_t representation to string
 * @param buf buffer fpor string
 * @param ip ip address as reversed int32 - for example 192.168.0.1 = 0x0100A8C0.
 */
void convertIP(char *buf,uint32_t ip) {
	sprintf(buf,"%d.%d.%d.%d",(uint8_t)ip,(uint8_t)(ip>>8),(uint8_t)(ip>>16),(uint8_t)(ip>>24));
}

ESP8266::ESP8266(HAL_UART *_uart, HAL_GPIO *_enable, HAL_GPIO *_gpio0) : updatethread(this) {
	// External Pointers for pure c interface
	uart_esp8266 = _uart;
	gpio0_esp8266 = _gpio0;
	enable_esp8266 = _enable;

	// internal class pointers
	uart = _uart;
	gpio0=_gpio0;
	enable=_enable;

	esp_singleton = this;

}
/**
 * Initialize the module.
 * @param _ssid SSID of the wifi to connect to
 * @param _pw password of the wifi (WPA2 is assumed)
 * @return
 */
int ESP8266::init(const char *_ssid, const char *_pw) {

	ssid = _ssid;
	pw = _pw;

	gpio0->init(true, 1, 1);
	enable->init(true, 1, 0);
	enable->setPins(1);

	// First Connection to ESP - default baudrate 115200 (hopefully always)
	if ((espRes = ESP_Init(&ESP, 115200, ESP_Callback)) == espOK) {
		#ifdef WIFI_DEBUG
	    	PRINTF("ESP module init successfully!\r\n");
		#endif
	} else {
		#ifdef WIFI_DEBUG
	    	PRINTF("ESP Init error. Status: %d\r\n", espRes);
		#endif
	    return -1;
	}

	// Configure for 2 Mbit
	if((espRes = ESP_SetUART(&ESP,2000000,0,1)) == espOK) {
		#ifdef WIFI_DEBUG
			PRINTF("Changed Baudrate\r\n");
		#endif
	} else {
		#ifdef WIFI_DEBUG
			PRINTF("Problems changing baudrate: %d\r\n", espRes);
		#endif
		return -1;
	}
	// Enable UART Flow Control
	uart->config(UART_PARAMETER_HW_FLOW_CONTROL,1);

	AT(NOW() + 1* SECONDS);
	// Connect to wifi
	if ((espRes = ESP_STA_Connect(&ESP, ssid, pw, NULL, 0, 1)) == espOK) {
		#ifdef WIFI_DEBUG
			PRINTF("Connected to network\r\n");
		#endif
	} else {
		#ifdef WIFI_DEBUG
			PRINTF("Problems trying to connect to network: %d\r\n", espRes);
		#endif
		return -1;
	}
	updatethread.resume();
	AT(NOW() + 1* SECONDS);
	address = (uint32_t)ESP.STAIP;
	return 0;
}

/**
 * Reset the module.
 */
void ESP8266::reset(){};

/**
 * Get current Status of the Wifi module. See wlan_state.
 * @return current status
 */
int ESP8266::status(){
	return 0;
};

/**
 * Sent a UDP message.
 * @param msg udp message to send.
 * @return message length if successful, else -1
 */
int ESP8266::write(UDPMsg *msg){
	ESP_Update(&ESP);
	if (ESP_IsReady(&ESP) == espOK) {
		uint32_t bw;

		espRes = ESP_CONN_Send(&ESP, conn, msg->data, msg->length, &bw, 0); /* Send data on other requests */

		return bw;

	}

	return -1;

};

/**
 * Check whether new UDP messages are available.
 * @return true if new messages are available, false if not.
 */
bool ESP8266::isDataReady(){
	return !rxFifo.isEmpty();
};

/**
 * Read a UDP message from the wf121 module and put it in the provided buffer.
 * @param msg Pointer to UDP message for writing.
 * @return 1 if message was written, 0 if no new message is available.
 */
int ESP8266::read(UDPMsg *msg){
	return rxFifo.get(*msg);
};

void ESP8266::recvMessage() {
	if(!rxFifo.put(esp8266rxMsg)) {
		PRINTF("Wifi - Recevive Buffer full\n\r");
	}
	udp_msgs_recv++;
	upCallDataReady();
}

void ESP8266::writeFinished() {
	upCallWriteFinished();
}


/**
 * Enable UDP Connection. The UDP Connection enabled here,
 * can be accessed via the read and write methods of this class.
 * If the wifi is not ready yet, the udp connection will
 * be activated as soon as the connection is ready.
 *
 * @param _udp_destination destination of the udp connection.
 * Reverse ip in hex format - for example 192.168.0.1 = 0x0100A8C0.
 * This can be a broadcast address, for example 0xFFFFFFFF.
 * Warning: in case of broadcast expect a higher packet loss rate.
 *
 * @param _udp_port UDP port of the local udp server as well of the remote udp connection.
 */
void ESP8266::enableUDPConnection(uint32_t _udp_destination, uint32_t _udp_port){
	udp_destination = _udp_destination;
	udp_port = _udp_port;

	char buf[16];
	convertIP(buf,_udp_destination);
	if ((espRes = ESP_CONN_Start(&ESP, &conn, ESP_CONN_Type_UDP, buf, udp_port, 1)) == espOK) {
		#ifdef WIFI_DEBUG
			PRINTF("UDP Server mode is enabled. IP: %d.%d.%d.%d\r\n", ESP.STAIP[0], ESP.STAIP[1], ESP.STAIP[2], ESP.STAIP[3]);
		#endif
	} else {
		#ifdef WIFI_DEBUG
	    	PRINTF("Problems trying to enable server mode: %d\r\n", espRes);
		#endif
	}

};


void ESP8266::enableTCPConnection(uint32_t _tcp_destination, uint32_t _tcp_port){
	tcp_destination = _tcp_destination;
	tcp_port = _tcp_port;

	char buf[16];
	convertIP(buf,_tcp_destination);
	if ((espRes = ESP_CONN_Start(&ESP, &conn, ESP_CONN_Type_TCP, buf, tcp_port, 1)) == espOK) {
		#ifdef WIFI_DEBUG
			PRINTF("TCP Connection is enabled. IP: %d.%d.%d.%d\r\n", ESP.STAIP[0], ESP.STAIP[1], ESP.STAIP[2], ESP.STAIP[3]);
		#endif
	} else {
		#ifdef WIFI_DEBUG
	    	PRINTF("Problems trying to enable tcp connection: %d\r\n", espRes);
		#endif
	}
	PRINTF("TCP is not implemented for this driver!");

};



void ESP8266::ESPUpdateTimeEvent::handle(){
  ESP_UpdateTime(&ESP, 1);
}

void ESP8266::ESPUpdateTimeEvent::init() {
  activatePeriodic(1*MILLISECONDS, 1*MILLISECONDS);
}



