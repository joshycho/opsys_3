/*
	PCB class that contains all the methods involving the PCB struct.
	
	@version 1
	4/8/16
	@author Joshua Cho
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PCB.h"
#include "Errors.h"


PCB_p PCB_construct (void) {// returns a pcb pointer to heap allocation
	PCB_p p = (PCB_p) malloc (sizeof(PCB));
	return p;
}

void PCB_destruct (PCB_p p) {  // deallocates pcb from the heap
	if (!p) {
		print_error(1);
	}
	else 
	free(p);
}

int PCB_init (PCB_p p) {      // sets default values for member data
	if (!p) {
		return 1;
	}
	else {
		p->pid = DEFAULT_PID;
		p->state = DEFAULT_STATE;
		p->priority = DEFAULT_PRIORITY;
		p->pc = DEFAULT_PC;
		return 0;
	}
}
int PCB_set_pid (PCB_p p, unsigned long num) {		//sets pid value for the pcb
	if (!p) {
		return 1;
	}
	else if (num < 0) return 2;
	else p->pid = num;
	return 0;
}

unsigned long PCB_get_pid (PCB_p p) {  // returns pid of the process
	if (!p) {
		print_error(1);
	}
	return p->pid;
}

int PCB_set_state (PCB_p p, enum state_type s) {	//sets state for the pcb
	if (!p) {
		return 1;
	}
	else p->state = s;
	return 0;
}

const char* PCB_get_state(PCB_p p) {	//returns char pointer containing state of the pcb
	char *state;
	if (!p) {
		print_error(1);
	}
	switch (p->state) {
      case created: 
		state = "created";
		break;
      case ready: 
		state = "ready";
		break;
	  case running: 
		state = "running";
		break;
	  case interrupted: 
		state = "interrupted";
		break;
	  case waiting: state = 
		"waiting";
		break;
	  case halted: 
	  state = "halted";
   }
   return state;
}

int PCB_set_priority (PCB_p p, unsigned short num) {	//sets priority of the pcb to num
	if (!p) {
		return 1;
	}
	else if (num < 0) return 2;
	else p->priority = num;
	return 0;
}

unsigned short PCB_get_priority (PCB_p p) {		//returns priority of the pcb
	if (!p) {
		print_error(1);
	}
	return p->priority;
}

int PCB_set_pc (PCB_p p, unsigned long num) {	//sets pc of the pcb to num
	if (!p) {
		return 1;
	}
	else p->pc = num;
	return 0;
}

unsigned long PCB_get_pc (PCB_p p) {	//returns pc of the pcb
	if (!p) {
		print_error(1);
	}
	return p->pc;
}

/*
void PCB_toString (PCB_p p, char* str) {  // returns a string representing the contents of the pcb
	char temp[200];
	snprintf(temp, 200, "pid = 0x%lu pc = 0x%lu state = %s\n\n", 
			PCB_get_pid(p), PCB_get_pc(p), PCB_get_state(p));
	strcpy(str, temp);
}
*/

void PCB_toString (PCB_p p) { 
	printf("pid = 0x%lu pc = 0x%lu state = %s\n", 
			PCB_get_pid(p), PCB_get_pc(p), PCB_get_state(p));
}


