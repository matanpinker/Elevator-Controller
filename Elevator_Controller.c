//FILE:    Example_2833xGpioToggle.c
//
// TITLE:   DSP2833x Device GPIO toggle test program.
//
// ASSUMPTIONS:
//
//    This program requires the DSP2833x header files.
//
//    ALL OF THE I/O'S TOGGLE IN THIS PROGRAM.  MAKE SURE
//    THIS WILL NOT DAMAGE YOUR HARDWARE BEFORE RUNNING THIS
//    EXAMPLE.
//
//    Monitor desired pins on an oscilloscope.
//
//    As supplied, this project is configured for "boot to SARAM"
//    operation.  The 2833x Boot Mode table is shown below.
//    For information on configuring the boot mode of an eZdsp,
//    please refer to the documentation included with the eZdsp,
//
//       $Boot_Table:
//
//         GPIO87   GPIO86     GPIO85   GPIO84
//          XA15     XA14       XA13     XA12
//           PU       PU         PU       PU
//        ==========================================
//            1        1          1        1    Jump to Flash
//            1        1          1        0    SCI-A boot
//            1        1          0        1    SPI-A boot
//            1        1          0        0    I2C-A boot
//            1        0          1        1    eCAN-A boot
//            1        0          1        0    McBSP-A boot
//            1        0          0        1    Jump to XINTF x16
//            1        0          0        0    Jump to XINTF x32
//            0        1          1        1    Jump to OTP
//            0        1          1        0    Parallel GPIO I/O boot
//            0        1          0        1    Parallel XINTF boot
//            0        1          0        0    Jump to SARAM	    <- "boot to SARAM"
//            0        0          1        1    Branch to check boot mode
//            0        0          1        0    Boot to flash, bypass ADC cal
//            0        0          0        1    Boot to SARAM, bypass ADC cal
//            0        0          0        0    Boot to SCI-A, bypass ADC cal
//                                              Boot_Table_End$
//
// DESCRIPTION:
//
//     Three different examples are included. Select the example
//     (data, set/clear or toggle) to execute before compiling using
//     the #define statements found at the top of the code.
//
//
//     Toggle all of the GPIO PORT pins
//
//    The pins can be observed using Oscilloscope.
//
//
//###########################################################################
// $TI Release: 2833x/2823x Header Files and Peripheral Examples V133 $
// $Release Date: June 8, 2012 $
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include <math.h>
#include <stdio.h>
// Select the example to compile in.  Only one example should be set as 1
// the rest should be set as 0.
#define long_delay  4000000L
#define short_delay 2000000L
static int x = 0;
// Prototype statements for functions found within this file.
void delay_loopN(long);
void Gpio_select(void);
int Get_numkey(void);
void LCD_print(char string[]);
void shabat_mode(int cf, int mode);


// Prototype statements for functions found within this file.
interrupt void xint3456_isr(void);
long GET_KEYCODE(void);
static long temp = 0xA;
volatile static int flag = 0;

//Define for the LCD
#define MAX_DELAY 6800000L
#define MAX_DELAY_floor 750000L
#define LCD_CLEAR 0x01
#define SET_DISPLAY_MODE 0x3C  // 8 bit data with 2 line display and 5x11 dots character format
#define CURSOR_MOVE_R 0x06
#define DISPLAY_ON 0x0F        // display on with blinking cursor
#define NEW_LINE 0xbf
#define CURSOR_OFF 0x0C
#define ENABLE_FALLING_EDGE {GpioDataRegs.GPBSET.bit.GPIO56 = 1; GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1;}
#define floor_1 49;
#define floor_2 50;
#define floor_3 51;
#define floor_4 52;
#define PULLUP 0


static int cf = 1;
static int mode = 1;
//LCD Functions
void delay_loopX(long);
void LCD_INIT(char*, void (*delay_loop)());
void LCD_initial(char string);
//Timer

