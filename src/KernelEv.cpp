/*
 * KernelEv.cpp
 *
 *  Created on: May 19, 2019
 *      Author: OS1
 */

#include "KernelEv.h"
#include "Context.h"
#include "Kernel.h"
IVTEntry * IVTEntry::ivtEntry[256];
KernelEv::KernelEv(IVTNo ivtNo) {
#ifndef BCC_BLOCK_IGNORE
	lock;
	owner =(PCB * ) PCB::running;
	ownerBlocked = 0;//Show if owner is blocked.
    this->ivtNo = ivtNo;
    //Set event in ivtEntry
    if(IVTEntry::ivtEntry!=0) IVTEntry::ivtEntry[ivtNo]->setEvent(this);
    unlock;
#endif
}

void KernelEv::wait(){
	if (PCB::running != owner) return;
    forbid
	//No need for this because if owner is blocked no one else could call wait, because of previous condition
	if (ownerBlocked==0){
		PCB::running->threadState = PCB::BLOCKED;
        ownerBlocked = 1;
        dispatch();//Nece se ovde izvrsiti promena konteksta, to mozda prepraviti.
	}
    allow
}

void KernelEv::signal() {
forbid
//If owner is not blocker, dont have any effect
	if(ownerBlocked==1){
		owner->threadState = PCB::READY;
		Scheduler::put(owner);
		ownerBlocked = 0;
		dispatch();

	}
allow
}

void IVTEntry::signal() {
forbid//No need for this, we call this in  interrupt routine
	if(kernerlEvent != 0)//Should be set, need to check because routine will be called before we make Event object.
		kernerlEvent->signal();
allow
}

IVTEntry::IVTEntry(IVTNo  ivtNo, interruptPointer routine) {
#ifndef BCC_BLOCK_IGNORE
lock
defaultRoutine = 0;
kernerlEvent = 0;
defaultRoutine = getvect(ivtNo);
setvect(ivtNo,routine);
//Set IVTEntry object in global array
ivtEntry[ivtNo] = this;
this->ivtNo = ivtNo;
unlock
#endif
}

void IVTEntry::setEvent(KernelEv* kernelEvent) {
#ifndef BCC_BLOCK_IGNORE
lock;
    //Used in KernelEv to set kernelEvent for the entry in IVT, fot the ivtEntryObject
	this->kernerlEvent = kernelEvent;
unlock;
#endif
}

void IVTEntry::initEntries() {
//Initialization of IVTEntry global array
	for(IVTNo ivtNo = 0;ivtNo<256;ivtNo++)
		ivtEntry[ivtNo] = 0;
}

IVTEntry::~IVTEntry() {
#ifndef BCC_BLOCK_IGNORE
	//Return old routine
	lock
	setvect(ivtNo, defaultRoutine);
	defaultRoutine = 0;
	kernerlEvent = 0;
	if(ivtEntry!=0) ivtEntry[ivtNo] = 0;
	ivtNo = 0;
    unlock
	#endif
}

KernelEv::~KernelEv() {
#ifndef BCC_BLOCK_IGNORE
	lock
	//Reset reference to object
	owner = 0;
	ownerBlocked = 0;
	if(IVTEntry::ivtEntry!=0 && IVTEntry::ivtEntry[ivtNo] !=0)
		IVTEntry::ivtEntry[ivtNo]->setEvent(0);
	ivtNo = 0;
	unlock
#endif
}
