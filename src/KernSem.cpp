#include "list.h"
#include <stdio.h>
#include <iostream.h>
#include "SCHEDULE.H"
#include "semaphor.h"
#include "KernSem.h"
#include "pcb.h"

//volatile List<KernelSem*> KernSemList;

KernelSem::KernelSem(int val) {
    lock;
    set_val(val);
    //KernSemList.add(this);
    unlock;
}

int KernelSem::get_val() {
    return this->val;
}

void KernelSem::set_val(int val) {
    this->val = val;
}

// Ukoliko je vrednost semafora negativna, tada 
// na semaforu mora biti onoliko blokiranih niti 
// koliko je apsolutna vrednost semafora

int KernelSem::wait(Time maxTimeToWait) {   
// maxTimeToWait - max trajanje blokiranostipozivajuce niti
// ako je 0 -> duzina cekanja nije vremenski ogranicena
// ako je nit deblokirana pozivom signal() nad tim istim semaforom, rezultat je 1
    lock;
    this->val--;
    if (get_val() < 0) {
        //suspendujemo running nit i stavimo u listu blokiranih
        PCB::running->blocked = 1;
        if (maxTimeToWait == 0) {
        	PCB::running->timeToWait = -1;	//neograniceno vremenski, posle lepo samo provera da li je >0 (najlakse tako), isto kao i sa timeSlice
        } else {
        	PCB::running->timeToWait = maxTimeToWait;
        }
        blocked.add((PCB*)PCB::running);
        //uzimamo drugu nit iz liste spremnih i promenimo kontekst
        //PCB::running = Scheduler::get();	//ne treba jer ce u dispatchu da uzme novu nit
        dispatch();
    } else {
        unlock;
        return 1;

    }
    unlock;
    //ako je odblokirana zbog signal onda vracamo 1
    if (PCB::running->timeToWait != 0) {
            return 1;
    } else {

            return 0;
    }
}

void KernelSem::signal() {  
    lock;
    this->val++;
    if (get_val() <= 0) {
        //uzmemo 1 nit iz liste blokiranih i odblokiramo je i stavimo u spremne
        PCB* tmp = blocked.get();
        tmp->ready = 1;
        tmp->blocked = 0;
        Scheduler::put(tmp);

    }
    unlock;
}
