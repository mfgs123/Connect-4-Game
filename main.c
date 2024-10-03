#include <stm32f031x6.h>
#include "display.h"

#define ROW 5
#define COL 7

void redOn(void);
void redOff(void);
void yellowOn(void);
void yellowOff(void);
int menu(int);
int printScreen(int);
int redcheck(int[][COL], int, int);
int redWin(int);
int yellowcheck(int[][COL], int, int);
int yellowWin(int);
int winPage(int, int, int);


void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);


volatile uint32_t milliseconds;


//sprites for pieces
const uint16_t yellowPiece[]=
{
0,0,0,0,0,24327,24327,24327,24327,24327,24327,0,0,0,0,0,0,0,0,24327,24327,24327,24327,24327,24327,24327,24327,24327,24327,0,0,0,0,0,24327,24327,24327,24327,24327,22021,22021,24327,24327,24327,24327,24327,0,0,0,24327,24327,24327,24327,22021,22021,22021,22021,22021,22021,24327,24327,24327,24327,0,0,24327,24327,24327,22021,22021,22021,22021,22021,22021,22021,22021,24327,24327,24327,0,24327,24327,24327,22021,22021,22021,22021,24327,24327,22021,22021,22021,22021,24327,24327,24327,24327,24327,24327,22021,22021,22021,22021,24327,24327,22021,22021,22021,22021,24327,24327,24327,24327,24327,22021,22021,22021,24327,24327,24327,24327,24327,24327,22021,22021,22021,24327,24327,24327,24327,22021,22021,22021,24327,24327,24327,24327,24327,24327,22021,22021,22021,24327,24327,24327,24327,24327,22021,22021,22021,22021,24327,24327,22021,22021,22021,22021,24327,24327,24327,24327,24327,24327,22021,22021,22021,22021,24327,24327,22021,22021,22021,22021,24327,24327,24327,0,24327,24327,24327,22021,22021,22021,22021,22021,22021,22021,22021,24327,24327,24327,0,0,24327,24327,24327,24327,22021,22021,22021,22021,22021,22021,24327,24327,24327,24327,0,0,0,24327,24327,24327,24327,24327,22021,22021,24327,24327,24327,24327,24327,0,0,0,0,0,24327,24327,24327,24327,24327,24327,24327,24327,24327,24327,0,0,0,0,0,0,0,0,24327,24327,24327,24327,24327,24327,0,0,0,0,0,
};
const uint16_t redPiece[]=
{
0,0,0,0,0,61456,61456,61456,61456,61456,61456,0,0,0,0,0,0,0,0,61456,61456,61456,61456,61456,61456,61456,61456,61456,61456,0,0,0,0,0,61456,61456,61456,61456,61456,3088,3088,61456,61456,61456,61456,61456,0,0,0,61456,61456,61456,61456,3088,3088,3088,3088,3088,3088,61456,61456,61456,61456,0,0,61456,61456,61456,3088,3088,3088,3088,3088,3088,3088,3088,61456,61456,61456,0,61456,61456,61456,3088,3088,3088,3088,61456,61456,3088,3088,3088,3088,61456,61456,61456,61456,61456,61456,3088,3088,3088,3088,61456,61456,3088,3088,3088,3088,61456,61456,61456,61456,61456,3088,3088,3088,61456,61456,61456,61456,61456,61456,3088,3088,3088,61456,61456,61456,61456,3088,3088,3088,61456,61456,61456,61456,61456,61456,3088,3088,3088,61456,61456,61456,61456,61456,3088,3088,3088,3088,61456,61456,3088,3088,3088,3088,61456,61456,61456,61456,61456,61456,3088,3088,3088,3088,61456,61456,3088,3088,3088,3088,61456,61456,61456,0,61456,61456,61456,3088,3088,3088,3088,3088,3088,3088,3088,61456,61456,61456,0,0,61456,61456,61456,61456,3088,3088,3088,3088,3088,3088,61456,61456,61456,61456,0,0,0,61456,61456,61456,61456,61456,3088,3088,61456,61456,61456,61456,61456,0,0,0,0,0,61456,61456,61456,61456,61456,61456,61456,61456,61456,61456,0,0,0,0,0,0,0,0,61456,61456,61456,61456,61456,61456,0,0,0,0,0,
};


