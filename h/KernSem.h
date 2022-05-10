#ifndef KERN_SEM_H
#define KERN_SEM_H
#include "semaphor.h"
#include "list.h"

extern List<KernelSem*> KernSemList;

class KernelSem {
public:
    KernelSem(int val);
    int get_val();
    void set_val(int val);
    List<PCB*> blocked;
    virtual int wait(Time maxTimeToWait);
    virtual void signal();

protected:
    friend class Semaphore;

private:
    int val;
};

#endif
