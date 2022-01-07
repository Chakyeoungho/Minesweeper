#include "pch.h"
#include "tipsware.h"
#include "Constant.h"
#include <stdlib.h>
#include <time.h>

typedef struct _GameData // 게임 플레이중 필요한 데이터
{
	char state_hard[HARD_Y_COUNT][HARD_X_COUNT];    // 가장 큰 사이즈의 보드만 있어도 모든 난이도의 게임을 만들 수 있다
	char click;
	char tile_state;    // 타일 상태
	int level;     // 선택한 난이도
	int game_step;    // 현재 게임 단계
} GameData, *pGameData;

void selectLvButton();    // 난이도 선택 버튼 생성
void easyLv_board(pGameData ap_data);    // 쉬움 난이도 판 생성
void normalLv_board(pGameData ap_data);    // 보통 난이도 판 생성
void hardLv_board(pGameData ap_data);    // 어려움 난이도 판 생성
void randMine(pGameData ap_data, int mineNum, int x_count, int y_count);    // 랜덤으로 지뢰 생성

void OnLButtonDown(int a_mixed_key, POINT a_pos)
{
	pGameData p_data = (pGameData)GetAppData();

	if (a_mixed_key & MK_CONTROL) {
		if (p_data->game_step == 1) {
			if (p_data->level == EASY) {
				unsigned int x = (unsigned int)a_pos.x / EASY_GRID_SIZE, y = (unsigned int)a_pos.y / EASY_GRID_SIZE;
				if (x < EASY_X_COUNT && y < EASY_Y_COUNT) {
					if (p_data->state_hard[y][x] == nothing_closed)
						p_data->state_hard[y][x] = flag;
					else if (p_data->state_hard[y][x] == flag)
						p_data->state_hard[y][x] = questionMark;
					else if (p_data->state_hard[y][x] == questionMark)
						p_data->state_hard[y][x] = nothing_closed;

					easyLv_board(p_data);
				}
			}
			else if (p_data->level == NORMAL) {
				unsigned int x = (unsigned int)a_pos.x / NORMAL_GRID_SIZE, y = (unsigned int)a_pos.y / NORMAL_GRID_SIZE;
				if (x < NORMAL_X_COUNT && y < NORMAL_Y_COUNT) {
					if (p_data->state_hard[y][x] == nothing_closed)
						p_data->state_hard[y][x] = flag;
					else if (p_data->state_hard[y][x] == flag)
						p_data->state_hard[y][x] = questionMark;
					else if (p_data->state_hard[y][x] == questionMark)
						p_data->state_hard[y][x] = nothing_closed;

					normalLv_board(p_data);
				}
			}
			else if (p_data->level == HARD) {
				unsigned int x = (unsigned int)a_pos.x / HARD_GRID_SIZE, y = (unsigned int)a_pos.y / HARD_GRID_SIZE;
				if (x < HARD_X_COUNT && y < HARD_Y_COUNT) {
					if (p_data->state_hard[y][x] == nothing_closed)
						p_data->state_hard[y][x] = flag;
					else if (p_data->state_hard[y][x] == flag)
						p_data->state_hard[y][x] = questionMark;
					else if (p_data->state_hard[y][x] == questionMark)
						p_data->state_hard[y][x] = nothing_closed;

					hardLv_board(p_data);
				}
			}
		}
	} else {
		if (p_data->game_step == 0) {
			if (a_pos.x >= 20 && a_pos.x <= 108 && a_pos.y >= 10 && a_pos.y <= 38) {
				randMine(p_data, EASY_MINE_NUM, EASY_X_COUNT, EASY_Y_COUNT);
				easyLv_board(p_data);
				p_data->level = EASY;
				p_data->game_step++;
			}
			else if (a_pos.x >= 120 && a_pos.x <= 208 && a_pos.y >= 10 && a_pos.y <= 38) {
				randMine(p_data, NORMAL_MINE_NUM, NORMAL_X_COUNT, NORMAL_Y_COUNT);
				normalLv_board(p_data);
				p_data->level = NORMAL;
				p_data->game_step++;
			}
			else if (a_pos.x >= 220 && a_pos.x <= 308 && a_pos.y >= 10 && a_pos.y <= 38) {
				randMine(p_data, HARD_MINE_NUM, HARD_X_COUNT, HARD_Y_COUNT);
				hardLv_board(p_data);
				p_data->level = HARD;
				p_data->game_step++;
			}
		}
		// 지뢰 타일 오픈
		else if (p_data->game_step == 1) {
			if (p_data->level == EASY) {
				unsigned int x = (unsigned int)a_pos.x / EASY_GRID_SIZE, y = (unsigned int)a_pos.y / EASY_GRID_SIZE;
				if (x < EASY_X_COUNT && y < EASY_Y_COUNT) {
					if (p_data->state_hard[y][x] == mine)
						p_data->game_step = 100;
					else if (p_data->state_hard[y][x] == nothing_closed)
						p_data->state_hard[y][x] = nothing_open;

					easyLv_board(p_data);
				}
			}
			else if (p_data->level == NORMAL) {
				unsigned int x = (unsigned int)a_pos.x / NORMAL_GRID_SIZE, y = (unsigned int)a_pos.y / NORMAL_GRID_SIZE;
				if (x < NORMAL_X_COUNT && y < NORMAL_Y_COUNT) {
					if (p_data->state_hard[y][x] == mine)
						p_data->game_step = 100;
					else if (p_data->state_hard[y][x] == nothing_closed)
						p_data->state_hard[y][x] = nothing_open;

					normalLv_board(p_data);
				}
			}
			else if (p_data->level == HARD) {
				unsigned int x = (unsigned int)a_pos.x / HARD_GRID_SIZE, y = (unsigned int)a_pos.y / HARD_GRID_SIZE;
				if (x < HARD_X_COUNT && y < HARD_Y_COUNT) {
					if (p_data->state_hard[y][x] == mine)
						p_data->game_step = 100;
					else if (p_data->state_hard[y][x] == nothing_closed)
						p_data->state_hard[y][x] = nothing_open;

					hardLv_board(p_data);
				}
			}
		}
	}


}

