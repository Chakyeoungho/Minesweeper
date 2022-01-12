#include "pch.h"
#include "tipsware.h"
#include "Constant.h"    // 필요한 상수를 모아놓은 헤더파일
#include <stdlib.h>      // srand()와 rand()를 사용하기 위한 헤더파일
#include <time.h>        // 난수의 시드값을 설정하기 위한 헤더파일

typedef struct _GameData // 게임 플레이중 필요한 데이터
{
	unsigned int board_state[HARD_Y_COUNT][HARD_X_COUNT];  // 가장 큰 사이즈의 보드만 있어도 모든 난이도의 게임을 만들 수 있다
	unsigned int board_temp[HARD_Y_COUNT][HARD_X_COUNT];   // 깃발과 물음표를 사용할 때 원래 보드의 상태를 기억
	unsigned int gridSize[3];    // 타일 하나의 크기 배열
	unsigned int x_count[3];     // x축 타일의 개수
	unsigned int y_count[3];     // y축 타일의 개수
	unsigned int mineNum[3];     // 지뢰의 개수
	int level;        // 선택한 난이도
	int game_step;    // 현재 게임 단계
} GameData, *pGameData;

void selectLvButton();    // 난이도 선택 버튼 생성
void selectLevel(pGameData ap_data, unsigned int x, unsigned int y);    // 난이도 선택
void drawBoard(pGameData ap_data);    // 보드판 그리기
void randMine(pGameData ap_data);    // 랜덤으로 지뢰 생성
void pluseMineNum(pGameData ap_data, int grid_size, int x_count, int y_count);    // 지뢰 주변 1씩 증가
void clickBoard(pGameData ap_data, unsigned int x, unsigned int y);    // 판 클릭
void openNothingClosed(pGameData ap_data, int x_count, int y_count, int x_num, int y_num);    // 연쇄적으로 판 오픈
void flagQuesBoard(pGameData ap_data, unsigned int x, unsigned int y);    // 깃발과 물음표 관리

void OnLButtonDown(int a_mixed_key, POINT a_pos)
{
	pGameData p_data = (pGameData)GetAppData();

	if (a_mixed_key & MK_CONTROL) {
		if (p_data->game_step == PLAYGAME) {
			unsigned int x = (unsigned int)a_pos.x / p_data->gridSize[p_data->level - 1000], y = (unsigned int)a_pos.y / p_data->gridSize[p_data->level - 1000];
			flagQuesBoard(p_data, x, y);
		}
	} else {
		if (p_data->game_step == SELECTLV) {
			selectLevel(p_data, a_pos.x, a_pos.y);
		}
		// 지뢰 타일 오픈
		else if (p_data->game_step == PLAYGAME) {
			unsigned int x = (unsigned int)a_pos.x / p_data->gridSize[p_data->level - 1000], y = (unsigned int)a_pos.y / p_data->gridSize[p_data->level - 1000];
			clickBoard(p_data, x, y);
		}
	}
}

MOUSE_MESSAGE(OnLButtonDown, NULL, NULL)

int main()
{
	GameData data = { { { 0, }, }, 
					  { { 0, }, },
					  {EASY_GRID_SIZE, NORMAL_GRID_SIZE, HARD_GRID_SIZE},
					  {EASY_X_COUNT, NORMAL_X_COUNT, HARD_X_COUNT},
					  {EASY_Y_COUNT, NORMAL_Y_COUNT, HARD_Y_COUNT},
					  {EASY_MINE_NUM, NORMAL_MINE_NUM, HARD_MINE_NUM},
					  0, 0 };
	/*
	memset(&data, 0, sizeof(GameData));

	data.gridSize[0] = EASY_GRID_SIZE;
	data.gridSize[1] = NORMAL_GRID_SIZE;
	data.gridSize[2] = HARD_GRID_SIZE;

	data.x_count[0] = EASY_X_COUNT;
	data.x_count[1] = NORMAL_X_COUNT;
	data.x_count[2] = HARD_X_COUNT;

	data.y_count[0] = EASY_Y_COUNT;
	data.y_count[1] = NORMAL_Y_COUNT;
	data.y_count[2] = HARD_Y_COUNT;

	data.mineNum[0] = EASY_MINE_NUM;
	data.mineNum[1] = NORMAL_MINE_NUM;
	data.mineNum[2] = HARD_MINE_NUM;
	*/
	SetAppData(&data, sizeof(GameData));

	SelectFontObject("굴림", 20, 1);
	selectLvButton();

	ShowDisplay();
	return 0;
}

void selectLvButton()
{
	Rectangle(20, 10, 108, 38, ORANGE, ORANGE);    // 쉬움lv
	TextOut(43, 13, BLACK, "쉬움");

	Rectangle(120, 10, 208, 38, ORANGE, ORANGE);    // 보통lv
	TextOut(143, 13, BLACK, "보통");

	Rectangle(220, 10, 308, 38, ORANGE, ORANGE);    // 어려움lv
	TextOut(233, 13, BLACK, "어려움");
}

