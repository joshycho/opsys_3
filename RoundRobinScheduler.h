
#include "queue.h"

#define MIN_NUM_NEW_PCB 0
#define MAX_NUM_NEW_PCB 5
#define MIN_PC_INCREMENT 3000
#define MAX_PC_INCREMENT 4000

enum interrupt_type {timer, IO, interrupt};	

typedef struct cpu {
	unsigned int pcRegister;
	void(*mainLoop) (struct cpu *);
}CPU;

void mainloopFunction(struct cpu *);
int randomNumber (int, int); 
void NewToReady(Queue_q, Queue_q);
PCB_p ISR(PCB_p, Queue_q);
PCB_p scheduler(enum interrupt_type, PCB_p, Queue_q);
PCB_p dispatcher(PCB_p, Queue_q);
PCB_p RoundRobinPrint(PCB_p, Queue_q);