int main()
{
	int hinverted = 0;
	int vinverted = 0;
	int hmoved = 1;
	int vmoved = 0;
	int i, j;
	uint16_t playerx = 4;
	uint16_t playery = 141;
	uint16_t piecex = 0;
	uint16_t piecey = 0;
	uint16_t oldx = playerx;
	uint16_t oldy = playery;

	//2D array for board (empty)
	int board[ROW][COL] = { 0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0,
							0, 0, 0, 0, 0, 0, 0};
	
	//first row -> x = 32, second row -> x = 52, third row -> x = 72, fourth row -> x = 92, fifth row -> 112
	//first column -> y=142, second column -> y=118 , third column -> y=94, fourth column -> y=71, fifth column -> y=49, sixth column -> y=25, seventh column -> y=1 


	//menu (0=menu screen, 1=red win/restart screen, 2=yellow win/restart screen, 3=play, 4=overall winner)
	int play = 0;

	//variable used to determine whether the grid should be printed
	int screen = 0;

	//turn counter 
	//(if turn%2 == 1), reds turn (always starts on red)
	//else, yellows turn
	int turn = 1;
	int totalMoves = 0;

	//keeping track of where piece will be placed
	int columnNum = 0;
	int row = 0;

	//keeping track of individual player wins
	int redWins = 0;
	int yellowWins = 0;


	initClock();
	initSysTick();
	setupIO();

	
	while(1)
	{
		//checking if menu screen should be show
		if (play == 0)
		{
			play = menu(play);

			//setting the player turn to red
			turn = 1;

			//allowing for screen to print again if replaying
			screen = 0; //grid will only reset when screen is 0

			//resetting number of wins
			redWins = 0;
			yellowWins = 0;

		}

		//checking if overall winner screen should be show
		if (play == 4)
		{
			play = winPage(redWins, yellowWins, play);

		}


		//play screen
		while (play == 3)
		{
			if (screen == 0)
			{
				///setting/resetting board at the beginning of each play
				for (i=0; i<ROW; i++)
				{
					for (j=0; j<COL; j++)
					{
						board[i][j] = 0;
					}
				}

				screen = printScreen(screen);

			}

			//deciding which sprite to print
			if (turn%2 == 1)
			{
				putImage(playerx,playery,16,16,redPiece,0,0);
				redOn();//turning on red led
			}
			else
			{
				yellowOn(); // turning on yellow led
				putImage(playerx,playery,16,16,yellowPiece,0,0);
			}
			
		
			hmoved = vmoved = 0;
			hinverted = vinverted = 0;


			//moving the sprite left 
			if ( (GPIOA->IDR & (1 << 11)) == 0) //left pressed
			{
				
				//moving the sprite by 23px
				if (playery < 140)
				{

					playery = playery + 23;			
					vmoved = 1;
					vinverted = 0;

					//keeping track of where piece is prior to placement
					if (columnNum != 0)
					{
						columnNum--; //keep moving one column to left until it reaches first column
					}

				} //end if
				
			} //end left button if

			//moving the sprite right
			if ( (GPIOA->IDR & (1 << 8)) == 0) //right pressed
			{		
				
				//moving the sprite by 23px
				if (playery > 16) 
				{

					playery = playery - 23;
					vmoved = 1;
					vinverted = 1;

					//keeping track of where piece is prior to placement
					if (columnNum < 7)
					{
						columnNum++;
					}

				} //end if
				
			} //end right button if

			if ((vmoved) || (hmoved))
			{

				// only redraw if there has been some movement (reduces flicker)
				fillRectangle(oldx,oldy,16,16,0);
				oldx = playerx;
				oldy = playery;					
				
				//deciding which sprite to move
				if (turn%2 == 1)
				{
					putImage(playerx,playery,16,16,redPiece,hinverted,0);
				}
				else
				{
					putImage(playerx,playery,16,16,yellowPiece,hinverted,0);
				}

			} //end flicker if	

			//moving the sprite down
			if ((GPIOB->IDR & (1 << 4))==0) // down pressed
			{		
				
				if (playerx < 140)
				{
					hmoved = 1;
					hinverted=0;

					row = 0;
					
					//finding empty spot to place the piece
					while ((row<5) && board[row][columnNum] == 0)
					{
						row++;
					}
					row--;
					

					//placing a red piece
					if (turn%2 == 1)
					{
						board[row][columnNum] = 1;

						//finding coordinates for the row
						if (row == 0)
						{
							piecex = 32;
						}
						else if (row == 1)
						{
							piecex = 52;
						}
						else if (row == 2)
						{
							piecex = 72;
						}
						else if (row == 3)
						{
							piecex = 92;
						}
						else
						{
							piecex = 112;
						}

						//finding coordinates for the column
						if (columnNum == 0)
						{
							piecey = 142;
						}
						else if (columnNum == 1)
						{
							piecey = 118;
						}
						else if (columnNum == 2)
						{
							piecey = 94;
						}
						else if (columnNum == 3)
						{
							piecey = 71;
						}
						else if (columnNum == 4)
						{
							piecey = 49;
						}
						else if (columnNum == 5)
						{
							piecey = 25;
						}
						else
						{
							piecey = 1;
						}

						putImage(piecex, piecey, 16, 16, redPiece, 0, 0);

						play = redcheck(board, row, columnNum);

						redOff();

					} //end red if

					//placing a yellow piece
					else
					{
						board[row][columnNum] = 2;

						//finding coordinates for the row
						if (row == 0)
						{
							piecex = 32;
						}
						else if (row == 1)
						{
							piecex = 52;
						}
						else if (row == 2)
						{
							piecex = 72;
						}
						else if (row == 3)
						{
							piecex = 92;
						}
						else
						{
							piecex = 112;
						}

						//finding coordinates for the column
						if (columnNum == 0)
						{
							piecey = 142;
						}
						else if (columnNum == 1)
						{
							piecey = 118;
						}
						else if (columnNum == 2)
						{
							piecey = 94;
						}
						else if (columnNum == 3)
						{
							piecey = 71;
						}
						else if (columnNum == 4)
						{
							piecey = 49;
						}
						else if (columnNum == 5)
						{
							piecey = 25;
						}
						else
						{
							piecey = 1;
						}

						putImage(piecex, piecey, 16, 16, yellowPiece, 0, 0);

						play = yellowcheck(board, row, columnNum);

						yellowOff();
						
					} //end yellow else

					turn++;
					totalMoves++;

					//if totalMoves has reached 36, return to menu screen (board has been filled)
					if (totalMoves == 36)
					{
						play = 0;
					}

				} //end if
					
			} //end down button if

			//delay to slow down registering of button presses
			delay(110);
		}

		//checking to see if red win screen should show
		if (play == 1)
		{
			//resetting screen for reprinting if game is replayed
			screen = 0;

			//incrementing for each additional win
			redWins++;
			
			//printing the red win screen
			play = redWin(play);
		}

		//checking to see if yellow win screen should show
		if (play == 2)
		{
			//resetting screen for reprinting if game is replayed
			screen = 0;

			//incrementing for each additional win
			yellowWins++;

			//printing the yellow win screen
			play = yellowWin(play);
		}

	} //end while

	return 0;

} //end main()


