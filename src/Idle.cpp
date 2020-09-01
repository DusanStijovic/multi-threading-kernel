/*
 * Idle.cpp
 *
 *  Created on: May 16, 2019
 *      Author: OS1
 */
#include "Idle.h"
#include "IOSTREAM.H"
#include "Kernel.h"
#include "Context.h"
void Idle::run(){
	volatile  int k = -2;
	int i = 2, j = 10;
	while(k<0){
			i = (-1) * i * j;
			j = j *j * i * i * (-1);
		  if(k > 0) k = -1;
	}
}
