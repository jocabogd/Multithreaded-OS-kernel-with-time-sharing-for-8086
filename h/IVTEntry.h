/*
 * IVTEntry.h
 *
 *  Created on: Aug 3, 2021
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "KernelEv.h"

typedef void interrupt(*pInterrupt)(...);

//extern IVTEntry* entries[256]; //ah...

class IVTEntry;
//kreiranje 1 objekta ivtentry i kreiranje prekidne rutine koju ce ovaj objekat prilikom svog kreiranja podmetnuti na odg. ulaz
#define PREPAREENTRY(numEntry, callOld)\
void interrupt inter##numEntry(...);\
IVTEntry newEntry##numEntry(numEntry, inter##numEntry);\
void interrupt inter##numEntry(...){\
	newEntry##numEntry.signal();\
	if (callOld == 1) newEntry##numEntry.callold();\
}

class IVTEntry {
public:
	IVTEntry(IVTNo ivtNo, pInterrupt);
	~IVTEntry();

	IVTNo entryNum;
	pInterrupt oldR;
	//pInterrupt newR;

	static IVTEntry* entries[256]; //neka bude staticka
	static IVTEntry* getEntry(IVTNo ivtNo);

	void signal();
	void callold();

	void set_event(KernelEv* my_event);

private:
	KernelEv* event;
};



#endif /* IVTENTRY_H_ */
