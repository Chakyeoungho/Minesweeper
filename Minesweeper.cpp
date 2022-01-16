#include "pch.h"
#include "tipsware.h"
#include "Constant.h"    // 필요한 상수를 모아놓은 헤더파일
#include <stdlib.h>      // srand()와 rand()를 사용하기 위한 헤더파일
#include <time.h>        // 난수의 시드값을 설정하기 위한 헤더파일

typedef struct _GameData // 게임 플레이중 필요한 데이터
{
	int board_state[HARD_Y_COUNT][HARD_X_COUNT];  // 가장 큰 사이즈의 보드만 있어도 모든 난이도의 게임을 만들 수 있다
	int board_temp[HARD_Y_COUNT][HARD_X_COUNT];   // 깃발과 물음표를 사용할 때 원래 보드의 상태를 기억
	int gridSize[3];    // 타일 하나의 크기 배열
	int x_count[3];     // x축 타일의 개수
	int y_count[3];     // y축 타일의 개수
	int mineNum[3];     // 지뢰의 개수
	int currFlagNum;
	int level;        // 선택한 난이도
	int game_step;    // 현재 게임 단계
	UINT64 start_time;    // 시작 시간
	UINT64 curr_time;     // 현재 시간
} GameData, *pGameData;

void selectLvButton();    // 난이도 선택 버튼 생성
void selectLevel(pGameData ap_data, int x, int y);    // 난이도 선택
void drawBoard(pGameData ap_data);    // 보드판 그리기
void randMine(pGameData ap_data);     // 랜덤으로 지뢰 생성
void clickBoard(pGameData ap_data, int x, int y);    // 판 클릭
void openNothingClosed(pGameData ap_data, int x_count, int y_count);    // 연쇄적으로 판 오픈
void flagQuesBoard(pGameData ap_data, int x_pos, int y_pos);    // 깃발과 물음표 관리
void checkAndOpenBoard(pGameData ap_data, int x, int y);    // 올바르게 깃발을 놓고 쉬프트와 좌클릭을 누르면 근처 8개의판이 열림

// 타이머가 1초마다 호출할 함수
TIMER StopWatchProc(NOT_USE_TIMER_DATA)
{
	pGameData ap_data = (pGameData)GetAppData();

	if (ap_data->game_step == PLAYGAME) {
		ap_data->curr_time = GetTickCount64() - ap_data->start_time;
		drawBoard(ap_data);
	}
}

// 마우스 왼쪽 버튼을 눌렀을 때 호출될 함수
void OnMouseLeftUP(int a_mixed_key, POINT a_pos)
{
	pGameData p_data = (pGameData)GetAppData();
	int x = a_pos.x / p_data->gridSize[p_data->level - 1000], y = a_pos.y / p_data->gridSize[p_data->level - 1000];

	// 좌클릭만 눌렀을 때
	// 게임 플레이 단계 | 지뢰 타일 오픈
	if (p_data->game_step == PLAYGAME || p_data->game_step == GAMEOVER) {
		if (a_pos.x >= 150 && a_pos.x <= 190 && a_pos.y >= 500 && a_pos.y <= 540) {
			p_data->game_step = PLAYGAME;
			memset(p_data, 0, sizeof(unsigned int) * 16 * 30 * 2);
			p_data->currFlagNum = 0;
			randMine(p_data);    // 지뢰 랜덤으로 생성
			drawBoard(p_data);    // 판 그리기
			p_data->start_time = GetTickCount64();
		}
		else if (p_data->game_step == PLAYGAME) {
			clickBoard(p_data, x, y);
		}
	}
	// 난이도 선택 단계
	else if (p_data->game_step == SELECTLV) {
		selectLevel(p_data, a_pos.x, a_pos.y);
		p_data->start_time = GetTickCount64();
	}
}

// 마우스 왼쪽 버튼은 자주 사용하기 때문에 EasyWin32 시스템이 기본으로 제공하지만 오른쪽 버튼은 
// 제공되지 않아서 메시지를 직접 처리해야 합니다. 그래서 사용자가 직접 메시지를 처리하기 위해 함수를 추가함
int OnUserMsg(HWND ah_wnd, UINT a_message_id, WPARAM wParam, LPARAM lParam)
{
	pGameData p_data = (pGameData)GetAppData();

	int x_pos = LOWORD(lParam);
	int y_pos = HIWORD(lParam);

	if (a_message_id == WM_MBUTTONUP || wParam == 3) {
		if (p_data->board_state[y_pos / p_data->gridSize[p_data->level - 1000]][x_pos / p_data->gridSize[p_data->level - 1000]] >= nothing_open &&
			p_data->board_state[y_pos / p_data->gridSize[p_data->level - 1000]][x_pos / p_data->gridSize[p_data->level - 1000]] <= mine_num8_open) {
			checkAndOpenBoard(p_data, x_pos / p_data->gridSize[p_data->level - 1000], y_pos / p_data->gridSize[p_data->level - 1000]);    // 지뢰를 깃발로 올바르게 찾았을 때 주변 8칸 오픈
		}
	}
	// 마우스 오른쪽 버튼이 눌러진 경우에 처리
	else if (a_message_id == WM_RBUTTONUP) {
		if (p_data->game_step == PLAYGAME) {
			flagQuesBoard(p_data, x_pos, y_pos);
		}
		return 1;
	}
	return 0;
}

