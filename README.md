# Process-State-Simulator
Will read an input file and show running, ready, suspended processes 
For my Operating Systems Class

Objective:
The objective of this lab is to use a fully working simulator to read an input file and
automatically suspend or unsuspend processes. This code will read the user’s input for the
percentage of blocked processes and once that percentage is hit will start automatically start
swapping out processes into secondary memory. The program will also read in the user’s input
to decide if it will swap in or out 1 or 2 processes at a time. As processes become ready and the
total percent of blocked processes decreases the program will start automatically swapping
processes back in

Assumptions:
1. The text file has each line beginning with “At time (number):”.
2. If there are multiple events occurring at a time the events are separated by a
semicolon(;) and the end of all the events is signified with a period(.).
3. The user will be prompted to enter a value for the blocked percentage.
4. The user will be prompted to enter a value for the number of processes being swapped
at once(either 1 or 2).
5. The way the processes are being swapped in can be completely random.
6. The code that was created for this purpose works as expected.
7. Since the bus can only swap once process at a time when two processes are being
swapped one must multiply the time it takes to swap one by 2 to get the time to swap 2
processes.