//turning on the red LED
void redOn(void)
{
	uint32_t original;
    original = GPIOA->ODR;
    original = original | 1; 
    GPIOA->ODR = original;
} //end redOn()


//turning off the red LED
void redOff(void)
{
	uint32_t original;
    original = GPIOA->ODR;
    original = original & ~(1);
    GPIOA->ODR = original;
} //end redOff()


//turning on the yellow LED
void yellowOn(void)
{
	GPIOA->ODR = GPIOA->ODR | (1<<0);
} //end yellowOn()


//turning off the yellow LED
void yellowOff(void)
{
	uint32_t original;
    original = GPIOA->ODR;
    original = original & ~(2);
    GPIOA->ODR = original;
} //end yellowOff()


//printing the menu screen
int menu(int play)
{
	int flick = 0;

	//clearing the screen (doesn't work unless it's formatted wrong for some reason?)
	while (flick = 0)
	{
		fillRectangle(0,0,128, 160, 0x0); 
		flick = 1;
	}

	//menu screen
	printTextX2("Connect4", 17, 20, RGBToWord(0xff,0x00,0), 0);
	printTextX2("Press up", 17, 60, RGBToWord(0xff,0xff,0), 0);
	printTextX2("to play!", 17, 80, RGBToWord(0xff,0xff,0), 0);

	//change to play screen by pressing up button
	if ((GPIOB->IDR & (1 << 5))==0) // up pressed
	{			
		//triggering switch to play screen
		play = 3;

	} //end up button if

	return play;

} //end menu()


int printScreen(int screen)
{
	screen = 1;

	//clearing the screen
	fillRectangle(0,0,128, 160, 0x0); 

	//drawing the grid
	//horizontal lines
	drawRectangle(29, 0, 1, 180, 0xff);
	drawRectangle(49, 0, 1, 180, 0xff);
	drawRectangle(69, 0, 1, 180, 0xff);
	drawRectangle(89, 0, 1, 180, 0xff);
	drawRectangle(109, 0, 1, 180, 0xff);
	//vertical lines
	drawRectangle(29, 20, 105, 1, 0xff);
	drawRectangle(29, 44, 105, 1, 0xff);
	drawRectangle(29, 68, 105, 1, 0xff);
	drawRectangle(29, 89, 105, 1, 0xff);
	drawRectangle(29, 113, 105, 1, 0xff);
	drawRectangle(29, 137, 105, 1, 0xff);

	return screen;

} //end printScreen()


