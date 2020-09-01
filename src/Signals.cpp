#include "Signals.h"
#include "Context.h"
#include "Kernel.h"
Signal::Option Signal::signalAllowedGlobally[16];

Signal::Signal() {
	for (SignalId id = 0; id < 15; id++) {
		first[id] = last[id] = 0;
		signalAllowed[id] = YES;
	}
	head = tail = 0;
	registerHandler(0,killThread);
}

Signal::~Signal() {
	for(SignalId id = 0;id<16;id++){
		this->unregisterAllHandlers(id);
	}
	removeReceivedSignals();
}

void Signal::signal(SignalId signalId) {
	forbid
	if(signalId != 1 && signalId!=2){
		ReceivedSignal* temp = new ReceivedSignal(signalId);
		if(tail!=0)tail->next = temp;
		tail = temp;
		if(head == 0)head =tail;

	}
	allow
}

void Signal::registerHandler(SignalId signal, SignalHandler handler) {
	forbid
	SignalToProcess *temp = new SignalToProcess(handler);
	if (last[signal] != 0)
		last[signal]->next = temp;
	last[signal] = temp;
	if (first[signal] == 0)
		first[signal] = last[signal];
	allow
}

void Signal::unregisterAllHandlers(SignalId id) {
	forbid
	SignalToProcess * help = first[id];
	while (help!=0) {
		SignalToProcess * temp = help;
		help = help->next;
		temp->next = 0;
		//delete temp;
	}
	first[id] = 0;
	last[id] = 0;
	allow
}

void Signal::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	forbid
	SignalToProcess * help1 = 0, *help2 = 0, *help = first[id];
	while (help) {
		if (help->signalHandler == hand1)
			help1 = help;
		if (help->signalHandler == hand2)
			help2 = help;
		if (help1 != 0 && help2 != 0)
			break;
		help = help->next;
	}
	if (help1 != 0 && help2 != 0) {
		SignalHandler temp = help1->signalHandler;
		help1->signalHandler = help2->signalHandler;
		help2->signalHandler = temp;
	}
	allow
}

void Signal::blockSignal(SignalId signal) {
	forbid
	signalAllowed[signal] = NO;
    allow
}

void Signal::blockSignalGlobally(SignalId signal) {
forbid
	signalAllowedGlobally[signal] = NO;
allow
}

void Signal::unblockSignal(SignalId signal) {
forbid
	signalAllowed[signal] = YES;
allow
}

void Signal::unblockSignalGlobally(SignalId signal) {
forbid
	signalAllowedGlobally[signal] = YES;
allow
}

void Signal::processReceivedSignals() {
	for (ReceivedSignal * temp = head, * prev = 0;temp!=0;) {
	    int	id = temp->signal;

		if (signalAllowedGlobally[id] == NO){
		    prev = temp;
		    temp = temp->next;
			continue;
		}
		if (signalAllowed[id] == NO){
			prev = temp;
			temp = temp->next;
			continue;
		}
	    processSignalHandlers(id);
	    if(id == 0) {
	    	if(PCB::running->parent!=0) PCB::running->parent->threadSignal->parentIAmDone();
	    	PCB * temp1 = (PCB *) PCB::running;
	    	if(temp1!=0) temp1->tellChildrenParentIsDead();
	    }
	    ReceivedSignal * help = temp;
	    if(temp == head) head = temp->next;
	    if(prev!=0) prev->next = temp->next;
	    if(temp == tail) tail = prev;
	    if(id == 0) {
	    	return;

	    }
	    temp = temp->next;
	    delete help;
	    help = 0;
	}
}
void Signal::processSignalHandlers(SignalId id) {
   SignalToProcess * process = first[id];
   while (process != 0) {
		process->signalHandler();
		if(process->signalHandler == Signal::killThread){
			process = 0;
			return;
		}
		process = process->next;
	}
}

void Signal::initialize() {
	for (SignalId id = 0; id < 15; id++){
		signalAllowedGlobally[id] = YES;
	}
}

void Signal::inheritSettings(Signal *signal) {
	for (SignalId id = 0; id < 15; id++){
		signalAllowed[id] = signal->signalAllowed[id];
		SignalToProcess * help = signal->first[id];
		while(help!=0){
         this->registerHandler(id,help->signalHandler);
         help = help->next;
		}

	}
}
void Signal::removeReceivedSignals(){
ReceivedSignal * help = head;
while(head){
 help = head;
 head = head->next;
 if(help!=0)delete help;
}

}
void Signal::killThread() {
 PCB::running->threadState = PCB::KILLED;
 PCB * help = (PCB * ) PCB::running;
 help->removeAllFromWaitingToFinished();
 PCB::numberofThreads--;
}

void Signal::parentIAmDone() {
	ReceivedSignal* temp = new ReceivedSignal(2);
			if(tail!=0)tail->next = temp;
			tail = temp;
			if(head == 0)head =tail;
}



void Signal::iAmDone() {
	ReceivedSignal* temp = new ReceivedSignal(1);
			if(tail!=0)tail->next = temp;
			tail = temp;
			if(head == 0)head =tail;
}
