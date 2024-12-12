/*
 * LCD_Driver.c
 *
 *  Created on: Sep 28, 2023
 *      Author: Xavion
 */

#include "LCD_Driver.h"

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */


static LTDC_HandleTypeDef hltdc;
static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
static FONT_t *LCD_Currentfonts;
static uint16_t CurrentTextColor   = 0xFFFF;

/*
 * fb[y*W+x] OR fb[y][x]
 * Alternatively, we can modify the linker script to have an end address of 20013DFB instead of 2002FFFF, so it does not place variables in the same region as the frame buffer. In this case it is safe to just specify the raw address as frame buffer.
 */
//uint32_t frameBuffer[(LCD_PIXEL_WIDTH*LCD_PIXEL_WIDTH)/2] = {0};		//16bpp pixel format. We can size to uint32. this ensures 32 bit alignment


//Someone from STM said it was "often accessed" a 1-dim array, and not a 2d array. However you still access it like a 2dim array,  using fb[y*W+x] instead of fb[y][x].
uint16_t frameBuffer[LCD_PIXEL_WIDTH*LCD_PIXEL_HEIGHT] = {0};			//16bpp pixel format.


void LCD_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable the LTDC clock */
  __HAL_RCC_LTDC_CLK_ENABLE();

  /* Enable GPIO clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* GPIOA configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 |
                           GPIO_PIN_11 | GPIO_PIN_12;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Alternate= GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

 /* GPIOB configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_8 | \
                           GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

 /* GPIOC configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

 /* GPIOD configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_6;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

 /* GPIOF configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

 /* GPIOG configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | \
                           GPIO_PIN_11;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* GPIOB configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStructure.Alternate= GPIO_AF9_LTDC;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* GPIOG configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_12;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void LTCD_Layer_Init(uint8_t LayerIndex)
{
	LTDC_LayerCfgTypeDef  pLayerCfg;

	pLayerCfg.WindowX0 = 0;	//Configures the Window HORZ START Position.
	pLayerCfg.WindowX1 = LCD_PIXEL_WIDTH;	//Configures the Window HORZ Stop Position.
	pLayerCfg.WindowY0 = 0;	//Configures the Window vertical START Position.
	pLayerCfg.WindowY1 = LCD_PIXEL_HEIGHT;	//Configures the Window vertical Stop Position.
	pLayerCfg.PixelFormat = LCD_PIXEL_FORMAT_1;  //INCORRECT PIXEL FORMAT WILL GIVE WEIRD RESULTS!! IT MAY STILL WORK FOR 1/2 THE DISPLAY!!! //This is our buffers pixel format. 2 bytes for each pixel
	pLayerCfg.Alpha = 255;
	pLayerCfg.Alpha0 = 0;
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
	if (LayerIndex == 0){
		pLayerCfg.FBStartAdress = (uintptr_t)frameBuffer;
	}
	pLayerCfg.ImageWidth = LCD_PIXEL_WIDTH;
	pLayerCfg.ImageHeight = LCD_PIXEL_HEIGHT;
	pLayerCfg.Backcolor.Blue = 0;
	pLayerCfg.Backcolor.Green = 0;
	pLayerCfg.Backcolor.Red = 0;
	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, LayerIndex) != HAL_OK)
	{
		LCD_Error_Handler();
	}
}

void clearScreen(void)
{
  LCD_Clear(0,LCD_COLOR_WHITE);
}

void LTCD__Init(void)
{
	hltdc.Instance = LTDC;
	/* Configure horizontal synchronization width */
	hltdc.Init.HorizontalSync = ILI9341_HSYNC;
	/* Configure vertical synchronization height */
	hltdc.Init.VerticalSync = ILI9341_VSYNC;
	/* Configure accumulated horizontal back porch */
	hltdc.Init.AccumulatedHBP = ILI9341_HBP;
	/* Configure accumulated vertical back porch */
	hltdc.Init.AccumulatedVBP = ILI9341_VBP;
	/* Configure accumulated active width */
	hltdc.Init.AccumulatedActiveW = 269;
	/* Configure accumulated active height */
	hltdc.Init.AccumulatedActiveH = 323;
	/* Configure total width */
	hltdc.Init.TotalWidth = 279;
	/* Configure total height */
	hltdc.Init.TotalHeigh = 327;
	/* Configure R,G,B component values for LCD background color */
	hltdc.Init.Backcolor.Red = 0;
	hltdc.Init.Backcolor.Blue = 0;
	hltdc.Init.Backcolor.Green = 0;

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
	PeriphClkInitStruct.PLLSAI.PLLSAIR = 4;
	PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
	/* Polarity */
	hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
	hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
	hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
	hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

	LCD_GPIO_Init();

	if (HAL_LTDC_Init(&hltdc) != HAL_OK)
	 {
	   LCD_Error_Handler();
	 }

	ili9341_Init();
}

