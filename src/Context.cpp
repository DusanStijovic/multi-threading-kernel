#include "Context.h"
#include "Kernel.h"
#include "Idle.h"
#include <IOSTREAM.H>
#include "Semaphor.h"
#include "SCHEDULE.H"
void tick();
volatile int Context::takeFromScheduler = 1;
volatile PCB * Context::changeContextTo = 0;
volatile int Context::forbidPreemption = 0;
volatile int Context::preemptionOnRequest = 0;
volatile int Context::timeForPreemption = 0;
volatile int Context::timeLeft = 0;
volatile int Context::counter = 0;
PCB* help = 0;
interruptPointer Context::timerRoutine = 0;
unsigned tempsp = 0, tempss = 0, tempbp = 0;
void interrupt Context::timer(...) {
	if (!preemptionOnRequest && timeLeft >= 0)
		timeLeft--;
	if (timeLeft == 0 || timeForPreemption || preemptionOnRequest) {
		if (!forbidPreemption) {
			timeForPreemption = 0;
#ifndef BCC_BLOCK_IGNORE
			asm {
				mov tempsp, sp
				mov tempss, ss
				mov tempbp, bp

			}
#endif
			if(PCB::running!=0){
			PCB::running->context.sp = tempsp;
			PCB::running->context.ss = tempss;
			PCB::running->context.bp = tempbp;

			if (PCB::running->threadState == PCB::RUNNING) {
				PCB::running->threadState = PCB::READY;
				Scheduler::put((PCB *) PCB::running);
			}
			}
			PCB::running = Scheduler::get();

			if (PCB::running == 0) {
				PCB::running = PCB::idlePCB;
				PCB::running->threadState = PCB::IDLE;
			} else
				PCB::running->threadState = PCB::RUNNING;

			tempsp = PCB::running->context.sp;
			tempss = PCB::running->context.ss;
			tempbp = PCB::running->context.bp;
			timeLeft = PCB::running->timeSlice;
#ifndef BCC_BLOCK_IGNORE
			asm {
				mov sp, tempsp
				mov ss, tempss
				mov bp, tempbp
			}
#endif

#ifndef BCC_BLOCK_IGNORE
			if(Context::forbidPreemption == 0 ) {
				forbid
				asm pushf
				asm sti
				//unlock
				PCB::running->threadSignal->processReceivedSignals();
				asm popf
			   //lock
				allow
			}
#endif
			if (PCB::running->threadState == PCB::KILLED) {

		        help =(PCB *) PCB::running;

				PCB::running = Scheduler::get();
				if (PCB::running == 0) {
					PCB::running = PCB::idlePCB;
					PCB::running->threadState = PCB::IDLE;
				} else
					PCB::running->threadState = PCB::RUNNING;

				tempsp = PCB::running->context.sp;
				tempss = PCB::running->context.ss;
				tempbp = PCB::running->context.bp;
				timeLeft = PCB::running->timeSlice;
#ifndef BCC_BLOCK_IGNORE
				asm {
					mov sp,tempsp
					mov ss, tempss
					mov bp, tempbp
				}
#endif
				delete help;
				help  = 0;
			}

			//Mark for preemption
		} else
			timeForPreemption = 1;
	}

	if (!preemptionOnRequest) {
#ifndef  BCC_BLOCK_IGNORE
		asm int 60h;
		tick();
#endif
		//Update time on semaphores
		KernelSem::shakeAllSemafor();
	}

	preemptionOnRequest = 0;

}

//Postavljanje i restauracija IVT tabele
void Context::init() {
	PCB::main = (new Thread(150, 5))->myPCB;
	PCB::main->threadState = PCB::RUNNING;
	PCB::running = PCB::main;
	PCB::idlePCB = (new Idle())->myPCB;
	PCB::idlePCB->threadState = PCB::IDLE;
	timeLeft = PCB::main->timeSlice;
	Signal::initialize();
#ifndef BCC_BLOCK_IGNORE
	lock
	timerRoutine = getvect(0x08);
	setvect(0x08,timer);
	setvect(0x60,timerRoutine);
	unlock
#endif
}
void Context::restore() {
#ifndef BCC_BLOCK_IGNORE
	asm cli
	setvect(0x08,timerRoutine);
	asm sti
#endif
	delete PCB::idlePCB->myThread;
	delete PCB::main->myThread;
	PCB::idlePCB = 0;
	PCB::main = 0;
	PCB::allPCB = 0;
	PCB::running = 0;
}
//Na kraju proci kroz sve i vratiti sve prekidne rutine, koje je korisnik uzeo.
