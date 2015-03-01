/*
	Compiler :      CCS C - 16F876
	Programmer :    Matthieu Gouin
	Date :          2003-07-06
	Filename :      template.c
	Description :   A template used to create new programs.
*/


/* -------------------- PRE-PROCESSOR DIRECTIVES -------------------- */
#case					// Case-sensitive compiler
#include <16F876.H>			// General library

// High Speed, NO WatchDog Timer, PowerUp Timer, 
// NO code PROTECTion, NO Low Voltage Programmation.
#fuses HS,NOWDT,PUT,NOPROTECT,NOLVP	
#use delay(clock=20000000)		// 20 MHz Clock


// Serial Port #1 using RS-232 with DB-9 connector
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, bits=8, parity=N, errors)

/* -------------------- PINS DEFINITION -------------------- */
#define LED PIN_A5
#define BUTTON PIN_B0



/* -------------------- FLASH FUNCTION -------------------- */
/*
	Used at the beginning of the program to test it 
	by flashing the LED x times.
*/
void flash_led(int x)
{
	int i;
	for (i=0;i<x;i++)
	{
		output_high(LED);
		delay_ms(50);
		output_low(LED);
		delay_ms(50);
	}
}


/* -------------------- MAIN FUNCTION -------------------- */
void main(void)
{
	flash_led(5);		// flash the onboard led 5 times


	while(1)
	// read the button and output the value to the LED. (on / off)
		output_bit(LED, input(BUTTON));


}




