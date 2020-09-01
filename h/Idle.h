/*
 * Idle.h
 *
 *  Created on: May 16, 2019
 *      Author: OS1
 */
#ifndef WA_IDLE_H_
#define WA_IDLE_H_
#include "Thread.h"
class Idle :public Thread{
private :
	int i, j;
public:
	Idle():Thread(1024,1), i(7), j(3){};
protected:
  void run();
  void start(){};
};

#endif /* WA_IDLE_H_ */
