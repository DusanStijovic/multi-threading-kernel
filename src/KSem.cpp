/*
 * KSem+.cpp
 *
 *  Created on: May 16, 2019
 *      Author: OS1
 */

#include "KSem.h"
#include "Semaphor.h"
volatile KernelSem * KernelSem::allKernelSem = 0;


void KernelSem::signal(){
		if(value<0){
			onMe.front();
		}
		value++;
}

int KernelSem::signa(int n) {
#ifndef BCC_BLOCK_IGNORE
	if (n < 0)
		return n;
	forbid
	if (n == 0) {
		lock;
		KernelSem::signal();
		unlock;
		allow
		return 0;
	}
	int number = 0;
	while (value < 0 && n > 0) {
		lock;
		signalHelper();
		unlock;
		number++;
		n--;
	}
	value += n;
	allow
	return number;
#endif
}

int KernelSem::wait(Time timeToSleep) {
	//Dodati fleg za update semafora.
#ifndef BCC_BLOCK_IGNORE
	forbid
	if (timeToSleep < 0){
		allow
		return timeToSleep;
	}
	value--;
	if (value < 0) {
		if (timeToSleep > 0 && onMe.headSleepers == 0) {
			lock;
			addToSleepyList(this);
			unlock;
		}
		lock;
		onMe.pushToSleep((PCB*) PCB::running, timeToSleep);
		unlock;
		PCB::running->wayOf = PCB::NOWAKEUP;
		dispatch(); //Nece se izvristi, kad se uradi allow izvrsice se promena konteksta.
	} else {
		PCB::running->wayOf = PCB::SIGNAL;
	}
	allow
	int wake = PCB::running->wayOf;
	return wake;
#endif
}

int KernelSem::val() const {
	return value;
}
void KernelSem::shakeAllSemafor() {
	KernelSem * help = (KernelSem *) KernelSem::allKernelSem;
	while (help != 0) {
		help->shakeNatalija();
		KernelSem * temp = help;
		help = help->nextSem;
		if (temp->onMe.headSleepers == 0){
			removeFromSleepyList(temp);
		}
	}
}
KernelSem::KernelSem(Semaphore * semaphore, int startValue) {
	forbid
	value = startValue;
	nextSem = 0;
	previousSem = 0;
	mySem = semaphore;
	allow
}

KernelSem::~KernelSem() {
#ifndef BCC_BLOCK_IGNORE
	forbid
	int a = value;
	lock;
	onMe.freeAllThreads();
    unlock;
	if (previousSem != 0)
		previousSem->nextSem = nextSem;
	if (nextSem != 0)
		nextSem->previousSem = previousSem;
	if (this == allKernelSem)
		allKernelSem = allKernelSem->nextSem;

	previousSem = 0;
	nextSem = 0;
	mySem = 0;
	allow
#endif
}

void KernelSem::removeAllKernelSem() {
//Samo one sa vremenom.
	while (allKernelSem != 0) {
		delete allKernelSem;

	}
}

void KernelSem::addToSleepyList(KernelSem* ker) {
	ker->nextSem = (KernelSem *) allKernelSem;
	ker->previousSem = 0;
	if (ker->nextSem != 0)
		ker->nextSem->previousSem = ker;
	allKernelSem = ker;
}

void KernelSem::removeFromSleepyList(KernelSem* ker) {
	if (ker->previousSem != 0)
		ker->previousSem->nextSem = ker->nextSem;
	if (ker->nextSem != 0)
		ker->nextSem->previousSem = ker->previousSem;
	if (ker == allKernelSem)
		allKernelSem = allKernelSem->nextSem;
	ker->previousSem = 0;
	ker->nextSem = 0;
}

void KernelSem::shakeNatalija() {//Funny function
	QUEUE::Elem * help = onMe.headSleepers, *previous = 0;
	while(help!=0){
		if(help->pcb->threadState == PCB::SLEEPING){
			help->timeToSleep--;
			if(help->timeToSleep == 0){
				if(previous == 0)
					onMe.headSleepers = help->next;
				else
					previous->next = help->next;
				if(help == onMe.tailSleepers) onMe.tailSleepers = previous;
				help->pcb->threadState = PCB::READY;
				PCB * help1 = help->pcb;
				QUEUE::Elem * temp = help;
				help = help->next;
				temp->next = 0;
				delete temp;
				temp = 0;
				help1->wayOf = PCB::TIMER;
				Scheduler::put(help1);
				value++;
			}
			else {
				previous = help;
				help = help->next;
			}
		}else{
			previous = help;
			help = help->next;
		}
	}


}