// 마우스 왼쪽 버튼을 클릭, 해제 그리고 시스템이 기본으로 처리하지 않는 메시지를 처리하는 함수
ON_MESSAGE(NULL, OnMouseLeftUP, NULL, NULL, NULL, OnUserMsg)

int main()
{
	GameData data = { { { 0, }, },    // 판 상태
					  { { 0, }, },    // 깃발과 물음표를 제외한 판 상태
					  {EASY_GRID_SIZE, NORMAL_GRID_SIZE, HARD_GRID_SIZE},    // 타일 하나의 크기 배열
					  {EASY_X_COUNT, NORMAL_X_COUNT, HARD_X_COUNT},          // x축 타일의 개수
					  {EASY_Y_COUNT, NORMAL_Y_COUNT, HARD_Y_COUNT},          // y축 타일의 개수
					  {EASY_MINE_NUM, NORMAL_MINE_NUM, HARD_MINE_NUM},       // 지뢰의 개수
					  0, 0, 0, 0 };
	SetAppData(&data, sizeof(GameData));    // data를 내부변수로 설정

	SelectFontObject("굴림", 20, 1);    // 글씨체와 크기 설정
	selectLvButton();    // 난이도 선택 버튼 생성

	SetTimer(1, 1000, StopWatchProc);

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
void selectLevel(pGameData ap_data, int x, int y)
{
	// 좌클릭시 마우스의 좌표에 따라 난이도 조정
	if (x >= 20 && x <= 108 && y >= 10 && y <= 38) {
		ap_data->level = EASY;    // 선택한 난이도 저장
		randMine(ap_data);    // 지뢰 랜덤으로 생성
		drawBoard(ap_data);    // 판 그리기
		ap_data->game_step++;    // 다음단계로 이동
	}
	else if (x >= 120 && x <= 208 && y >= 10 && y <= 38) {
		ap_data->level = NORMAL;
		randMine(ap_data);
		drawBoard(ap_data);
		ap_data->game_step++;
	}
	else if (x >= 220 && x <= 308 && y >= 10 && y <= 38) {
		ap_data->level = HARD;
		randMine(ap_data);
		drawBoard(ap_data);
		ap_data->game_step++;
	}
}

// 판 그리기
void drawBoard(pGameData ap_data)
{
	Clear();    // 화면 초기화

	// 판 그리기
	for (int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
		for (int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
			Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], RGB(0, 0, 128), RGB(0, 100, 200));
		}
	}

	// 지뢰 개수, 빈칸, 깃발, 물음표 출력
	for (int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
		for (int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
			if (ap_data->board_state[y][x] >= mine_num1_open && ap_data->board_state[y][x] <= mine_num8_open) {
				Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], BLACK, GRAY);
				TextOut(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], WHITE, "%d", ap_data->board_state[y][x] - 10);
			}
			else if (ap_data->board_state[y][x] == nothing_open)
				Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], BLACK, GRAY);
			else if (ap_data->board_state[y][x] == flag)
				Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], RGB(200, 100, 0), RGB(128, 0, 0));
			else if (ap_data->board_state[y][x] == questionMark)
				Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], WHITE, BLACK);
		}
	}

	if (ap_data->game_step == GAMEOVER) {
		for (int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
			for (int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
				if (ap_data->board_state[y][x] == mine_closed)
					Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], BLACK, BLACK);
			}
		}
	}

	Rectangle(150, 500, 190, 540, ORANGE, ORANGE);
	TextOut(10, 500, BLACK, "%03d", ap_data->curr_time / 1000);
	TextOut(300, 500, BLACK, "%02d", ap_data->mineNum[ap_data->level - 1000] - ap_data->currFlagNum);

	ShowDisplay();
}

