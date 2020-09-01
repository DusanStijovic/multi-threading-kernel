/*
 * Thead.cpp
 *
 *  Created on: May 11, 2019
 *      Author: OS1
 */
#include "Context.h"
#include "Kernel.h"
#include <IOSTREAM.H>
#include <MATH.H>
#include "Signals.h"
volatile ID Thread::id = 1;
Thread::Thread(StackSize stackSize, Time timeSlice) {
	forbid
	myPCB = new PCB(this, stackSize, timeSlice);
	threadID = id++;
	allow
}

void Thread::start() {
	if (myPCB != 0)
		myPCB->start();
}
ID Thread::getId() {
	return threadID;
}
Thread * Thread::getThreadById(ID identity) {
	return PCB::getThreadByID(identity);
}
ID Thread::getRunningId() {
	return PCB::getRunningId();
}
void Thread::waitToComplete() {
	if (myPCB != 0)
		myPCB->waitToComplete();
}
Thread::~Thread() {
	forbid
	if (myPCB != 0) {
		myPCB->waitToComplete();
		myPCB->myThread = 0;
		delete myPCB;
		myPCB = 0;
	}
	allow
}
void dispatch() {
	Context::preemptionOnRequest = 1;
#ifndef BCC_BLOCK_IGNORE
	asm int 0x08
#endif
}

void Thread::signal(SignalId signalId) {
	forbid
	if (myPCB != 0) {
	    myPCB->threadSignal->signal(signalId);
		if (signalId == 0) {
	     	myPCB->myThread = 0;
			myPCB = 0;
		}
	}

	allow
}

void Thread::swap(SignalId is, SignalHandler hand1, SignalHandler hand2) {
	if(myPCB!=0) myPCB->threadSignal->swap(is, hand1, hand2);
}

void Thread::blockSignal(SignalId signal) {
	if(myPCB!=0) myPCB->threadSignal->blockSignal(signal);

}

void Thread::blockSignalGlobally(SignalId signal) {
	Signal::blockSignalGlobally(signal);
}

void Thread::unblockSignal(SignalId signal) {
	if(myPCB!=0) myPCB->threadSignal->unblockSignal(signal);

}

void Thread::unblockSignalGlobally(SignalId signal) {
	Signal::unblockSignalGlobally(signal);
}

void Thread::registerHandler(SignalId signalID, SignalHandler handler) {
	if(myPCB!=0) myPCB->threadSignal->registerHandler(signalID, handler);
}

void Thread::unregisterAllHandlers(SignalId signalID) {
	if(myPCB!=0) myPCB->threadSignal->unregisterAllHandlers(signalID);
}
