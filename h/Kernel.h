/*
 * Kernel.h
 *
 *  Created on: May 9, 2019
 *      Author: OS1
 */
#ifndef WA_KERNEL_H_
#define WA_KERNEL_H_
#include "Thread.h"
#include "SCHEDULE.H"
#include "KSem.h"
typedef const int OFF;
OFF PSW = 1, ALLREGISTER = 12;
OFF FUNCTIONOFF = 3, FUNCTIONSEGMENT = 2;


class PCB{
public:
	enum ThreadState{CREATED,STARTED,RUNNING,READY,BLOCKED,SLEEPING, WAITING, DEAD,DELETED,IDLE, THREADKILLER,KILLED};//Dodacemo jos vrv.
	enum WakeUp{ TIMER, SIGNAL, NOWAKEUP};
	struct ThreadContext{
		   unsigned sp,ss,bp;
	};
	volatile WakeUp wayOf;
	//Thread running on processor
	volatile static PCB * running;

	Thread *myThread;
	ThreadState threadState;
    Time timeSlice;
    ThreadContext  context;
    unsigned * stack;
    //List of threads
    PCB * waitingToFinished;
    PCB * nextToWait;
    //Global list of threads
    PCB * nextPCB;
    PCB * previousPCB;
    void initializeStack(StackSize stackSize);
    PCB(Thread * myThread, StackSize stackSize,Time timeSlice);
    ~PCB();
    void addToWaitingToFinished(PCB * myPCB);
    void removeAllFromWaitingToFinished();
    void tellChildrenParentIsDead();
    //Methods for thread
    void start();
    void waitToComplete();
    static ID getRunningId();


    //Static members of PCB
    static void wrapper();
    //Head of global list
    volatile static PCB * allPCB;
    //Main and idle thread;
    static PCB* idlePCB, *main;
    static int numberofThreads;
    void removeFromAllPCB();
    static Thread * getThreadByID(ID threadID);

    //SIGNALS
    Signal* threadSignal;
    PCB* parent;
};

#endif /* OS1PROJE_WA_KERNEL_H_ */
