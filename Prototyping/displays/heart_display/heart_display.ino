//﻿//*************************************************************************
////----------------1.开发环境:Arduino IDE or Visual Studio 2010----------------
////----------------2.使用开发板型号：Arduino UNO          ----------------
////----------------3.单片机使用晶振：16M		            ----------------
////----------------4.店铺名称：大学生电子商铺/小强电子设计  ----------------
////----------------5.淘宝网址：Ilovemcu.taobao.com		 ----------------
////----------------	    52dpj.taobao.com                ----------------
////----------------6.作者：神秘藏宝室			    ----------------
////***********************************************************************
#include <Arduino.h>

//IO配置
#define LEDARRAY_D 2
#define LEDARRAY_C 3
#define LEDARRAY_B 4
#define LEDARRAY_A 5
#define LEDARRAY_G 6
#define LEDARRAY_DI 7
#define LEDARRAY_CLK 8
#define LEDARRAY_LAT 9
#define KEY_Left 10
#define KEY_Right 11

#define led 13


#define Num_Word 1				//液晶能显示的汉字个数

#define DURATION 400

unsigned char Display_Buffer[2];
unsigned char Display_Swap_Buffer[Num_Word][32]={0};					//显示缓冲区
bool Shift_Bit = 0;
bool Flag_Shift = 0;
bool win = 1;
bool firstTime = true;
unsigned char Timer0_Count = 0;
unsigned char temp = 0x80;
unsigned char Shift_Count = 0;
unsigned int randomDot = 0;
unsigned int playerShift = 7;
int points = -1;
unsigned int lives = 5;
unsigned char Word[1][32] = 
{

0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF/*"强",1*/
};

unsigned char Player[1][32] = 
{

0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF/*"强",1*/
};

