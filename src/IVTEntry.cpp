/*
 * IVTEntry.cpp
 *
 *  Created on: Aug 3, 2021
 *      Author: OS1
 */

#include <stdio.h>
#include <iostream.h>
#include "SCHEDULE.H"
#include "event.h"
#include "IVTEntry.h"
#include "KernelEv.h"
#include "pcb.h"
#include <DOS.H>

IVTEntry* IVTEntry::entries[256] = {0};

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt newRoutine) {
	lock;
	this->entryNum = ivtNo;
	//this->newR = newRoutine;
#ifndef BCC_BLOCK_IGNORE
	this->oldR = getvect(this->entryNum);
	setvect(this->entryNum, newRoutine);
#endif
	entries[ivtNo] = this;
	unlock;
}

IVTEntry::~IVTEntry() {
	lock;
	//restauracija stare rutine
#ifndef BCC_BLOCK_IGNORE
	setvect(this->entryNum, this->oldR);
#endif
	this->oldR();
	unlock;
}

IVTEntry* IVTEntry::getEntry(IVTNo ivtNo) {
	//dohvatanje pokazivaca na objekat IVTEntry koji je vezan za zadati ulaz ivtNo
	lock;
	return entries[ivtNo];
	unlock;
}

void IVTEntry::signal() {
	lock;
	//??
	this->event->signal();
	unlock;
}

void IVTEntry::callold() {
	//lock;
	//poziv stare rutine
	this->oldR();
	//unlock;
}

void IVTEntry::set_event(KernelEv* my_event) {
	lock;
	this->event = my_event;
	unlock;
}

