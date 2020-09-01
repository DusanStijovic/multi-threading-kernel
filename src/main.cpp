/*
 * OS1PROJE.cpp
 *
 *  Created on: May 10, 2019
 *      Author: OS1
 */
#include "Kernel.h"
#include "Context.h"
#include <IOSTREAM.H>
#include "Semaphor.h"
#include "Signals.h"
#include "KernelEv.h"
//#include "ww.h"

extern int userMain(int argc, char **argv);

Semaphore * a = new Semaphore(0);
int main(int argc, char**argv){
Context::init();

int a = userMain(argc,argv);

Context::restore();
return a;
}

