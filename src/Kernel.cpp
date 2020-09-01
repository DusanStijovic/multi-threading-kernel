/*
 * Kernel.cppa
 *
 *  Created on: May 9, 2019
 *      Author: OS1
 */
#include "Context.h"
#include "Kernel.h"
#include <dos.h>
#include <IOSTREAM.H>
//Initialization
PCB * PCB::idlePCB = 0;
PCB * PCB::main = 0;
volatile PCB * PCB::running = 0;
volatile PCB * PCB::allPCB = 0;
int PCB::numberofThreads = 0;

void PCB::removeAllFromWaitingToFinished() {
	while (waitingToFinished != 0) {
		PCB * helPCB = waitingToFinished;
		waitingToFinished = waitingToFinished->nextToWait;
		helPCB->threadState = READY;
		helPCB->nextToWait = 0;
		Scheduler::put(helPCB);
	}
}
void PCB::initializeStack(StackSize stackSize) {
	stackSize = stackSize / sizeof(unsigned);
	if (stackSize > maxStackSize)
		stackSize = maxStackSize;
	unsigned * stack = new unsigned[stackSize];
	if(stack == 0) return;
	//Put data on stack
	stack[stackSize - PSW] = 0x200;
#ifndef BCC_BLOCK_IGNORE
	stack[stackSize - FUNCTIONSEGMENT] = FP_SEG(&PCB::wrapper);
	stack[stackSize - FUNCTIONOFF] = FP_OFF(&PCB::wrapper);
	context.bp = FP_OFF(stack + stackSize - ALLREGISTER);
	context.sp = FP_OFF(stack + stackSize - ALLREGISTER); //SP point to location after allregisters,becaue we will restaurate all of them.
	context.ss = FP_SEG(stack + stackSize - ALLREGISTER);
#endif
	//context.bp = context.sp;
	this->stack = stack;

}
PCB::PCB(Thread * myThread, StackSize stackSize, Time timeSlice) {
	forbid
	initializeStack(stackSize);
	this->timeSlice = timeSlice;
	this->threadState = CREATED;
	threadSignal = new Signal();
	//Inherits signal setting
	if(PCB::running != 0){
		threadSignal->inheritSettings(PCB::running->threadSignal);
	}
    //Initialize fields
	parent = (PCB*)PCB::running;
	this->myThread = myThread;
	waitingToFinished = 0;
	nextToWait = 0;
	wayOf = NOWAKEUP;
	//Add to global thread list
	nextPCB =(PCB *) allPCB;
	previousPCB = 0;
	if (nextPCB != 0)
		nextPCB->previousPCB = this;
	allPCB = this;
	allow
}
Thread * PCB::getThreadByID(ID identity) {
	forbid
	volatile PCB * helpPCB = allPCB;
	//find pcb with that id
	while (helpPCB) {
		if (helpPCB->myThread->threadID == identity)
			break;
		helpPCB = helpPCB->nextPCB;
	}
	Thread * thread = 0;
	//Check if exist thread with that id and forbid to get idle thread
	if(helpPCB != PCB::idlePCB && helpPCB!=0) thread = helpPCB->myThread;
	allow
	return thread;
}
void PCB::addToWaitingToFinished(PCB * myPCB) {
	if (myPCB == 0)
		return;
	//Add to waiting list
	myPCB->nextToWait = waitingToFinished;
	myPCB->threadState = WAITING;
	waitingToFinished = myPCB;
}
void PCB::wrapper() {
	PCB::running->myThread->run();
	forbid
	//if have parent send signal.
	if(PCB::running->parent!=0 && PCB::running->parent->threadSignal!=0)//Not nedded, because main doesnt go here
		PCB::running->parent->threadSignal->parentIAmDone();
	if(PCB::running->threadSignal!=0){
	PCB::running->threadSignal->iAmDone();
	PCB::running->threadSignal->processReceivedSignals();
	}
	PCB::running->threadState = DEAD;
	PCB * pcb = (PCB*) PCB::running;
	pcb->removeAllFromWaitingToFinished();
	pcb->tellChildrenParentIsDead();
	numberofThreads--;
	dispatch();
	allow
}

PCB::~PCB() {
forbid
	//Remove from global list
	removeFromAllPCB();
	if(stack!=0) delete stack;
	stack = 0;
	nextPCB = 0;
	nextToWait = 0;
	if(myThread!=0){
		myThread->myPCB = 0;
	}
	myThread = 0;
	threadState = DELETED;
	this->waitingToFinished = 0;
	parent = 0;
    if (threadSignal != 0)
    	delete threadSignal;
    threadSignal = 0;


allow
}
//Promena konteksta
void PCB::start() {
	forbid
	if (threadState != CREATED) {
		allow
		return;
	}
	threadState = PCB::READY;
	Scheduler::put(this);
	numberofThreads++;
	allow
}
void PCB::waitToComplete() {
	if (this == PCB::idlePCB)
		return;
	if (this == PCB::main)
		return;
	if (this == PCB::running)
		return;
	if (this->threadState == PCB::DEAD)
		return;
	if (this->threadState == CREATED)
		return;
	forbid
	addToWaitingToFinished((PCB*) PCB::running);
	dispatch();
	allow
}
ID PCB::getRunningId() {
	forbid
	int id = PCB::running->myThread->threadID;
	allow
	return id;
}
void PCB::removeFromAllPCB() {
	if (previousPCB != 0)
		previousPCB->nextPCB = nextPCB;
	if (nextPCB != 0)
		nextPCB->previousPCB = previousPCB;
	if (this == allPCB)
		allPCB = allPCB->nextPCB;
	previousPCB = 0;
	nextPCB = 0;
}
void PCB::tellChildrenParentIsDead() {
PCB * temp = (PCB*)allPCB;
while(temp){
	if(temp->parent != 0 && temp->parent == this)
		temp->parent = 0;
     temp  = temp->nextPCB;
}
}
