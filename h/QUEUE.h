/*
 * QUEUE.h
 *
 *  Created on: May 16, 2019
 *      Author: OS1
 */

#ifndef WA_QUEUE_H_
#define WA_QUEUE_H_
typedef unsigned int Time;
class PCB;
typedef unsigned int WaitNumber;
class QUEUE {
struct Elem{
PCB * pcb;
Elem * next;
Time timeToSleep;
WaitNumber assignedNumber;
Elem(PCB * pcb,Time maxTimeToSleep,WaitNumber assign){
	this->pcb = pcb;
	next = 0;
	timeToSleep = maxTimeToSleep;
	assignedNumber = assign; }
};
public:
Elem * headSleepers, * tailSleepers;
Elem * head, *tail;
static WaitNumber waitNumber;
public:
void front();
void pushToSleep(PCB * pcb, Time timeToSleep);
QUEUE();
virtual ~QUEUE();
void freeAllThreads();
};

#endif /* WA_QUEUE_H_ */
