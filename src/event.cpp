/*
 * event.cpp
 *
 *  Created on: Aug 3, 2021
 *      Author: OS1
 */

#include <stdio.h>
#include <iostream.h>
#include "SCHEDULE.H"
#include "event.h"
#include "IVTEntry.h"
#include "pcb.h"
#include "KernelEv.h"

Event::Event(IVTNo ivtNo) {
	lock;
	this->myImpl = new KernelEv(ivtNo);
	unlock;
}

void Event::wait() {
	lock;
	this->myImpl->wait();
	unlock;
}

void Event::signal() {
	lock;
	this->myImpl->signal();
	unlock;
}

Event::~Event() {
	lock;
	//signal();
	delete this->myImpl;
	unlock;
}