void LCD_Draw_Pixel(uint16_t x, uint16_t y, uint16_t color)
{
	if (x<0 || x>239 || y<0 || y> 319){return;}
	frameBuffer[y*LCD_PIXEL_WIDTH+x] = color;
}


void LCD_Draw_Circle_Fill(uint16_t Xpos, uint16_t Ypos, uint16_t radius, uint16_t color)
{
    for(int16_t y=-radius; y<=radius; y++)
    {
        for(int16_t x=-radius; x<=radius; x++)
        {
            if(x*x+y*y <= radius*radius)
            {
            	LCD_Draw_Pixel(x+Xpos, y+Ypos, color);
            }
        }
    }
}

// done

void LCD_Draw_BlockOnScreen(uint16_t x, uint16_t y,uint16_t c){
	for(int i = y; i <= y + 28 ; i++)
	{
		LCD_Draw_Horizontal_Line(x ,i, 19 ,c );
	}
}

uint16_t getRandomTetriminoColor()
{
    int randomIndex = rand() % 10;

    switch (randomIndex)
    {
        case 1:
            return LCD_COLOR_BLACK;
        case 2:
            return LCD_COLOR_GREY;
        case 3:
            return LCD_COLOR_BLUE;
        case 4:
            return LCD_COLOR_RED;
        case 5:
            return LCD_COLOR_MAGENTA;
        case 6:
            return LCD_COLOR_GREEN;
        case 7:
            return LCD_COLOR_YELLOW;
        case 8:
            return LCD_COLOR_DARK_GREY;
        default:
            return LCD_COLOR_ORANGE; // Default to orange if the index is out of range
    }
}

// done
void DrawLines(){
	for(int i =0; i <= 240; i=i+22)
	{
		LCD_Draw_Vertical_Line(i ,0,320,LCD_COLOR_WHITE);
	}
	for(int i =0; i<= 320; i= i+30)
	{
		LCD_Draw_Horizontal_Line(0,i ,240,LCD_COLOR_WHITE);
	}
}

// done
void DrawBlock(uint32_t tetris_board[11][11], uint16_t tetromino_color){

	LCD_Clear(0,LCD_COLOR_CYAN);

	for(int i =0; i<11; i++)
	{
		for(int j = 0; j<11; j++)
		{
			if(tetris_board[i][j])
			{
                uint16_t randomColor = getRandomTetriminoColor();
				LCD_Draw_BlockOnScreen(j * 22 , i * 30, randomColor);
			}
		}
	}
	DrawLines();
}




void LCD_Draw_Vertical_Line(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
  for (uint16_t i = 0; i < len; i++)
  {
	  LCD_Draw_Pixel(x, i+y, color);
  }
}

void LCD_Draw_Horizontal_Line(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
  for (uint16_t i = 0; i < len; i++)
  {
	  LCD_Draw_Pixel(i+x, y, color);
  }
}

void LCD_Clear(uint8_t LayerIndex, uint16_t Color)
{
	if (LayerIndex == 0){
		for (uint32_t i = 0; i < LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT; i++){
			frameBuffer[i] = Color;
		}
	}
  // TODO: Add more Layers if needed
}

