/*
 * Semaphor.h
 *
 *  Created on: May 16, 2019
 *      Author: OS1
 */

#ifndef WA_SEMAPHOR_H_
#define WA_SEMAPHOR_H_
#include "Thread.h"
class KernelSem;
class Semaphore{
public:
	Semaphore(int init = 1);
	virtual ~Semaphore();

	virtual int wait(Time maxTimeToWait);
	virtual int signal(int n = 0);
	int val()const;
	friend class KernelSem;
private:
	KernelSem * myImpl;
};


#endif /* WA_SEMAPHOR_H_ */
