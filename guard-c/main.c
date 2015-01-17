/*
*  Author: piFra
*  date: 04/01/2015
*  license: MIT license
*
*  TODO:
*      insert a table of event -> state entries [v]
*      perform a table-lookup inside a loop instead of if-based transitions [v]
*      add a command line argument to create dynamic number of states and events
*      many other small things ;D
*/
#include <stdio.h>
#include <stdlib.h>

#ifdef __gnu_linux__
	#include <unistd.h>
#endif

#ifdef __linux__
	#include <unistd.h>
#endif

// defined for both 32 and 64bit windows environment
#ifdef _WIN32
	#include <Windows.h>
#endif

#define N_OF_STATES 3

// Call "state" a pointer to a function that
// takes an array of void* as argument and returns void
typedef void(*state)(void* arg[]);

typedef int(*event)(void);

typedef struct s_information
{
	int intValue;
	char charValue;
} information;

// our core component
typedef struct s_machine
{
	information info;
	state actState;
} stateMachine;

// possible states
void defaultState(void *arg[]);
void state1(void *arg[]);
void state2(void *arg[]);
void guardState(void *arg[]);

// possible events
// a value != 0 means event occured
int event0(void){ return 255; }
int event1(void){ return 1; }
int event2(void){ return 2; }

// create a element that holds the event and the state associated to the event
typedef struct s_eventState
{
	event ev;
	state st;
} eventState;

// simplify pointers :D
typedef stateMachine* stateMachinePtr;
typedef eventState* eventStatePtr;
typedef information* informationPtr;

// transition to states
void transitionToState(stateMachinePtr instance, state newState, void *arg[]);

// default initialization for states
void setDefaultState(stateMachinePtr instance, state _default, void *arg[]);

// function to copy "information" struct into another
int copyInformation(information source, informationPtr dest);

// function to initialize the machine
int initializeMachine(stateMachinePtr instance, informationPtr newInfo);

int main(void)
{
	stateMachinePtr machine;
	eventState table[N_OF_STATES];
	void *args[N_OF_STATES];
	information info;

	// create a stateMachinePtr
	machine = (stateMachinePtr)malloc(sizeof(machine));
	
	// dummy values
	info.charValue = '7';
	info.intValue = 7;

	if (!initializeMachine(machine, &info))
	{
		perror("Error while initializing machine");
		return -1;
	}

	table[0].ev = event0;
	table[0].st = defaultState;
	table[1].ev = event1;
	table[1].st = state1;
	table[2].ev = event2;
	table[2].st = state2;

	args[0] = machine;
	args[1] = table;

	//setDefaultState(machine, defaultState, args);

	while (1)
	{
		transitionToState(machine, guardState, args);
	}

	return 0;
}


// Functions implementation
void transitionToState(stateMachinePtr instance, state newState, void *arg[])
{
	instance -> actState = newState;
	instance -> actState(arg);
}

void setDefaultState(stateMachinePtr instance, state _default, void *arg[])
{
	transitionToState(instance, _default, arg);
}

void defaultState(void *arg[])
{
	Sleep(1000);
	printf("\n--- Default State ---\n");
}
void state1(void *arg[])
{
	Sleep(1000);
	printf("\n--- State [1] ---\n");
}
void state2(void *arg[])
{
	Sleep(1000);
	printf("\n--- State [2] ---\n");
}

// pseudo-syntax: guardState(stateMachinePtr instance, eventState table);
void guardState(void *arg[])
{
	int cnt = 0;
	stateMachinePtr machine;
	eventStatePtr table;

	//printf("\n--- Guard State ---\n");     

	machine = (stateMachinePtr)arg[0];
	table = (eventStatePtr)arg[1];

	for (; cnt < N_OF_STATES; cnt++)
	{
		// printf("--- table[%d].ev() int value: [%d]", cnt, (int)(table[cnt].ev()));
		// if event occured, change state
		if ((int)(table[cnt].ev()))
		{
			transitionToState(machine, table[cnt].st, arg);
			// machine -> actState(arg);
		}
	}
}

int copyInformation(information source, informationPtr dest)
{
	void *ret = NULL;

	ret = memcpy(dest, &source, sizeof(dest));

	// memcpy failed
	if (ret == NULL)
		return 0;

	return 1;
}

int initializeMachine(stateMachinePtr instance, informationPtr newInfo)
{
	if (instance == NULL)
	{
		perror("instance not initialized, NULL pointer");
		return 0;
	}
		
	// copy failed
	if (!copyInformation(instance->info, newInfo)) return 0;

	return 1;
}