//checking to see if red player has connected 4
int redcheck(int board[][COL], int row, int columnNum)
{

	//checking pieces along horizontal (beginning with left)
	//if columnNum isn't 0 and 2nd piece to the left is also red
	if ((columnNum >= 1) && (board[row][columnNum-1] == 1))
	{

		//if columnNum isn't 1 or 0 and 3rd piece to the left is also red
		if((columnNum >= 2) && (board[row][columnNum-2] == 1))
		{

			//if 4th piece to the left is also red (AKA red win along horizontal, 3 left of og piece)
			if((board[row][columnNum-3] == 1))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//else 4th piece to the left is not red, check 1st piece to the right
			else
			{

				//if 1st piece to the right is red (AKA red win along horizontal, 2 left and 1 right of og piece)
				if((board[row][columnNum+1] == 1))
				{
					//clearing the screen
					fillRectangle(0,0,128, 160, 0x0); 
					return 1;
				}

				//if not entered -> 1st piece to the right is not red, start vertical check

			}
		}

		//if columnNum isn't 5 or 6 and 3rd piece to the left isn't red
		if (((columnNum < 5)) && (board[row][columnNum-2] != 1))
		{

			//if columnNum isn't 6 and 1st piece to the right is red
			if ((columnNum != 6) && (board[row][columnNum+1] == 1))
			{

				//if 2nd piece to the right is red (AKA red win along horizontal, 1 left and 2 right of og piece
				if (board[row][columnNum+2] == 1)
				{
					//clearing the screen
					fillRectangle(0,0,128, 160, 0x0); 
					return 1;
				}

				//if not entered -> 2nd piece to the right is not red, start vertical check
		
			}

			//if not entered -> 1st piece to the right is not red, start vertical check

		}

	} //end left check

	//if first left piece isn't red
	//if columnNum isn't 6 and 2nd piece to the right is also red
	else if((columnNum != 6) && (board[row][columnNum+1]==1))
	{
		//if columnNum isn't 5 or 6 and 3rd piece to the right is also red
		if ((columnNum < 5) && (board[row][columnNum+2])==1)
		{

			//if 4th piece to the right is also red (AKA red win along horizontal, 3 right of og piece)
			if ((columnNum < 4) && (board[row][columnNum+3])==1)
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//if not entered -> 4th piece to the left is not red, start vertical check

		}

	} //end right check


	//checking pieces along vertical (starting below)
	//if row isn't 0 and 2nd piece below is red
	if ((row >= 1) && (board[row-1][columnNum] == 1))
	{

		//if row isn't 1 or 0 and 3rd below is red
		if((row >= 2) && (board[row-2][columnNum] == 1))
		{

			//if 4th piece below is also red (AKA red win along vertical, 3 below original piece)
			if((board[row-3][columnNum] == 1))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//else 4th piece below is not red, check 1st piece above
			//if 1st above is red (AKA red win along horizontal, 2 below and 1 above original piece)
			else if((board[row+1][columnNum] == 1))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//if not entered -> 1st piece above is not red, start perpendicular check
	
		}

		//if row isn't 3 or 4 and 3rd piece below isn't red
		if (((row < 3)) && (board[row-2][columnNum] != 1))
		{

			//if row isn't 4 and 1st piece above is red
			if ((row != 4) && (board[row+1][columnNum] == 1))
			{

				//if 2nd piece above is red (AKA red win along vertical, 1 below and 2 above original piece)
				if (board[row+2][columnNum] == 1)
				{
					//clearing the screen
					fillRectangle(0,0,128, 160, 0x0); 
					return 1;
				}

				//if not entered -> 2nd piece above is not red, start diagonal check

			}

			//if not entered -> 1st piece above is not red, start diagonal check

		}

	} //end below check

	//if below isn't red
	//if row isn't 4 and 2nd piece above is also red
	else if((row != 4) && (board[row+1][columnNum]==1))
	{
		//if columnNum isn't 3 or 4 and 3rd piece above is also red
		if ((row < 3) && (board[row+2][columnNum])==1)
		{

			//if 4th piece above is also red (AKA red win along horizontal, 3 above original piece)
			if ((row < 2) && (board[row+3][columnNum])==1)
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//if not entered -> 4th piece above is not red, start diagonal check

		}

	} //end above check


	//checking pieces along diagonal left to right
	//if row isn't 0, columnNum isn't 0 and 2nd piece left below is red
	if ((row != 0) && (columnNum != 0) && (board[row-1][columnNum-1] == 1))
	{

		//if row isn't 1 or 0, columnNum isn't 1 or 0 and 3rd below is red
		if((row > 1) && (columnNum > 1) && (board[row-2][columnNum-2] == 1))
		{

			//if 4th piece below is also red (AKA red win along diagonal, 3 left below original piece)
			if((board[row-3][columnNum-3] == 1))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//else 4th piece left below is not red, check 1st piece right above
			//if 1st right above is red (AKA red win along diagonal, 2 left below and 1 right above original piece)
			else if((board[row+1][columnNum+1] == 1))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//if not entered -> 1st piece right above is not red, check other

		}
		
		//if row isn't 4, columnNum isn't 6 and 1st piece right above is red
		if ((row != 4) && (columnNum != 6) && (board[row+1][columnNum+1] == 1))
		{

			//if 2nd piece right above is red (AKA red win diagonal, 1 left below and 2 right above original piece)
			if (board[row+2][columnNum+2] == 1)
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//if not entered -> 2nd piece right above is not red, check other direction

		}

		//if not entered -> 1st piece right above is not red, check other direction
		

	} //end left below check

	//if left below isn't red
	//if row isn't 4, columnNum isn't 6 and 2nd piece right above is also red
	else if((row != 4) && (columnNum != 6) && (board[row+1][columnNum+1]==1))
	{
		//if columnNum isn't 3 or 4, columnNum isn't 5 or 6 and 3rd piece right above is also red
		if ((row < 3) && (columnNum < 5) && (board[row+2][columnNum+2])==1)
		{

			//if 4th piece above is also red (AKA red win along diagonal, 3 right above original piece)
			if ((row < 2) && (columnNum < 4) && (board[row+3][columnNum+3])==1)
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//if not entered -> 4th piece right above is not red, check other direction

		}

	} //end right above check


	//checking pieces along diagonal right to left
	//if row isn't 0, columnNum isn't 6 and 2nd piece right below is red
	if ((row != 0) && (columnNum != 6) && (board[row-1][columnNum+1] == 1))
	{

		//if row isn't 1 or 0, columnNum isn't 5 or 6 and 3rd right below is red
		if((row >= 2) && (columnNum < 5) && (board[row-2][columnNum+2] == 1))
		{

			//if 4th piece right below is also red (AKA red win along diagonal, 3 right below original piece)
			if((board[row-3][columnNum+3] == 1))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//else 4th piece left below is not red, check 1st piece right above
			//if 1st left above is red (AKA red win along diagonal, 2 right below and 1 left above original piece)
			else if((board[row+1][columnNum-1] == 1))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//if not entered -> 1st piece left above is not red, no win this turn
			else
			{
				return 3;
			}

		}

		//if row isn't 3 or 4, columnNum isn't 5 or 6 and 3rd piece right below isn't red
		if ((row < 3) && (columnNum < 5) && (board[row-2][columnNum+2] != 1))
		{

			//if row isn't 4, columnNum isn't 6 and 1st piece left above is red
			if ((row != 4) && (columnNum != 6) && (board[row+1][columnNum-1] == 1))
			{

				//if 2nd piece left above is red (AKA red win diagonal, 1 right below and 2 left above original piece)
				if (board[row+2][columnNum-2] == 1)
				{
					//clearing the screen
					fillRectangle(0,0,128, 160, 0x0); 
					return 1;
				}

				//if not entered -> 2nd piece right above is not red, no win this turn
				else
				{
					return 3;
				}

			}

			//if not entered -> 1st piece right above is not red, no win this turn
			else
			{
				return 3;
			}

		}

	} //end left below check

	//if right below isn't red
	//if row isn't 4, columnNum isn't 0 and 2nd piece left above is also red
	else if((row != 4) && (columnNum != 0) && (board[row+1][columnNum-1]==1))
	{
		//if columnNum isn't 3 or 4, columnNum isn't 0 or 1 and 3rd piece left above is also red
		if ((row < 3) && (columnNum > 1) && (board[row+2][columnNum-2])==1)
		{

			//if 4th piece left above is also red (AKA red win along diagonal, 3 left above original piece)
			if ((row < 2) && (columnNum > 2) && (board[row+3][columnNum-3])==1)
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 1;
			}

			//if not entered -> 4th piece right above is not red, no win this round
			else
			{
				return 3;
			}

		}

	} //end left above check

	else
	{
		return 3;
	}

} //end redcheck()


