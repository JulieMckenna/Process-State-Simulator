// Process Model with Two Suspend States
// COMP3499 Operating Systems for Engineers
// Simple simulator - includes swapping in and swapping out events.

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define NUM_PROCESSES 21

int blockedpercent;
int numtoswap;
int numprocessesblocked;
int activeprocesses;
static int inmain = 0;
int totalprocesses;
static int blockedinmain = 0;
double blockedper;
int suspendedprocesses = 0;
int blockedprocesses[NUM_PROCESSES];


/*	The PROCESS struct
	Each process has:
	- a name e.g. P0, P12
	- state e.g. Ready, Blocked, etc
	- a valid bit to say whether the process is used in the simulation. 20 processes are created P1 to P20 but not all are used
	- a changed bit. When you change state you can set this to 1. Use this bit to know whether to print an asterisk indicating the process changed */
typedef struct 
{
	char name[4];		//process name
	char state[17];		//state
	int valid;			//is this process being used in the current simulation?
	int changed;		//was state changed?
} PROCESS;

void initializeProcesses(PROCESS [], char []);	//passes array of structs and first line of file to initialize the processes
void printProcesses(PROCESS []);				//prints the valid processes i.e. those used in the current simulation
void updateState(PROCESS [], char []);			//updates the process state
void suspenedprocess(PROCESS[], int num);		//updates blocked process array to indicate that process number is suspeneded
void initialcount(PROCESS[]);					//gets the count of active and blocked processes when their beginning state is read
void unsuspendprocess(PROCESS processes[]);		//unsuspends processes into meain memory

int main( int argc, char* argv[] )
{
	int i;
	char *rch;
	char str[200];
	char tokenizedLine[10][300];
	int lineP;
	int num;
	

	printf("At what percent would you like to start sending blocked processes to be suspended?");
	scanf("%d", &blockedpercent);
	printf("How many processes would you like to swap in or out at a time?(1 or 2)");
	scanf("%d", &numtoswap);

	PROCESS processes[21];	//array of 21 PROCESSES - highest number is P20
	
	FILE *fp;
	fp = fopen ("inp1.txt", "r");
	//you can change this to use command line if you'd like

	lineP = 0;
	i = 0;

	//use the first line to setup initial states
	if (fgets(str, sizeof(str),fp) != NULL)				//fgets reads in the first line with the initial states
	{
		initializeProcesses(processes, str);			//pass that line to the function to initialize processes
		initialcount(processes);						//get the current number of active and blocked processes at the beginning
		printProcesses(processes);						//print current state
	}

	while (fgets(str, sizeof(str),fp) != NULL)		//get the next line in the file (events); fgets in a while loop will read one line at a time
	{
		printf("%s\n", str);						//print the line (as required)
		lineP = 0;
		rch = strtok(str, ":;.");					// use strtok to break up the line by : or . or ; This would separate each line into the different events
													// e.g. the line At time 5:  P3 requests the disk; P7 is dispatched. 
													// has two events "P3 requests the disk" AND "P7 is dispatched"
		while(rch != NULL)
		{
			strcpy(tokenizedLine[lineP], rch);		//copy the events into an array of strings
			lineP++;								//keep track of how many events you need to run
			rch = strtok(NULL, ":;.");				//needed for strtok to continue in the while loop
		}
				
		for(i = 1; i < lineP-1; i++)							
		{
			updateState(processes, tokenizedLine[i]);			//for each event (e.g. P3 requests the disk) call update
		}
		printf("\nAfter executing the line:\n");
		printProcesses(processes);
		printf("Number of blocked processes in main: %d\n", blockedinmain); 
		printf("Total processes in main: %d\n", inmain);
		blockedper = ((double)(blockedinmain) / (double)(inmain)) * 100;
		printf("Percent of blocked processes in main: %lf\n\n\n", blockedper);


		if (blockedpercent > ((double)blockedinmain / (double)(inmain)) * 100) {
			if (numtoswap == 1 && suspendedprocesses >= 1) {
				//swapping one at a time
				unsuspendprocess(processes);
			}
			else if (numtoswap >= 2 && suspendedprocesses >= 2) {
				//swapping 2 at a time
				unsuspendprocess(processes);
				unsuspendprocess(processes);
			}
			//else
				//printf("Not 2 process to swap into memory yet.");

		}
		else if (blockedpercent <= ((double)blockedinmain / (double)(inmain)) * 100) {
			if (numtoswap == 1 ){
				if (numprocessesblocked > 0) {
					for (int i = 0; i < NUM_PROCESSES; i++) {
						if (blockedprocesses[i] == 2) {
							num = i;
						}
					}
					//printf("num to swap %d\n", num);
					suspenedprocess(processes, num);
					blockedprocesses[num] = 3;
				}
			}
			else if (numtoswap ==2){
				//swapping two
				if (numprocessesblocked >= 2){
					for (int j = 0; j < 2; j++) {
						for (int i = 0; i < NUM_PROCESSES; i++) {
							if (blockedprocesses[i] == 2) {
								num = i;
							}
						}
						//printf("suspending num: %d", num);
						suspenedprocess(processes, num);
						blockedprocesses[num] = 3;
					}
				}
				else
					printf("Not 2 process to swap out of memory yet.");
			}
		}


		printf("After swaping:\n");
		printProcesses(processes);								// after making all updates in that line print process states
		printf("Number of ready/running processes in main: %d\n", activeprocesses);
		printf("Number of blocked processes in main: %d\n", blockedinmain);
		printf("Number of processes in main: %d\n", inmain);
		printf("Number of suspended processes: %d\n", suspendedprocesses);
		blockedper = ((double)(blockedinmain) / (double)(inmain))*100;
		printf("Percent of blocked processes in main: %lf\n\n\n", blockedper);
	}

	fclose(fp);
	return 0;
}