// Keypad functions
long GET_KEYCODE(void);
void Emergency_case(int sw3);
void start_mode(void);
void up(int current_stage, int next_stage);
void down(int current_stage, int next_stage);
void main(void)
{
	int y, sw2_1 = 0, sw3;

	// Step 1. Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the DSP2833x_SysCtrl.c file.
	InitSysCtrl();

	// Step 2. Initalize GPIO:
	// This example function is found in the DSP2833x_Gpio.c file and
	// illustrates how to set the GPIO to it's default state.
	// InitGpio();  // Skipped for this example


	// Step 4. Initialize all the Device Peripherals:


	// Enable Xint3 to XINT6 in the PIE: Group 1 interrupt 4 & 5
	// Enable int1 which is connected to WAKEINT:



	// GPIO30 & GPIO31 are outputs, start GPIO30 high and GPIO31 low
	InitPieCtrl();





	// Step 3. Clear all interrupts and initialize PIE vector table:
	// Disable CPU interrupts
	DINT;

	// Initialize PIE control registers to their default state.
	// The default state is all PIE interrupts disabled and flags
	// are cleared.
	// This function is found in the DSP2833x_PieCtrl.c file.


	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;

	// Initialize the PIE vector table with pointers to the shell Interrupt
	// Service Routines (ISR).
	// This will populate the entire table, even if the interrupt
	// is not used in this example.  This is useful for debug purposes.
	// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
	// This function is found in DSP2833x_PieVect.c.
	InitPieVectTable();

	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	EALLOW;	// This is needed to write to EALLOW protected registers
	PieVectTable.XINT3 = &xint3456_isr;
	PieVectTable.XINT4 = &xint3456_isr;
	PieVectTable.XINT5 = &xint3456_isr;
	PieVectTable.XINT6 = &xint3456_isr;
	EDIS;   // This is needed to disable write to EALLOW protected registers


	// Enable Xint3 to XINT6 in the PIE: Group 1 interrupt 4 & 5
	// Enable int1 which is connected to WAKEINT:
	PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
	PieCtrlRegs.PIEIER12.bit.INTx1 = 1;          // Enable PIE Gropu 1 INT3  /////???????????????????
	PieCtrlRegs.PIEIER12.bit.INTx2 = 1;          // Enable PIE Gropu 1 INT4
	PieCtrlRegs.PIEIER12.bit.INTx3 = 1;          // Enable PIE Gropu 1 INT5
	PieCtrlRegs.PIEIER12.bit.INTx4 = 1;          // Enable PIE Gropu 1 INT6
 // Enable CPU int12
	IER |= M_INT12;
	EINT;                                       // Enable Global Interrupts

 // GPIO30 & GPIO31 are outputs, start GPIO30 high and GPIO31 low
	Gpio_select();

	// GPIO0 is XINT1, GPIO1 is XINT2
	EALLOW;
	GpioIntRegs.GPIOXINT3SEL.bit.GPIOSEL = 12;   // Xint3 is GPI44
	GpioIntRegs.GPIOXINT4SEL.bit.GPIOSEL = 13;   // XINT4 is GPI45
	GpioIntRegs.GPIOXINT5SEL.bit.GPIOSEL = 14;   // Xint5 is GPI46
	GpioIntRegs.GPIOXINT6SEL.bit.GPIOSEL = 15;   // XINT6 is GPI47
	EDIS;

	// Configure XINT
	XIntruptRegs.XINT3CR.bit.POLARITY = 0;      // Falling edge interrupt
	XIntruptRegs.XINT4CR.bit.POLARITY = 0;      // Falling edge interrupt
	XIntruptRegs.XINT5CR.bit.POLARITY = 0;      // Falling edge interrupt
	XIntruptRegs.XINT6CR.bit.POLARITY = 0;      // Falling edge interrupt

 // Enable XINT3 to XINT6
	XIntruptRegs.XINT3CR.bit.ENABLE = 1;        // Enable Xint1
	XIntruptRegs.XINT4CR.bit.ENABLE = 1;        // Enable XINT2
	XIntruptRegs.XINT5CR.bit.ENABLE = 1;        // Enable Xint3
	XIntruptRegs.XINT6CR.bit.ENABLE = 1;        // Enable XINT4



// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP2833x_InitPeripherals.c
// InitPeripherals(); // Not required for this example

// Step 5. User specific code:


	int i;
	int keyask;
	int cuf;
	double nef_temp;
	int nef;

	// This example uses DATA registers to toggle I/O's
	GpioDataRegs.GPADAT.bit.GPIO27 = 0; // buzzer off
	start_mode();
	while (1)
	{
		char mess[] = "You are at       Floor Already  ";
		y = GpioDataRegs.GPADAT.all;
		// sw2_1=(y>>10) & 0x1; //Shabat switch
		sw3 = (y >> 10) & 0x3; //Emergency switch
		if (cf == 4)
			mode = 0;
		if (cf == 1)
			mode = 1;
		switch (sw3)
		{
		case 0:
		{
			keyask = Get_numkey();
			cuf = keyask / 10;
			nef_temp = keyask % 10;
			nef = nef_temp;
			if (keyask != 0)
			{
				if (cf > cuf)
				{
					down(cf, cuf);
					cf = cuf;
				}

				if (cf < cuf)
				{
					up(cf, cuf);
					cf = cuf;
				}
				if (nef > cuf)
				{

					up(cuf, nef);
					cf = nef;
				}
				if (nef < cuf)
				{
					down(cuf, nef);
					cf = nef;
				}
				else if (nef == cuf)
				{

					mess[12] = 48 + cuf;
					LCD_print(mess);
				}
			}
		}
		break;

		case 1:
		{
			LCD_print(" *** Shabat ***                 ");
			if (mode == 1)
			{
				up(cf, cf + 1);
				cf++;
			}
			else
			{
				down(cf, cf - 1);
				cf--;
			}
		}
		break;
		default: Emergency_case(sw3);
			break;
		}

	}

}

