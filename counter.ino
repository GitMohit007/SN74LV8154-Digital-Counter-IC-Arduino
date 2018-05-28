#include "Arduino.h"

//Flag for counting mode
//Set to True for two 16 bit counters
//Set to False for a single 32 bit counter
int dual = 1;

int freq = 5000;      //Frequency to read data in ms
int dt = 0;

//************************************************
//SN54HC165 Parallel to Serial Shift Register Pins
//************************************************

//SH/LD -- Pin 1 on IC
int latchPin = 51;    //Set high to latch parallel data

//QH -- Pin 9 on IC
int dataPin = 49;     //Serial Output from IC

//CLK -- Pin 2 on IC
int clkPin = 53;      //Rising edge shifts bits

//CLKINH -- Pin 15 on IC (optional)
//  int clk_inhPin = 29;  //Set high to inhibit clocking

//*************************************************
//SN74LV8154 Dual 16 bit Counters
//*************************************************

//CCLR -- Pin 11 on IC
int CCLR = 31;        //Set low to clear register

//GAL -- Pin 3 on IC
int GAL = 29;         //Gate A Lower Byte

//GAU -- Pin 4 on IC
int GAU = 27;		  //Gate A Upper Byte

//GBL -- Pin 5 on IC
int GBL = 25;		  //Gate B Lower Byte

//GBU -- Pin 6 on IC
int GBU = 23;         //Gate B Upper Byte

//Initialize byte variables
byte LowA = 0;
byte HighA = 0;
byte LowB = 0;
byte HighB = 0;
uint16_t CNT_A = 0;
uint16_t CNT_B = 0;
uint32_t CNT_TOT = 0;


byte shiftIn(int data_pin, int clk_pin, int latch_pin, int gate) {
	//Function retrieves byte from 'gate' on SN74LV8154
	//Transfers byte onto shift register of SN54HC165
	//Arduino then reads shift register and returns byte
	byte data_in = 0;
	int i;
	byte temp = 0;

	//Select gate on counter IC
	digitalWrite(gate, LOW);
	delayMicroseconds(2);

	//Transfer parallel data from counter IC to shift register
	digitalWrite(latch_pin, LOW);
	delayMicroseconds(20);
	digitalWrite(latch_pin, HIGH);
	delayMicroseconds(20);

	for (i=0; i<8; i++) {
	//IMPORTANT! First bit on shift register does not have to be clocked!
		if (i == 0) {
			temp = digitalRead(data_pin);

			data_in = data_in | temp;
		}
		else {
			digitalWrite(clk_pin, HIGH);
			delayMicroseconds(10);
			digitalWrite(clk_pin, LOW);
			delayMicroseconds(10);
			temp = digitalRead(data_pin);

			data_in = (data_in << 1) | temp;
		}

	}

	/*For Debugging
	Serial.println();
	Serial.print("Gate: ");
	Serial.print(gate);
	Serial.print("  ");
	Serial.print("SR Byte: ");
	Serial.print(data_in);
	Serial.print("  ");
	Serial.println();
	Serial.println();
	digitalWrite(gate, HIGH);
	delayMicroseconds(0.2);*/

	return data_in;
}



void setup()
{
	Serial.begin(9600);
	//Set pin modes
	pinMode(latchPin, OUTPUT);
	pinMode(dataPin, INPUT);
	pinMode(clkPin, OUTPUT);
	pinMode(GAL, OUTPUT);
	pinMode(GAU, OUTPUT);
	pinMode(GBL, OUTPUT);
	pinMode(GBU, OUTPUT);
	pinMode(CCLR, OUTPUT);
	//pinMode(clk_inhPin, OUTPUT);

	//Set initial pin values
	digitalWrite(latchPin, HIGH);
	digitalWrite(clkPin, LOW);
	digitalWrite(GAL, HIGH);
	digitalWrite(GAU, HIGH);
	digitalWrite(GBL, HIGH);
	digitalWrite(GBU, HIGH);
	digitalWrite(CCLR, HIGH);
	//digitalWrite(clk_inhPin, HIGH);

}


void loop()
{

	//Get Bytes from Counter
	LowA = shiftIn(dataPin, clkPin, latchPin, GAL);
	HighA = shiftIn(dataPin, clkPin, latchPin, GAU);
	LowB = shiftIn(dataPin, clkPin, latchPin, GBL);
	HighB = shiftIn(dataPin, clkPin, latchPin, GBU);
	byte all_bytes[] = {HighB, LowB, HighA, LowA};

	Serial.print("Time: ");
	Serial.println(dt);

	if (dual) {
		CNT_A = (HighA << 8) | LowA;
		CNT_B = (HighB << 8) | LowB;
		Serial.print("Counter A: ");
		Serial.println(CNT_A);
		Serial.print("Counter B: ");
		Serial.println(CNT_B);
		Serial.println();
	}
	else {
		for (int i=0; i<4; i++) {
			CNT_TOT = CNT_TOT + all_bytes[i];
			CNT_TOT <<= 8;
		}

	}
		CNT_TOT >>= 8;
	    Serial.print("Count: ");
		Serial.println(CNT_TOT);
		Serial.println();


	dt += freq/1000;
	delay(freq);
	CNT_TOT = 0;

}
