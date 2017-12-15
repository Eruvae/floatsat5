/*
 * RaspiComm.h
 *
 *  Created on: 14.12.2017
 *      Author: TobiZ
 */

#ifndef RASPICOMM_H_
#define RASPICOMM_H_

#include <rodos.h>
#include "CommInterfaces.h"

class RaspiComm : public Thread
{
public:
	RaspiComm();
	virtual ~RaspiComm();

	void run();
};

#endif /* RASPICOMM_H_ */
