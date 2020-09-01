/*
 * Thread.h
 *
 *  Created on: May 9, 2019
 *      Author: OS1
 */

#ifndef OS1PROJE_THREAD_H_
#define OS1PROJE_THREAD_H_
#include "Signals.h"
//STACK SIZE
typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;//Defaultna velicina steka niti.
const StackSize maxStackSize = 65535;
//TIME SLICE
typedef unsigned int Time;//time in 55ms
const Time defaultTimeSlice = 2;//default time 2*55ms
//THREAD ID
typedef int ID;


class PCB;
class Idle;
class Context;
class Thread{
public:
	void start();
	void waitToComplete();
    virtual ~Thread();
	ID getId();
	static ID getRunningId();
	static Thread *getThreadById(ID id);
public:
	friend class PCB;
	friend class IDLE;
	friend class Context;
	Thread(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run(){};
	virtual void cao(){waitToComplete();}
	//Signals
	void signal(SignalId signalId);
	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId, SignalHandler hand1, SignalHandler hand2);

	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal );
    static void unblockSignalGlobally(SignalId signal);
private:
	PCB * myPCB;
	volatile static ID id;
	ID threadID;
};

void dispatch();


#endif /* OS1PROJE_THREAD_H_ */
