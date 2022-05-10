#include <stdio.h>
#include <iostream.h>
#include <dos.h>
#include "list.h"
#include "thread.h"
#include "pcb.h"
#include "SCHEDULE.H"

int PCB::ID = 0;

PCB::PCB(StackSize stackSize, Time timeSlice) {
    //lock
	//lock
	//cout<<"usla sam ovde, ID je: "<<ID<<endl;
	//unlock
	if (ID != 0) {
		//cout << "usla sam u "<< ID << endl;
		if (stackSize > MAX_SIZE) {
			stackSize = MAX_SIZE/2;
		} else stackSize = stackSize / 2;
		stack = new unsigned[stackSize];

#ifndef BCC_BLOCK_IGNORE
    stack[stackSize - 1] = FP_SEG(myThread);
    stack[stackSize - 2] = FP_OFF(myThread);

    stack[stackSize - 5] = 0x200; //fleg I=1
    stack[stackSize - 6] = FP_SEG(wrapper);
    stack[stackSize - 7] = FP_OFF(wrapper);

    ss = FP_SEG(stack + stackSize - 16);
    sp = FP_OFF(stack + stackSize - 16);
    bp=sp;
#endif
    }

    this->idPCB = ID++;
    if (timeSlice > 0) this->timeSlice = timeSlice;
    else this->timeSlice = -1;	//oznacava da je neograniceno

    ready = 0;
    finished = 0;
    blocked = 0;
    sleeping = 0;
    //lock
	//cout<<"nit ima idPCB: "<<this->idPCB<<endl;//<<"izasla sam ovde"<<endl;
    //unlock
    //unlock
}

PCB::PCB(void (*body)(), int time) {
    lock

    //int stackSize = 1024;
    unsigned* stack = new unsigned[SIZE];

    stack[SIZE - 1] = 0x200; //fleg I=1
#ifndef BCC_BLOCK_IGNORE
    stack[SIZE - 2] = FP_SEG(body);
    stack[SIZE - 3] = FP_OFF(body);

    ss = FP_SEG(stack + SIZE - 12);    //svi sacuvani registri pri ulasku u interrupt rutinu
    sp = FP_OFF(stack + SIZE - 12);
    bp = sp; 
#endif

    //idPCB = ++ID;
    //runningPCB = (PCB*)running;
    this->timeSlice = time;
    this->ready = 0;
    //this->myThread = my_thread;

    unlock
}

PCB::~PCB() {
    lock

    finished = 1;
    /*ready = 0;
    sleeping = 0;
    blocked = 0;
    if (stack != 0) {
        delete[] stack;
    }
    stack = 0;*/

    unlock
}

void PCB::wrapper() {
//	lock
//	cout<<"pozivamo run"<<endl;
//	unlock
    PCB::running->myThread->run();
	lock
	//cout<<"gotov run"<<endl;

	int n = PCB::running->wait_to_finish.number;

	//cout<<"da li ima neceg u wait_to_finish? n je: "<<n<<endl;

    List<PCB*>::Elem* tmp = PCB::running->wait_to_finish.head;
    while (tmp) {
        tmp->t->ready = 1;
        tmp->t->blocked = 0;
        Scheduler::put(tmp->t);
        tmp = tmp->next;
    }
    PCB::running->finished = 1;
    //izbaciti iz liste
    //unlock
    dispatch();
}

//postavi na myThread
void PCB::set_thread(Thread* my_thread) {
	lock
	myThread = my_thread;
	unlock
}
