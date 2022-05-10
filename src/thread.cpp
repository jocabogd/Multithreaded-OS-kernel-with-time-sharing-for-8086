#include "thread.h"
#include "pcb.h"
#include "list.h"
#include <stdio.h>
#include <iostream.h>
#include "SCHEDULE.H"

List<PCB*> PCBList;
//timer deklaracija samo
void timer();

Thread::Thread(StackSize stackSize, Time timeSlice) {
    lock
    myPCB = new PCB (stackSize, timeSlice);
    myPCB->set_thread(this);
    PCBList.add(myPCB);
    //cout<<"---------------------------"<<endl;
    unlock
}

Thread::~Thread() {
    lock
    //PCBList.delete_element(myPCB);
   //delete myPCB;
    unlock
}

void Thread::waitToComplete() {
    lock
	if(myPCB->finished == 1){
		unlock
		return;
	}
	//cout<<"wait to complete"<<endl;
    myPCB->wait_to_finish.add((PCB*)PCB::running);
    //cout<<"u wait to finish ima: "<<myPCB->wait_to_finish.number<<endl;
    PCB::running->blocked = 1;
    unlock
    dispatch();
}

void Thread::start() {
    lock
	//cout<<"startovana je nit"<<endl;
	if (myPCB->ready == 1) {
		unlock
		return;
	}
    myPCB->ready = 1;
    myPCB->blocked = 0;
    myPCB->sleeping = 0;
    myPCB->finished = 0;
    Scheduler::put(myPCB);
    //cout<<"1. stavili smo u Scheduler: "<<myPCB->idPCB<<endl;
    unlock 
}

ID Thread::getId() {
    return myPCB->idPCB;
}

ID Thread::getRunningId() {
    return PCB::running->idPCB;
}

Thread* Thread::getThreadById(ID id) {
    return PCBList.get_element(id)->myThread;
}

extern int context_switch_on_demand;

//sinhrona promena konteksta
void dispatch() {
    lock
	//cout<<"u dispatch-u sam"<<endl;
    context_switch_on_demand = 1;
    asm int 8h;
    unlock
}