MOUSE_MESSAGE(OnLButtonDown, NULL, NULL)

int main()
{
	GameData data;
	memset(&data, 0, sizeof(GameData));
	SetAppData(&data, sizeof(GameData));

	SelectFontObject("굴림", 20, 1);
	selectLvButton();

	ShowDisplay();
	return 0;
}

/*
* 보드판 상태
* 0 : 아무것도 없음 (타일 닫힘)
* 10 : 아무것도 없음 (타일 열림)
* 1 ~ 8 : 주변 지뢰의 개수 (타일 닫힘)
* 11 ~ 18 : 주변 지뢰의 개수 (타일 열림)
* 20 : 지뢰
* 21 : 깃발
* 22 : 물음표
*/

void selectLvButton()
{
	Rectangle(20, 10, 108, 38, ORANGE, ORANGE);    // 쉬움lv
	TextOut(43, 13, BLACK, "쉬움");

	Rectangle(120, 10, 208, 38, ORANGE, ORANGE);    // 보통lv
	TextOut(143, 13, BLACK, "보통");

	Rectangle(220, 10, 308, 38, ORANGE, ORANGE);    // 어려움lv
	TextOut(233, 13, BLACK, "어려움");
}

void easyLv_board(pGameData ap_data)
{
	Clear();
	for (int y = 0; y < EASY_Y_COUNT; y++) {
		for (int x = 0; x < EASY_X_COUNT; x++) {
			Rectangle(x * EASY_GRID_SIZE, y * EASY_GRID_SIZE, (x + 1) * EASY_GRID_SIZE, (y + 1) * EASY_GRID_SIZE, RGB(0, 100, 200), RGB(0, 0, 128));
		}
	}

	for (int y = 0; y < EASY_Y_COUNT; y++) {
		for (int x = 0; x < EASY_X_COUNT; x++) {
			if (ap_data->state_hard[y][x] == nothing_open)
				Rectangle(x * EASY_GRID_SIZE, y * EASY_GRID_SIZE, (x + 1) * EASY_GRID_SIZE, (y + 1) * EASY_GRID_SIZE, RGB(200, 100, 0), RGB(128, 0, 0));
			else if (ap_data->state_hard[y][x] == flag)
				Ellipse(x * EASY_GRID_SIZE, y * EASY_GRID_SIZE, (x + 1) * EASY_GRID_SIZE, (y + 1) * EASY_GRID_SIZE, RGB(200, 100, 0), RGB(128, 0, 0));
			else if (ap_data->state_hard[y][x] == questionMark)
				Ellipse(x * EASY_GRID_SIZE, y * EASY_GRID_SIZE, (x + 1) * EASY_GRID_SIZE, (y + 1) * EASY_GRID_SIZE, WHITE, BLACK);
		}
	}

	for (int y = 0; y < EASY_Y_COUNT; y++) {
		for (int x = 0; x < EASY_X_COUNT; x++) {
			if (ap_data->state_hard[y][x] == mine)
				Rectangle(x * EASY_GRID_SIZE, y * EASY_GRID_SIZE, (x + 1) * EASY_GRID_SIZE, (y + 1) * EASY_GRID_SIZE, BLACK, BLACK);
		}
	}

	ShowDisplay();
}

