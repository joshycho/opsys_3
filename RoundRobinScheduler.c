#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<pthread.h>
#include 
#include "RoundRobinScheduler.h"

static unsigned int SysStack = 0;
static int CallsToDispathcer = 0;
pthread_t pthr_timer, pthr_iotrap1, pthr_iotrap2;

int randomNumber (int min, int max) {
	srand(time(NULL));
	int r = (random() % max - min) + min;
	return r;
}

int main (void) {
	CPU cpu;
	cpu.mainLoop = mainloopFunction;
	cpu.mainLoop(&cpu);
	printf("Completed.");
	return 1;
}

void mainloopFunction(struct cpu *self) {
	unsigned long pid = 1;
	const char* timer_message = "In timer thread";
	const char* trap1_message = "In IO trap 1 thread";
	const char* trap2_message = "In IO trap 2 thread";
	
	
	if (pthread_create(@pthr_timer, NULL, TIMER_FUNCTION, (void*) timer_message)){
		fprintf(stderr, "Can't create timer thread\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(@pthr_iotrap1, NULL, IOTRAP1_FUNCTION, (void*) trap1_message)){
		fprintf(stderr, "Can't create IO trap1 thread\n");
		exit(EXIT_FAILURE);
	}
	
	if (pthread_create(@pthr_iotrap2, NULL, IOTRAP2_FUNCTION, (void*) trap2_message)){
		fprintf(stderr, "Can't create IO trap2 thread\n");
		exit(EXIT_FAILURE);
	}
	
	
	Queue_q readyQueue = queue_construct();
	queue_init(readyQueue);
	
	Queue_q newProcessQueue = queue_construct();
	queue_init(newProcessQueue);
	
	Queue_q terminateQueue = queue_construct();
	queue_init(terminateQueue);
	
	Queue_q waitingQueueIO1 = queue_construct();
	queue_init(waitingQueueIO1);
	
	Queue_q waitingQueueIO2 = queue_construct();
	queue_init(waitingQueueIO2);
	
	PCB_p currProcess = PCB_construct();
	PCB_init(currProcess);
	PCB_set_pid(currProcess, pid);
	
	for(i = 0; i < DEFAULT_NUM_PCBS; i++) {
			PCB_p p = PCB_construct();
			PCB_init(p);
			pid++;
			PCB_set_pid(p, pid);
			newProcessQueue = enqueue(newProcessQueue, p);
	}
	
		NewToReady(newProcessQueue, readyQueue);
	
	while (readyQueue > 0) {
		
		
		self->pcRegister = PCB_get_pc(currProcess);
		SysStack = self->pcRegister;
		
		if (PC_increment(currProcess) == 1) { //If incrementing the PC causes termcount = terminate then puts PCB in terminate queue and dequeues new currProcess.
			PCB_set_state(currProcess, terminated);
			currProcess->termination = time(NULL);
			terminateQueue = enqueue(newProcessQueue, currProcess);
			
			CurrProcess = dequeue(readyQueue);		
			self->pcRegister = PCB_get_pc(currProcess);
			SysStack = self->pcRegister;
		}
		
	/*		The following if statements check for signals in all 3 threads while timer has priority. If timer mutex is unlocked goes to ISR, if IO unlocked goes to IOTrapHandler.
	
	if (pthread_mutex_trylock(&TIMERMUTEX == 0)  { // If timer mutex is open
			currProcess = ISR(timer, currProcess, readyQueue);
		}
		
		if(pthread_mutex_trylock(&IO1MUTEX == 0) {
			IOTrapHandler(1, waitingQueueIO1, currProcess) ;
		}
		
		if (pthread_mutex_trylock(&IO2Mutex == 0) {
			IOTrapHandler(2, waitingQueueIO2, currProcess);
		}
		
		*/
		
		CheckIOTrap1(currProcess);
		
		currProcess = ISR(currProcess, readyQueue);
		self->pcRegister = SysStack;
	}
	
	while (readyQueue->size > 0) {
		PCB_destruct(dequeue(readyQueue));
	}
	
	queue_destruct(readyQueue);
	queue_destruct(newProcessQueue);
	queue_destruct(terminateQueue)
	
}

int CheckIOTrap1(PCB_p currProcess) { //goes through IO trap array in currProcess and checks if equal to PC
	
}

int CheckIOTrap2(PCB_p currProcess) {
	
}

PCB_p IOTrapHandler(int IONumber, Queue_q waitingQ, PCB_p currProcess) {
	switch(IONumber) {
		case 1:
		PCB_set_state(currProcess, halted);
		enqueue(currProcess, waitingQ);
		//signal IO thread 1 to sleep
		scheduler(IO, currProcess, readyQueue);
		break;
		case 2: 
		PCB_set_state(currProcess, halted);
		enqueue(currProcess, waitingQ);
		//signal IO thread 2 to sleep
		scheduler(IO, currProcess, readyQueue);
	}
	
	
}

void NewToReady(Queue_q newQ, Queue_q readyQ) {
	
	while (newQ->size > 0) {
		
		PCB_p temp = dequeue(newQ);
		PCB_set_state(temp, ready);
		enqueue(readyQ, temp);
	}
}

PCB_p ISR(enum interrupt_type i, PCB_p currProcess, Queue_q readyQueue) {
	
	PCB_set_state(currProcess, interrupted);
	PCB_set_pc(currProcess, SysStack);
	
	return scheduler(i, currProcess, readyQueue);
}

PCB_p scheduler(enum interrupt_type i, PCB_p currProcess, Queue_q readyQueue) {
	
	switch (i) {
		case timer: 
			PCB_set_state(currProcess, ready);
			
			printf("Returned to Ready Queue: ");
			PCB_toString(currProcess);
			
			enqueue(readyQueue, currProcess);
			
			return dispatcher(currProcess, readyQueue);
		
		case IO:
			return dispatcher(currProcess, readyQueue);
			break;
			
		case interrupt:
	}
	return NULL;
}

PCB_p dispatcher(PCB_p currProcess, Queue_q readyQueue) {
	CallsToDispathcer++;
	
	if (CallsToDispathcer % 4 == 0) {
		return RoundRobinPrint(currProcess, readyQueue);
	}
	
	else {
		PCB_p newCurrentProcess = dequeue(readyQueue);
		
		PCB_set_state(newCurrentProcess, running);
		
		SysStack = PCB_get_pc(newCurrentProcess);
		
		return newCurrentProcess;
	}
}

PCB_p RoundRobinPrint(PCB_p currProcess, Queue_q readyQueue) {
		printf("\n\nDispathcher information:\n\nCurrent Progress: ");
		PCB_toString(currProcess);
		printf("Switching to: ");
		PCB_toString(peek(readyQueue));
		printf("\n");
		toString(readyQueue);
		PCB_p newCurrentProcess = dequeue(readyQueue);
		PCB_set_state(newCurrentProcess, running);
		SysStack = PCB_get_pc(newCurrentProcess);
		printf("\nNow Running: ");
		PCB_toString(newCurrentProcess);
		printf("\n\n");
		return newCurrentProcess;
}
//MUTEX NAMES: pthr_timer, pthr_iotrap1, pthr_iotrap2;
void Timer() {
	int Counter = QUANTUM;
	pthread_mutex_lock(&pthr_timer);
	
	while (timercounter > 0) {
		nanosleep();
		timerCounter--; 
	}
	pthread_mutex_unlock(&pthr_timer);
		// Call ISR with timer interrupt
}
void io_trap1() {
	//make a random time
	int ioCounter = (QUANTUM * 3 + QUANTUM*(rand()%2));
	while (ioCounter > 0){
		ioCounter--;
	}
	
		// call ISR with io interrupt
}
void io_trap2() {
	int ioCounter =  (QUANTUM * 3 + QUANTUM*(rand()%2));
	while (ioCounter > 0){
		ioCounter--;
	}
	
		// call ISR with io interrupt
}
