/*
 * KSem.h
 *
 *  Created on: May 16, 2019
 *      Author: OS1
 */
#include "Context.h"
#include "Kernel.h"
#include "QUEUE.h"
#include <IOSTREAM.H>
#ifndef KSEM_H_
#define KSEM_H_
class Semaphore;
class KernelSem {
public:
	QUEUE onMe;
	Semaphore * mySem;
	KernelSem * nextSem;
	KernelSem * previousSem;
	int value;
	volatile static KernelSem * allKernelSem;
	int signa(int n = 0);
	int wait(Time timeToSleep);
	int val() const;
	static void removeAllKernelSem();
	KernelSem(Semaphore * semaphore, int startValue);
	void signalHelper(){
		if(value >=0) return;
		onMe.front();
		value++;
	}
	void signal();
	~KernelSem();
static void addToSleepyList(KernelSem * ker);
static void removeFromSleepyList(KernelSem * ker);
static void shakeAllSemafor();
void shakeNatalija();

};

#endif /* KSEM_H_ */
