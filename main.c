#include <avr/io.h>
#include "ST7735.h"
#include "LCD_GFX.h"
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>

uint8_t Curser_Mainmenu = 0;
uint8_t Curser_Delay = 0;
uint8_t Curser_BacktoMain = 0;
uint8_t Curser_PhoneCall = 0;
uint8_t Curser_Right = 0;
uint8_t Curser_Left = 0;
uint8_t Curser_RobotArm = 0;
uint8_t Curser_Up = 0;
uint8_t Curser_Down = 0;
uint8_t Curser_Unlock = 0;
uint16_t ADCValue;

volatile int Angle1 = 90;
volatile int Angle2 = 90;
volatile int Angle3 = 90;
volatile int Angle4 = 180;

void Initialize()
{
	lcd_init();
}

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
	DDRD |= (1 << DDD3);
	DDRD |= (1 << DDD2);
	DDRC |= (1 << DDC1);
	DDRC |= (1 << DDC2);
	PORTD &= ~ (1 << PORTD3);
	PORTD &= ~ (1 << PORTD2);
	PORTC &= ~ (1 << PORTC1);
	PORTC &= ~ (1 << PORTC2);
	cli();
    //Disable Interrupt
    TIMSK1 &= ~(1 << OCIE1A);
    TIMSK1 &= ~(1 << OCIE1B);
	// Set Timer 1 to Normal
	TCCR1A &= ~ (1<< WGM10);
	TCCR1A &= ~ (1<< WGM11);
	TCCR1B &= ~ (1<< WGM12);
    TCCR1B &= ~ (1<< WGM13);
	
	//prescaler 256
	TCCR1B &= ~(1<<CS10);
	TCCR1B &= ~(1<<CS11);
	TCCR1B |= (1<<CS12) ;
	
    OCR1A = 938;
    OCR1B = 12500;
	TCNT1 = 0;
    //sei();
}

ISR(TIMER1_COMPA_vect){
	if(Curser_RobotArm == 4){
		PORTC &= ~(1 << PORTC1);
	}
	else if(Curser_RobotArm == 16){
		PORTC &= ~(1 << PORTC2);
	}
	else if(Curser_RobotArm == 28){
		PORTD &= ~(1 << PORTD2);
	}	
	else if (Curser_RobotArm == 40){
		PORTD &= ~(1 << PORTD3);
	}
}

ISR(TIMER1_COMPB_vect){
	if(Curser_RobotArm == 4){
		PORTC |= (1 << PORTC1);
	}
	else if(Curser_RobotArm == 16){
		PORTC |= (1 << PORTC2);
	}
	else if(Curser_RobotArm == 28){
		PORTD |= (1 << PORTD2);
	}	
	else if (Curser_RobotArm == 40){
		PORTD |= (1 << PORTD3);
	}
    TCNT1 = 0;
}

void SetServoAngle(int Angle){
	int pulseWidth = 625 + (Angle * 3.47);
	OCR1A = pulseWidth;
}

int ButtonPushed(void)
{
    if (!(PIND & (1<<PORTD4)))
    {
        Delay_ms(10);
        if(!(PIND & (1<<PORTD4)))
        {
            return 1;
            PORTD &= ~(1<<PORTD4);
        }
    }
    return 0;
}

int ButtonReleased(void)
{
    if ((PIND & (1<<PORTD4)))
    {
        Delay_ms(10);
        if((PIND & (1<<PORTD4)))
        {
            return 1;
            PORTD |= (1<<PORTD4);
        }
    }
    return 0;
}

void MoveMotor(void){
    if (Curser_RobotArm == 4)
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

    else if (Curser_RobotArm == 16)
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

    else if (Curser_RobotArm == 28)
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

	else if (Curser_RobotArm == 40)
	{
		Angle4 = 0;
		SetServoAngle(Angle4);
        while (Curser_Unlock == 0)
        {
            if (ButtonPushed())
            {
                while (!(ButtonReleased()))
                {}
                Angle4 = 180;
                SetServoAngle(Angle4);
                Curser_Unlock = 1;
            }
		}
    }
}

void LockMode(void)
{
    Curser_Unlock = 0;
    LCD_drawBlock(57,Curser_RobotArm,64,Curser_RobotArm+7,GREEN);
    //Enable Interrupt
	//TIMSK0 |= (1 << OCIE1A);
    //TIMSK0 |= (1 << OCIE1B);

    while (Curser_Unlock == 0)
    {
        MoveMotor();
        if (ButtonPushed())
        {
            while (!(ButtonReleased()))
            {}
            Curser_Unlock = 1;
        }
    }
    //Disable Interrupt
    //TIMSK1 &= ~(1 << OCIE1A);
    //TIMSK0 &= ~(1 << OCIE1B);
    LCD_drawBlock(57,Curser_RobotArm,64,Curser_RobotArm+7,BLACK);
}

