#ifndef PCB_H
#define PCB_H

//zabranjuje prekide
#define lock asm cli;

//dozvoljava prekide
#define unlock asm sti;

//maksimalna velicina steka
#define MAX_SIZE 65535

//standardna velicina steka
#define SIZE 1024

#include "list.h"
#include "thread.h"

//da probam samo
extern List<PCB*> PCBList;

class PCB {
public:
    volatile unsigned ss;
    volatile unsigned sp;
    volatile unsigned bp;
    volatile int timeSlice;
    unsigned* stack;
    volatile int finished;   //0 - nije zavrsena, 1 - jeste zavrsena
    volatile int blocked;    //0 - nije blokirana, 1 - jeste blokirana
    volatile int sleeping;   //0 - ne spava, 1 - spava
    volatile int ready;      //0 - nije spremna, 1 - jeste spremna
    List<PCB*> wait_to_finish;  //za waitToComplete() i wrapper
    Thread* myThread;
    volatile static PCB* running;  //sta je volatile? - da se nesto ne bi zeznulo tokom pr.rutine

    static int ID;
    int idPCB;

    volatile int timeToWait;	//za semafore, koliko treba da ceka

    void set_thread(Thread* my_thread);	//stavlja myThread
    PCB(void (*body)(), int time);  //za onu idle nit
    PCB(StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
    ~PCB();
    static void wrapper(/*Thread* running_thread*/);

protected:
    friend class Thread;
};

#endif
