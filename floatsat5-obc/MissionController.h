/*
 * MissionController.h
 *
 *  Created on: 30.01.2018
 *      Author: TobiZ
 */

#ifndef MISSIONCONTROLLER_H_
#define MISSIONCONTROLLER_H_

#include <rodos.h>

class MissionController : public Thread
{
public:
	MissionController();
	virtual ~MissionController();

	void run();
};

#endif /* MISSIONCONTROLLER_H_ */
