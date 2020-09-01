/*
 * QUEUE.cpp
 *
 *  Created on: May 16, 2019
 *      Author: OS1
 */
#include <IOSTREAM.H>
#include "QUEUE.h"
#include "Kernel.h"
#include "Context.h"
WaitNumber QUEUE::waitNumber = 0;

QUEUE::QUEUE() {
	head = tail =  0;
	headSleepers = tailSleepers = 0;

}


void QUEUE::front() {
if (head == 0 && headSleepers == 0) return;
Elem * help = 0;
if(head == 0 && headSleepers!=0){
	help = headSleepers;
	headSleepers = headSleepers->next;
	if(headSleepers == 0) tailSleepers = 0;
	help->pcb->threadState = PCB::READY;
	Scheduler::put(help->pcb);
	help->pcb->wayOf = PCB::SIGNAL;
	delete help;
	return;
}
if(headSleepers == 0 && head!=0){
	help = head;
	head = head->next;
	if(head == 0) tail = 0;
	help->pcb->threadState = PCB::READY;
	Scheduler::put(help->pcb);
	help->pcb->wayOf = PCB::SIGNAL;
	delete help;
	return;
}
if(headSleepers->assignedNumber < head->assignedNumber){
	help = headSleepers;
	headSleepers = headSleepers->next;
	if(headSleepers == 0) tailSleepers = 0;
	help->pcb->threadState = PCB::READY;
	Scheduler::put(help->pcb);
	help->pcb->wayOf = PCB::SIGNAL;
	delete help;
	return;
}

if(head->assignedNumber<headSleepers->assignedNumber){
	help = head;
	head = head->next;
	if(head == 0) tail = 0;
	help->pcb->threadState = PCB::READY;
	Scheduler::put(help->pcb);
	help->pcb->wayOf = PCB::SIGNAL;
	delete help;
	return;

}

}
void QUEUE::pushToSleep(PCB* pcb, Time timeToSleep) {
	if(timeToSleep>0){
		pcb->threadState = PCB::SLEEPING;
		Elem * temp = new Elem(pcb, timeToSleep,waitNumber++);
			if (tailSleepers != 0) tailSleepers->next = temp;
			tailSleepers = temp;
			if(headSleepers == 0) headSleepers = tailSleepers;
	}
	else{
		pcb->threadState = PCB::BLOCKED;
		Elem * temp = new Elem(pcb, timeToSleep,waitNumber++);
			if(tail != 0)	tail->next = temp;
			tail = temp;
			if(head == 0) head = tail;
	}

}


QUEUE::~QUEUE() {
	//Semaphore delete list , in his construcotr
	freeAllThreads();
	head = headSleepers = 0;
	tail = tailSleepers = 0;
	this->waitNumber = 0;
}

void QUEUE::freeAllThreads() {
forbid
while(head!=0){
		head->pcb->threadState = PCB::READY;
		head->pcb->wayOf = PCB::SIGNAL;
		Scheduler::put(head->pcb);
		Elem * help = head;
		head = head->next;
		delete help;
	}
while(headSleepers!=0){
		headSleepers->pcb->threadState = PCB::READY;
		head->pcb->wayOf = PCB::SIGNAL;
		Scheduler::put(head->pcb);
		Elem * help = headSleepers;
		headSleepers = headSleepers->next;
		delete help;

	}

tail = tailSleepers = 0;
head = headSleepers = 0;
allow
}


