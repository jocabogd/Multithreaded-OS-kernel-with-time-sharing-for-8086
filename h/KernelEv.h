/*
 * KernelEv.h
 *
 *  Created on: Aug 3, 2021
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_

#include "event.h"
#include "IVTEntry.h"

class IVTEntry;

class KernelEv {
public:
	KernelEv(IVTNo ivtNo);
	int get_val();
	void set_val(int val);
	PCB* creatorPCB;
	void wait();
	void signal();

	IVTEntry* entry;

protected:
	friend class Event;

private:
	int val;
};



#endif /* KERNELEV_H_ */