//printing red win screen
int redWin(int play)
{

	int flick = 0;

	//clearing the screen (doesn't work unless it's formatted wrong for some reason?)
	while (flick = 0)
	{
		fillRectangle(0,0,128, 160, 0x0); 
		flick = 1;
	}

	//win screen text
	printTextX2("Red", 46, 15, RGBToWord(0xff,0x00,0), 0);
	printTextX2("Player", 30, 40, RGBToWord(0xff,0x00,0), 0);
	printTextX2("Wins!", 37, 65, RGBToWord(0xff,0x00,0), 0);
	printText("Press Up To", 25, 90, RGBToWord(0xff,0x00,0), 0);
	printText("See Winner", 25, 105, RGBToWord(0xff,0x00,0), 0);
	printText("Press Down To", 18, 125, RGBToWord(0xff,0x00,0), 0);
	printText("Play Again", 28, 140, RGBToWord(0xff,0x00,0), 0);


	//change to winner screen by pressing up button
	if ((GPIOB->IDR & (1 << 5))==0) // up pressed
	{
		//triggering switch to winner screen
		play = 4;

		//clearing the screen 
		fillRectangle(0,0,128, 160, 0x0); 

		delay(120);

	} //end up button if

	//change to play screen by pressing down button
	if ((GPIOB->IDR & (1 << 4))==0) // up pressed
	{
		//triggering switch to play screen
		play = 3;

		//clearing the screen 
		fillRectangle(0,0,128, 160, 0x0); 

		delay(120);

	} //end up button if

	return play;

} //end redWin()

