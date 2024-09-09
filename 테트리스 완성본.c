#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#include "block.h"
#pragma warning(disable:4996)

#define LEFT 75
#define RIGHT 77
#define UP 72
#define SPACE 32
#define ENTER 13
#define ESC 27

#define GBOARD_WIDTH 10
#define GBOARD_HEIGHT 20

#define GBOARD_ORIGIN_X 4
#define GBOARD_ORIGIN_Y 2

#define R 114  // reverse, 방향키 반대로 적용


int block_id, curPosX, curPosY, speed, clear_num = 0, reverse_flag = 0;
int gameBoardInfo[GBOARD_HEIGHT + 1][GBOARD_WIDTH + 2];

void SetCurrentCursorPos(int x, int y)
{
	COORD pos = { x,y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


COORD GetCurrentCursorPos(void)
{
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO curInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);

	curPoint.X = curInfo.dwCursorPosition.X;
	curPoint.Y = curInfo.dwCursorPosition.Y;

	return curPoint;
}

void ShowBlock(char blockInfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();

	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (blockInfo[y][x] == 1)
			{
				printf("■");
			}
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void DeleteBlock(char blockInfo[4][4])
{
	int x, y;
	COORD curPos = GetCurrentCursorPos();

	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			SetCurrentCursorPos(curPos.X + (x * 2), curPos.Y + y);

			if (blockInfo[y][x] == 1)
			{
				printf("  ");
			}
		}
	}
	SetCurrentCursorPos(curPos.X, curPos.Y);
}

void RemoveCursor(void) // 깜박이는 커서 제거 함수
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

int DetectCollision(int posX, int posY, char blockModel[4][4]) // 충돌 -> 0 / 충돌X -> 1
{
	int x, y;
	int arrX = (posX - GBOARD_ORIGIN_X) / 2;
	int arrY = posY - GBOARD_ORIGIN_Y;

	for (x = 0; x < 4; x++)
	{
		for (y = 3; y >= 0; y--) // 밑에서부터 훑기
		{
			if (blockModel[y][x] == 1)
			{
				if (arrX + x <= 0 || arrX + x >= (GBOARD_WIDTH+1) || arrY + y >= GBOARD_HEIGHT) 
					// 벽에 부딪히거나, 맨 마지막 바닥에 닿으면 충돌
				{
					return 0; // 충돌함
				}

				if (gameBoardInfo[arrY + y][arrX + x] == 1)
				{
					return 0; // 충돌함
				}
			}			
		}
	}
	return 1; // 충돌 안함
}


int BlockDown()
{
	if (!(DetectCollision(curPosX, curPosY + 1, blockModel[block_id]))) return 0; // 더 이상 내려갈 수 없다. -> 0

	DeleteBlock(blockModel[block_id]);
	curPosY += 1;
	SetCurrentCursorPos(curPosX, curPosY);
	ShowBlock(blockModel[block_id]);
	return 1; 
}


void ShiftLeft()
{

	if (!(DetectCollision(curPosX - 2, curPosY, blockModel[block_id]))) return;
	DeleteBlock(blockModel[block_id]);
	curPosX -= 2;
	SetCurrentCursorPos(curPosX, curPosY);
	ShowBlock(blockModel[block_id]);
}

void ShiftRight()
{
	if (!(DetectCollision(curPosX+2, curPosY, blockModel[block_id]))) return;
	DeleteBlock(blockModel[block_id]);
	curPosX += 2;
	SetCurrentCursorPos(curPosX, curPosY);
	ShowBlock(blockModel[block_id]);
}


void BlockUp()
{
	if (!(DetectCollision(curPosX, curPosY - 1, blockModel[block_id]))) return;
	DeleteBlock(blockModel[block_id]);
	curPosY -= 1;
	SetCurrentCursorPos(curPosX, curPosY);
	ShowBlock(blockModel[block_id]);
}

void BlockLeftUp()
{
	if (!(DetectCollision(curPosX - 2, curPosY - 1, blockModel[block_id]))) return;
	DeleteBlock(blockModel[block_id]);
	curPosX -= 2;
	curPosY -= 1;
	SetCurrentCursorPos(curPosX, curPosY);
	ShowBlock(blockModel[block_id]);
}

void BlockLeftDown()
{
	if (!(DetectCollision(curPosX - 2, curPosY + 1, blockModel[block_id]))) return;
	DeleteBlock(blockModel[block_id]);
	curPosX -= 2;
	curPosY += 1;
	SetCurrentCursorPos(curPosX, curPosY);
	ShowBlock(blockModel[block_id]);
}