//This function uses the first line in the file to initialize the process states
//pass in the array of PROCESSES and the string
void initializeProcesses(PROCESS processes [], char str [])	
{
	int pCount;
	char number[3];
	int num;
	char *pch;
	char *qch;
	char tokenizedLine[40][17];
	int i;
	int lineP;
	lineP = 0;

	//Make everything "NULL"
	for(pCount = 0; pCount < NUM_PROCESSES; pCount++)
	{
		strcpy(processes[pCount].name, "P");
		_itoa(pCount, number, 10);
		strcat(processes[pCount].name, number);
		strcpy(processes[pCount].state, "NONE");
		processes[pCount].valid = 0;
		processes[pCount].changed = 0;
	}

	//use first line from file to set states
	pch = strtok(str, " ");
	while (pch != NULL)
	{
		strcpy(tokenizedLine[lineP], pch);	//tokenize line by spaces and put in array of strings
		lineP++;							//count number of valid elements
		pch = strtok(NULL, " ");
	}
	lineP--;								//don't count "end" indicator at end of first line

	for(i = 0; i < lineP; i+=2)							
	{
		qch = strtok(tokenizedLine[i],"P ");				//separate P from number
		num = atoi(qch);									// turn the character(s) into an int
		strcpy(processes[num].state, tokenizedLine[i+1]);	//set process state
		processes[num].valid = 1;							//this process is part of the current simulation and will be printed
		processes[num].changed = 0;							//keep this 0 since state has not been changed yet
		totalprocesses++;
	}
	printf("Total number of processes: %d\n", totalprocesses);
}

void initialcount(PROCESS processes[])
{
	int i;
	for (i = 0; i < NUM_PROCESSES; i++) {
		if (processes[i].valid == 1) {
			if (strcmp(processes[i].state, "Blocked") == 0) {
				numprocessesblocked++;
				blockedprocesses[i] = 2;
				inmain++;
				blockedinmain++;
			}
			if (strcmp(processes[i].state, "Running") == 0) {
				activeprocesses++;
				blockedprocesses[i] = 1;
				inmain++;
			}
			if (strcmp(processes[i].state, "Ready") == 0) {
				activeprocesses++;
				blockedprocesses[i] = 1;
				inmain++;
			}
			if (strcmp(processes[i].state, "New") == 0)
			{
				activeprocesses++;
				blockedprocesses[i] = 1;
				inmain++;
			}
		}
	}
	printf("Number of active processes: %d\n", activeprocesses);
	printf("Number of blocked processes: %d\n", numprocessesblocked);
}

void printProcesses(PROCESS processes[])
{
	int pCount;
	for (pCount = 0; pCount < NUM_PROCESSES; pCount++)
	{
		if (processes[pCount].valid == 1)		//if process is in this simulation print, ignore all others
		{
			printf("%s %s", processes[pCount].name, processes[pCount].state);	//print name and state
			if (processes[pCount].changed == 1)		//if state has changed since last print
			{
				printf("*");						//print *
				processes[pCount].changed = 0;		//and reset changed
			}
			printf("\t");
		}
	}
	printf("\n\n");
}

void suspenedprocess(PROCESS processes[], int num) {
	if ( numprocessesblocked >= numtoswap) {
		printf("Suspeneding process\n");
		suspendedprocesses++;
		if (strcmp(processes[num].state, "Blocked") == 0)
		{
			strcpy(processes[num].state, "Blocked/Suspended");
			processes[num].changed = 1;
			inmain--;
			blockedinmain--;
		}
		else if (strcmp(processes[num].state, "Ready") == 0)
		{
			strcpy(processes[num].state, "Ready/Suspended");
			processes[num].changed = 1;
			inmain--;
		}
		else
			printf("Error: %s tried to suspend but its not blocked\n", processes[num].name);
	}
}

