#ifndef LISTA_H
#define LISTA_H

#include <iostream.h>

extern volatile int lock_flag;

template <class T>
class List
{
public:
    List() {
    	lock_flag = 0;
    	head = 0;
    	tail = 0;
    	number = 0;
    	lock_flag = 1;
    }

    ~List()
    {
        delete_list();
    }

    void add(T t)
    { //dodaj na kraj liste
    	lock_flag = 0; //ne smeju prekidi
        Elem *new_elem = new Elem(t);
        tail = (!head ? head : tail->next) = new_elem;
        number++;
        lock_flag = 1;
    }

    T get_element(int index); //dohvata element sa odgovarajucim indeksom
    void delete_element(T t); //brise element iz liste

    volatile int number;

    struct Elem
    {
        T t;
        Elem *next;
        Elem(T tt) : t(tt), next(0) {}
    };
    Elem *head, *tail;

    void delete_list();
    T get(); //uzima sa pocetka i brise posle
};

template <class T>
void List<T>::delete_list()
{
	lock_flag = 0;
    while (head)
    {
        Elem *old = head;
        head = head->next;
        delete old;
    }
    head = tail = 0;
    number = 0;
    lock_flag = 1;
}

template <class T>
void List<T>::delete_element(T t1)
{
	lock_flag = 0;
    Elem *curr = head;
    //cout<<"curr: "<<curr->t<<endl;
    Elem *prev = 0;

    while(curr) {
        if (curr->t == t1) {
            //izbacujemo element
            if (prev == 0) {
                //izbacujemo prvi
                Elem *tmp = curr;
                curr = curr->next;
                head = curr;
                number--;
                //cout<<"Proba: "<<curr->t<<" "<<curr->next->t<<endl;
                delete tmp;
            } else {
            	if (curr == tail && tail->t == t1) {
            		//izbacujemo poslednji
            		Elem* tmp = curr;
            		curr = curr->next;
            		tail = prev;
            		tail->next = 0;
            		prev->next = 0;
            		number--;
            		delete tmp;
            	} else {
            		//izbacujemo neki u sredini
            		Elem* tmp = curr;
            		curr = curr->next;
            		prev->next = curr;
            		number--;
            		delete tmp;
            	}
            }
        } else {
            //ne izbacujemo element
            prev = curr;
            curr = curr->next;
        }
    }
    //cout<<"Proba 2: "<<head->t<<" "<<head->next->t<<" "<<head->next->next->t<<endl;
    lock_flag = 1;
    //return *this;
}

template<class T>
T List<T>::get_element(int index)
{
	lock_flag = 0;
    if (index < 0 || index >= number) {
    	lock_flag = 1;
    	return 0;
	}
	Elem* curr = head;
	for (; index > 0; index--, curr = curr->next);
	lock_flag = 1;
	return curr->t;
}

template<class T>
T List<T>::get() {
	lock_flag = 0;
	if (number == 0) {
		lock_flag = 1;
		return 0;
	}
	T tmp = head->t;
	delete_element(head->t);
	//number--;
	lock_flag = 1;
	return tmp;
}

#endif
