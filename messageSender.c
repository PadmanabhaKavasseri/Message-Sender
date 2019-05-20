/********************************************
 *
 *  Name: Padmanabha Kavasseri
 *  Email: kavasser@usc.edu
 *  Section: Friday 12:30
 *  Assignment: Lab 9 
 *
 ********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "lcd.h"
#include "adc.h"

// Serial communications functions and variables
void serial_init(unsigned short);
void serial_stringout(char *s);
void serial_txchar(char);

#define FOSC 16000000           
#define BAUD 9600               
#define MYUBRR (FOSC/16/BAUD-1) 

// ADC functions and variables
void adc_init(unsigned char);

#define ADC_CHAN 0              //ADC channel 0

volatile int flag = 0;
volatile char buff[17];
volatile int i = 0;

char *messages[] = {
    "Hello           ",
    "What's up       ",
    "Fight On        ",
    "Where's Karen   ",
    "I love C        ",
    "Illedelph       ",
    "Gucci Gang      ",
    "CECS            ",
    "Tom Brady =GOAT ",
    "RIP Mac Miller  "
};


int main(void) {

    // Initialize the modules and LCD
    lcd_init();
    adc_init(ADC_CHAN);
    serial_init(MYUBRR);

    // Enable interrupts
    UCSR0B |= (1 << RXCIE0); 
    sei();                   

    // Splash screen
    lcd_writecommand(1);
    lcd_moveto(0, 0);
    lcd_stringout("Lab 9 ");
    lcd_moveto(1, 0);
    lcd_stringout("Padmanabha Kavasseri");
    _delay_ms(1000);
    lcd_writecommand(1);

    // intialize adc_result
    unsigned char adc_result;

    int x = 0;

    while (1) {                 // Loop forever

        // Get an ADC sample
        adc_result = adc_sample();

        // is the Up button pressed?
        if (adc_result > 31 && adc_result < 71)
        {

            //debouncing code
             _delay_ms(200);
            adc_result = adc_sample();
            while (adc_result > 31 && adc_result < 71) 
            {
                adc_result = adc_sample();
            }
            _delay_ms(5);

            //clears the screen
            lcd_writecommand(1);

            //output
            x++;
            if (x == 10)
            {
                x = 0;
                lcd_stringout((messages[x]));
            }
            lcd_stringout((messages[x]));
        }

        // Down button pressed?
        if (adc_result < 121 && adc_result > 81)
        {
            //debouncing code
             _delay_ms(200);
            adc_result = adc_sample();
            while (adc_result < 121 && adc_result > 81) 
            {
                adc_result = adc_sample();
            }
            _delay_ms(5);

            //clear screen
            lcd_writecommand(1);

            //output 
            x--;
            if (x == -1)
            {
                x = 9;
                lcd_stringout((messages[x]));
            }
            lcd_stringout((messages[x]));
        }

        // Select button pressed?
        if (adc_result > 200 && adc_result < 210)
        {
            //debouncing code
             _delay_ms(200);
            adc_result = adc_sample();
            while (adc_result > 200 && adc_result < 210) 
            {
                adc_result = adc_sample();
            }
            _delay_ms(5);

            //call serial_stringout
            serial_stringout((messages[x]));
        }



        // Message received from remote device?
        if (flag == 1)
        {
            //display output on second line of arduino LCD
            //lcd_writecommand(1);
            lcd_moveto(1,0);
            lcd_stringout(buff);
            flag=0;
        }
    }
}

/* ----------------------------------------------------------------------- */

void serial_init(unsigned short ubrr_value)
{
    // Set up USART0 registers
    // Enable tri-state
    //Initialization of the serial port (USART0)
    UBRR0 = ubrr_value;                       
    UCSR0C = (3 << UCSZ00);               
                                          
    UCSR0B |= (1 << TXEN0 | 1 << RXEN0);  
    DDRD |= (1 << PD3);                  
    PORTD &= ~(1 << PD3);               
}

void serial_txchar(char ch)
{
    
    while ((UCSR0A & (1<<UDRE0)) == 0){};
    UDR0 = ch;
}

void serial_stringout(char *s)
{
    
    int i=0;
    for (i = 0; i < 17; i++)
    {
       serial_txchar(s[i]); 
    }
}

ISR(USART_RX_vect)
{
    char y;

    y = UDR0;
    
    if (i < 16) {
        buff[i] = y;
        i++;
    }
    else if (i == 16) {
        buff[16] = '\0';
        flag = 1; 
        i = 0; 
    }

}