#pragma once
#include "segmentlcd.h"
#include "segmentlcd_individual.h"
#include "stdlib.h"

typedef struct //structban NEM lehet kezd� �rt�ket adni sadge
{
	uint8_t body[37];
	uint8_t size;
	enum direction { upup , downdown , leftleft , rightright, upleft, downleft, upright, downright, rightdown, leftdown, rightup, leftup } dir;
// dir els� tagja a m�ltbeli ir�ny, h�ts� tagja pedig a k�vetkez� �r�ny, ez egy�rtelm�en maghat�rozza a k�gy� ir�ny�t
	bool isAlive;
} snake;

snake SnakeInit(snake mysnake);
uint8_t PlaceFood(snake mysnake);
snake NextDirUART(snake mysnake, char uartdir);
snake NextDirNoUART(snake mysnake);
snake MoveSnake(snake mysnake, uint8_t *fruit);
SegmentLCD_LowerCharSegments_TypeDef SnakeandFoodtoLCD(snake thesnake, uint8_t food, SegmentLCD_LowerCharSegments_TypeDef *disp);
