/*
 * linkinterfacewf121.cpp
 *
 *  Created on: 31.08.2015
 *      Author: tmikschl
 */

#include "linkinterfacewifi.h"
UDPMsg txMsg;
UDPMsg rxMsg;

LinkinterfaceWifi::LinkinterfaceWifi(HAL_Wifi *_wf) : Linkinterface(-1){
	wf = _wf;
}

void LinkinterfaceWifi::init() {
	wf->setIoEventReceiver(this);
}

void LinkinterfaceWifi::onWriteFinished() {
    if(threadToResume) threadToResume->resume();
}

void LinkinterfaceWifi::onDataReady() {
    if(threadToResume) threadToResume->resume();
}

bool LinkinterfaceWifi::sendNetworkMsg(NetworkMessage &outMsg)	{
	txMsg.length=outMsg.numberOfBytesToSend();


	memcpy(txMsg.data,&outMsg,txMsg.length);

	//uint64_t time = NOW();
	wf->write(&txMsg);
	//xprintf(" %d\n",(int)(NOW()-time));
    return true;
}




bool LinkinterfaceWifi::getNetworkMsg(NetworkMessage &inMsg,int32_t &numberOfReceivedBytes) {
	if(wf->read(&rxMsg)) {
		memcpy(&inMsg,rxMsg.data,rxMsg.length);
		numberOfReceivedBytes = rxMsg.length;
		return true;
	}
	return false;
}
