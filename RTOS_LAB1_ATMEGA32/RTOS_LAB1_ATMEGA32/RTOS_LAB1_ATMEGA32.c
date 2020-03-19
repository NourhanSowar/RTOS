
#define F_CPU 8000000
#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"
#include "lcd_4bit.h"
#include <util/delay.h>

/*HANDLERS OF TASKS */
xTaskHandle xHANDLE1;
xTaskHandle xHANDLE2;


/*tasks codes prototypes */
 void vxtask1 (void*pvParamter);
 void vxtask2 (void*pvParamter);
 
 /*varivables */
 int i ; 
 
int main(void)
{
	
	lcd_init();
	
	xTaskCreate(vxtask1,NULL,85,NULL,2,&xHANDLE1);
	
	xTaskCreate(vxtask2,NULL,85,NULL,1,&xHANDLE2);
	
	vTaskStartScheduler();
		
	return 0;
    
}
//*******************TASKS FUNCTIONS *******************************//
void vxtask1 (void*pvParamter)
{
	for (;;)
	{
		lcd_clrScreen();
		
		for (i =0; i<7 ; i++  )
		{
			//lcd_gotoxy(0,i);
			//lcd_dispString("hello");
			lcd_disp_string_xy("hello" , 0 , i);
			_delay_ms(100);
			
			lcd_clrScreen();			
		}
		vTaskPrioritySet( xHANDLE1 , 1 );
		
		vTaskPrioritySet( xHANDLE2 , 2 );
		
	}
}
void vxtask2 (void*pvParamter)
{
	for (;;)
	{
		lcd_clrScreen();
		
		for (i =0; i<7 ; i++  )
		{
			lcd_disp_string_xy("world" , 1 , 7-i);
			
			_delay_ms(100);
			
			lcd_clrScreen();
		}
		
		vTaskPrioritySet( xHANDLE1 , 2 );
		
		vTaskPrioritySet( xHANDLE2 , 1 );
	}
}
