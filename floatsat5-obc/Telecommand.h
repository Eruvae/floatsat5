/*
 * Telecommand.h
 *
 *  Created on: 11.11.2017
 *      Author: TobiZ
 */

#ifndef TELECOMMAND_H_
#define TELECOMMAND_H_

#include "rodos.h"
#include "CommInterfaces.h"

class Telecommand : public Thread
{
public:
	Telecommand();
	void run();
};

#endif /* TELECOMMAND_H_ */
