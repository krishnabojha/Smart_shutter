/*
 * lcd_project.c
 *
 * Created: 7/7/2019 3:41:45 PM
 * Author : krishna
 */ 
/*
 * lcd_project.c
 *
 * Created: 7/7/2019 3:41:45 PM
 * Author : krishna
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "LCD.h"
unsigned char received_value(void);
#include <avr/interrupt.h>
#define  Trigger_pin	PA0	/* Trigger pin */
int TimerOverflow = 0;

unsigned char received_data,a,b,c;
unsigned char msg[15];
unsigned int z,i=0,j=0;
unsigned char cmd_1[]={"ATD 9860103308;"};
ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;	/* Increment Timer Overflow count */
}


int main(void)
{
	
	char string[10];
	long count;
	double distance;
	char data[15]="BUSY";
	// initializing the uart with baud rate 9600
	UCSRA=(0<<RXC) | (0<<TXC) | (0<<UDRE) | (0<<FE) | (0<<DOR) | (0<<PE) | (0<<U2X) | (0<<MPCM);
	UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (1<<RXEN) | (1<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);
	UCSRC=(1<<URSEL) | (0<<UMSEL) | (0<<UPM1) | (0<<UPM0) | (0<<USBS) | (1<<UCSZ1) | (1<<UCSZ0) | (0<<UCPOL);
	UBRRH=0x00;
	UBRRL=0x06;
	DDRD =0x08;
	DDRA = 0x03;		/* Make trigger pin and buzzer as output */
	PORTD = 0x01;		/* Turn on Pull-up */
//	PORTD=0x10;
	sei();			/* Enable global interrupt */
	TIMSK = (1 << TOIE1);	/* Enable Timer1 overflow interrupts */
	TCCR1A = 0;		/* Set all bit to zero Normal operation */

    initialize();
	
	send_command(0x01);
	_delay_ms(2);
	printstring("Welcome...");
	_delay_ms(2000);
	send_command(0x01);
	_delay_ms(2);
	printstring("Initializing GSM");
	_delay_ms(4000);
	PORTD=0b00001000;
    
	while(!(PIND & (1<<PD2))) 
    {
		
		/* Give 10us trigger pulse on trig. pin to HC-SR04 */
		PORTA |= (1 << Trigger_pin);
		_delay_us(10);
		PORTA &= (~(1 << Trigger_pin));
		
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x41;	/* Capture on rising edge, No pre scaler*/
		TIFR = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR = 1<<TOV1;	/* Clear Timer Overflow flag */

		/*Calculate width of Echo by Input Capture (ICP) */
		
		while ((TIFR & (1 << ICF1)) == 0);/* Wait for rising edge */
		TCNT1 = 0;	/* Clear Timer counter */
		TCCR1B = 0x01;	/* Capture on falling edge, No prescaler */
		TIFR = 1<<ICF1;	/* Clear ICP flag (Input Capture flag) */
		TIFR = 1<<TOV1;	/* Clear Timer Overflow flag */
		TimerOverflow = 0;/* Clear Timer overflow count */

		while ((TIFR & (1 << ICF1)) == 0);/* Wait for falling edge */
		count = ICR1 + (65535 * TimerOverflow);	/* Take count */
		/* 1MHz Timer freq, sound speed =343 m/s */
		distance = (double)count / 58.31;

		dtostrf(distance, 2, 2, string);/* distance to string */
		strcat(string, " cm ");	/* Concat unit i.e.cm */
		
		send_command(0x01);
		_delay_ms(2);
		printstring(string);
		_delay_ms(1000);
		send_command(0x01);
		if(distance>=10){
			PORTA =0b00000010;
			_delay_ms(100);
			
			for(z=0;cmd_1[z]!='\0';z++)
			{   while (!(UCSRA & (1<<UDRE)));
				UDR = cmd_1[z];
				_delay_ms(100);
				send_command(0x01);
				
			}
			
			UDR = ('\r');
			_delay_ms(100);
			send_command(0x01);
			_delay_ms(2);
			printstring("Processing call...");
			//_delay_ms(100);
			if(UDR=='+'|UDR=='C'|UDR=='I'|UDR=='E'|UDR=='U'){
				i=2;
				
			}
			if(i==2){
				send_command(0x01);
				_delay_ms(2);
				printstring("sending.......");
				_delay_ms(20);
				PORTA =0b00000000;
				while (!(UCSRA & (1 << RXC)));
				while(UDR!='C');
				while(UDR!='A');
				while(UDR!='R');
				send_command(0x01);
				_delay_ms(2);
				printstring("No CARRIER");
				_delay_ms(20);
				PORTA =0b00000010;
				i=0;
				j=0;
				

			}
			_delay_ms(1000);
			
		}
	}		
		
	
	
	//while(UDR!='n');
	//while(UDR!='a');
	for(int i=0;i<4;i++){
		PORTA=0b00000010;
		_delay_ms(100);
		PORTA=0b00000000;
		_delay_ms(100);
	}
	send_command(0x00);
	_delay_ms(2);
	printstring("USer detected");
	_delay_ms(1000);
}




unsigned char received_value(void)
{
	while(!(UCSRA&(1<<RXC)));
	{
		received_data=UDR;
		return received_data;
	}
}
		


