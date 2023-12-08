#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

uint16_t ADCValue;
uint16_t mode = 1;
volatile int Angle1 = 90;
volatile int Angle2 = 90;
volatile int Angle3 = 90;
volatile int Angle4 = 180;

void ADCSetup(void)
{
    // Setup for ADC
    // Clear power reduction for ADC
    PRR &= ~(1<<PRADC);
    // Select Vref = AVccA
    ADMUX = (1<<REFS0);
    ADMUX &= ~(1<<REFS1);
    // Set the ADC Clock div by 128 (clock must be between 50 and 200 khz,
    // 16M/128=125K)
    ADCSRA |= (1<<ADPS0);
    ADCSRA |= (1<<ADPS1);
    ADCSRA |= (1<<ADPS2);
    // Select channel 0
    ADMUX &= ~(1<<MUX0);
    ADMUX &= ~(1<<MUX1); 
    ADMUX &= ~(1<<MUX2);
    ADMUX &= ~(1<<MUX3);
    // Set auto trigger
    ADCSRA |= (1<<ADATE);
    // Set to free running
    ADCSRB &= ~(1<<ADTS0);
    ADCSRB &= ~(1<<ADTS1);
    ADCSRB &= ~(1<<ADTS2);
    // Disable digital input buffer on ADC pin
    DIDR0 |= (1<<ADC0D);
    // Enable ADC
    ADCSRA |= (1<<ADEN);
    // Start conversion
    ADCSRA |= (1<<ADSC);
}

void Timer1Setup(void)
{
    // set PD23 PC12 output
	DDRB |= (1 << DDB1);
	DDRB |= (1 << DDB2);
	DDRB |= (1 << DDB3);
	DDRB |= (1 << DDB4);
	PORTB &= ~ (1 << PORTB1);
	PORTB &= ~ (1 << PORTB2);
	PORTB &= ~ (1 << PORTB3);
	PORTB &= ~ (1 << PORTB4);
	cli();
    //Disable Interrupt
    
	// Set Timer 1 to Normal
	TCCR1A &= ~ (1<< WGM10);
	TCCR1A &= ~ (1<< WGM11);
	TCCR1B &= ~ (1<< WGM12);
    TCCR1B &= ~ (1<< WGM13);
	
	//prescaler 64
	TCCR1B |= (1<<CS10);
	TCCR1B |= (1<<CS11);
	TCCR1B &= ~(1<<CS12) ;
	
    OCR1A = 375;
    OCR1B = 5000;
	TCNT1 = 0;
    TIMSK1 |= (1 << OCIE1A);
    TIMSK1 |= (1 << OCIE1B);
    sei();
}

ISR(TIMER1_COMPA_vect){
	if(mode == 1){
		PORTB &= ~(1 << PORTB1);
	}
	else if(mode == 2){
		PORTB &= ~(1 << PORTB2);
	}
	else if(mode == 3){
		PORTB &= ~(1 << PORTB3);
	}	
	else if (mode == 4){
		PORTB &= ~(1 << PORTB4);
	}
}

ISR(TIMER1_COMPB_vect){
	if(mode == 1){
		PORTB |= (1 << PORTB1);
	}
	else if(mode == 2){
		PORTB |= (1 << PORTB2);
	}
	else if(mode == 3){
		PORTB |= (1 << PORTB3);
	}	
	else if (mode == 4){
		PORTB |= (1 << PORTB4);
	}
    TCNT1 = 0;
}

void SetServoAngle(int Angle){
	int pulseWidth = 375 + (Angle * 2.08);
	OCR1A = pulseWidth;
}

void MoveMotor(void){
    if (mode == 1)
		{
        ADCValue = ADC;
		if (ADCValue <= 450 && Angle1 > 0) {
			Angle1 -= 5; 
		}
		else if (ADCValue >= 750 && Angle1 < 180) {
			Angle1 += 5; 
		}
        SetServoAngle(Angle1);
    }

    else if (mode == 2)
		{
        ADCValue = ADC;
		if (ADCValue <= 450 && Angle2 > 0) {
			Angle2 -= 5;
		}
		else if (ADCValue >= 750 && Angle2 < 180) {
			Angle2 += 5; 
		}
        SetServoAngle(Angle2);
    }

    else if (mode == 3)
		{
        ADCValue = ADC;
		if (ADCValue <= 450 && Angle3 > 0) {
			Angle3 -= 5; 
		}
		else if (ADCValue >= 750 && Angle3 < 180) {
			Angle3 += 5; 
		}
        SetServoAngle(Angle3);
    }
	else if (mode == 4)
	{
		Angle4 = 0;
		SetServoAngle(Angle4);
        _delay_ms(5000);
        Angle4 = 0;
        SetServoAngle(Angle4);
    }
	_delay_ms(100);
}

int main(void)
{
    ADCSetup();
    Timer1Setup();
    while(1)
    {
        MoveMotor();
    }
    return 0;
}