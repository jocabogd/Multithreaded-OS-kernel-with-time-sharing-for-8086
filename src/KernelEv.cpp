/*
 * KerenlEv.cpp
 *
 *  Created on: Aug 3, 2021
 *      Author: OS1
 */

#include "list.h"
#include <stdio.h>
#include <iostream.h>
#include "SCHEDULE.H"
#include "event.h"
#include "KernelEv.h"
#include "IVTEntry.h"
#include "pcb.h"

KernelEv::KernelEv(IVTNo ivtNo) {
	lock;
	set_val(1);
	this->creatorPCB = (PCB*)PCB::running;
	//treba ovde postaviti ulaz
	//bice potrebno da se dohvati ulaz ivtentry
	this->entry = IVTEntry::getEntry(ivtNo);
	this->entry->set_event(this);
	unlock;
}

int KernelEv::get_val() {
    return this->val;
}

void KernelEv::set_val(int val) {
    this->val = val;
}

void KernelEv::wait() {
	lock;
	if ((PCB*)PCB::running == creatorPCB) {
		//samo ona koja je kreirala dogadjaj moze da se blokira
		if (get_val() == 1) {
			// Blokira se na wait
			set_val(0);
			PCB::running->blocked = 1;
			PCB::running->ready = 0;

			dispatch();
		}
	}
	unlock;
}

void KernelEv::signal() {
	//lock;
	//cout << "KernelEv.Signal!" << endl;
	if (get_val() == 0) {

		set_val(1);

		creatorPCB->blocked = 0;
		creatorPCB->ready = 1;

		Scheduler::put(creatorPCB);
		//cout << "dispatch!!!" << endl;
		dispatch();
	}
	//unlock;
}