//This was taken and adapted from stm32's mcu code
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}

//This was taken and adapted from stm32's mcu code
void LCD_SetFont(FONT_t *fonts)
{
  LCD_Currentfonts = fonts;
}

//This was taken and adapted from stm32's mcu code
void LCD_Draw_Char(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, counter = 0;
  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {
    for(counter = 0; counter < LCD_Currentfonts->Width; counter++)
    {
      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> counter)) == 0x00) && (LCD_Currentfonts->Width <= 12)) || (((c[index] & (0x1 << counter)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))
      {
         //Background If want to overrite text under then add a set color here
      }
      else
      {
    	  LCD_Draw_Pixel(counter + Xpos,index + Ypos,CurrentTextColor);
      }
    }
  }
}

void LCD_Draw_L(uint16_t X, uint16_t Y, uint16_t color)
{
	LCD_Draw_BlockOnScreen(X, Y + 32, color);

	LCD_Draw_BlockOnScreen(X + 24, Y + 32, color);

	LCD_Draw_BlockOnScreen(X + 48, Y + 32, color);

	LCD_Draw_BlockOnScreen(X, Y, color);

}

void LCD_Draw_L2(uint16_t X, uint16_t Y, uint16_t color)
{
	LCD_Draw_BlockOnScreen(X, Y, color);

	LCD_Draw_BlockOnScreen(X, Y + 32, color);

	LCD_Draw_BlockOnScreen(X - 24, Y + 32, color);

	LCD_Draw_BlockOnScreen(X - 48, Y + 32, color);
}


void LCD_Draw_I(uint16_t X, uint16_t Y, uint16_t color)
{

	LCD_Draw_BlockOnScreen(X + 24, Y, color);

	LCD_Draw_BlockOnScreen(X + 48, Y, color);

	LCD_Draw_BlockOnScreen(X + 72, Y, color);

	LCD_Draw_BlockOnScreen(X, Y, color);

}

void LCD_Draw_O(uint16_t X, uint16_t Y, uint16_t color)
{
	LCD_Draw_BlockOnScreen(X, Y + 32, color);

	LCD_Draw_BlockOnScreen(X + 24, Y, color);

	LCD_Draw_BlockOnScreen(X + 24, Y + 32, color);

	LCD_Draw_BlockOnScreen(X, Y, color);

}

void LCD_Draw_S(uint16_t X, uint16_t Y, uint16_t color)
{
	LCD_Draw_BlockOnScreen(X + 24, Y, color);

	LCD_Draw_BlockOnScreen(X + 24, Y + 32, color);

	LCD_Draw_BlockOnScreen(X + 48, Y + 32, color);

	LCD_Draw_BlockOnScreen(X, Y, color);

}

void LCD_Draw_S2(uint16_t X, uint16_t Y, uint16_t color)
{
	LCD_Draw_BlockOnScreen(X, Y + 32, color);

	LCD_Draw_BlockOnScreen(X + 24, Y, color);

	LCD_Draw_BlockOnScreen(X - 24, Y + 32, color);

	LCD_Draw_BlockOnScreen(X, Y, color);

}

void LCD_Draw_T(uint16_t X, uint16_t Y, uint16_t color)
{
	LCD_Draw_BlockOnScreen(X, Y , color);

	LCD_Draw_BlockOnScreen(X - 24, Y , color);

	LCD_Draw_BlockOnScreen(X + 24, Y , color);

	LCD_Draw_BlockOnScreen(X, Y + 32, color);

}