void start_mode(void)
{
	GpioDataRegs.GPCDAT.all = 4;
	LCD_print("                                ");
	LCD_print("Matan Pinker &   Abed Jabaly     ");
	LCD_print("Project Summer   2022  Afeka    ");
	LCD_print("Elevator at      floor: 1       ");
}


void Emergency_case(int sw3)
{
	LCD_print("Emergency mode!                 ");
	GpioDataRegs.GPASET.bit.GPIO27 = 1;  //Buzzer=ON
	delay_loopX(4 * MAX_DELAY);
	GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;  //Buzzer=OFF
}



void delay_loopX(long x)
{
	long i;
	for (i = 0; i < x; i++) {}
}




void LCD_INIT(char* init_set, void (*delay_loop)())
{
	const int size = (sizeof init_set) / (sizeof(char));
	int i;
	EALLOW;
	for (i = 0; i < size; i++)
	{
		GpioDataRegs.GPBDAT.all = (long)init_set[i] << 16;   // Instruction send to LCD
		GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;    // LCD RS bit = 0, Instruction Select
		ENABLE_FALLING_EDGE
			delay_loopX(MAX_DELAY);		               // Delay Function
	}
	EDIS;
}





void delay_loopN(long x)
{
	while (x--);
}

void Gpio_select(void)
{
	EALLOW;
	GpioCtrlRegs.GPBMUX1.all = 0x00000000;  //  GPIO 31-0
	GpioCtrlRegs.GPBMUX2.all = 0x00000000;  //  GPIO 63-48
	GpioCtrlRegs.GPBDIR.all = 0xFBFF0F00;   // bits 63-48 as outputs, 58-input
	GpioCtrlRegs.GPADIR.bit.GPIO27 = 1;     // for buzzer
	GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;   // buzzer off
	GpioCtrlRegs.GPAMUX1.all = 0x00000000;  // All GPIOA
	GpioCtrlRegs.GPAMUX2.all = 0x00000000;  // All GPIOA
//       	GpioCtrlRegs.GPADIR.all = 0x00000000;
	GpioCtrlRegs.GPCMUX1.all = 0x00000000;  // All GPIOC
	GpioCtrlRegs.GPCMUX2.all = 0x00000000;  // All GPIOC
	GpioCtrlRegs.GPCDIR.all = 0x0000000F;   // All outputs 4 Leds
	GpioCtrlRegs.GPBPUD.bit.GPIO44 = PULLUP;
	GpioCtrlRegs.GPBPUD.bit.GPIO45 = PULLUP;
	GpioCtrlRegs.GPBPUD.bit.GPIO46 = PULLUP;
	GpioCtrlRegs.GPBPUD.bit.GPIO47 = PULLUP;

	EDIS;
}

int Get_numkey(void)
{
	int numkey;
	GpioDataRegs.GPBDAT.all = 0;
	if (flag)
	{
		temp = GET_KEYCODE();
		flag = 0;

		switch (temp)
		{
		case 0xEE: numkey = 11;
			break;
		case 0xED: numkey = 21;
			break;
		case 0xEB: numkey = 31;
			break;
		case 0xE7: numkey = 41;
			break;
		case 0xDE: numkey = 12;
			break;
		case 0xDD: numkey = 22;
			break;
		case 0xDB: numkey = 32;
			break;
		case 0xD7: numkey = 42;
			break;
		case 0xBE: numkey = 13;
			break;
		case 0xBD: numkey = 23;
			break;
		case 0xBB: numkey = 33;
			break;
		case 0xB7: numkey = 43;
			break;
		case 0x7E: numkey = 14;
			break;
		case 0x7D: numkey = 24;
			break;
		case 0x7B: numkey = 34;
			break;
		case 0x77: numkey = 44;
			break;
		default: numkey = 0;
			break;
		}
		return numkey;
	}
}