void normalLv_board(pGameData ap_data)
{
	Clear();
	for (int y = 0; y < NORMAL_Y_COUNT; y++) {
		for (int x = 0; x < NORMAL_X_COUNT; x++) {
			Rectangle(x * NORMAL_GRID_SIZE, y * NORMAL_GRID_SIZE, (x + 1) * NORMAL_GRID_SIZE, (y + 1) * NORMAL_GRID_SIZE, RGB(0, 100, 200), RGB(0, 0, 128));
		}
	}

	for (int y = 0; y < NORMAL_Y_COUNT; y++) {
		for (int x = 0; x < NORMAL_X_COUNT; x++) {
			if (ap_data->state_hard[y][x] == nothing_open)
				Rectangle(x * NORMAL_GRID_SIZE, y * NORMAL_GRID_SIZE, (x + 1) * NORMAL_GRID_SIZE, (y + 1) * NORMAL_GRID_SIZE, RGB(200, 100, 0), RGB(128, 0, 0));
			else if (ap_data->state_hard[y][x] == flag)
				Ellipse(x * NORMAL_GRID_SIZE, y * NORMAL_GRID_SIZE, (x + 1) * NORMAL_GRID_SIZE, (y + 1) * NORMAL_GRID_SIZE, RGB(200, 100, 0), RGB(128, 0, 0));
			else if (ap_data->state_hard[y][x] == questionMark)
				Ellipse(x * NORMAL_GRID_SIZE, y * NORMAL_GRID_SIZE, (x + 1) * NORMAL_GRID_SIZE, (y + 1) * NORMAL_GRID_SIZE, WHITE, BLACK);
		}
	}

	for (int y = 0; y < NORMAL_Y_COUNT; y++) {
		for (int x = 0; x < NORMAL_X_COUNT; x++) {
			if (ap_data->state_hard[y][x] == mine)
				Rectangle(x * NORMAL_GRID_SIZE, y * NORMAL_GRID_SIZE, (x + 1) * NORMAL_GRID_SIZE, (y + 1) * NORMAL_GRID_SIZE, BLACK, BLACK);
		}
	}

	ShowDisplay();
}

void hardLv_board(pGameData ap_data)
{
	Clear();
	for (int y = 0; y < HARD_Y_COUNT; y++) {
		for (int x = 0; x < HARD_X_COUNT; x++) {
			Rectangle(x * HARD_GRID_SIZE, y * HARD_GRID_SIZE, (x + 1) * HARD_GRID_SIZE, (y + 1) * HARD_GRID_SIZE, RGB(0, 100, 200), RGB(0, 0, 128));
		}
	}

	for (int y = 0; y < HARD_Y_COUNT; y++) {
		for (int x = 0; x < HARD_X_COUNT; x++) {
			if (ap_data->state_hard[y][x] == nothing_open)
				Rectangle(x * HARD_GRID_SIZE, y * HARD_GRID_SIZE, (x + 1) * HARD_GRID_SIZE, (y + 1) * HARD_GRID_SIZE, RGB(200, 100, 0), RGB(128, 0, 0));
			else if (ap_data->state_hard[y][x] == flag)
				Ellipse(x * HARD_GRID_SIZE, y * HARD_GRID_SIZE, (x + 1) * HARD_GRID_SIZE, (y + 1) * HARD_GRID_SIZE, RGB(200, 100, 0), RGB(128, 0, 0));
			else if (ap_data->state_hard[y][x] == questionMark)
				Ellipse(x * HARD_GRID_SIZE, y * HARD_GRID_SIZE, (x + 1) * HARD_GRID_SIZE, (y + 1) * HARD_GRID_SIZE, WHITE, BLACK);
		}
	}

	for (int y = 0; y < HARD_Y_COUNT; y++) {
		for (int x = 0; x < HARD_X_COUNT; x++) {
			if (ap_data->state_hard[y][x] == mine)
				Rectangle(x * HARD_GRID_SIZE, y * HARD_GRID_SIZE, (x + 1) * HARD_GRID_SIZE, (y + 1) * HARD_GRID_SIZE, BLACK, BLACK);
		}
	}

	ShowDisplay();
}

void randMine(pGameData ap_data, int mineNum, int x_count, int y_count)
{
	srand(time(NULL));
	int tempX, tempY;
	int tempMineNum = 0;

	while(tempMineNum != mineNum) {
		if (ap_data->state_hard[tempY = (rand() % y_count)][tempX = (rand() % x_count)] != mine) {
			ap_data->state_hard[tempY][tempX] = mine;
			tempMineNum++;
		}
	}
}