//checking to see if yellow piece has connected 4
int yellowcheck(int board[][COL], int row, int columnNum)
{
	//checking pieces along horizontal (beginning with left)
	//if columnNum isn't 0 and 2nd piece to the left is also yellow
	if ((columnNum >= 1) && (board[row][columnNum-1] == 2))
	{

		//if columnNum isn't 1 or 0 and 3rd piece to the left is also yellow
		if((columnNum >= 2) && (board[row][columnNum-2] == 2))
		{

			//if 4th piece to the left is also yellow (AKA yellow win along horizontal, 3 left of original piece)
			if((board[row][columnNum-3] == 2))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//else 4th piece to the left is not yellow, check 1st piece to the right
			else
			{

				//if 1st piece to the right is yellow (AKA yellow win along horizontal, 2 left and 1 right of original piece)
				if((board[row][columnNum+1] == 2))
				{
					//clearing the screen
					fillRectangle(0,0,128, 160, 0x0); 
					return 2;
				}

				//if not entered -> 1st piece to the right is not yellow, start vertical check

			}
		}

		//if columnNum isn't 5 or 6 and 3rd piece to the left isn't yellow
		if (((columnNum < 5)) && (board[row][columnNum-2] != 2))
		{

			//if columnNum isn't 6 and 1st piece to the right is yellow
			if ((columnNum != 6) && (board[row][columnNum+1] == 2))
			{

				//if 2nd piece to the right is yellow (AKA yellow win along horizontal, 1 left and 2 right of original piece
				if (board[row][columnNum+2] == 2)
				{
					//clearing the screen
					fillRectangle(0,0,128, 160, 0x0); 
					return 2;
				}

				//if not entered -> 2nd piece to the right is not yellow, start vertical check
		
			}

			//if not entered -> 1st piece to the right is not yellow, start vertical check

		}

	} //end left check

	//if first left piece isn't yellow
	//if columnNum isn't 6 and 2nd piece to the right is also yellow
	else if((columnNum != 6) && (board[row][columnNum+1]==2))
	{
		//if columnNum isn't 5 or 6 and 3rd piece to the right is also yellow
		if ((columnNum < 5) && (board[row][columnNum+2])==2)
		{

			//if 4th piece to the right is also yellow (AKA yellow win along horizontal, 3 right of og piece)
			if ((columnNum < 4) && (board[row][columnNum+3])==2)
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//if not entered -> 4th piece to the left is not yellow, start vertical check

		}

	} //end right check


	//checking pieces along vertical (starting below)
	//if row isn't 0 and 2nd piece below is yellow
	if ((row >= 1) && (board[row-1][columnNum] == 2))
	{

		//if row isn't 1 or 0 and 3rd below is yellow
		if((row >= 2) && (board[row-2][columnNum] == 2))
		{

			//if 4th piece below is also yellow (AKA yellow win along vertical, 3 below original piece)
			if((board[row-3][columnNum] == 2))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//else 4th piece below is not yellow, check 1st piece above
			//if 1st above is yellow (AKA yellow win along horizontal, 2 below and 1 above original piece)
			else if((board[row+1][columnNum] == 2))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//if not entered -> 1st piece above is not yellow, start diagonal check
	
		}

		//if row isn't 3 or 4 and 3rd piece below isn't yellow
		if (((row < 3)) && (board[row-2][columnNum] != 2))
		{

			//if row isn't 4 and 1st piece above is yellow
			if ((row != 4) && (board[row+1][columnNum] == 2))
			{

				//if 2nd piece above is yellow (AKA yellow win along vertical, 1 below and 2 above original piece)
				if (board[row+2][columnNum] == 2)
				{
					//clearing the screen
					fillRectangle(0,0,128, 160, 0x0); 
					return 2;
				}

				//if not entered -> 2nd piece above is not red, start diagonal check

			}

			//if not entered -> 1st piece above is not red, start diagonal check

		}

	} //end below check

	//if below isn't red
	//if row isn't 4 and 2nd piece above is also yellow
	else if((row != 4) && (board[row+1][columnNum]==2))
	{
		//if columnNum isn't 3 or 4 and 3rd piece above is also yellow
		if ((row < 3) && (board[row+2][columnNum])==2)
		{

			//if 4th piece above is also yellow (AKA yellow win along horizontal, 3 above original piece)
			if ((row < 2) && (board[row+3][columnNum])==2)
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//if not entered -> 4th piece above is not yellow, start diagonal check

		}

	} //end above check


	//checking pieces along diagonal left to right
	//if row isn't 0, columnNum isn't 0 and 2nd piece left below is yellow
	if ((row != 0) && (columnNum != 0) && (board[row-1][columnNum-1] == 2))
	{

		//if row isn't 1 or 0, columnNum isn't 1 or 0 and 3rd below is yellow
		if((row > 1) && (columnNum > 1) && (board[row-2][columnNum-2] == 2))
		{

			//if 4th piece below is also yellow (AKA yellow win along diagonal, 3 left below original piece)
			if((board[row-3][columnNum-3] == 2))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//else 4th piece left below is not red, check 1st piece right above
			//if 1st right above is yellow (AKA yellow win along diagonal, 2 left below and 1 right above original piece)
			else if((board[row+1][columnNum+1] == 2))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//if not entered -> 1st piece right above is not yellow, check other

		}
		
		//if row isn't 4, columnNum isn't 6 and 1st piece right above is yellow
		if ((row != 4) && (columnNum != 6) && (board[row+1][columnNum+1] == 2))
		{

			//if 2nd piece right above is yellow (AKA yellow win diagonal, 1 left below and 2 right above original piece)
			if (board[row+2][columnNum+2] == 2)
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//if not entered -> 2nd piece right above is not yellow, check other direction

		}

		//if not entered -> 1st piece right above is not yellow, check other direction

	} //end left below check

	//if left below isn't yellow
	//if row isn't 4, columnNum isn't 6 and 2nd piece right above is also yellow
	else if((row != 4) && (columnNum != 6) && (board[row+1][columnNum+1]==2))
	{
		//if columnNum isn't 3 or 4, columnNum isn't 5 or 6 and 3rd piece right above is also yellow
		if ((row < 3) && (columnNum < 5) && (board[row+2][columnNum+2])==2)
		{

			//if 4th piece above is also yellow (AKA yellow win along diagonal, 3 right above original piece)
			if ((row < 2) && (columnNum < 4) && (board[row+3][columnNum+3])==2)
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//if not entered -> 4th piece right above is not yellow, check other direction

		}

	} //end right above check


	//checking pieces along diagonal right to left
	//if row isn't 0, columnNum isn't 6 and 2nd piece right below is yellow
	if ((row != 0) && (columnNum != 6) && (board[row-1][columnNum+1] == 2))
	{

		//if row isn't 1 or 0, columnNum isn't 5 or 6 and 3rd right below is yellow
		if((row >= 2) && (columnNum < 5) && (board[row-2][columnNum+2] == 2))
		{

			//if 4th piece right below is also yellow (AKA yellow win along diagonal, 3 right below original piece)
			if((board[row-3][columnNum+3] == 2))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//else 4th piece left below is not yellow, check 1st piece right above
			//if 1st left above is yellow (AKA yellow win along diagonal, 2 right below and 1 left above original piece)
			else if((board[row+1][columnNum-1] == 2))
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//if not entered -> 1st piece left above is not yellow, no win this turn
			else
			{
				return 3;
			}

		}

		//if row isn't 3 or 4, columnNum isn't 5 or 6 and 3rd piece right below isn't yellow
		if ((row < 3) && (columnNum < 5) && (board[row-2][columnNum+2] != 2))
		{

			//if row isn't 4, columnNum isn't 6 and 1st piece left above is yellow
			if ((row != 4) && (columnNum != 6) && (board[row+1][columnNum-1] == 2))
			{

				//if 2nd piece left above is yellow (AKA yellow win diagonal, 1 right below and 2 left above original piece)
				if (board[row+2][columnNum-2] == 2)
				{
					//clearing the screen
					fillRectangle(0,0,128, 160, 0x0); 
					return 2;
				}

				//if not entered -> 2nd piece right above is not yellow, no win this turn
				else
				{
					return 3;
				}

			}

			//if not entered -> 1st piece right above is not yellow, no win this turn
			else
			{
				return 3;
			}

		}

	} //end left below check

	//if right below isn't yellow
	//if row isn't 4, columnNum isn't 0 and 2nd piece left above is also yellow
	else if((row != 4) && (columnNum != 0) && (board[row+1][columnNum-1]==2))
	{
		//if columnNum isn't 3 or 4, columnNum isn't 0 or 1 and 3rd piece left above is also yellow
		if ((row < 3) && (columnNum > 1) && (board[row+2][columnNum-2])==2)
		{

			//if 4th piece left above is also yellow (AKA yellow win along diagonal, 3 left above original piece)
			if ((row < 2) && (columnNum > 2) && (board[row+3][columnNum-3])==2)
			{
				//clearing the screen
				fillRectangle(0,0,128, 160, 0x0); 
				return 2;
			}

			//if not entered -> 4th piece right above is not red, no win this round
			else
			{
				return 3;
			}

		}

	} //end left above check

	else
	{
		return 3;
	}

} //end yellowcheck


