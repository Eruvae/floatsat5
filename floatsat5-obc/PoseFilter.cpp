/*
 * PoseFilter.cpp
 *
 *  Created on: 04.12.2017
 *      Author: TobiZ
 */

#include "PoseFilter.h"

PoseFilter::PoseFilter()
{
	// TODO Auto-generated constructor stub

}

void PoseFilter::run()
{
	setPeriodicBeat(15*MILLISECONDS, 100*MILLISECONDS);
}
