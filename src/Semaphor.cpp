/*
 * Semaphor.cpp
 *
 *  Created on: May 16, 2019
 *      Author: OS1
 */

#include "Semaphor.h"
#include "KSem.h"
Semaphore::Semaphore(int init) {
forbid
myImpl = new KernelSem(this,init);
allow
}

Semaphore::~Semaphore() {
forbid
if(myImpl!=0){
	myImpl->mySem = 0;
	delete myImpl;
}
myImpl = 0;
allow
}

int Semaphore::wait(Time maxTimeToWait) {
	if(myImpl == 0) return 0;
	return myImpl->wait(maxTimeToWait);
}

int Semaphore::signal(int n) {
	if(myImpl == 0) return 0;
return myImpl->signa(n);
}

int Semaphore::val() const {
	if(myImpl == 0) return 0;
return myImpl->value;
}


