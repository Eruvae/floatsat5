/*
 * linkinterfacewf121.h
 *
 *  Created on: 31.08.2015
 *      Author: tmikschl
 */

#ifndef SRC_LINKINTERFACEWIFI_H_
#define SRC_LINKINTERFACEWIFI_H_

#include <gateway/linkinterface.h>
#include <hal/genericIO.h>
#include "hal_wifi.h"

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

class LinkinterfaceWifi: public Linkinterface,IOEventReceiver {
private:
	HAL_Wifi *wf;
public:
	LinkinterfaceWifi(HAL_Wifi *_wf);
	void onDataReady();
	void onWriteFinished();
	void init();

	bool sendNetworkMsg(NetworkMessage &outMsg);
	bool getNetworkMsg(NetworkMessage &inMsg,int32_t &numberOfReceivedBytes);
};

#ifndef NO_RODOS_NAMESPACE
}
#endif

#endif /* SRC_LINKINTERFACEWIFI_H_ */
