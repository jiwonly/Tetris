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

#define R 114  // reverse, ����Ű �ݴ�� ����


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
				printf("��");
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

void RemoveCursor(void) // �����̴� Ŀ�� ���� �Լ�
{
	CONSOLE_CURSOR_INFO curInfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
	curInfo.bVisible = 0;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

int DetectCollision(int posX, int posY, char blockModel[4][4]) // �浹 -> 0 / �浹X -> 1
{
	int x, y;
	int arrX = (posX - GBOARD_ORIGIN_X) / 2;
	int arrY = posY - GBOARD_ORIGIN_Y;

	for (x = 0; x < 4; x++)
	{
		for (y = 3; y >= 0; y--) // �ؿ������� �ȱ�
		{
			if (blockModel[y][x] == 1)
			{
				if (arrX + x <= 0 || arrX + x >= (GBOARD_WIDTH+1) || arrY + y >= GBOARD_HEIGHT) 
					// ���� �ε����ų�, �� ������ �ٴڿ� ������ �浹
				{
					return 0; // �浹��
				}

				if (gameBoardInfo[arrY + y][arrX + x] == 1)
				{
					return 0; // �浹��
				}
			}			
		}
	}
	return 1; // �浹 ����
}


int BlockDown()
{
	if (!(DetectCollision(curPosX, curPosY + 1, blockModel[block_id]))) return 0; // �� �̻� ������ �� ����. -> 0

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

void RotateBlock() // �ݽð� 
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

void RotateBlock180() // �ݽð� 180��
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

void ReverseRotateBlock() // �ð� , 270��
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

void SpaceDown() // space Ű�� ������ ���� �ٷ� ������ ��
{
	while (BlockDown());
}

void RedrawBlocks(void);

void ProcessKeyInput()
{
	int i, key;
	for (i = 0; i < 20; i++) // for�� �̿��ϱ�
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
				SpaceDown(); // �����̽� ������ ���� �ٷ� �ٴ����� ����
				break;
			case ENTER: // �Ͻ�����
				Sleep(3000); // 3�� ����
				break;
			case R:
				if (reverse_flag == 0) reverse_flag = 1; // ����Ű ���� �ݴ�
				else reverse_flag = 0; // �ٽ� ������ ���󺹱�
				break;
			}
		}
		Sleep(speed); // Sleep(1000)�� 1�� �Ͻ�����, �ʹݿ��� 1�ʴ� 200ms
	}

}

void DrawGameBoard() // ������ �׸��� �Լ�
{
	int x, y;

	// ���� ���μ� �׸���
	for (y = 0; y <= GBOARD_HEIGHT; y++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT)
			printf("��");
		else
			printf("��");
	}

	// ������ ���μ� �׸���
	for (y = 0; y <= GBOARD_HEIGHT; y++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X + (GBOARD_WIDTH + 1) * 2, GBOARD_ORIGIN_Y + y);
		if (y == GBOARD_HEIGHT)
			printf("��");
		else
			printf("��");
	}

	// �ϴ� ���μ� �׸���
	for (x = 1; x < GBOARD_WIDTH + 1; x++)
	{
		SetCurrentCursorPos(GBOARD_ORIGIN_X + x * 2, GBOARD_ORIGIN_Y + GBOARD_HEIGHT);
		printf("��");
	}
}


void AddBlockToBoard(void) // ���忡 ����(���ο� �� �߰�)
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			/* Convert X,Y coordinate to array index */
			arrCurX = (curPosX - GBOARD_ORIGIN_X) / 2; // ��� ��ġ -> �迭 �ε���
			arrCurY = curPosY - GBOARD_ORIGIN_Y;

			if (blockModel[block_id][y][x] == 1) 
			{
				gameBoardInfo[arrCurY + y][arrCurX + x] = 1;
			}
		}
	}
}

