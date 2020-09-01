/*
 * Context.h
 *
 *  Created on: May 15, 2019
 *      Author: OS1
 */
#include "DOS.H"
#ifndef CONTEXT_H_
#define CONTEXT_H_
/*
	#define lock asm cli;
	#define unlock asm sti;
*/

#define lock asm {pushf;cli};
#define unlock asm {popf};
#define forbid \
	Context::forbidPreemption = 1;\
	Context::counter++;
#define allow \
	Context::counter--;\
	if(Context::counter == 0) \
		Context::forbidPreemption = 0;\
	if(Context::timeForPreemption) dispatch();

#define dispatchWhenCan\
	Context::timeForPreemtion = 1;
class PCB;
typedef void interrupt(*interruptPointer )(...);
class Context{
public:
	static volatile int takeFromScheduler;
	static volatile PCB *changeContextTo;
	static volatile int forbidPreemption;
	static volatile int timeLeft;
	static volatile int timeForPreemption;
	static volatile int preemptionOnRequest;
	static volatile int counter;
	static interruptPointer timerRoutine;
	static void interrupt timer(...);
static void init();
static void restore();
};
void dispatch();

#endif /* CONTEXT_H_ */
