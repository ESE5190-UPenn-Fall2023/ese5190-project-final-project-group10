/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: You
 */ 

#include "LCD_GFX.h"
#include "ST7735.h"
#include <math.h>
#include <string.h>

/******************************************************************************
* Local Functions
******************************************************************************/



/******************************************************************************
* Global Functions
******************************************************************************/

/**************************************************************************//**
* @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
* @brief		Convert RGB888 value to RGB565 16-bit color data
* @note
*****************************************************************************/
uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
{
	return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
}

/**************************************************************************//**
* @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
* @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
* @note
*****************************************************************************/
void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
	LCD_setAddr(x,y,x,y);
	SPI_ControllerTx_16bit(color);
}

/**************************************************************************//**
* @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
* @brief		Draw a character starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
	uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
	int i, j;
	if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
		for(i=0;i<5;i++){
		uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
			for(j=0;j<8;j++){
				if ((pixels>>j)&1==1){
					LCD_drawPixel(x+i,y+j,fColor);
				}
				else {
					LCD_drawPixel(x+i,y+j,bColor);
				}
			}
		}
	}
}


/******************************************************************************
* LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
* You are free to create and add any additional files, libraries, and/or
*  helper function. All code must be authentically yours.
******************************************************************************/

/**************************************************************************//**
* @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
* @brief		Draw a colored circle of set radius at coordinates
* @note
*****************************************************************************/
void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
{
	uint16_t x = 0;
    uint16_t y = 0;
    uint16_t r = radius * radius;
	LCD_setAddr(x0 - radius, y0 - radius, x0 + radius, y0 + radius);
	clear(LCD_PORT, LCD_TFT_CS);
	for (uint8_t i = 0; i <= 2*radius; i++)
	{
		for (uint8_t j = 0; j <= 2*radius; j++)
		{
			x = (i-radius)*(i-radius);
			y = (j-radius)*(j-radius);
			if (x + y <= r)
			{
				SPI_ControllerTx_16bit_stream(color);
			}
			else
			{
				SPI_ControllerTx_16bit_stream(BLACK);
			}
		}
		
	}
	set(LCD_PORT, LCD_TFT_CS);
}

/**************************************************************************//**
* @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
* @brief		Draw a line from and to a point with a color
* @note
*****************************************************************************/
void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
{
	uint8_t dx = (x1 - x0);
    uint8_t dy = (y1 - y0);
	if(dy < 0)
	{
		dy = - dy;
	}
    uint8_t sy = 1;

    if (y0 > y1) 
	{
        sy = -1;
    }

    uint8_t err = dx - dy;

    while (x0 != x1 || y0 != y1) {
        LCD_drawPixel(x0,y0,c);
        uint8_t e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0++;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
    LCD_drawPixel(x1,y1,c);
}

/**************************************************************************//**
* @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
* @brief		Draw a colored block at coordinates
* @note
*****************************************************************************/
void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
{
	LCD_setAddr(x0, y0, x1, y1);
	clear(LCD_PORT, LCD_TFT_CS);
	for (uint8_t i = x0; i <= x1; i++)
	{
		for (uint8_t j = y0; j <= y1; j++)
		{
			SPI_ControllerTx_16bit_stream(color);
		}
		
	}
	set(LCD_PORT, LCD_TFT_CS);
}

/**************************************************************************//**
* @fn			void LCD_setScreen(uint16_t color)
* @brief		Draw the entire screen to a color
* @note
*****************************************************************************/
void LCD_setScreen(uint16_t color) 
{
	LCD_setAddr(0,0,179,127);
	clear(LCD_PORT, LCD_TFT_CS);
	for(uint16_t i = 0; i < 128 * 180; i++){
		SPI_ControllerTx_16bit_stream(color);
	}
	set(LCD_PORT, LCD_TFT_CS);
}

/**************************************************************************//**
* @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
* @brief		Draw a string starting at the point with foreground and background colors
* @note
*****************************************************************************/
void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
{
	uint16_t character = str[1];
	uint8_t shift = 0;
	for (int k=0;k<strlen(str);k++)
	{
		character = str[k];
		uint16_t row = character - 0x20; //Determine row of ASCII table starting at space
		int i,j;
		if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
			for(i=0;i<5;i++){
				uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
				for(j=0;j<8;j++){
					if ((pixels>>j)&1==1){
						LCD_drawPixel(x+i+shift,y+j,fg);
					}
					else {
						LCD_drawPixel(x+i+shift,y+j,bg);
					}
				}
			}
			shift +=8;
		}

	}
}

// Drawball x,y r=4

void LCD_drawBall(uint8_t x, uint8_t y)
{
	LCD_setAddr(x-4,y-4,x+4,y+4);
	clear(LCD_PORT, LCD_TFT_CS);
	for(uint8_t i = 0; i < 9; i++){
		SPI_ControllerTx_16bit_stream(BLACK);
		SPI_ControllerTx_16bit_stream(BLACK);
		for(int j = 0; j < 5; j++){
		SPI_ControllerTx_16bit_stream(RED);
		}
		SPI_ControllerTx_16bit_stream(BLACK);
		SPI_ControllerTx_16bit_stream(BLACK);
		SPI_ControllerTx_16bit_stream(BLACK);
		for(int j = 0; j < 7; j++){
		SPI_ControllerTx_16bit_stream(RED);
		}
		SPI_ControllerTx_16bit_stream(BLACK);
		for(int j = 0; j < 45; j++){
		SPI_ControllerTx_16bit_stream(RED);
		}
		SPI_ControllerTx_16bit_stream(BLACK);
		for(int j = 0; j < 7; j++){
		SPI_ControllerTx_16bit_stream(RED);
		}
		SPI_ControllerTx_16bit_stream(BLACK);
		SPI_ControllerTx_16bit_stream(BLACK);
		SPI_ControllerTx_16bit_stream(BLACK);
		for(int j = 0; j < 5; j++){
		SPI_ControllerTx_16bit_stream(RED);
		}
		SPI_ControllerTx_16bit_stream(BLACK);
		SPI_ControllerTx_16bit_stream(BLACK);
	}
	set(LCD_PORT, LCD_TFT_CS);
}

// DrawPaddle x,y r=4