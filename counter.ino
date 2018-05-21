#include "Arduino.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <string>
#include <bitset>
#include <sstream>
#include "WString.h"
#include <cmath>
#include <algorithm>
#include <sstream>



// Define Arduino I/O Pins
int Y0 = 53;
int Y1 = 51;
int Y2 = 49;
int Y3 = 47;
int Y4 = 45;
int Y5 = 43;
int Y6 = 41;
int Y7 = 39;
int CCLR = 37;
int GAL = 29;
int GAU = 27;
int GBL = 25;
int GBU = 23;
int RCLK = 31;
int CLKBEN = 22;

//Initialize variables
int LowerA[8] = {0};
int UpperA[8] = {0};
int LowerB[8] = {0};
int UpperB[8] = {0};
int i;
int reg;
int dt = 0;
unsigned int acount;
int bcount;


std::vector<int> reg_byte(8);
std::vector<int> LowA(8);
std::vector<int> UpA(8);
std::vector<int> LowB(8);
std::vector<int> UpB(8);
std::vector<int> count(32);
std::vector<int> byte_vals(6);
std::vector<int> byteAB(32);





void setup() {

  Serial.begin(9600);

  // Define input pins
  pinMode(Y0, INPUT);    // Y0  Data Bit 0
  pinMode(Y1, INPUT);    // Y1  Data Bit 1
  pinMode(Y2, INPUT);    // Y2  Data Bit 2
  pinMode(Y3, INPUT);    // Y3  Data Bit 3
  pinMode(Y4, INPUT);    // Y4  Data Bit 4
  pinMode(Y5, INPUT);    // Y5  Data Bit 5
  pinMode(Y6, INPUT);    // Y6  Data Bit 6
  pinMode(Y7, INPUT);    // Y7  Data Bit 7

  // Define output pins
  pinMode(CCLR, OUTPUT);     // CCLR  Clears All Registers (hold low for > 20 ns)
  pinMode(GAL, OUTPUT);     // GAL  Lower byte in A storage (hold low for Y)
  pinMode(GAU, OUTPUT);     // GAU  Upper byte in A storage (hold low for Y)
  pinMode(GBL, OUTPUT);     // GBL  Lower byte in B storage (hold low for Y)
  pinMode(GBU, OUTPUT);     // GBU  Upper byte in B storage (hold low for Y)
  pinMode(RCLK, OUTPUT);
  pinMode(CLKBEN, OUTPUT);

  //Set initial state
  digitalWrite(CCLR, HIGH);
  digitalWrite(GAL, HIGH);
  digitalWrite(GAU, HIGH);
  digitalWrite(GBL, HIGH);
  digitalWrite(GBU, HIGH);
  digitalWrite(RCLK, LOW);
  digitalWrite(CLKBEN, LOW);

}


//Data retrieval from 16-bit counter IC
void getData(int reg, std::vector<int> & v){
    
    //Transfer byte to output bus
    digitalWrite(reg, LOW);
    delayMicroseconds(30);

    //Read byte
    int b1 = digitalRead(Y0);
    v[7] = b1;
    int b2 = digitalRead(Y1);
    v[6] = b2;
    int b3 = digitalRead(Y2);
    v[5] = b3;
    int b4 = digitalRead(Y3);
    v[4] = b4;
    int b5 = digitalRead(Y4);
    v[3] = b5;
    int b6 = digitalRead(Y5);
    v[2] = b6;
    int b7 = digitalRead(Y6);
    v[1] = b7;
    int b8 = digitalRead(Y7);
    v[0] = b8;
	
    //Release byte from output bus
    digitalWrite(reg, HIGH);
    delayMicroseconds(30);
    

}

std::string comb_bytes(std::vector<int> &upA, std::vector<int> &lowA, std::vector<int> &upB,
		                std::vector<int> &lowB) {
	int num = 32;

	std::vector<int> comb(num);

	for (int i = 0; i < num; i++) {
		if (i < 8) {
			comb[i] = upB[i];
		}
		else if (i >= 8 && i < 16) {
			comb[i] = lowB[i-8];
		}
		else if (i >= 16 && i < 24) {
			comb[i] = upA[i-16];
		}
		else {
			comb[i] = lowA[i-24];
		}
	}

	std::ostringstream ss;
	for (std::vector<int>::iterator it = comb.begin(); it!=comb.end(); it++) {
		ss << *it;
	}
    	std::string byte1 = ss.str();

	return byte1;
}

void print_byte(std::vector<int> &b) {
	for (size_t i = 0; i < b.size(); i++) {
		Serial.print(b[i]);
	}
	Serial.println();
}

// the loop routine runs over and over again forever:
void loop() {

  dt += 5;
  
  //Read Lower A Byte
  getData(GAL, reg_byte);
  LowA = reg_byte;
  //Read Upper A Byte
  getData(GAU, reg_byte);
  UpA = reg_byte;
  //Read Lower B Byte
  getData(GBL, reg_byte);
  LowB = reg_byte;
  //Read Upper B Byte
  getData(GBU, reg_byte);
  UpB = reg_byte;

  //Combine bytes
  std::string byte1 = comb_bytes(UpA, LowA, UpB, LowB);
  char * end;
  const char * b1 = byte1.c_str();
  //Convert to integer
  long int val = std::strtol(b1,&end,2);

  //Print to console
  Serial.println("=========== Time (sec) ===========");
  Serial.println(dt);
  Serial.print("GAL: ");
  print_byte(LowA);
  Serial.print("GaU: ");
  print_byte(UpA);
  Serial.print("GBL: ");
  print_byte(LowB);
  Serial.print("GBU: ");
  print_byte(UpB);


  Serial.println();
  Serial.print("Byte: ");
  Serial.println(b1);
  Serial.print("Count: ");
  Serial.println(val);
  Serial.println();

  Serial.println("===================================");
  Serial.println();
  
  //Delay
  delay(5000);

}