void BlockRightUp()
{
	if (!(DetectCollision(curPosX + 2, curPosY - 1, blockModel[block_id]))) return;
	DeleteBlock(blockModel[block_id]);
	curPosX += 2;
	curPosY -= 1;
	SetCurrentCursorPos(curPosX, curPosY);
	ShowBlock(blockModel[block_id]);
}

void BlockRightDown()
{
	if (!(DetectCollision(curPosX + 2, curPosY + 1, blockModel[block_id]))) return;
	DeleteBlock(blockModel[block_id]);
	curPosX += 2;
	curPosY += 1;
	SetCurrentCursorPos(curPosX, curPosY);
	ShowBlock(blockModel[block_id]);
}

void RotateBlock() // 반시계 
{
	int block_base = block_id - block_id % 4;
	int block_rotated = block_base + (block_id + 1) % 4;
	if (!DetectCollision(curPosX, curPosY, blockModel[block_rotated]))
	{
		return;
	}
	DeleteBlock(blockModel[block_id]);
	block_id = block_rotated;
	ShowBlock(blockModel[block_id]);
}

void RotateBlock180() // 반시계 180도
{
	int block_base = block_id - block_id % 4;
	int block_rotated = block_base + (block_id + 2) % 4;
	if (!DetectCollision(curPosX, curPosY, blockModel[block_rotated]))
	{
		return;
	}
	DeleteBlock(blockModel[block_id]);
	block_id = block_rotated;
	ShowBlock(blockModel[block_id]);
}

void ReverseRotateBlock() // 시계 , 270도
{
	int block_base = block_id - block_id % 4;
	int block_rotated = block_base + (block_id + 3) % 4;
	if (!DetectCollision(curPosX, curPosY, blockModel[block_rotated]))
	{
		return;
	}
	DeleteBlock(blockModel[block_id]);
	block_id = block_rotated;
	ShowBlock(blockModel[block_id]);
}

void SpaceDown() // space 키를 누르면 블럭이 바로 밑으로 감
{
	while (BlockDown());
}

void RedrawBlocks(void);

void ProcessKeyInput()
{
	int i, key;
	for (i = 0; i < 20; i++) // for문 이용하기
	{
		if (_kbhit() != 0)
		{
			key = _getch();
			switch (key)
			{
			case LEFT:
				if (reverse_flag == 0) ShiftLeft();
				else ShiftRight();
				break;
			case RIGHT:
				if (reverse_flag == 0) ShiftRight();
				else ShiftLeft();
				break;
			case UP:
				if (block_id < 28)
					RotateBlock();
				break;
			case SPACE:
				SpaceDown(); // 스페이스 누르면 블럭이 바로 바닥으로 가기
				break;
			case ENTER: // 일시정지
				Sleep(3000); // 3초 정지
				break;
			case R:
				if (reverse_flag == 0) reverse_flag = 1; // 방향키 동작 반대
				else reverse_flag = 0; // 다시 누르면 원상복귀
				break;
			}
		}
		Sleep(speed); // Sleep(1000)은 1초 일시정지, 초반에는 1초당 200ms
	}

}

void DrawGameBoard() // 게임판 그리기 함수
{
	int x, y;

	// 왼쪽 세로선 그리기
	for (y = 0; y <= GBOARD_HEIGHT; y++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT)
			printf("└");
		else
			printf("│");
	}

	// 오른쪽 세로선 그리기
	for (y = 0; y <= GBOARD_HEIGHT; y++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 1) * 2, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT)
			printf("┘");
		else
			printf("│");
	}

	// 하단 가로선 그리기
	for (x = 1; x < GBOARD_WIDTH + 1; x++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT);
		printf("─");
	}
}


void AddBlockToBoard(void) // 보드에 적음(새로운 블럭 추가)
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			/* Convert X,Y coordinate to array index */
			arrCurX = (curPosX - GBOARD_ORIGIN_X) / 2; // 블록 위치 -> 배열 인덱스
			arrCurY = curPosY - GBOARD_ORIGIN_Y;

			if (blockModel[block_id][y][x] == 1) 
			{
				gameBoardInfo[arrCurY + y][arrCurX + x] = 1;
			}
		}
	}
}

