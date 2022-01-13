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
void randMine(pGameData ap_data);     // 랜덤으로 지뢰 생성
void pluseMineNum(pGameData ap_data, int grid_size, int x_count, int y_count);    // 지뢰 주변 1씩 증가
void clickBoard(pGameData ap_data, unsigned int x, unsigned int y);    // 판 클릭
void openNothingClosed(pGameData ap_data, int x_count, int y_count);    // 연쇄적으로 판 오픈
void flagQuesBoard(pGameData ap_data, unsigned int x, unsigned int y);    // 깃발과 물음표 관리

void OnLButtonDown(int a_mixed_key, POINT a_pos)
{
	pGameData p_data = (pGameData)GetAppData();    // 내부변수 사용

	// 컨트롤 키와 마우스 좌클릭을 동시에 눌렀을 때
	if (a_mixed_key & MK_CONTROL) {
		if (p_data->game_step == PLAYGAME) {
			unsigned int x = a_pos.x / p_data->gridSize[p_data->level - 1000], y = a_pos.y / p_data->gridSize[p_data->level - 1000];
			flagQuesBoard(p_data, x, y);
		}
	} else {    // 좌클릭만 눌렀을 때
		// 게임 플레이 단계 | 지뢰 타일 오픈
		if (p_data->game_step == PLAYGAME) {
			unsigned int x = a_pos.x / p_data->gridSize[p_data->level - 1000], y = a_pos.y / p_data->gridSize[p_data->level - 1000];
			clickBoard(p_data, x, y);
		}
		// 난이도 선택 단계
		else if (p_data->game_step == SELECTLV) {
			selectLevel(p_data, a_pos.x, a_pos.y);
		}
	}
}

// 마우스 메시지 눌렀을때만 사용
MOUSE_MESSAGE(OnLButtonDown, NULL, NULL)

int main()
{
	GameData data = { { { 0, }, },    // 판 상태
					  { { 0, }, },    // 깃발과 물음표를 제외한 판 상태
					  {EASY_GRID_SIZE, NORMAL_GRID_SIZE, HARD_GRID_SIZE},    // 타일 하나의 크기 배열
					  {EASY_X_COUNT, NORMAL_X_COUNT, HARD_X_COUNT},          // x축 타일의 개수
					  {EASY_Y_COUNT, NORMAL_Y_COUNT, HARD_Y_COUNT},          // y축 타일의 개수
					  {EASY_MINE_NUM, NORMAL_MINE_NUM, HARD_MINE_NUM},       // 지뢰의 개수
					  0, 0 };
	SetAppData(&data, sizeof(GameData));    // data를 내부변수로 설정

	SelectFontObject("굴림", 20, 1);    // 글씨체와 크기 설정
	selectLvButton();    // 난이도 선택 버튼 생성

	ShowDisplay();    // 화면에 출력
	return 0;
}

// 난이도 선택 버튼 생성
void selectLvButton()
{
	Rectangle(20, 10, 108, 38, ORANGE, ORANGE);     // 쉬움lv
	TextOut(43, 13, BLACK, "쉬움");

	Rectangle(120, 10, 208, 38, ORANGE, ORANGE);    // 보통lv
	TextOut(143, 13, BLACK, "보통");

	Rectangle(220, 10, 308, 38, ORANGE, ORANGE);    // 어려움lv
	TextOut(233, 13, BLACK, "어려움");
}