void shabat_mode(int cf, int mode)
{
	LCD_print(" *** Shabat ***                 ");
	if (mode == 1)
	{
		up(cf, cf + 1);
		cf++;
	}
	else
	{
		down(cf, cf - 1);
		cf--;
	}

}


void LCD_print(char string[])
{

	char control_set[] = { LCD_CLEAR,SET_DISPLAY_MODE,CURSOR_MOVE_R,DISPLAY_ON };	// Control Words To Boot Up LCD
	int i;

	LCD_INIT(control_set, &delay_loopX);
	for (i = 0; i < 31; i++)		// All 32 Characters to send to LCD
	{
		if (i == 16)
		{
			GpioDataRegs.GPBDAT.all = (long)NEW_LINE << 16;  // Move cursor to next line start
			GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;    // LCD RS bit = 0, Instruction Select
			ENABLE_FALLING_EDGE
				delay_loopX(MAX_DELAY);		            // Delay Function
		}

		GpioDataRegs.GPBDAT.all = (long)string[i] << 16;
		GpioDataRegs.GPBSET.bit.GPIO57 = 1;       // LCD RS bit = 1, Data Select
		ENABLE_FALLING_EDGE
			delay_loopX(MAX_DELAY_floor);		               //Delay Function

	}

	GpioDataRegs.GPBDAT.all = (long)CURSOR_OFF << 16;
	GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;  	 // LCD RS bit = 0, Instruction Select
	ENABLE_FALLING_EDGE
		delay_loopX(MAX_DELAY);		             // Delay Function
}




void up(int current_stage, int next_stage)
{
	int i;
	GpioDataRegs.GPCDAT.all = 8;
	delay_loopX(MAX_DELAY); // 2s delay
	GpioDataRegs.GPCDAT.all = 9;
	char st[] = "  Elevator Up      Floor   ->     ";
	st[25] = 48 + current_stage;
	st[30] = 48 + next_stage;
	LCD_print(st);
	int temp = next_stage - current_stage;
	//if(temp>1)
	//{
	for (i = current_stage; i < next_stage; i++)
	{
		char st1[] = "  Elevator Up     Floor:         ";
		st1[25] = 49 + i;
		LCD_print(st1);
	}
	//		temp=1;
	//	}
	//	else
	//	{
	GpioDataRegs.GPASET.bit.GPIO27 = 1;  //Buzzer=ON
	delay_loopX(MAX_DELAY); // 2s delay
	GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;  //Buzzer=OFF
	GpioDataRegs.GPCDAT.all = 4;
	delay_loopX(MAX_DELAY); // 2s delay
	delay_loopX(MAX_DELAY); // 2s delay
//	}
}

void down(int current_stage, int next_stage)

{
	int i;
	GpioDataRegs.GPCDAT.all = 8;
	delay_loopX(MAX_DELAY); // 2s delay
	GpioDataRegs.GPCDAT.all = 10;
	char st[] = "  Elevator Down   Floor   ->     ";
	st[24] = 48 + current_stage;
	st[29] = 48 + next_stage;
	LCD_print(st);
	int temp = current_stage - next_stage;
	//		if(temp>1)
	//		{
	for (i = current_stage; i > next_stage; i--)
	{
		char st1[] = "  Elevator Down   Floor:         ";
		st1[25] = 47 + i;
		LCD_print(st1);
	}
	//			temp=1;
	//		}
	//		else
	//		{
	GpioDataRegs.GPASET.bit.GPIO27 = 1;  //Buzzer=ON
	delay_loopX(MAX_DELAY); // 2s delay
	GpioDataRegs.GPACLEAR.bit.GPIO27 = 1;  //Buzzer=OFF
	GpioDataRegs.GPCDAT.all = 4;
	delay_loopX(MAX_DELAY); // 2s delay
	delay_loopX(MAX_DELAY); // 2s delay
//		}

}





interrupt void xint3456_isr(void)
{
	flag = 1;
	// Acknowledge this interrupt to get more from group 12
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;
}


long GET_KEYCODE(void)
{
	long active_row[4] = { 0x0E,0x0D,0x0B,0x07 };
	long keycode, row = 3, column = 0x0F;
	while (column == 0x0F)
	{
		row = (row + 1) % 4;
		GpioDataRegs.GPBDAT.all = active_row[row] << 8;
		DELAY_US(1000);
		column = (GpioDataRegs.GPBDAT.all >> 12) & 0x0F;
	}
	return keycode = ((active_row[row] << 4) | column);
}

