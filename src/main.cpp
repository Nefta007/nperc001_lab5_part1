#include "timerISR.h"
#include "helper.h"
#include "periph.h"
#include "serialATmega.h"

//TODO: declare variables for cross-task communication

/* You have 5 tasks to implement for this lab */
#define NUM_TASKS 2


//Task struct for concurrent synchSMs implmentations
typedef struct _task{
	signed 	 char state; 		//Task's current state
	unsigned long period; 		//Task period
	unsigned long elapsedTime; 	//Time elapsed since last task tick
	int (*TickFct)(int); 		//Task tick function
} task;


//TODO: Define Periods for each task
// e.g. const unsined long TASK1_PERIOD = <PERIOD>
const unsigned long Sonar_Period = 1000;
const unsigned long display_Period = 1000;
const unsigned long GCD_PERIOD = 500; /* TODO: Calulate GCD of tasks */

task tasks[NUM_TASKS]; // declared task array with 5 tasks

//TODO: Define, for each task:
// (1) enums and
// (2) tick functions
enum Sonar_states {sonar_off, sonar_on};
int TickFtn_Sonar(int state);

/////task 2
// enum Display_states {display_off, display_on};
// int TickFtn_Display(int state);

void TimerISR() {
    
    //TODO: sample inputs here

	for ( unsigned int i = 0; i < NUM_TASKS; i++ ) {                   // Iterate through each task in the task array
		if ( tasks[i].elapsedTime == tasks[i].period ) {           // Check if the task is ready to tick
			tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
			tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
		}
		tasks[i].elapsedTime += GCD_PERIOD;                        // Increment the elapsed time by GCD_PERIOD
	}
}


int main(void) {
    //TODO: initialize all your inputs and ouputs
      //  Output: DDR = 1, PORT = 0
  //  Input: DDR = 0, PORT = 1
  DDRC = 0b111100; PORTC = 0b000011;
  DDRB = 0b111110; PORTB = 0b000001;
  DDRD = 0b11111111; PORTD = 0b00000000;
//   serial_init(9600);

    ADC_init();   // initializes ADC
    init_sonar(); // initializes sonar
    // serial_println(read_sonar());

    //TODO: Initialize tasks here
    // e.g. tasks[0].period = TASK1_PERIOD
    // tasks[0].state = ...
    // tasks[0].timeElapsed = ...
    // tasks[0].TickFct = &task1_tick_function;
    unsigned char i  = 0;
    tasks[i].state = sonar_off;
    tasks[i].period = Sonar_Period;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &TickFtn_Sonar;
    // i++;
    // tasks[i].state = display_off;
    // tasks[i].period = display_Period;
    // tasks[i].elapsedTime = tasks[i].period;
    // tasks[i].TickFct = &TickFtn_Display;

    TimerSet(GCD_PERIOD);
    TimerOn();

    while (1) {}

    return 0;
}

//enum Sonar_states {sonar_off, sonar_on, sonar_echo};
int TickFtn_Sonar(int state){
    switch (state)
    {
    case sonar_off:
        //(PINC >> 2) == 1;
        state = sonar_on;
        break;
    
    case sonar_on:
        if((PINC >> 2) == 1){
            serial_println(read_sonar());
        }
        else{
            state = sonar_off;
        }
        break;

    default:
        break;
    }
    return state;
}