unsigned char Win_Screen[1][32] = {
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

unsigned char Full[1][32] = {0};

unsigned char Blank_Screen[1][32] =
{
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF

};

unsigned char livesScreen[6][1][32]=
{

// Broken Heart

{0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xF0, 0xB8, 0x8D, 0xC7, 0xE2, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0x0F, 0x07, 0x03, 0x03, 0x03, 0x07, 0x4F, 0xDF, 0xBF, 0x2F, 0x07, 0x03, 0x03, 0x03, 0x07, 0x0F																																																
},


// 1


{0xFF, 0xFF, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0xC0, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0x0F, 0x07, 0x03, 0x03, 0x03, 0x07, 0x0F, 0x0F, 0x07, 0x03, 0x03, 0x03, 0x07, 0x0F, 0xFF																								
},



// 2


{0xF3, 0xE1, 0xC1, 0x83, 0x83, 0xC1, 0xE1, 0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xE1, 0xC1, 0x83, 0x83, 0xC1, 0xE1, 0xF3																																																
},


// 3

{0xE7, 0xC3, 0x87, 0xC3, 0xE7, 0xFE, 0xFC, 0xF8, 0xF8, 0xFC, 0xFE, 0xE7, 0xC3, 0x87, 0xC3, 0xE7, 0xFF, 0xFF, 0xC7, 0x83, 0x01, 0x01, 0x03, 0x07, 0x07, 0x03, 0x01, 0x01, 0x83, 0xC7, 0xFF, 0xFF																																																
},

// 4
{0xFF, 0xE7, 0xC3, 0x87, 0xC3, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3, 0x87, 0xC3, 0xE7, 0xFF, 0xFF, 0xE7, 0xC3, 0x87, 0xC3, 0xE7, 0xFF, 0xFF, 0xFF, 0xFF, 0xE7, 0xC3, 0x87, 0xC3, 0xE7, 0xFF																								
																								
},

// 5
{0xE7, 0xC3, 0x87, 0xC3, 0xE6, 0xFC, 0xF8, 0xF0, 0xF0, 0xF8, 0xFC, 0xE6, 0xC3, 0x87, 0xC3, 0xE7, 0xE7, 0xC3, 0x87, 0xC3, 0x67, 0x3F, 0x3F, 0x7F, 0x7F, 0x3F, 0x3F, 0x67, 0xC3, 0x87, 0xC3, 0xE7																								
}

};







const unsigned char blank = 0xFF;
const unsigned char oneDot = 0xFE;
int gameSpeed = 35;





void setup()
{
	pinMode(LEDARRAY_D, OUTPUT); 
	pinMode(LEDARRAY_C, OUTPUT);
	pinMode(LEDARRAY_B, OUTPUT);
	pinMode(LEDARRAY_A, OUTPUT);
	pinMode(LEDARRAY_G, OUTPUT);
	pinMode(LEDARRAY_DI, OUTPUT);
	pinMode(LEDARRAY_CLK, OUTPUT);
	pinMode(LEDARRAY_LAT, OUTPUT);
  pinMode(KEY_Right, INPUT);
	pinMode(KEY_Left, INPUT);
  Serial.begin(9600);

  randomSeed(analogRead(0));
  randomDot = random(16);
  Word[0][randomDot] = oneDot;
  
  Shift_Count = 0;
	Clear_Display();
}

void loop()
{ 
  unsigned int i;
  // while(1) {
  //   Display(Blank_Screen);
  // }
  // for(i = 0; i < 32; i++) {
  //   Player[0][i] = blank;
  // }
  
  // Serial.print(Shift_Count);
  // Serial.print("\n");
  
  

  // if(flipDir) {
  //   Player[0][15-playerShift] = 0x7F;    
  // }
  // else {
  //   Player[0][playerShift] = 0x7F;
  // }
      
  if(firstTime) {
      for(i = 0 ; i < DURATION; i ++)			//Movement speed setting
        {
          Display(livesScreen[5]);
        }   
    firstTime = false;
  }
  
  
  Display_Swap_Buffer[0][playerShift] = (Display_Swap_Buffer[0][playerShift] & Player[0][playerShift]);

  int oneDotCount = 0;
  if(Shift_Count == 7) {
    for(int i = 0; i < 16; i++) {
      if(Display_Swap_Buffer[0][i] == 0x7F) {
        oneDotCount++;
      }
    }
    if(oneDotCount == 1) {
      points++;
      if(points%8 == 0) {
        gameSpeed-=5;        
      }
      if(gameSpeed <=15) {
        gameSpeed = 15;
      }
      Serial.print("Points: ");
      Serial.print(points);
      Serial.print("\n");
    }
    else {
      lives--;
      Serial.print("Lives Remaining: ");
      Serial.print(lives);
      Serial.print("\n");

      for(i = 0 ; (i < DURATION); i ++)			//Movement speed setting
      {
          Display(livesScreen[lives]);
      } 
    }

    if(lives == 0) {
      Serial.print("Game Over\n");
      for(i = 0 ; i < 200; i ++)			//Movement speed setting
      {
        Display(Full);
      }
      while(1) {
        Display(Blank_Screen);
      }
    }
  }
  



 

 

  // if(Display_Swap_Buffer[0][playerShift] == 0x7F) {
  //   Display(Full);
  // }

	for(i = 0 ; i < gameSpeed; i ++)			//Movement speed setting
	{
		Display(Display_Swap_Buffer);
	}


  
  Scan_Key();
	Calc_Shift();

	Shift_Count++;
  



	if(Shift_Count == 16)
	{ 
    Word[0][randomDot] = blank;
    // Serial.print(playerShift);
    // Serial.print("\n");    
    randomDot = random(16);
    Word[0][randomDot] = oneDot; 
		Shift_Count = 0;    
	}



}

//************************************************************
//清空缓冲区
//************************************************************
void Clear_Display()
{
	unsigned char i,j;
	for(j = 0 ; j < Num_Word; j++)
	{
		for(i = 0 ; i < 32 ;i++)
		{
			Display_Swap_Buffer[j][i] = 0xff;				//0=显示  1=不显示 	
		}
	}
}

void Scan_Key()
{ 
	if(digitalRead(KEY_Right) == 0)
	{
    // Serial.print("Right Key Pressed\n");
    Player[0][playerShift] = blank;
    playerShift--;
    if(playerShift == -1) 
    {
      playerShift = 15;
    }

    Player[0][playerShift] = 0x7F;	
	}

	if(digitalRead(KEY_Left) == 0)
	{
    // Serial.print("Left Key Pressed\n");
    Player[0][playerShift] = blank;
    playerShift++;
    if(playerShift == 16) 
    {
      playerShift = 0;
    }

    Player[0][playerShift] = 0x7F;
	}
}

//************************************************************
//计算移动数据，存在在缓冲器
//************************************************************
void Calc_Shift() // Calculates how the word should shift (Display_Swap_Buffer dictates what ends up on LED Array) 
{
	unsigned char i;
		for(i = 0;i < 16;i++)
		{	// First if else ensures that an LED gets transferred over the 'gap' between row i and i+16 
			if((Display_Swap_Buffer[0][16+i]&0x80) == 0)									//if left bit==0
			{
				Display_Swap_Buffer[0][i] = (Display_Swap_Buffer[0][i] << 1)&0xfe; 			//light the light below on next display buffer
			}
			else
			{
				Display_Swap_Buffer[0][i] = (Display_Swap_Buffer[0][i] << 1)|0x01;			//or with 0x01, keeps all the bits, excpet for the rightmosts bit (turns off rightmost bit)
			}
			// temp is analyzing each position of either the first or second half of the word. This is where the actual shifting happens
			if(Shift_Count < 8) // If we are still in the first 8 shifts, we are looking at the 'front' side of the word. See if the leading position (frontmost) should be switched on
			{
				Shift_Bit = Word[0][i]&temp;
			}
			else if(Shift_Count < 16) // If we are in the between 8 and 16 shifts, we are looking at the 'latter' side of the word. See if the leading position (frontmost) should be switched on
			{
				Shift_Bit = Word[0][16+i]&temp;
			}
			else // The entire word has been displayed on screen, now it will begin to shift off the screen and loop back from the other side - force shift bit to 1
			{
				Shift_Bit = 1;						//move word out of buffer
			}

			if( Shift_Bit == 0)		// NO new LED should be lit up or 'enter' on this iteration 												//last 8 bit shift
			{
				Display_Swap_Buffer[0][16+i] = (Display_Swap_Buffer[0][16+i] << 1)&0xfe; 		//The least significant bit is cleared
			}
			else // // New LED SHOULD be lit up or 'enter' on this iteration 
			{
				Shift_Bit =	1;
				Display_Swap_Buffer[0][16+i] = (Display_Swap_Buffer[0][16+i] << 1)|0x01;		//lowest position one			
			}
			
		}
		temp = (temp>>1)&0x7f; // Temp is shifted to analyze next LED position of the word
		if(temp == 0x00)
		{
			temp = 0x80; // Reset back to analyze first position
		}



    // for(i = 0 ; i < 16 ;i++)
		// { 	
		// }


    
}

//************************************************************
//num为字数  dat[][32]为字模的名称
//*************************************************************
void Display(unsigned char dat[][32])					
{
	unsigned char i;
  // Serial.print(playerShift);
  // Serial.print("\n");
	for( i = 0 ; i < 16 ; i++ )
	{
		digitalWrite(LEDARRAY_G, HIGH);		//更新数据时候关闭显示。等更新完数据，打开138显示行。防止重影。
		// Split display buffer into 2 arrays -> Display_Buffer[0] contains bottom half, 
		//										 Display_Buffer[1] contains top half
		Display_Buffer[0] = dat[0][i];		
		Display_Buffer[1] = dat[0][i+16];
		// Send Data to actually be written
		Send(Display_Buffer[1]);
		Send(Display_Buffer[0]);

		digitalWrite(LEDARRAY_LAT, HIGH);					//锁存数据
		delayMicroseconds(1);
	
		digitalWrite(LEDARRAY_LAT, LOW);
		delayMicroseconds(1);

		Scan_Line(i);						// Select row i

		digitalWrite(LEDARRAY_G, LOW);
		
		delayMicroseconds(100);;		//延时一段时间，让LED亮起来。				
	}	
}

//****************************************************
//扫描某一行
//****************************************************
void Scan_Line( unsigned char m)
{	
	switch(m)
	{
		case 0:			
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW); 					
			break;
		case 1:					
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 2:					
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 3:					
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 4:
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 5:
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 6:
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 7:
			digitalWrite(LEDARRAY_D, LOW);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 8:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 9:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH); 		
			break;	
		case 10:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 11:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, LOW);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 12:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 13:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, LOW);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		case 14:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, LOW); 		
			break;
		case 15:
			digitalWrite(LEDARRAY_D, HIGH);digitalWrite(LEDARRAY_C, HIGH);digitalWrite(LEDARRAY_B, HIGH);digitalWrite(LEDARRAY_A, HIGH); 		
			break;
		default : break;	
	}
}

//****************************************************
//发送数据
//****************************************************
void Send( unsigned char dat)
{
	unsigned char i;
	digitalWrite(LEDARRAY_CLK, LOW);
	delayMicroseconds(1);;	
	digitalWrite(LEDARRAY_LAT, LOW);
	delayMicroseconds(1);;

	for( i = 0 ; i < 8 ; i++ ) // For all 8 bits in dat - XXXX XXXX, write on Array
	{
		if( dat&0x01 ) // Writing right most bit (0000 0001)
		{
			digitalWrite(LEDARRAY_DI, HIGH);	
		}
		else
		{
			digitalWrite(LEDARRAY_DI, LOW);
		}


		digitalWrite(LEDARRAY_CLK, HIGH);				//上升沿发送数据
			delayMicroseconds(1);;
		digitalWrite(LEDARRAY_CLK, LOW);
			delayMicroseconds(1);;		
		dat >>= 1; // Shift the 8 bits right once, to write next bit
			
	}			
}