void SendCom(void)
{
    uint8_t Com1;
    uint8_t Com2;
    uint8_t Com3;
    uint8_t Com4;
    if (Curser_PhoneCall == 0)
    {
        Com1 = 0;
        Com2 = 0;
        Com3 = 0;
        Com4 = 0;
    }
    else if (Curser_PhoneCall == 10)
    {
        Com1 = 0;
        Com2 = 0;
        Com3 = 0;
        Com4 = 1;
    }
    else if (Curser_PhoneCall == 20)
    {
        Com1 = 0;
        Com2 = 0;
        Com3 = 1;
        Com4 = 0;
    }
    else if (Curser_PhoneCall == 30)
    {
        Com1 = 0;
        Com2 = 0;
        Com3 = 1;
        Com4 = 1;
    }
    else if (Curser_PhoneCall == 40)
    {
        Com1 = 0;
        Com2 = 1;
        Com3 = 0;
        Com4 = 0;
    }
    else if (Curser_PhoneCall == 50)
    {
        Com1 = 0;
        Com2 = 1;
        Com3 = 0;
        Com4 = 1;
    }
    else if (Curser_PhoneCall == 60)
    {
        Com1 = 0;
        Com2 = 1;
        Com3 = 1;
        Com4 = 0;
    }
    else if (Curser_PhoneCall == 70)
    {
        Com1 = 0;
        Com2 = 1;
        Com3 = 1;
        Com4 = 1;
    }
    else if (Curser_PhoneCall == 80)
    {
        Com1 = 1;
        Com2 = 0;
        Com3 = 0;
        Com4 = 0;
    }
    else if (Curser_PhoneCall == 90)
    {
        Com1 = 1;
        Com2 = 0;
        Com3 = 0;
        Com4 = 1;
    }
    else if (Curser_PhoneCall == 100)
    {
        Com1 = 1;
        Com2 = 1;
        Com3 = 1;
        Com4 = 1;
    }
    else if (Curser_PhoneCall == 110)
    {
        Com1 = 1;
        Com2 = 1;
        Com3 = 1;
        Com4 = 0;
    }
    else if (Curser_PhoneCall == 120)
    {
        Com1 = 1;
        Com2 = 1;
        Com3 = 0;
        Com4 = 1;
    }
    
    PORTD |= (1<<PORTD7);

    if (Com1 == 1)
    {
        PORTC |= (1<<PORTC4);
    }
    else if(Com1 == 0)
    {
        PORTC &= ~(1<<PORTC4);
    }
    Delay_ms(10);
    PORTC |= (1<<PORTC3);
    Delay_ms(10);
    while (!(PINC & (1<<PORTC5)))
    {}
    PORTC &= ~(1<<PORTC3);
    Delay_ms(10);
    while (PINC & (1<<PORTC5))
    {}
    Delay_ms(10);

    if (Com2 == 1)
    {
        PORTC |= (1<<PORTC4);
    }
    else if(Com2 == 0)
    {
        PORTC &= ~(1<<PORTC4);
    }
    Delay_ms(10);
    PORTC |= (1<<PORTC3);
    Delay_ms(10);
    while (!(PINC & (1<<PORTC5)))
    {}
    PORTC &= ~(1<<PORTC3);
    Delay_ms(10);
    while (PINC & (1<<PORTC5))
    {}
    Delay_ms(10);

    if (Com3 == 1)
    {
        PORTC |= (1<<PORTC4);
    }
    else if(Com3 == 0)
    {
        PORTC &= ~(1<<PORTC4);
    }
    Delay_ms(10);
    PORTC |= (1<<PORTC3);
    Delay_ms(10);
    while (!(PINC & (1<<PORTC5)))
    {}
    PORTC &= ~(1<<PORTC3);
    Delay_ms(10);
    while (PINC & (1<<PORTC5))
    {}
    Delay_ms(10);

    if (Com4 == 1)
    {
        PORTC |= (1<<PORTC4);
    }
    else if(Com4 == 0)
    {
        PORTC &= ~(1<<PORTC4);
    }
    Delay_ms(10);
    PORTC |= (1<<PORTC3);
    Delay_ms(10);
    while (!(PINC & (1<<PORTC5)))
    {}
    PORTC &= ~(1<<PORTC3);
    Delay_ms(10);
    while (PINC & (1<<PORTC5))
    {}
    Delay_ms(10);

    PORTD &= ~(1<<PORTD7);
}

