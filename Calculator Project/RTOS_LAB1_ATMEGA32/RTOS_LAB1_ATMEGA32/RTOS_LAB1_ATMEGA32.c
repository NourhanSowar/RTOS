/*includes files*/
#define F_CPU 8000000
#include "avr/io.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "util/delay.h"
#include "lcd_4bit.h"

/*prototypes*/
void  InitPWM();
void  LcdFlash(void);
void  LcdWelcome(void );
void LCD_Display_KP(char x);
char  KEYPAD_Scan(void);

void  PeriodicTask(void * pvParameters );
void  OperationTask(void * pvParameters);
void  BlinkingPatternTask(void * pvParameters);
void SwitchTask(void * pvParameters );

 /*define handles */
xTaskHandle   xHANDLE1 ;
xTaskHandle   xHANDLE2 ;
xTaskHandle   xHANDLE3 ;
xSemaphoreHandle sem;

/* Define  Global Variables*/ 
int x = 0;
char pressed_number = 0;
int i = 0 , x ,j =8 ;
long  o1=0 ,o2=0 ,o3=0;
char check =0 ,ch1=0;
char  op=0 ;
char result ;

int main( void )
{
	lcd_init();
	
	/* portA for Keypad */
	DDRA = 0x0F;
	PORTA= 0xFF; 
	
	/*PORTC for switches*/
	DDRC  = 0x0F;
	PORTC = 0xFE; //Pull up resistors for inputs and Set outputs to High
	
	/* creating Tasks */
	xTaskCreate( PeriodicTask, "Task2_Name", 100, (void*)1,3,&xHANDLE1);
	xTaskCreate( BlinkingPatternTask, "Task1_Name", 200, NULL,1,&xHANDLE2);	
    //xTaskCreate( OperationTask , "Task_Name", 200, NULL, 2, &xHANDLE3);	
	//xTaskCreate( SwitchTask, "Task1_Name", 100, NULL, 2, &xHANDLE1);

	/* Start the scheduler. */
	vTaskStartScheduler();
		return 0;
}
 
 /* ================Architecture layer======================= */
 void PeriodicTask(void * pvParameters )
 {
	 long paramter ;
	 paramter = (long) pvParameters ;
	 portTickType LastTime = xTaskGetTickCount();
	 char counter = 1;
	 if (paramter == 1)
	 {
		 while (1)
		 {
			 for (counter ; counter<=24 ; counter++) // 24 is to repeat 3 times right to left
			 {
				 
				 vTaskDelayUntil(&LastTime ,62.5);  // 62.5 is 1000 /16 where 8 is a number of digits on screen
				 LcdWelcome();
				 i++;
				 
				 if (i==8)
				 {
					 i=0;
				 }
				 if (counter==24)
				 {
					 lcd_clrScreen();
					 LcdFlash();
					
					 vTaskDelay(10 );
					 
					 //if ( (xTaskGetTickCount() - LastTime)<=10000 )
					 //{ 
						 /*wait for 10 seconds then blink is on */
						 
					 //}
					 
					// lcd_disp_string_xy("start" , 0,0); //error !
				 }	
			 }
		 } 
	 } 
	 vTaskDelete(NULL);
 }
 
 
void BlinkingPatternTask(void * pvParameters )
{
	InitPWM();
	portTickType LastTimeCounter = xTaskGetTickCount();
	int duty = 0 ;
	int counter1 = 0 , counter2 =0 , counter3=0 ,counter4 ;
	
	
	while(1)
	{
		/* first  rising Stage */
		 for (counter1 ; counter1<=154; counter1++) //154 is number to get 250 ms through to proteus run time  ,I notice that 250ms equals 4.17*10^3
		 { 
			  for(duty;duty<50;)
			  {
				OCR2 = duty ;
				duty = duty+10 ; 	 
			  }
			  if (counter1==154)
			  {
				  OCR2 = 255;				  
			  }
			  /*high stage*/
			  OCR2 =255;
			  _delay_ms(250);
			/*falling edge*/
			  for (counter2; counter2<=154; counter2++ )
			  {
				  for(duty=255;duty>0;)
				  {
					  OCR2 = duty ;
					  duty = duty-10 ;
				  }
			  }
			 
			  if (counter2>=154)
			  {
				 OCR2 =0;	 
			  }
			 /*low stage*/
			  OCR2 =0;
			 _delay_ms(250);
			  while (1)
			  {
				  PORTD = 0x00;
			  }
		  }
	}
	vTaskDelete(NULL);
}
 
