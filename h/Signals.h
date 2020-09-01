/*
 * Signal.h
 *
 *  Created on: May 18, 2019
 *      Author: OS1
 */

#ifndef SIGNALS_H_
#define SIGNALS_H_
typedef void (* SignalHandler)();
typedef unsigned int SignalId;
class Signal {
public:
	struct SignalToProcess{
		SignalHandler signalHandler;
		SignalToProcess *next;
		SignalToProcess(SignalHandler signalHandler ){
			this->signalHandler  = signalHandler;
			next= 0;
		}
	};
	struct ReceivedSignal{
		int signal;
		ReceivedSignal * next;
		ReceivedSignal(int signall){this->signal = signall;next = 0;}
	};
	enum Option{NO,YES};
	Signal();
	virtual ~Signal();
    void signal(SignalId signalId);
    void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId, SignalHandler hand1, SignalHandler hand2);

	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal );
    static void unblockSignalGlobally(SignalId signal);
    void processReceivedSignals();
    void processSignalHandlers(SignalId id);
    static void initialize();
    void inheritSettings(Signal *signal);
    void parentIAmDone();
    void iAmDone();
    void removeReceivedSignals();
private:
    SignalToProcess  *first[16], *last[16];
    Option signalAllowed[16];
    static Option signalAllowedGlobally[16];
    ReceivedSignal * head, *tail;
   static void killThread();
public:
   static void test();
   static void test1();
   static void test2();
   static void test3();
   static void test4();
   static void test5();
   static void test6();
};



#endif /* BCC_SIGNAL_H_ */