void Draw_PhoneMenu(void)
{
    LCD_setScreen(BLACK);
    char* Menu = "0";
    LCD_drawString(4,4,Menu,WHITE,BLACK);
    Menu = "1";
    LCD_drawString(14,4,Menu,WHITE,BLACK);
    Menu = "2";
    LCD_drawString(24,4,Menu,WHITE,BLACK);
    Menu = "3";
    LCD_drawString(34,4,Menu,WHITE,BLACK);
    Menu = "4";
    LCD_drawString(44,4,Menu,WHITE,BLACK);
    Menu = "5";
    LCD_drawString(54,4,Menu,WHITE,BLACK);
    Menu = "6";
    LCD_drawString(64,4,Menu,WHITE,BLACK);
    Menu = "7";
    LCD_drawString(74,4,Menu,WHITE,BLACK);
    Menu = "8";
    LCD_drawString(84,4,Menu,WHITE,BLACK);
    Menu = "9";
    LCD_drawString(94,4,Menu,WHITE,BLACK);
    Menu = "C";
    LCD_drawString(104,4,Menu,WHITE,BLACK);
    Menu = "P";
    LCD_drawString(114,4,Menu,WHITE,BLACK);
    Menu = "H";
    LCD_drawString(124,4,Menu,WHITE,BLACK);
    Menu = "B";
    LCD_drawString(134,4,Menu,WHITE,BLACK);
}

void Draw_RobotArm(void)
{
    LCD_setScreen(BLACK);
    char* RMenu = "Mode 1";
    LCD_drawString(4,4,RMenu,WHITE,BLACK);
    RMenu = "Mode 2";
    LCD_drawString(4,16,RMenu,WHITE,BLACK);
    RMenu = "Mode 3";
    LCD_drawString(4,28,RMenu,WHITE,BLACK);
    RMenu = "Mode 4";
    LCD_drawString(4,40,RMenu,WHITE,BLACK);
    RMenu = "Back";
    LCD_drawString(4,52,RMenu,WHITE,BLACK);
}

void Block_PhoneCall(void)
{
    if (Curser_Left)
    {
        LCD_drawBlock(Curser_PhoneCall+21,1,Curser_PhoneCall+22,14,BLACK);
        LCD_drawBlock(Curser_PhoneCall+13,1,Curser_PhoneCall+20,2,BLACK);
        LCD_drawBlock(Curser_PhoneCall+13,13,Curser_PhoneCall+20,14,BLACK);
        Curser_Left = 0;
    }
    if (Curser_Right)
    {
        LCD_drawBlock(Curser_PhoneCall-9,1,Curser_PhoneCall-8,14,BLACK);
        LCD_drawBlock(Curser_PhoneCall-7,1,Curser_PhoneCall,2,BLACK);
        LCD_drawBlock(Curser_PhoneCall-7,13,Curser_PhoneCall,14,BLACK);
        Curser_Right = 0;
    }
    LCD_drawBlock(Curser_PhoneCall+1,1,Curser_PhoneCall+2,14,WHITE);
    LCD_drawBlock(Curser_PhoneCall+11,1,Curser_PhoneCall+12,14,WHITE);
    LCD_drawBlock(Curser_PhoneCall+3,1,Curser_PhoneCall+10,2,WHITE);
    LCD_drawBlock(Curser_PhoneCall+3,13,Curser_PhoneCall+10,14,WHITE);
}

void Block_RobotArm(void)
{
    if (Curser_Up == 1)
    {
        Curser_Up = 0;
        LCD_drawBlock(1,Curser_RobotArm+21,54,Curser_RobotArm+22,BLACK);
        LCD_drawBlock(1,Curser_RobotArm+11,2,Curser_RobotArm+20,BLACK);
        LCD_drawBlock(53,Curser_RobotArm+11,54,Curser_RobotArm+20,BLACK);
    }
    else if(Curser_Down == 1)
    {
        Curser_Down = 0;
        LCD_drawBlock(1,Curser_RobotArm-15,54,Curser_RobotArm-14,BLACK);
        LCD_drawBlock(1,Curser_RobotArm-13,2,Curser_RobotArm-4,BLACK);
        LCD_drawBlock(53,Curser_RobotArm-13,54,Curser_RobotArm-4,BLACK);
    }
    LCD_drawBlock(1,Curser_RobotArm-3,54,Curser_RobotArm-2,WHITE);
    LCD_drawBlock(1,Curser_RobotArm+9,54,Curser_RobotArm+10,WHITE);
    LCD_drawBlock(1,Curser_RobotArm-1,2,Curser_RobotArm+8,WHITE);
    LCD_drawBlock(53,Curser_RobotArm-1,54,Curser_RobotArm+8,WHITE);
}

void Shift_Block_PhoneCall(void)
{
    ADCValue = ADC;
    if (ADCValue > 600 && Curser_PhoneCall < 130)
    {
        Curser_PhoneCall += 10;
        Curser_Right = 1;
        Curser_Delay = 1;
        Block_PhoneCall();
    }
    else if (ADCValue < 400 && Curser_PhoneCall > 0)
    {
        Curser_PhoneCall -= 10;
        Curser_Left = 1;
        Curser_Delay = 1;
        Block_PhoneCall();
    }
}