void selectLevel(pGameData ap_data, unsigned int x, unsigned int y)
{
	if (x >= 20 && x <= 108 && y >= 10 && y <= 38) {
		ap_data->level = EASY;
		randMine(ap_data);
		pluseMineNum(ap_data, EASY_GRID_SIZE, EASY_X_COUNT, EASY_Y_COUNT);
		drawBoard(ap_data);
		ap_data->game_step++;
	}
	else if (x >= 120 && x <= 208 && y >= 10 && y <= 38) {
		ap_data->level = NORMAL;
		randMine(ap_data);
		pluseMineNum(ap_data, NORMAL_GRID_SIZE, NORMAL_X_COUNT, NORMAL_Y_COUNT);
		drawBoard(ap_data);
		ap_data->game_step++;
	}
	else if (x >= 220 && x <= 308 && y >= 10 && y <= 38) {
		ap_data->level = HARD;
		randMine(ap_data);
		pluseMineNum(ap_data, HARD_GRID_SIZE, HARD_X_COUNT, HARD_Y_COUNT);
		drawBoard(ap_data);
		ap_data->game_step++;
	}
}

void drawBoard(pGameData ap_data)
{
	Clear();

	for (unsigned int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
		for (unsigned int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
			Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], RGB(0, 100, 200), RGB(0, 0, 128));
		}
	}

	for (unsigned int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
		for (unsigned int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
			if (ap_data->board_state[y][x] >= mine_num1_open && ap_data->board_state[y][x] <= mine_num8_open)
				TextOut(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], WHITE, "%d", ap_data->board_state[y][x] - 10);
			else if (ap_data->board_state[y][x] == nothing_open)
				Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], RGB(200, 100, 0), RGB(128, 0, 0));
			else if (ap_data->board_state[y][x] == flag)
				Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], RGB(200, 100, 0), RGB(128, 0, 0));
			else if (ap_data->board_state[y][x] == questionMark)
				Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], WHITE, BLACK);
		}
	}

	for (unsigned int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
		for (unsigned int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
			if (ap_data->board_state[y][x] == mine_open)
				Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], BLACK, BLACK);
		}
	}

	ShowDisplay();
}

void randMine(pGameData ap_data)
{
	srand((unsigned int)time(NULL));
	unsigned int tempX, tempY;
	int tempMineNum = 0;

	while (tempMineNum != ap_data->mineNum[ap_data->level - 1000]) {
		if (ap_data->board_state[tempY = (rand() % ap_data->y_count[ap_data->level - 1000])][tempX = (rand() % ap_data->x_count[ap_data->level - 1000])] != mine_closed) {
			ap_data->board_state[tempY][tempX] = mine_closed;
			tempMineNum++;
		}
	}
}

void pluseMineNum(pGameData ap_data, int grid_size, int x_count, int y_count)
{
	int mine_num;

	for (int i = 0; i < y_count; i++)
	{
		for (int j = 0; j < x_count; j++)
		{
			if (ap_data->board_state[i][j] == mine_closed) {
				continue;
			}
			else {
				mine_num = 0;
				for (int y = i - 1; y <= i + 1; y++)
				{
					for (int x = j - 1; x <= j + 1; x++)
					{
						if (y < 0 || x < 0 || y >= y_count || x >= x_count)
							continue;
						else if (ap_data->board_state[y][x] == mine_closed)
							mine_num += 1;
					}
				}
				ap_data->board_state[i][j] = mine_num;
			}
		}
	}
}

void clickBoard(pGameData ap_data, unsigned int x, unsigned int y)
{
	if (x < ap_data->x_count[ap_data->level - 1000] && y < ap_data->y_count[ap_data->level - 1000]) {
		if (ap_data->board_state[y][x] == mine_closed) {
			for (unsigned int i = 0; i < ap_data->y_count[ap_data->level - 1000]; i++) {
				for (unsigned int j = 0; j < ap_data->x_count[ap_data->level - 1000]; j++) {
					if (ap_data->board_state[y][x] == mine_closed)
						ap_data->board_state[y][x] += 10;
				}
			}
			ap_data->game_step = GAMEOVER;
		}
		else if (ap_data->board_state[y][x] == nothing_closed)
			openNothingClosed(ap_data, x, y, ap_data->x_count[ap_data->level - 1000], ap_data->y_count[ap_data->level - 1000]);
		else if (ap_data->board_state[y][x] <= mine_num8_closed)
			ap_data->board_state[y][x] += 10;

		drawBoard(ap_data);
	}
}

void openNothingClosed(pGameData ap_data, int x_count, int y_count, int x_num, int y_num)
{
	for (int y = y_count - 1; y <= y_count + 1; y++) {
		for (int x = x_count - 1; x <= x_count + 1; x++) {
			if (y < 0 || x < 0 || x >= x_num || y >= y_num || ap_data->board_state[y][x] > mine_num8_closed)
				continue;

			ap_data->board_state[y][x] += 10;

			if (ap_data->board_state[y][x] == nothing_open) {
				openNothingClosed(ap_data, x, y, x_num, y_num);
			}
		}
	}
}

void flagQuesBoard(pGameData ap_data, unsigned int x, unsigned int y)
{
	if (x < ap_data->x_count[ap_data->level - 1000] && y < ap_data->y_count[ap_data->level - 1000]) {
		if (ap_data->board_state[y][x] <= mine_closed) {
			ap_data->board_temp[y][x] = ap_data->board_state[y][x];
			ap_data->board_state[y][x] = flag;
		}
		else if (ap_data->board_state[y][x] == flag) {
			ap_data->board_state[y][x] = questionMark;
		}
		else if (ap_data->board_state[y][x] == questionMark) {
			ap_data->board_state[y][x] = ap_data->board_temp[y][x];
		}

		drawBoard(ap_data);
	}
}