// 랜덤으로 지뢰 생성
void randMine(pGameData ap_data)
{
	srand((unsigned int)time(NULL));    // 현재 시간을 시드값으로 설정
	int tempX, tempY;    // 난수를 저장할 임시 변수
	int tempMineNum = 0;    // 현재 생성된 지뢰 개수

	while (tempMineNum != ap_data->mineNum[ap_data->level - 1000]) {    // 난이도에 따른 지뢰 개수만큼
		if (ap_data->board_state[tempY = (rand() % ap_data->y_count[ap_data->level - 1000])][tempX = (rand() % ap_data->x_count[ap_data->level - 1000])] != mine_closed) {
			ap_data->board_state[tempY][tempX] = mine_closed;    // 지뢰가 없으면 지뢰 생성
			tempMineNum++;

			for (int y = tempY - 1; y <= tempY + 1; y++) {
				for (int x = tempX - 1; x <= tempX + 1; x++) {
					if (y < 0 || x < 0 || x >= ap_data->x_count[ap_data->level - 1000] || y >= ap_data->y_count[ap_data->level - 1000] || ap_data->board_state[y][x] == mine_closed)
						continue;
					ap_data->board_state[y][x]++;
				}
			}
		}
	}

	if (tempMineNum > ap_data->mineNum[ap_data->level - 1000]) {
		ap_data->game_step = GAMEOVER;
		return;
	}
}

// 판 클릭시
void clickBoard(pGameData ap_data, int x, int y)
{
	if (x < ap_data->x_count[ap_data->level - 1000] && y < ap_data->y_count[ap_data->level - 1000]) {
		if (ap_data->board_state[y][x] == mine_closed) {    // 지뢰를 누르면 게임 오버, 지뢰 출력
			ap_data->game_step = GAMEOVER;
		}

		// 아무것도 없는 곳을 누르면 열기
		if (ap_data->board_state[y][x] == nothing_closed)
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
void flagQuesBoard(pGameData ap_data, int x_pos, int y_pos)
{
	int x = x_pos / ap_data->gridSize[ap_data->level - 1000], y = y_pos / ap_data->gridSize[ap_data->level - 1000];

	if (x < ap_data->x_count[ap_data->level - 1000] && y < ap_data->y_count[ap_data->level - 1000]) {
		if (ap_data->board_state[y][x] <= mine_closed) {    // 닫힌 칸에만 적용
			ap_data->board_temp[y][x] = ap_data->board_state[y][x];    // 원래 상태를 임시 판에 저장
			ap_data->board_state[y][x] = flag;    // 원래 상태를 깃발로 변경
			ap_data->currFlagNum++;
		}
		else if (ap_data->board_state[y][x] == flag) {
			ap_data->board_state[y][x] = questionMark;    // 깃발을 물음표로 변경
			ap_data->currFlagNum--;
		}
		else if (ap_data->board_state[y][x] == questionMark) {
			ap_data->board_state[y][x] = ap_data->board_temp[y][x];    // 물음표를 원래 상태로 변경
		}

		drawBoard(ap_data);    // 판 그리기
	}
}

// 올바르게 깃발을 놓고 쉬프트와 좌클릭을 누르면 근처 8개의판이 열림
void checkAndOpenBoard(pGameData ap_data, int x, int y)
{
	int rightMineNum = 0;
	int flagNum = 0;

	for (int i = y - 1; i <= y + 1; i++) {
		for (int j = x - 1; j <= x + 1; j++) {
			if (i < 0 || j < 0 || j >= ap_data->x_count[ap_data->level - 1000] || i >= ap_data->y_count[ap_data->level - 1000])
				continue;

			if (ap_data->board_state[i][j] == flag && ap_data->board_temp[i][j] == mine_closed)    // 깃발로 찾은 지뢰 개수
				rightMineNum++;
			if (ap_data->board_state[i][j] == flag)    // 깃발 개수
				flagNum++;
		}
	}

	if (flagNum == ap_data->board_state[y][x] - 10 && rightMineNum == ap_data->board_state[y][x] - 10) {
		for (int i = y - 1; i <= y + 1; i++) {
			for (int j = x - 1; j <= x + 1; j++) {
				if (i < 0 || j < 0 || j >= ap_data->x_count[ap_data->level - 1000] || i >= ap_data->y_count[ap_data->level - 1000] ||
					(ap_data->board_state[i][j] >= nothing_open && ap_data->board_state[i][j] <= mine_num8_open))
					continue;

				if (ap_data->board_state[i][j] == nothing_closed)
					openNothingClosed(ap_data, j, i);
				else if (ap_data->board_state[i][j] >= mine_num1_closed && ap_data->board_temp[i][j] <= mine_num8_closed)
					ap_data->board_state[i][j] += 10;
			}
		}
		drawBoard(ap_data);
	}
	else if (flagNum == ap_data->board_state[y][x] - 10) {
		ap_data->game_step = GAMEOVER;
		drawBoard(ap_data);
	}
}