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
	
	PCB_p currProcess = PCB_construct();
	PCB_init(currProcess);
	PCB_set_state(currProcess, ready);
	
	
	while (readyQueue->size < 30) {
		int i, j = randomNumber(MIN_NUM_NEW_PCB, MAX_NUM_NEW_PCB);
		for(i = 0; i < j; i++) {
			PCB_p p = PCB_construct();
			PCB_init(p);
			pid++;
			PCB_set_pid(p, pid);
			newProcessQueue = enqueue(newProcessQueue, p);
		}
		NewToReady(newProcessQueue, readyQueue);
		self->pcRegister += randomNumber(MIN_PC_INCREMENT, MAX_PC_INCREMENT);
		SysStack = self->pcRegister;
		currProcess = ISR(currProcess, readyQueue);
		self->pcRegister = SysStack;
	}
	
	while (readyQueue->size > 0) {
		PCB_destruct(dequeue(readyQueue));
	}
	
	queue_destruct(readyQueue);
	queue_destruct(newProcessQueue);
	
}

void NewToReady(Queue_q newQ, Queue_q readyQ) {
	
	while (newQ->size > 0) {
		
		PCB_p temp = dequeue(newQ);
		PCB_set_state(temp, ready);
		enqueue(readyQ, temp);
	}
}

PCB_p ISR(PCB_p currProcess, Queue_q readyQueue) {
	
	PCB_set_state(currProcess, interrupted);
	PCB_set_pc(currProcess, SysStack);
	
	return scheduler(interrupt, currProcess, readyQueue);
}

PCB_p scheduler(enum interrupt_type i, PCB_p currProcess, Queue_q readyQueue) {
	
	switch (i) {
		case timer: 
			break;
		
		case IO:
			break;
			
		case interrupt:
			PCB_set_state(currProcess, ready);
			
			printf("Returned to Ready Queue: ");
			PCB_toString(currProcess);
			
			enqueue(readyQueue, currProcess);
			
			return dispatcher(currProcess, readyQueue);
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
