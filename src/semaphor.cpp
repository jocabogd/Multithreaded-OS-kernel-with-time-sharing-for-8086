#include "list.h"
#include <stdio.h>
#include <iostream.h>
#include "SCHEDULE.H"
#include "semaphor.h"
#include "KernSem.h"
#include "pcb.h"

//List<KernelSem*> KernSemList;

Semaphore::Semaphore (int init) {
    lock
    this->myImpl = new KernelSem(init);
    KernSemList.add(myImpl);
    unlock
}

Semaphore::~Semaphore() {
    lock
    KernSemList.delete_element(myImpl);
    delete myImpl;
    unlock
}

int Semaphore::wait(Time maxTimeToWait) {
    return myImpl->wait(maxTimeToWait);
}

void Semaphore::signal() {
    myImpl->signal();
}

int Semaphore::val() const {
    return myImpl->get_val();
}