//printing red win screen
int yellowWin(int play)
{

	int flick = 0;

	//clearing the screen (doesn't work unless it's formatted wrong for some reason?)
	while (flick = 0)
	{
		fillRectangle(0,0,128, 160, 0x0); 
		flick = 1;
	}

	//win screen text
	printTextX2("Yellow", 28, 15, RGBToWord(0xff,0xff,0), 0);
	printTextX2("Player", 30, 40, RGBToWord(0xff,0xff,0), 0);
	printTextX2("Wins!", 37, 65, RGBToWord(0xff,0xff,0), 0);
	printText("Press Up To", 25, 90, RGBToWord(0xff,0xff,0), 0);
	printText("See Winner", 15, 105, RGBToWord(0xff,0xff,0), 0);
	printText("Press Down To", 18, 125, RGBToWord(0xff,0xff,0), 0);
	printText("Play Again", 28, 140, RGBToWord(0xff,0xff,0), 0);


	//change to winner screen by pressing up button
	if ((GPIOB->IDR & (1 << 5))==0) // up pressed
	{			
		//triggering switch to winner screen
		play = 4;

		//clearing the screen
		fillRectangle(0,0,128, 160, 0x0); 

		delay(120);

	} //end up button if

	//change to play screen by pressing down button
	if ((GPIOB->IDR & (1 << 4))==0) // up pressed
	{
		//triggering switch to play screen
		play = 3;

		//clearing the screen 
		fillRectangle(0,0,128, 160, 0x0); 

		delay(120);

	} //end up button if

	return play;

} //end yellowWin()