void Shift_Block_RobotArm(void)
{
    ADCValue = ADC;
    if (ADCValue > 600 && Curser_RobotArm < 52)
    {
        Curser_RobotArm += 12;
        Curser_Down = 1;
        Curser_Delay = 1;
        Block_RobotArm();
    }
    else if (ADCValue < 400 && Curser_RobotArm > 4)
    {
        Curser_RobotArm -= 12;
        Curser_Up = 1;
        Curser_Delay = 1;
        Block_RobotArm();
    }
}

void PhoneCall(void)
{
    Curser_PhoneCall = 0;
    Draw_PhoneMenu();
    Block_PhoneCall();
    while (Curser_BacktoMain == 0)
    {
        Shift_Block_PhoneCall();
        if (Curser_Delay)
        {
            Delay_ms(200);
            Curser_Delay = 0;
        }
        if (ButtonPushed())
        {
            while (!(ButtonReleased()))
            {}
            if (Curser_PhoneCall == 130)
            {
                Curser_BacktoMain = 1;
            }
            else
            {
                SendCom();
            }
        }
    }
}

void RobotArm(void){
    Timer1Setup();
    Curser_RobotArm = 4;
    Draw_RobotArm();
    Block_RobotArm();
    while (Curser_BacktoMain == 0)
    {
        Shift_Block_RobotArm();
        if (Curser_Delay)
        {
            Delay_ms(200);
            Curser_Delay = 0;
        }
        if (ButtonPushed())
        {
            while (!(ButtonReleased()))
            {}
            if (Curser_RobotArm == 52)
            {
                Curser_BacktoMain = 1;
            }
            else
            {
                LockMode();
            }
        }
    }
}

void ButtonBehave (void)
{
    PORTD |= (1<<PORTD7);
    while (!(ButtonReleased()))
    {}
    PORTD &= ~(1<<PORTD7);
    if (Curser_Mainmenu == 0)
    {
        RobotArm();
    }
    else if(Curser_Mainmenu == 1)
    {
        PhoneCall();
    }
    
}

void Draw_Mainmenu(void)
{
    char* Menu1 = "Robot Arm";
    LCD_drawString(4,4,Menu1,WHITE,BLACK);
    char* Menu2 = "Phone Call";
    LCD_drawString(4,16,Menu2,WHITE,BLACK);
}

void Block_Mainmenu(void)
{
    if (Curser_Mainmenu)
    {
        LCD_drawBlock(1,1,85,2,BLACK);
        LCD_drawBlock(1,3,2,12,BLACK);
        LCD_drawBlock(84,3,85,12,BLACK);

        LCD_drawBlock(1,13,85,14,WHITE);
        LCD_drawBlock(1,25,85,26,WHITE);
        LCD_drawBlock(1,15,2,24,WHITE);
        LCD_drawBlock(84,15,85,24,WHITE);
    }
    else
    {
        LCD_drawBlock(1,25,85,26,BLACK);
        LCD_drawBlock(1,15,2,24,BLACK);
        LCD_drawBlock(84,15,85,24,BLACK);

        LCD_drawBlock(1,1,85,2,WHITE);
        LCD_drawBlock(1,13,85,14,WHITE);
        LCD_drawBlock(1,3,2,12,WHITE);
        LCD_drawBlock(84,3,85,12,WHITE);
    }
}

void Shift_Block_Mainmenu(void)
{
    ADCValue = ADC;
    if (ADCValue < 400 && Curser_Mainmenu == 1)
    {
        Curser_Mainmenu = 0;
        Block_Mainmenu();
        Curser_Delay = 1;
    }
    else if(ADCValue > 600 && Curser_Mainmenu == 0)
    {
        Curser_Mainmenu = 1;
        Block_Mainmenu();
        Curser_Delay = 1;
    }
}


int main(void)
{
    Initialize();
    ADCSetup();

    DDRD &= ~(1<<DDD4);
    DDRD |= (1<<DDD7);
    DDRC |= (1<<DDC3);
    DDRC |= (1<<DDC4);
    DDRC &= ~(1<<DDC5);
    PORTD &= ~(1<<PORTD7);
    PORTC &= ~(1<<PORTC3);
    PORTC &= ~(1<<PORTC4);

    while (1)
    {
        Curser_BacktoMain = 0;
        Curser_Mainmenu = 0;
        LCD_setScreen(BLACK);
        Draw_Mainmenu();
        Block_Mainmenu();
        
        while(Curser_BacktoMain == 0)
        {
            if (ButtonPushed())
            {
                ButtonBehave();
            }
            Shift_Block_Mainmenu();
            if(Curser_Delay)
            {
                Delay_ms(200);
                Curser_Delay = 0;
            }
        }
    }
    
    return 0;
}