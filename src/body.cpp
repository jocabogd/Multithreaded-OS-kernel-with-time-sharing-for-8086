#include <stdio.h>
#include <iostream.h>
#include <dos.h>
#include "list.h"
#include "thread.h"
#include "pcb.h"
#include "SCHEDULE.H"
#include "semaphor.h"
#include "KernSem.h"

//stara prekidna rutina
unsigned oldTimerOFF, oldTimerSEG;

//deklaracija nove prekidne rutine
void interrupt timer();

//tick metoda
void tick();

//sinhrona promena konteksta - dispatch
void dispatch();

//"zaludna" nit
void idle() {
	//lock
	//cout<<"u idle sam"<<endl;
	//unlock
    while(1);
}

static PCB* idle_pcb = new PCB(idle, 1); 

//pomocne promenljive za prekid tajmera
volatile unsigned tsp;
volatile unsigned tss;
volatile unsigned tbp;

volatile int cnt = 2;
volatile int context_switch_on_demand = 0;

//fleg za zabranu promene konteksta
volatile int lock_flag = 1;

//definisemo staticke promenljive
volatile PCB* PCB::running = 0;

//postavlja novu prekidnu rutinu
void inic() {
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; inicijalizuje rutinu za tajmer
		mov es,ax	//es = 0

		mov ax, word ptr es:0022h //; pamti staru rutinu
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer	 //postavlja
		mov word ptr es:0020h, offset timer //novu rutinu

		mov ax, oldTimerSEG	 //	postavlja staru rutinu
		mov word ptr es:0182h, ax //; na int 60h
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
		sti
	}
}


//vraca staru prekidnu rutinu
void restore() {
	asm {
			cli
			push es
			push ax

			mov ax,0
			mov es,ax


			mov ax, word ptr oldTimerSEG
			mov word ptr es:0022h, ax
			mov ax, word ptr oldTimerOFF
			mov word ptr es:0020h, ax

			pop ax
			pop es
			sti
		}
}

List<KernelSem*> KernSemList;

//nova prekidna rutina tajmera
void interrupt timer() {

	if (!context_switch_on_demand) {
		asm int 0x60;
		tick();
		//deo sa semaforima
		//prolazimo kroz listu semafora i kroz liste blokiranih niti na njima i ako je isteklo vreme odblokiramo
 		List<KernelSem*>::Elem* head = KernSemList.head;
		while (head != 0) {
			List<PCB*>::Elem* head_block = head->t->blocked.head;
			List<PCB*>::Elem* tmp = head_block;
			List<PCB*>::Elem *tmpnext;
			while (tmp != 0) {
				if (tmp->t->timeToWait > 0) {
					tmp->t->timeToWait--;
					if (tmp->t->timeToWait == 0) {
						tmp->t->ready = 1;
						tmp->t->blocked = 0;
						int val1 = head->t->get_val() + 1;
						head->t->set_val(val1);
						Scheduler::put(tmp->t);
						tmpnext = tmp->next;
						head->t->blocked.delete_element(tmp->t);
						tmp = tmpnext;
					} else {
						tmp = tmp->next;
					}
				} else {
					tmp = tmp->next;
				}
				//tmp = tmpnext;
			}
			head = head->next;
		}

	}

    //int dummy = 0;
    if (!context_switch_on_demand && cnt > 0) cnt--;
    if (cnt == 0 || context_switch_on_demand) {
    	//lock
		//cout<<PCB::running->idPCB<<" --- 111"<<endl<<"lock_flag je: "<<lock_flag<<endl;
    	//unlock
        if (lock_flag) {
            context_switch_on_demand = 0;

            asm {
                mov tsp, sp 
                mov tss, ss 
                mov tbp, bp 
            }

            PCB::running->sp = tsp;
            PCB::running->ss = tss;
            PCB::running->bp = tbp;

            //stavimo u Scheduler
            //lock
			//cout<<"2 ------------ finished: "<<PCB::running->finished<<endl<<"3 --------------- blocked: "<<PCB::running->blocked<<endl<<"4 ----------------------- ready: "<<PCB::running->ready<<endl;
			//unlock
            if (!PCB::running->finished && !PCB::running->blocked && PCB::running->ready) {
            	/*lock
				cout<<"stavljamo u Scheduler: "<<PCB::running->idPCB<<endl;
            	unlock*/
            	Scheduler::put((PCB*)PCB::running);
            }
            //uzmemo iz Schedulera
            PCB::running = Scheduler::get();
            /*lock
			cout<<"uzeli smo iz Scheduler-a: "<<PCB::running->idPCB<<endl;
			unlock*/

            //ako Scheduler vrati 0, onda stavimo running na ovu zaludnu nit
            if (PCB::running == 0) {

                PCB::running = idle_pcb;
            }

            tsp = PCB::running->sp;
            tss = PCB::running->ss;
            tbp = PCB::running->bp;

            cnt = PCB::running->timeSlice;

            asm {
                //restaurira sp
                mov sp, tsp
                mov ss, tss
                mov bp, tbp
            }
        } else {
            context_switch_on_demand = 1;
        }
    }
    //poziv stare prekidne rutine, koja se nalazila na 08h, a sad je na 60h
    //poziva se samo kada nije zahtevana promena konteksta
    //tako da se stara prekidna rutina poziva samo kada je stvarno doslo do prekida
    if (!context_switch_on_demand) {
    	//asm int 0x60;
    }
    //context_switch_on_demand = 0; - sad se menja u zavisnosti od lock_flag 
}

