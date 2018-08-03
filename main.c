#define F_CPU 16000000UL
#include </usr/avr/include/avr/io.h>
#include </usr/avr/include/util/delay.h>
#include </usr/avr/include/avr/eeprom.h>
#include </usr/avr/include/avr/interrupt.h>

//===============CONFIG===============
// this is the max number of samples that can be recorded by the AVR
#define SAMPLES 400
// this is the threshold variable for TIMER2
// this is the time F_CPU prescaled by 8
// it is meant to be set correctly in order to ignore pulse transitions shorter than the said time
#define THRESH_COUNT 50 //currently set to (1/(16000000/8))*50 ~25us, because that was the signal i was trying to record
//====================================

#define PIN_LOW (PIND & (1 << PD2))
typedef unsigned char BYTE;
typedef unsigned int WORD;
WORD memory[SAMPLES];
WORD counter = 0, tempcounter;
BYTE first = 1;
void timer_init();
void external_interrupt_init();
void uart_init();

void uart_send(BYTE data);

void main()
{
    DDRB = 0b11; //setting PB0 and PB1 as output
    PORTB = 1; //turning on PD0 LED
    _delay_ms(1000); //delay 1000ms
    PORTB = 0; //turning off PD0 lED
    _delay_ms(1000); //delay 1000ms
    PORTB = 1; //turning on LED again
    cli(); //clear interrupt
    uart_init(); //setups the uart
    for(tempcounter = 0; tempcounter < SAMPLES; tempcounter++) //clears all memory
	memory[tempcounter] = 0;
    while(PIN_LOW); //while the PD2 is low, wait here. Basically wait for the first pulse
    PORTB = 0; //just as the PD2 becomes high, turn off PB0 LED
    timer_init(); //timer init, starts both timers
    external_interrupt_init(); //interrupt init
    sei(); //enable interrupts again
    while(1)
    {
    }
}

void timer_init()
{
    TCNT1 = 0;
    TCNT2 = 0;
    TCCR2 |= (1 << CS21); //starting timer2 (8 bit), with prescaler as 8
    TCCR1B |= (1 << CS10); //starting timer1 (16 bit) without prescaler
    OCR2 = THRESH_COUNT; //setting threshold as THRESH_COUNT
    TIMSK |= (1 << OCIE2) | (1 << TOIE2);
}

void external_interrupt_init()
{
    MCUCR |= (1 << ISC00); //INT0 will be triggered at any logical change
    GICR |= (1 << INT0); //enabling on INT0 interrupt
    PORTD |= (1 << PD2); //enabling the PD2 pin pull up resistor
}

void uart_init()
{
    UCSRB |= (1 << TXEN) | (1 << RXEN) | (1 << RXCIE);
    UCSRC |= (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);
    UBRRL = 103; //baud rate is set to 9600 right now
}

void uart_send(BYTE data)
{
    while((UCSRA & (1 << UDRE)) == 0){}; //while the UDR is not empty, loop this
    UDR = data; //write data out
}

ISR(INT0_vect) //INT0 service routine
{
    memory[counter] = TCNT1; //writes the value of timer1 to memory
    TCNT2 = 0; //resets timer2
    if(counter > SAMPLES) //if sampling is done, turn off all interrupts
    {
	GICR &= ~(1 << INT0); //we turn off the INT0 (this) interrupt
	TIMSK &= ~(1 << OCIE2); //turning off all timer related interrupts
    }
}

ISR(TIMER2_COMP_vect) //this subroutine is executed when the threshold is exceeded
{
    sei();
    TCNT1 = TCNT2 *8; //saving TCNT1 in TCNT2 to timekeep the low time
    TCCR2 &= ~(1 << CS21); //stopping timer2
    TCNT2 = 0; //reseting timer2
    counter++; //incementing memory counter
    GICR &= ~(1 << INT0); //we turn off the INT0


    while(PIN_LOW); //stop while the pin is low
    TCCR2 |= (1 << CS21); //starting timer2 (8 bit), with prescaler as 8
    memory[counter] = TCNT1; //when the pin is high again, save the low time in memory
    counter++; //incrementing memory counter
    TCNT1 = TCNT2 * 8; //setting TCNT1 as TCNT2, because TCNT2 has been running since the state change
    GICR |= (1 << INT0); //turning on INT0
}

ISR(TIMER2_OVF_vect)
{
    GICR &= ~(1 << INT0); //we turn off the INT0
    TIMSK &= ~((1 << OCIE2) | (1 << TOIE2)); //turning off all timer related interrupts
    PORTB = 0b10;
}

ISR(USART_RXC_vect) //UART Receive complete routine
{
    //test message
    uart_send('T');
    uart_send('e');
    uart_send('s');
    uart_send('t');
    uart_send('\n');
    uart_send('\r'); //return carriage
    for(tempcounter = 0; tempcounter < SAMPLES; tempcounter++) //loop to print all the data out of uart. tempcounter is used so that even if not all SAMPLES are taken, the data can still be dumped
    {
	uart_send(48 + (memory[tempcounter] / 10000) % 10);
	uart_send(48 + (memory[tempcounter] / 1000) % 10);
	uart_send(48 + (memory[tempcounter] / 100) % 10);
	uart_send(48 + (memory[tempcounter] / 10) % 10);
	uart_send(48 + (memory[tempcounter] % 10));
	uart_send('\n'); //newline
	uart_send('\r'); //return carriage
    }
    BYTE temp = UDR; //reading UDR because the datasheet states that in the ISR for RXC, UDR has to be read at least once to avoid an interrupt loop
}