//This was taken and adapted from stm32's mcu code
void LCD_DisplayChar(uint16_t X, uint16_t Y, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_Draw_Char(X, Y, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}



void Start_Game_LCD()
{

	LCD_Clear(0, LCD_COLOR_CYAN);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);
	for(int x = 0;  x <= 240; x = x + 22)
	{
		LCD_Draw_Vertical_Line(x,0,320,LCD_COLOR_WHITE);

	}

	for(int y = 0; y <= 320; y = y + 32)
	{
		LCD_Draw_Horizontal_Line(0, y ,240, LCD_COLOR_WHITE);

	}

	LCD_Draw_O(25, 66, LCD_COLOR_GREEN);
	LCD_Draw_I(112, 226, LCD_COLOR_BLUE);
	LCD_Draw_S(23, 162, LCD_COLOR_YELLOW);
	LCD_Draw_S2( 136,66 ,LCD_COLOR_RED);
	LCD_Draw_L(47, 258, LCD_COLOR_MAGENTA);
	LCD_Draw_L2(177, 130, LCD_COLOR_GREY);
	LCD_Draw_T(90, 2, LCD_COLOR_ORANGE);

	LCD_DisplayChar(100,140,'S');
	LCD_DisplayChar(110,140,'t');
	LCD_DisplayChar(120,140,'a');
	LCD_DisplayChar(130,140,'r');
	LCD_DisplayChar(135,140,'t');

	LCD_DisplayChar(100,160,'T');
	LCD_DisplayChar(109,160,'e');
	LCD_DisplayChar(120,160,'t');
	LCD_DisplayChar(130,160,'r');
	LCD_DisplayChar(136,160,'i');
	LCD_DisplayChar(142,160,'s');

	LCD_DisplayChar(100,180,'G');
	LCD_DisplayChar(117,180,'a');
	LCD_DisplayChar(132,180,'m');
	LCD_DisplayChar(146,180,'e');





}


void End_Game_LCD(char elapsedtimer[12])
{

	LCD_SetFont(&Font16x24);
	LCD_SetTextColor(LCD_COLOR_BLACK);
//	LCD_Clear(0, LCD_COLOR_CYAN);


//	for(int x = 0;  x <= 240; x = x + 24)
//	{
//		LCD_Draw_Vertical_Line(x, 0, 320, LCD_COLOR_WHITE);
//
//	}
//
//	for(int y = 0; y <= 320; y = y + 32)
//	{
//		LCD_Draw_Horizontal_Line(0, y, 240, LCD_COLOR_WHITE);
//
//	}

	LCD_DisplayChar(100,100,'G');
	LCD_DisplayChar(117,100,'a');
	LCD_DisplayChar(132,100,'m');
	LCD_DisplayChar(146,100,'e');

	LCD_DisplayChar(100,120,'O');
	LCD_DisplayChar(115,120,'v');
	LCD_DisplayChar(126,120,'e');
	LCD_DisplayChar(137,120,'r');

	uint32_t timer_x = 100;

	for(int i=0; i<12; i++)
	{
		if (elapsedtimer[i] !='\0')
		{
			LCD_DisplayChar(timer_x, 140,elapsedtimer[i]);
			timer_x = timer_x + 16;
		}
		else
		{
			continue;
		}
	}
	LCD_DisplayChar(timer_x+16, 140, 's');

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void LCD_Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

// Touch Functionality   //

#if COMPILE_TOUCH_FUNCTIONS == 1

void InitializeLCDTouch(void)
{
  if(STMPE811_Init() != STMPE811_State_Ok)
  {
	 for(;;); // Hang code due to error in initialzation
  }
}

STMPE811_State_t returnTouchStateAndLocation(STMPE811_TouchData * touchStruct)
{
	return STMPE811_ReadTouch(touchStruct);
}

void DetermineTouchPosition(STMPE811_TouchData * touchStruct)
{
	STMPE811_DetermineTouchPosition(touchStruct);
}

uint8_t ReadRegisterFromTouchModule(uint8_t RegToRead)
{
	return STMPE811_Read(RegToRead);
}

void WriteDataToTouchModule(uint8_t RegToWrite, uint8_t writeData)
{
	STMPE811_Write(RegToWrite, writeData);
}

#endif // COMPILE_TOUCH_FUNCTIONS