void EraseBlockToBoard(void) // 블록 지우는 함수
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			arrCurX = (curPosX - GBOARD_ORIGIN_X) / 2; // 블록 위치 -> 배열 인덱스
			arrCurY = curPosY - GBOARD_ORIGIN_Y;

			if (blockModel[block_id][y][x] == 1)
			{
				for (y = 0; y < 4; y++)
				{
					for (x = 0; x < 4; x++)
					{
						gameBoardInfo[arrCurY + y][arrCurX + x] = 0;
					}
				}
			}
		}
	}
}

int IsGameOver(void) // 게임 오버 체크 함수
{
	if (!DetectCollision(curPosX, curPosY, blockModel[block_id]))
	{
		return 1; // 충돌 발생
	}
	return 0;
}

void RedrawBlocks(void) // 블럭 다시 그리기
{
	int x, y;
	int cursX, cursY;

	for (y = 0; y < GBOARD_HEIGHT; y++)
	{
		for (x = 1; x < GBOARD_WIDTH + 1; x++)
		{
			cursX = x * 2 + GBOARD_ORIGIN_X;
			cursY = y + GBOARD_ORIGIN_Y;

			SetCurrentCursorPos(cursX, cursY);
			if (gameBoardInfo[y][x] == 1)
			{
				printf("■");
			}
			else { printf("  "); }
		}
	}
}

void RemoveFillUpLine(void)
{
	int line, x, y;
	for (y = GBOARD_HEIGHT - 1; y > 0; y--) // 완성된 라인이 있는지 검사
	{
		for (x = 1; x < GBOARD_WIDTH + 1; x++)
		{
			if (gameBoardInfo[y][x] != 1) break; // 1이 아닌 것 = 완성X
		}
		if (x == (GBOARD_WIDTH + 1)) // 한 줄 완성되었다면
		{
			for (line = 0; y - line > 0; line++)
			{
				memcpy(&gameBoardInfo[y - line][1],
					&gameBoardInfo[(y - line) - 1][1], GBOARD_WIDTH * sizeof(int));
				//memcpy(목적지,주소,-)
			}
			y++;
			clear_num++; // 몇 줄 완성되었는지 저장하는 전역 변수
		}
	}
	RedrawBlocks();
}

int main()
{
	int try_num = 0; // 게임 블럭이 몇 번 생성되었는지 저장하는 변수
	speed = 10; // 스피드 조절 ( 전체 : 200ms )
	srand((unsigned int)time(NULL)); // 랜덤 함수
	block_id = (rand() % 7) * 4; // 처음에는 아이템 없이 블럭만 나오게 하기.
	
	RemoveCursor(); // 커서 없애기
	DrawGameBoard(); // 게임판 그리기

	for (int y = 0; y < GBOARD_HEIGHT; y++)
	{
		gameBoardInfo[y][0] = 1;
		gameBoardInfo[y][GBOARD_WIDTH + 1] = 1;
	}
	for (int x = 0; x < GBOARD_WIDTH + 2; x++)
	{
		gameBoardInfo[GBOARD_HEIGHT][x] = 1;
	}

	curPosX = GBOARD_ORIGIN_X + GBOARD_WIDTH; // 블록 x좌표 설정
	curPosY = 0; // 블록 y좌표 설정

	SetCurrentCursorPos(curPosY, curPosX); // 위치 초기화

	while (1)
	{
		try_num++; // 게임 시도 횟수 갱신
		block_id = (rand() % 7) * 4; // 블록 랜덤(0~27)
		

		//3개의 줄 완성 시
		if (clear_num > 0)
		{
			if ((clear_num) % 3 == 0) speed -= 1; // 줄을 3개 완성시키면 속도가 20ms씩 감소
		}
		
		SetCurrentCursorPos(30, 2);
		printf("완성시킨 줄의 개수 : %d", clear_num);
		SetCurrentCursorPos(30, 4);
		printf("속도 : 1초 동안 0.%d s", speed * 20);


		curPosX = GBOARD_ORIGIN_X + GBOARD_WIDTH; // 블록 x좌표 설정
		curPosY = 0; // 블록 y좌표 설정

		SetCurrentCursorPos(curPosY, curPosX); // 위치 초기화
		
		if (IsGameOver()) break; // 게임 오버면 while문 탈출
		
		while (1)
		{
			if (BlockDown() == 0) // 바닥에 도착
				{
				AddBlockToBoard(); //보드에 블록 추가
				RemoveFillUpLine(); // 완성된 줄 삭제
				break;
			}
			ProcessKeyInput(); // 키 누름에 따라 변화			
		}
	}

	SetCurrentCursorPos(14, 0);
	puts("Game Over!");
	getchar();
}