void OperationTask(void * pvParameters)
{ 
	while(1)
	{
		 op=0,ch1=0, o1=0,o2=0,o3=0 ,check =0;
		 /*get first operand and operator*/
		 while( ( check!='+')&(check!='-')&(check!='*')&(check!='/') )
		 {
			check = KEYPAD_Scan();
			
			ch1 = check;
			if ( (check!='N')&(check!='=')&(check!='+')&(check!='-')&(check!='*')&(check!='/'))
			{
				LCD_Display_KP(check);
				o1 =((o1*10)+(ch1-0x30));
			} 
			else 
			{
				op  =check ;
				LCD_Display_KP(check);
				break;
			}
		}
		check =0 , ch1 =0 ;
		/*get second operand */
		while( ( check!='+')&(check!='-')&(check!='*')&(check!='/') )
		{
			check = KEYPAD_Scan();
			ch1 = check;
			
			if ( (check!='N')&(check!='=')&(check!='+')&(check!='-')&(check!='*')&(check!='/'))
			{
				LCD_Display_KP(check);
				o2 = (o2*10)+(ch1-0x30);
			}
			else
			{
				break;
			}
		}
		/* operations */
		switch (op)
		{
		case '+':
		{
			 o3 = o1+o2;	
			LCD_Display_KP('a');
			_delay_ms(10);
			lcd_displayChar((o3%10)+0x30);
			o3 = o3/10 ;
			break;
		}
		case '-':
		{
			o3= o1-o2 ;
			LCD_Display_KP('s');
			_delay_ms(10);
			lcd_displayChar((o3%10)+0x30);
			o3 = o3/10 ;	
			break;
		}
		case '*':
		{
			o3= o1*o2 ;
			LCD_Display_KP('m');
			_delay_ms(10);
			lcd_displayChar((o3%10)+0x30);
			o3 = o3/10 ;
			break;
		}
		case '/':
		{
			o3= o1-o2 ;
			LCD_Display_KP('d');
			_delay_ms(10);
			lcd_displayChar((o3%10)+0x30);
			o3 = o3/10 ;
			break;	
		}
		}
		check = KEYPAD_Scan();
		_delay_ms(10);
	 }
	 vTaskDelete(NULL);
 }
 
 void SwitchTask(void * pvParameters )
 {
	 
	 portTickType LastTime = xTaskGetTickCount();
	 while (1)
	 {
		 char ret = 0;
		 static char flag = 0;
		 ret = (PINC&(1<<1)) ;
		 if(ret == 0)
		 {
			 if(flag == 0)   // pressed and hold the switch
			 {
				 if (( xTaskGetTickCount() - LastTime ) <= 3000)
				 {
					 /*last num is cleard */
					 lcd_displayChar('1');
				 }
				 else if (( xTaskGetTickCount() - LastTime ) > 3000)
				 {
					 /*clear lcd*/
					 lcd_displayChar('2');
				 }
				 
				 
				 flag = 1;
			 }
		 }else
		 {
			 flag = 0;
			 //Do Nothing
		 }
		 
	 }
	 
 }

 


 
 // ----------------programmer layer --------------//

 void LcdWelcome(void )
 {
	 lcd_clrScreen();
	 lcd_disp_string_xy("Welcome" , 0 , i);
 }
 void LcdFlash(void)
 {
	 lcd_clrScreen();
	 lcd_disp_string_xy("press any key " , 0,0);
	 vTaskDelay(500);
	 lcd_clrScreen();
	 vTaskDelay(250);
 }
 
 void InitPWM()
 {
	 DDRD |= 0xFF ; 
	 TCCR2 |=(1<<WGM00)|(1<<WGM01)|(1<<COM01)|(1<<CS00);
 }
 
 void LCD_Display_KP(char x)
 {
	if(x != 0)
	{
		lcd_displayChar(x);
	}else
	{
		//Do Nothing
	}	
}

 char KEYPAD_Scan(void)
 {
	 char i = 0, j = 0, flag = 0, new = 0;
	 static char old = 0;
	 
	 for(i = 0; i < 4; i++)
	 { 
		 PORTA |= 0x0F;
		 PORTA &= ~(1<<i);

		 for(j = 1; j <4; j++)
		 {
			 if (( ~PINA & (1<<(7)) ) == (1<<(7)) && i==0  )
			 {
				 new = '/';
				 flag = 1;
				 break;
				 
			 }
			 else if (( ~PINA & (1<<(6)) ) == (1<<(6)) && i==0  )
			 {
				 new = '*';
				 flag = 1;
				 break;				 
			 }
			 else if (( ~PINA & (1<<(5)) ) == (1<<(5)) && i==0  )
			 {
				 new = '-';
				 flag = 1;
				 break;
			 }
			 else if (( ~PINA & (1<<(4)) ) == (1<<(4)) && i==0  )
			 {
				 new = '+';
				 flag = 1;
				 break;
			 }
			 else if (( ~PINA & (1<<(4)) ) == (1<<(4)) && i==1)
			 {
				 new = 'N';
				 flag = 1;
				 break;
				 
			 }
			 else if (( ~PINA & (1<<(4)) ) == (1<<(4)) && i==3)
			 {
				 new = '=';
				 flag = 1;
				 break;
				 
			 }
			 
			 
			 if( ( ~PINA & (1<<(j+4)) ) == (1<<(j+4)) )
			 { //For buttons : 1->9
				 new = (j-1)*3 + i + 48;
				 flag = 1;
				 break;
				 
			 }
			 else if(( ~PINA & (1<<4) ) == (1<<4) && i == 2)
			 { //For buttons : 0
				 new = '0';
				 flag = 1;
				 break;
			 }	 
		 }
		 if(flag == 1) break;
	 }
	 
	 if(flag == 1)
	 { //Button Pressed
		 if(old != new)
		 {
			 old = new;
		 }
		 else
		 {
			 new = 0;
		 }
	 }
	 else
	 { //Button Released
		 old = 0;
		 new = 0;
	 }
	 
	 pressed_number = new;
	
	return pressed_number;
 }


