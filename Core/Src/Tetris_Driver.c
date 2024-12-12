/*
 * Tetris_Driver.c
 *
 *  Created on: Dec 4, 2024
 *      Author: iifba
 */
#include <Tetris_Driver.h>

char elapsedtimer[12] = {'\0'};
static uint32_t FallingBoard[11][11] ={0};
static uint32_t TetrisBoard[11][11] ={0};
static uint32_t EmptyBoard[11][11] ={0};
static uint8_t starting_posX = 3;
static uint8_t starting_posY = 0;
static uint8_t starting_rotation = 0;
static uint32_t removeTetrimino[4][4] = {0};
static uint32_t myTetrimino[4][4];
static uint8_t rnum;

void TetriminoAdd(uint32_t GameBoard[11][11])
{
    for (uint8_t i = 0; i < 4; i++)
    {
        for (uint8_t j = 0; j < 4; j++)
        {
            if (myTetrimino[i][j])
            {
                uint8_t xposition = starting_posX+j;
                uint8_t yposition = starting_posY+i;
                if (xposition <11)
                {
                	if(yposition<11)
                	{
                		GameBoard[yposition][xposition] = 1;
                	}
                }
            }
        }
    }
}

void TetriminoGenerate(){
	rnum = generateNum();
    memcpy(myTetrimino, Tetriminos[rnum][0], sizeof(myTetrimino));
}


void TetriminoRotate(){
	uint8_t previousRot = starting_rotation;
	starting_rotation++;
	if (starting_rotation >= 4)
	{
	        starting_rotation = 0;
	}
	memcpy(myTetrimino, removeTetrimino, sizeof(myTetrimino));
	memcpy(myTetrimino, Tetriminos[rnum][starting_rotation], sizeof(myTetrimino));
	bool collision = TetriminoCol();
	if (!collision)
	{
		memcpy(FallingBoard, TetrisBoard, sizeof(TetrisBoard));
		TetriminoAdd(FallingBoard);
		DrawBlock(TetrisBoard);
		DrawBlock(FallingBoard);
	} else {
		starting_rotation = previousRot;
		memcpy(myTetrimino, removeTetrimino, sizeof(myTetrimino));
		memcpy(myTetrimino, Tetriminos[rnum][starting_rotation], sizeof(myTetrimino));
	}
}


void TetriminoDrop(){
	uint8_t temp = starting_posY;
	starting_posY++;
	if (!TetriminoCol())
	{
		TetriminoAdd(FallingBoard);
	} else
	{
		starting_posY = temp;
		TetriminoAdd(TetrisBoard);
	}
}

bool TetriminoCol(){
	for (uint8_t y= 0; y < 4; y++)
	{
		for (uint8_t x = 0; x < 4; x++)
		{
			if (myTetrimino[y][x])
			{
				uint32_t xposition = starting_posX + x;
				uint32_t yposition = starting_posY + y;
				if ((yposition >= 11) ||xposition >= 11 )
				{
					memcpy(FallingBoard, EmptyBoard, sizeof(EmptyBoard));
					memcpy(TetrisBoard, EmptyBoard, sizeof(EmptyBoard));
					return 1;
				}
				if (TetrisBoard[yposition][xposition]!= 0)
				{
					return 1;
				}
			}
		}
	}
    return 0;
}

void ElapsedTime(uint32_t timestart, uint32_t timeend)
{
	if(timeend < timestart)
	{
		timeelapsed = timemax-timestart+timeend;
	} else
	{
		timeelapsed = timeend - timestart;
	}
	sprintf(elapsedtimer, "%d",timeelapsed/timedivisor);
}

void CollisionOperation(){
	TetriminoAdd(TetrisBoard);
	memcpy(FallingBoard, TetrisBoard, sizeof(TetrisBoard));
	starting_posY = 0;
	starting_rotation = 0;
}

bool TetrisNewBlock()
{
	TetriminoGenerate();
	if (TetriminoCol())
	{
		TetriminoAdd(TetrisBoard);
		DrawBlock(TetrisBoard);
		return true;
	} else {
		TetriminoAdd(FallingBoard);
		DrawBlock(FallingBoard);
	}
	return false;
}


void NoCollisionOperation(){
	memcpy(FallingBoard, TetrisBoard, sizeof(TetrisBoard));
	TetriminoDrop();
	if (!TetriminoCol())
	{
		DrawBlock(FallingBoard);
	}
}

bool TetrisTopRow()
{
	for(int i =0; i <11; i++)
	{
    	if(TetrisBoard[0][i])
    	{
    	    timeend = HAL_GetTick();
    	    ElapsedTime(timestart,timeend);
    	    return true;
    	}

	}
	return false;
}

void TetrisDelay(uint32_t delay){
	HAL_Delay(delay);
}

void TetriminoStart()
{
    timestart = HAL_GetTick();
    TetriminoGenerate();
    TetriminoAdd(FallingBoard);
    DrawBlock(FallingBoard);
    TetrisDelay(750);
    bool top_row = false;
    while(true)
    {
    	top_row = TetrisTopRow();
    	if(top_row)
    	{
    		return;
    	}
    	// check if collision
		if (TetriminoCol())
		{
			bool newblockcollision = false;
			CollisionOperation();
			newblockcollision = TetrisNewBlock();
			if(newblockcollision)
			{
				break;
			}
		}
		else
		{
			NoCollisionOperation();
		}
		TetrisDelay(750);
    }
    timeend = HAL_GetTick();
    ElapsedTime(timestart,timeend);
    End_Game_LCD(elapsedtimer);
}
