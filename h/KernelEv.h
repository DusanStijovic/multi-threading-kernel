/*
 * KernelEv.h
 *
 *  Created on: May 19, 2019
 *      Author: OS1
 */
#include "Context.h"
#ifndef KERNELEV_H_
#define KERNELEV_H_
class PCB;
typedef unsigned char IVTNo;
class KernelEv {
public:
	KernelEv(IVTNo ivtNo);
	~KernelEv();
	void wait();
	void signal();
	int ownerBlocked;

private:
	PCB* owner;
	IVTNo ivtNo;

};

class IVTEntry {
	KernelEv* kernerlEvent;
public:
	IVTNo ivtNo;
	static IVTEntry* ivtEntry[256];
	IVTEntry(IVTNo ivtNo, interruptPointer routine);
	~IVTEntry();
	void signal();
	void setEvent(KernelEv * kernelEvent);
	interruptPointer defaultRoutine;
	static void initEntries();
};

#define PREPAREENTRY(entryNumber, shouldCallDefault)\
	void interrupt inter##entryNumber(...);\
	IVTEntry ivtEntry##entryNumber(entryNumber, inter##entryNumber);\
	void interrupt inter##entryNumber(...){\
        if(shouldCallDefault == 1) \
			(ivtEntry##entryNumber.defaultRoutine)();\
		ivtEntry##entryNumber.signal();\
}

#endif /* KERNELEV_H_ */
