/*
 * wf121.h
 *
 *  Created on: 27.08.2015
 *      Author: tmikschl
 */

#ifndef SRC_WF121_H_
#define SRC_WF121_H_

#include <rodos.h>
#include "apitypes.h"
#include "wifi_bglib.h"
#include "../../hal_wifi.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class HTTPHandler {
public:
	virtual void handleRequest(uint32_t request, uint8array * ressource, uint8	method) {}
};

class WF121 : public HAL_Wifi {
public:
	/** States of the internal state machine. */
	enum wlan_state
	{
		wlan_state_inactive,
	    /** Send Hello command. */
	    wlan_state_hello,
	    /** Reset the module. */
	    wlan_state_reset,
	    /** Switch Wi-Fi on. */
	    wlan_state_wifi_on,
		/** Waiting for WiFi on. */
		wlan_state_wait_wifi_on,
		/** set Password */
		wlan_state_set_pw,
	    /** Connect to the selected Wi-Fi access point. */
	    wlan_state_connect,
	    /** Waiting for wlan connection. */
	    wlan_state_wait_connect,
		/* Ready */
		wlan_state_ready
	};
private:
	int udp_endpoint=-1;
	int tcp_endpoint=-1;
/*	const char *pw;
	const char *ssid;*/
	uint32_t udp_destination;
	uint32_t udp_port;

//	uint32_t udp_msgs_send;
//	uint32_t udp_msgs_recv;

	uint32_t tcp_destination;
	uint32_t tcp_port;

	uint32_t tcp_msgs_send;
	uint32_t tcp_msgs_recv;

	HAL_UART * uart;
	Fifo<UDPMsg,5> rxFifo;
/*	ipv4 address;
	ipv4 gateway;
	ipv4 netmask;
	uint8_t dhcp;*/
	bool httpServerEnabled = false;
	bool udpConnectionEnabled = false;
	bool tcpConnectionEnabled = false;
	HTTPHandler *httpHandler;

	void processMsg();
	void startHTTPServer();
	void startUDPConnection();
	void startTCPConnection();

	class RecvThread : public Thread {
		WF121 *wf121;
		HAL_UART *uart;
	public:
		RecvThread(WF121 *_wf121,HAL_UART * _uart);
		void run();
		int getMsg(char *buf, int len);
	} recvthread;
protected:
	enum wlan_state internal_state;

public:
	/**
	 * Constructor.
	 * @param _uart UART the WF121 module is connected to. In case of the extension board it should be UART_IDX3.
	 */
	WF121(HAL_UART *_uart);

	/**
	 * Initialize the module.
	 * @param _ssid SSID of the wifi to connect to
	 * @param _pw password of the wifi (WPA2 is assumed)
	 * @return
	 */
	int init(const char *_ssid, const char *_pw);

	/**
	 * Reset the module.
	 */
	void reset();

	/**
	 * Get current Status of the Wifi module. See wlan_state.
	 * @return current status
	 */
	int status();

	/**
	 * Sent a UDP message.
	 * @param msg udp message to send.
	 * @return message length if successful, else -1
	 */
	int write(UDPMsg *msg);

	/**
	 * Check whether new UDP messages are available.
	 * @return true if new messages are available, false if not.
	 */
	bool isDataReady();

	/**
	 * Read a UDP message from the wf121 module and put it in the provided buffer.
	 * @param msg Pointer to UDP message for writing.
	 * @return 1 if message was written, 0 if no new message is available.
	 */
	int read(UDPMsg *msg);

	/**
	 * Enable HTTPServer. Normal Request will be handles by the webpage contained in the internal
	 * flash of the Wifi Module, however requests to /api will be handed over to the HTTPHandler class provided.
	 * @param _httpHandler HTTPHandler for requests to /api
	 */
	void enableHTTPServer(HTTPHandler *_httpHandler);

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
	void enableUDPConnection(uint32_t _udp_destination, uint32_t _udp_port);


	void enableTCPConnection(uint32_t _tcp_destination, uint32_t _tcp_port);


    bool isWriteFinished()  {return false;}
    bool isReadFinished()   {return false;}
    int config() {return 0;}

};

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* SRC_WF121_H_ */