void unsuspendprocess(PROCESS processes[]) {
	int num = 0;
	//if (blockedpercent > ((double)(numprocessesblocked - suspendedprocesses) / (double)(numprocessesblocked + activeprocesses)) * 100 ) {
		printf("Swapping blocked processes back in\n");
		for (int i = 0; i < NUM_PROCESSES; i++)
		{
			if (blockedprocesses[i] == 3) {
				num = i;
				break;
			}
		}
		//swapped in
		if (strcmp(processes[num].state, "Ready/Suspended") == 0)
		{
			strcpy(processes[num].state, "Ready");
			processes[num].changed = 1;
			blockedprocesses[num] = 1;
			inmain++;
		}
		else if (strcmp(processes[num].state, "Blocked/Suspended") == 0)
		{
			strcpy(processes[num].state, "Blocked");
			processes[num].changed = 1;
			blockedprocesses[num] = 2;
			inmain++;
			blockedinmain++;
		}
		suspendedprocesses--;
	
}


//Updates the state based on the current state and the event
//pass in the array of structs and the event string (e.g. Time slice for P7 expires)
void updateState(PROCESS processes[], char rch[])
{
	char *sch;
	char *tch;
	char tokenizedLine[5][10];
	int lineP;
	int num;
	lineP = 0;

	sch = strtok(rch, " ");
	while(sch != NULL)
	{
		strcpy(tokenizedLine[lineP], sch);		//use strtok to break up each line into separate words and put the words in the array of strings
		lineP++;								//count number of valid elements
		sch = strtok(NULL, " ");
	}

	//Requests I/O: running -> blocked
	if (strcmp(tokenizedLine[1], "requests") == 0)	// if the process requests - needs something
	{
		tch = strtok(tokenizedLine[0], "P");
		num = atoi(tch);

		if (strcmp(processes[num].state, "Running") == 0)
		{
			strcpy(processes[num].state, "Blocked");
			processes[num].changed = 1;
			numprocessesblocked++;
			activeprocesses--;
			blockedprocesses[num] = 2;
			blockedinmain++;
		}
		else 
			printf("Error: %s requested I/O but not Running\n", processes[num].name);
	}
	
	//Dipatched: ready -> running
	if (strcmp(tokenizedLine[2], "dispatched") == 0)	// if the process is dispatched - starts running
	{
		tch = strtok(tokenizedLine[0], "P");
		num = atoi(tch);

		if (strcmp(processes[num].state, "Ready") == 0)
		{
			strcpy(processes[num].state, "Running");
			processes[num].changed = 1;
		}
		else if (strcmp(processes[num].state, "New") == 0)
		{
			strcpy(processes[num].state, "Running");
			processes[num].changed = 1;
		}
		else 
			printf("Error: %s tried to run but its not Ready\n", processes[num].name);
	}

	//Time Slice expired: running -> ready
	if (strcmp(tokenizedLine[0], "Time") == 0 && strcmp(tokenizedLine[1], "slice") == 0)	// if the processes time runs out
	{
		tch = strtok(tokenizedLine[3], "P");
		num = atoi(tch);

		if (strcmp(processes[num].state, "Running") == 0)
		{
			strcpy(processes[num].state, "Ready");
			processes[num].changed = 1;
		}
		else 
			printf("Error: %s time slice ran out but not Running\n", processes[num].name);
	}
	
	//Interrupt Occurred
	//Blocked -> Ready (process is suspended)
	if (strcmp(tokenizedLine[1], "interrupt") == 0) {
		tch = strtok(tokenizedLine[4], "P");
		num = atoi(tch);

			if (strcmp(processes[num].state, "Blocked/Suspended") == 0)
			{
				strcpy(processes[num].state, "Ready/Suspended");
				processes[num].changed = 1;
				numprocessesblocked--;
				activeprocesses++;
			}
			else if (strcmp(processes[num].state, "Blocked") == 0)
			{
				strcpy(processes[num].state, "Ready");
				processes[num].changed = 1;
				numprocessesblocked--;
				activeprocesses++;
				blockedinmain--;
				blockedprocesses[num] = 1;

			}
			else 
				printf("Error: %s tried to change to ready but its not blocked\n", processes[num].name);
	}

	//Terminates: Any state to -> Exit
	if (strcmp(tokenizedLine[2], "terminated") == 0)	// if a program ends
	{
		tch = strtok(tokenizedLine[0], "P");
		num = atoi(tch);
		blockedprocesses[num] = 5;
		inmain--;

		if (strcmp(processes[num].state, "Running") == 0)
		{
			strcpy(processes[num].state, "Exit");
			processes[num].changed = 1;
			activeprocesses--;
		}
		else if (strcmp(processes[num].state, "Ready") == 0)
		{
			strcpy(processes[num].state, "Exit");
			processes[num].changed = 1;
			activeprocesses--;
		}
		else if (strcmp(processes[num].state, "Blocked") == 0)
		{
			strcpy(processes[num].state, "Exit");
			processes[num].changed = 1;
			activeprocesses--;
		}
		else 
			printf("Error: %s tried to terminate but its not running\n", processes[num].name);
	}
}