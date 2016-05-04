
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "RoundRobinScheduler.h"

static unsigned int SysStack = 0;
static int CallsToDispathcer = 0;

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

