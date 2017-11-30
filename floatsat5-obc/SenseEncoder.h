/*
 * SenseEncoder.h
 *
 *  Created on: 29.11.2017
 *      Author: TobiZ
 */

#ifndef SENSEENCODER_H_
#define SENSEENCODER_H_

#include <rodos.h>

class SenseEncoder : public Thread
{
public:
	SenseEncoder();
	void run();
};

#endif /* SENSEENCODER_H_ */