// 난이도 선택
void selectLevel(pGameData ap_data, unsigned int x, unsigned int y)
{
	// 좌클릭시 마우스의 좌표에 따라 난이도 조정
	if (x >= 20 && x <= 108 && y >= 10 && y <= 38) {
		ap_data->level = EASY;    // 선택한 난이도 저장
		randMine(ap_data);    // 지뢰 랜덤으로 생성
		pluseMineNum(ap_data, EASY_GRID_SIZE, EASY_X_COUNT, EASY_Y_COUNT);    // 지뢰 주변 1씩 증가
		drawBoard(ap_data);    // 판 그리기
		ap_data->game_step++;    // 다음단계로 이동
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

// 판 그리기
void drawBoard(pGameData ap_data)
{
	Clear();    // 화면 초기화

	// 판 그리기
	for (unsigned int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
		for (unsigned int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
			Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], RGB(0, 100, 200), RGB(0, 0, 128));
		}
	}

	// 숫자 빈칸 등 출력
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

// 랜덤으로 지뢰 생성
void randMine(pGameData ap_data)
{
	srand((unsigned int)time(NULL));    // 현재 시간을 시드값으로 설정
	unsigned int tempX, tempY;    // 난수를 저장할 임시 변수
	int tempMineNum = 0;    // 현재 생성된 지뢰 개수

	while (tempMineNum != ap_data->mineNum[ap_data->level - 1000]) {    // 난이도에 따른 지뢰 개수만큼
		if (ap_data->board_state[tempY = (rand() % ap_data->y_count[ap_data->level - 1000])][tempX = (rand() % ap_data->x_count[ap_data->level - 1000])] != mine_closed) {
			ap_data->board_state[tempY][tempX] = mine_closed;    // 지뢰가 없으면 지뢰 생성
			tempMineNum++;
		}
	}
}

// 지뢰 주변 1씩 증가
void pluseMineNum(pGameData ap_data, int grid_size, int x_count, int y_count)
{
	int mine_num;    // 주변 지뢰 개수

	for (int i = 0; i < y_count; i++)
	{
		for (int j = 0; j < x_count; j++)
		{
			if (ap_data->board_state[i][j] == mine_closed) {
				continue;    // 지뢰면 건너뛰기
			} else {
				mine_num = 0;    // 주변 지뢰 개수 0으로 초기화
				for (int y = i - 1; y <= i + 1; y++)
				{
					for (int x = j - 1; x <= j + 1; x++)
					{
						if (y < 0 || x < 0 || y >= y_count || x >= x_count)
							continue;
						else if (ap_data->board_state[y][x] == mine_closed)
							mine_num += 1;    // 주변의 지뢰 개수만큼 증가
					}
				}
				ap_data->board_state[i][j] = mine_num;    // 지뢰 개수 설정
			}
		}
	}
}

// 판 클릭시
void clickBoard(pGameData ap_data, unsigned int x, unsigned int y)
{
	if (x < ap_data->x_count[ap_data->level - 1000] && y < ap_data->y_count[ap_data->level - 1000]) {
		if (ap_data->board_state[y][x] == mine_closed) {    // 지뢰를 누르면 게임 오버, 지뢰 출력
			for (unsigned int i = 0; i < ap_data->y_count[ap_data->level - 1000]; i++) {
				for (unsigned int j = 0; j < ap_data->x_count[ap_data->level - 1000]; j++) {
					if (ap_data->board_state[y][x] == mine_closed)
						ap_data->board_state[y][x] += 10;
				}
			}
			ap_data->game_step = GAMEOVER;
		}    // 아무것도 없는 곳을 누르면 열기
		else if (ap_data->board_state[y][x] == nothing_closed)
			openNothingClosed(ap_data, x, y);    // 연쇄적으로 오픈
		else if (ap_data->board_state[y][x] <= mine_num8_closed)
			ap_data->board_state[y][x] += 10;    // 닫힌 숫자들에 10을 더해 열린 10으로 만듦

		drawBoard(ap_data);    // 판 그리기
	}
}

// 아무것도 없는 판을 연쇄적으로 열기
void openNothingClosed(pGameData ap_data, int x_pos, int y_pos)
{
	// 좌클릭 좌표 기준 8칸
	for (int y = y_pos - 1; y <= y_pos + 1; y++) {
		for (int x = x_pos - 1; x <= x_pos + 1; x++) {
			if (y < 0 || x < 0 || x >= ap_data->x_count[ap_data->level - 1000] || y >= ap_data->y_count[ap_data->level - 1000] || ap_data->board_state[y][x] > mine_num8_closed)
				continue;    // 범위를 벗어나거나 닫히지 않은 것들을 만나면 건너뛰기

			ap_data->board_state[y][x] += 10;    // 10을 더해 열어준다

			// 열린 빈칸이면 함수를 다시 호출하여 그 칸에서도 다시 빈칸들을 열어준다
			if (ap_data->board_state[y][x] == nothing_open) {
				openNothingClosed(ap_data, x, y);
			}
		}
	}
}

// 깃발과 물음표, 원래 상태를 토글
void flagQuesBoard(pGameData ap_data, unsigned int x, unsigned int y)
{
	if (x < ap_data->x_count[ap_data->level - 1000] && y < ap_data->y_count[ap_data->level - 1000]) {
		if (ap_data->board_state[y][x] <= mine_closed) {    // 닫힌 칸에만 적용
			ap_data->board_temp[y][x] = ap_data->board_state[y][x];    // 원래 상태를 임시 판에 저장
			ap_data->board_state[y][x] = flag;    // 원래 상태를 깃발로 변경
		}
		else if (ap_data->board_state[y][x] == flag) {
			ap_data->board_state[y][x] = questionMark;    // 깃발을 물음표로 변경
		}
		else if (ap_data->board_state[y][x] == questionMark) {
			ap_data->board_state[y][x] = ap_data->board_temp[y][x];    // 물음표를 원래 상태로 변경
		}

		drawBoard(ap_data);    // 판 그리기
	}
}