//displaying the overall winner
int winPage(int red, int yellow, int play)
{

	if (red>yellow)
	{
		printTextX2("Red is the", 10, 20, RGBToWord(0xff,0x00,0), 0);
		printTextX2("overall", 25, 45, RGBToWord(0xff,0x00,0), 0);
		printTextX2("winner!", 25, 70, RGBToWord(0xff,0x00,0), 0);
		printText("Press Up To", 25, 100, RGBToWord(0xff,0x00,0), 0);
		printText("Return To Menu", 15, 120, RGBToWord(0xff,0x00,0), 0);
	}
	else
	{
		printTextX2("Yellow is the", 5, 20, RGBToWord(0xff,0x00,0), 0);
		printTextX2("overall", 25, 45, RGBToWord(0xff,0xff,0), 0);
		printTextX2("winner!", 25, 70, RGBToWord(0xff,0xff,0), 0);
		printText("Press Up To", 25, 100, RGBToWord(0xff,0xff,0), 0);
		printText("Return To Menu", 15, 120, RGBToWord(0xff,0xff,0), 0);
	}

	//returning to menu
	if ((GPIOB->IDR & (1 << 5))==0) // up pressed
	{
		//triggering switch to play screen
		play = 0;

		//clearing the screen 
		fillRectangle(0,0,128, 160, 0x0); 

		delay(120);

	} //end up button if

	return play;

} //end winPage()


void initSysTick(void)
{

	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts

} //end initSysTick()


//any background processes in here?
void SysTick_Handler(void)
{

	milliseconds++;

} //end SysTick_Handler()


void initClock(void)
{
	// This is potentially a dangerous function as it could result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
	// Warning here: if system clock is greater than 24MHz then wait-state(s) need to be inserted into Flash memory interface
				
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));

        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);

        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) ); 

        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);

        // and turn the PLL back on again
        RCC->CR |= (1<<24);        

        // set PLL as system clock source 
        RCC->CFGR |= (1<<1);

} //end initClock()

void delay(volatile uint32_t dly)
{

	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep

} //end delay()

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{

	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit

} //end enablePullUp()


void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{

	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;

} //end pinMode()


int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{

	// checks to see if point px,py is within the rectangle defined by x,y,w,h
	uint16_t x2,y2;
	x2 = x1+w;
	y2 = y1+h;
	int rvalue = 0;

	if ( (px >= x1) && (px <= x2))
	{

		// ok, x constraint met
		if ((py >= y1) && (py <= y2))
		{

			rvalue = 1;

		} //end if

	} //end if

	return rvalue;

} //end isInside()


void setupIO()
{

	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	display_begin();
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	pinMode(GPIOA,0,1);
	pinMode(GPIOA,1,1);
	enablePullUp(GPIOA,0);
	enablePullUp(GPIOA,1);
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);

} //end setupIO()