/*
 * ESP8266.h
 *
 *  Created on: 11.11.2015
 *      Author: tmikschl
 */

#ifndef SRC_ESP8266_H_
#define SRC_ESP8266_H_

#include "rodos.h"
#include "hal/hal_gpio.h"
#include "../../hal_wifi.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif


class ESP8266 : public HAL_Wifi {
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
	const char *pw;
	const char *ssid;
	uint32_t udp_destination;
	uint32_t udp_port = 0;

	//uint32_t udp_msgs_send;
	//uint32_t udp_msgs_recv;

	uint32_t tcp_destination;
	uint32_t tcp_port = 0;

	uint32_t tcp_msgs_send;
	uint32_t tcp_msgs_recv;

	HAL_GPIO *enable;
	HAL_GPIO *gpio0;

	HAL_UART * uart;
	Fifo<UDPMsg,5> rxFifo;
	Fifo<UDPMsg,5> txFifo;


	/*uint32_t address;
	uint32_t gateway;
	uint32_t netmask;
	uint8_t dhcp;*/

	bool udpConnectionEnabled = false;
	bool tcpConnectionEnabled = false;





	class ESPUpdateTimeEvent : public TimeEvent {
	public:
	  void handle();
	  void init();
	} esptimeevent;

protected:
	enum wlan_state internal_state;

public:
	ESP8266(HAL_UART *_uart, HAL_GPIO *_enable, HAL_GPIO *_gpio0);
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

    void recvMessage();
    void writeFinished();

	class UpdateThread : public Thread {
		ESP8266 *esp8266;
	public:
		UpdateThread(ESP8266 *_esp8266);
		void run();
	} updatethread;
};

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* SRC_ESP8266_H_ */
