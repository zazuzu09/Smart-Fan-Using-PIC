#include <math.h>
#include <xc.h>
// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define _XTAL_FREQ 20000000  // tan so thach anh 1MHz
#include"_Library_LCD.h"//khai bao thu vien LCD 
#include"_DHT11.h"

#define UP RB0
#define DOWN RB1
#define MODE RB2
#define START_STOP RB3
#define ENABLE RC7

unsigned int VALUE, CAPTOCDO, nd;
__bit TTDC, st;

void PWM_init(){
    float PWM_freq=1/(0.8*pow(10,-3));      //Xung PWM = 0.8ms
    PR2 = (_XTAL_FREQ/(PWM_freq*4*16)) - 1; 			//Setting the PR2 formulae using Datasheet // Makes the PWM work in 5KHZ
    CCP1M3 = 1; CCP1M2 = 1;  			//Configure the CCP1 module 
    T2CKPS0 = 1;T2CKPS1 = 0; TMR2ON = 1; 			//Configure the Timer module
    TRISC = 0; 					// make port pin on C as output
}

void PWM_Duty(unsigned int duty){
    float PWM_freq=1/(0.8*pow(10,-3));      
	if(duty<1023){
        duty = ((float)duty/1023)*(_XTAL_FREQ/(PWM_freq*16)); 			// On reducing //duty = (((float)duty/1023)*(1/PWM_freq)) / ((1/_XTAL_FREQ) * TMR2PRESCALE);
        CCP1X = duty & 1; 			//Store the 1st bit
        CCP1Y = duty & 2; 			//Store the 0th bit
        CCPR1L = duty>>2;			// Store the remining 8 bit
	}
}

void hienthi_LCD(){
    LCD_Clear();                                    //xoa man hinh
    LCD_PutString("DoAnVDK");
    LCD_Gotoxy(1,0);                                //con tro xuong hang 2 
    LCD_PutString("QuatThongMinh");
}

void check_START_STOP(){
    if(START_STOP==0){
		__delay_ms(10);
		if(START_STOP==0){
			while(START_STOP==0);
            st = ~st;
        }
    }
}

void check_MODE(){
	if(MODE==0){
		__delay_ms(10);
		if(MODE==0){
			while(MODE==0);
			TTDC =~ TTDC;
			if(TTDC==1){
				CAPTOCDO=1;
			}
	    }
	}
}

void check_UP(){
	if(UP==0){
		__delay_ms(10);
		if(UP==0){
			while(UP==0){
				CAPTOCDO++;
				if(CAPTOCDO>10){
					CAPTOCDO=10;
				}
			}
		}
	}
}

void check_DOWN(){
	if(DOWN==0){
		__delay_ms(10);
		if(DOWN==0){
			while(DOWN==0){
				CAPTOCDO--;
				if(CAPTOCDO<1){
					CAPTOCDO=1;
				}
			}
		}
	}
}

void kich_dong_co_quay(){
   if(nd>35){
    	PWM_Duty(1000);      
   }
   if(nd>15 && nd<=35){
      	PWM_Duty(500);          
   }
   if(nd<=15){
      	PWM_Duty(0);        
   }  
}

int main(void){
	TRISD = 0x00;
	TRISC = 0x00;
	TRISB = 0xFF;
	LCD_Init();                                  //khoi tao LCD     
	hienthi_LCD();                                 //hien thi ten 
    PORTC = PORTC & 0xf7;
	PWM_init();
	PORTC=PORTC|0x80;
	VALUE=0;
	TTDC=0;
	CAPTOCDO=0;
    nd=0;
    st=0;
	PWM_Duty(VALUE);
	while(1){         
    	__delay_ms(10);                                   // ~ delay 1s 
    	if(DHT11_start()!=1){                      //giao tiep DHT11 
        	LCD_Clear(); 
        	LCD_PutString("Communica Failed");
    	}              
    	if(Read_DTH11()!=1){                        //doc du lieu 
        	LCD_Clear(); 
        	LCD_PutString("Data Failed");
   		}
        nd=DATA[2];
        hienthi_DHT11();                            //hien thi ket qua
        check_START_STOP();
        if(st==1){
            check_MODE();
            if(TTDC==1){
                check_UP();
                check_DOWN();
            }
            VALUE = CAPTOCDO*100;
            PWM_Duty(VALUE);
            if(TTDC==0){
                kich_dong_co_quay();
            }
        }
        else{
            PWM_Duty(0);
        }
	}
}