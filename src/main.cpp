#include "thread.h"
#include "pcb.h"
#include "list.h"
#include <stdio.h>
#include <iostream.h>
#include "SCHEDULE.H"
#include <dos.h>
#include <stdarg.h>
#include <stdlib.h>
#include "semaphor.h"
#include "KernSem.h"
#include "IVTEntry.h"
#include "KernelEv.h"
#include "event.h"
#include "user.h"
#include "intLock.h"
#include "user.h"
#include "bounded.h"
#include "keyevent.h"

//deklaracija inic i restore
void inic();
void restore();

int syncPrintf(const char *format, ...)
{
	int res;
	va_list args;
	lock
	va_start(args, format);
	res = vprintf(format, args);
	va_end(args);
	unlock
		return res;
}

int userMain(int argc, char* argv[]);

int main(int argc, char* argv[]) {
	inic();

	lock
    PCB* main_thread = new PCB(MAX_SIZE, 2);
    PCB::running = main_thread;
    //cout<<"main nit je napravljena"<<endl<<"-----------------------------"<<endl;;
    //cout<<"ID running PCB-a u main-u: "<<PCB::running->idPCB<<endl;
    //idPCB main niti je 0
    unlock
	userMain(argc, argv);

	restore();
	return 0;
}