void EraseBlockToBoard(void) // ��� ����� �Լ�
{
	int x, y, arrCurX, arrCurY;
	for (y = 0; y < 4; y++)
	{
		for (x = 0; x < 4; x++)
		{
			arrCurX = (curPosX - GBOARD_ORIGIN_X) / 2; // ��� ��ġ -> �迭 �ε���
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

int IsGameOver(void) // ���� ���� üũ �Լ�
{
	if (!DetectCollision(curPosX, curPosY, blockModel[block_id]))
	{
		return 1; // �浹 �߻�
	}
	return 0;
}

void RedrawBlocks(void) // �� �ٽ� �׸���
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
				printf("��");
			}
			else { printf("  "); }
		}
	}
}

void RemoveFillUpLine(void)
{
	int line, x, y;
	for (y = GBOARD_HEIGHT - 1; y > 0; y--) // �ϼ��� ������ �ִ��� �˻�
	{
		for (x = 1; x < GBOARD_WIDTH + 1; x++)
		{
			if (gameBoardInfo[y][x] != 1) break; // 1�� �ƴ� �� = �ϼ�X
		}
		if (x == (GBOARD_WIDTH + 1)) // �� �� �ϼ��Ǿ��ٸ�
		{
			for (line = 0; y - line > 0; line++)
			{
				memcpy(&gameBoardInfo[y - line][1],
					&gameBoardInfo[(y - line) - 1][1], GBOARD_WIDTH * sizeof(int));
				//memcpy(������,�ּ�,-)
			}
			y++;
			clear_num++; // �� �� �ϼ��Ǿ����� �����ϴ� ���� ����
		}
	}
	RedrawBlocks();
}

int main()
{
	int try_num = 0; // ���� ���� �� �� �����Ǿ����� �����ϴ� ����
	speed = 10; // ���ǵ� ���� ( ��ü : 200ms )
	srand((unsigned int)time(NULL)); // ���� �Լ�
	block_id = (rand() % 7) * 4; // ó������ ������ ���� ���� ������ �ϱ�.
	
	RemoveCursor(); // Ŀ�� ���ֱ�
	DrawGameBoard(); // ������ �׸���

	for (int y = 0; y < GBOARD_HEIGHT; y++)
	{
		gameBoardInfo[y][0] = 1;
		gameBoardInfo[y][GBOARD_WIDTH + 1] = 1;
	}
	for (int x = 0; x < GBOARD_WIDTH + 2; x++)
	{
		gameBoardInfo[GBOARD_HEIGHT][x] = 1;
	}

	curPosX = GBOARD_ORIGIN_X + GBOARD_WIDTH; // ��� x��ǥ ����
	curPosY = 0; // ��� y��ǥ ����

	SetCurrentCursorPos(curPosY, curPosX); // ��ġ �ʱ�ȭ

	while (1)
	{
		try_num++; // ���� �õ� Ƚ�� ����
		block_id = (rand() % 7) * 4; // ��� ����(0~27)
		

		//3���� �� �ϼ� ��
		if (clear_num > 0)
		{
			if ((clear_num) % 3 == 0) speed -= 1; // ���� 3�� �ϼ���Ű�� �ӵ��� 20ms�� ����
		}
		
		SetCurrentCursorPos(30, 2);
		printf("�ϼ���Ų ���� ���� : %d", clear_num);
		SetCurrentCursorPos(30, 4);
		printf("�ӵ� : 1�� ���� 0.%d s", speed * 20);


		curPosX = GBOARD_ORIGIN_X + GBOARD_WIDTH; // ��� x��ǥ ����
		curPosY = 0; // ��� y��ǥ ����

		SetCurrentCursorPos(curPosY, curPosX); // ��ġ �ʱ�ȭ
		
		if (IsGameOver()) break; // ���� ������ while�� Ż��
		
		while (1)
		{
			if (BlockDown() == 0) // �ٴڿ� ����
				{
				AddBlockToBoard(); //���忡 ��� �߰�
				RemoveFillUpLine(); // �ϼ��� �� ����
				break;
			}
			ProcessKeyInput(); // Ű ������ ���� ��ȭ			
		}
	}

	SetCurrentCursorPos(14, 0);
	puts("Game Over!");
	getchar();
}

