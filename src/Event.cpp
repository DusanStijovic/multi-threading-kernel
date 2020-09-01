/*
 * Event.cpp
 *
 *  Created on: May 19, 2019
 *      Author: OS1
 */

#include "Event.h"


Event::Event(IVTNo ivtNo) {
	myImpl = new KernelEv(ivtNo);
}

Event::~Event() {
	delete myImpl;
}

void Event::wait() {
if(myImpl!=0)myImpl->wait();
}

void Event::signal() {
if(myImpl!=0) myImpl->signal();//No need for this
}
