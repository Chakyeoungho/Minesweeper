#include "pch.h"
#include "tipsware.h"
#include "Constant.h"    // 필요한 상수를 모아놓은 헤더파일
#include <Windowsx.h>    // lParam의 값을 x, y좌표로 바꾸기 위한 매크로가 들어있는 헤더파일
#include <time.h>        // 난수의 시드값을 설정하기 위한 헤더파일

typedef struct _GameData // 게임 플레이중 필요한 데이터
{
	char board_state[HARD_Y_COUNT][HARD_X_COUNT];   // 가장 큰 사이즈의 보드만 있어도 모든 난이도의 게임을 만들 수 있다
	char board_temp[HARD_Y_COUNT][HARD_X_COUNT];    // 깃발과 물음표를 사용할 때 원래 보드의 상태를 기억
	char click_state[HARD_Y_COUNT][HARD_X_COUNT];   // 판 클릭 상태
	char gridSize[3];     // 타일 하나의 크기 배열
	char x_count[3];      // x축 타일의 개수
	char y_count[3];      // y축 타일의 개수
	char mineNum[3];      // 지뢰의 개수
	char currFlagNum;     // 현재 깃발의 개수
	char game_step;       // 현재 게임 단계
	short level;          // 선택한 난이도
	UINT64 start_time;    // 시작 시간
	UINT64 curr_time;     // 현재 시간
	POINT temp_pos;       // 임시 커서 좌표
} GameData, *pGameData;

void selectLvButton();    // 난이도 선택 버튼 생성
void selectLevel(pGameData ap_data, int x, int y);    // 난이도 선택
void resetClickState(pGameData ap_data);    // 클릭 상태 초기화
void drawBoard(pGameData ap_data);    // 보드판 그리기
void randMine(pGameData ap_data);     // 랜덤으로 지뢰 생성
void clickBoard(pGameData ap_data, int x, int y);    // 판 클릭
void checkClear(pGameData ap_data);    // 클리어 확인
void openNothingClosed(pGameData ap_data, int x_pos, int y_pos);    // 연쇄적으로 판 오픈
void flagQuesBoard(pGameData ap_data, int x, int y);    // 깃발과 물음표 관리
void checkAndOpenBoard(pGameData ap_data, int x, int y);    // 올바르게 깃발을 놓고 휠을 누르면 근처 8개의판이 열림

// 타이머가 1초마다 호출할 함수
TIMER StopWatchProc(NOT_USE_TIMER_DATA)
{
	pGameData ap_data = (pGameData)GetAppData();

	if (ap_data->game_step == PLAYGAME) {    // 게임중일 때만
		ap_data->curr_time = GetTickCount64() - ap_data->start_time;    // 현재시간 구하기
		Rectangle(5, 495, 50, 523, WHITE, WHITE);    // 숫자 지우는 용도
		TextOut(10, 500, BLACK, "%03d", ap_data->curr_time / 1000);    // 현재시간 출력
		ShowDisplay();
	}
}

// 마우스 왼쪽 버튼을 눌렀을 때 호출될 함수
void OnMouseLeftDOWN(int a_mixed_key, POINT a_pos)
{
	pGameData p_data = (pGameData)GetAppData();
	p_data->temp_pos = a_pos;

	if (p_data->game_step == PLAYGAME) {
		resetClickState(p_data);

		if (p_data->board_state[a_pos.y / p_data->gridSize[p_data->level - 1000]][a_pos.x / p_data->gridSize[p_data->level - 1000]] <= mine) {
			p_data->click_state[a_pos.y / p_data->gridSize[p_data->level - 1000]][a_pos.x / p_data->gridSize[p_data->level - 1000]] = CLICKED;
			drawBoard(p_data);
		}
	}
}

// 마우스 왼쪽 버튼을 땠을 때 호출될 함수
void OnMouseLeftUP(int a_mixed_key, POINT a_pos)
{
	pGameData p_data = (pGameData)GetAppData();

	// 난이도 선택 단계
	if (p_data->game_step == SELECTLV) {
		selectLevel(p_data, a_pos.x, a_pos.y);
		p_data->start_time = GetTickCount64();
	}
	// 다시 시작 버튼 클릭
	else if (a_pos.x >= 150 && a_pos.x <= 190 && a_pos.y >= 500 && a_pos.y <= 540) {
		resetClickState(p_data);

		if (p_data->temp_pos.x >= 150 && p_data->temp_pos.x <= 190 && p_data->temp_pos.y >= 500 && p_data->temp_pos.y <= 540) {
			p_data->game_step = PLAYGAME;
			p_data->currFlagNum = 0;    // 깃발 개수 초기화
			memset(p_data, 0, sizeof(char) * 16 * 30 * 2);    // 게임정보 초기화
			randMine(p_data);    // 지뢰 랜덤으로 생성
			p_data->start_time = GetTickCount64();    // 현재시간 구하기
			p_data->curr_time = GetTickCount64() - p_data->start_time;    // 현재시간 구하기
		}
		drawBoard(p_data);    // 판 그리기
	}
	// 게임 플레이 단계 | 지뢰 타일 오픈
	else if (p_data->game_step == PLAYGAME) {
		resetClickState(p_data);

		// 좌클릭으로 판 열기
		if (a_pos.x / p_data->gridSize[p_data->level - 1000] == p_data->temp_pos.x / p_data->gridSize[p_data->level - 1000] &&
			a_pos.y / p_data->gridSize[p_data->level - 1000] == p_data->temp_pos.y / p_data->gridSize[p_data->level - 1000]) {
			clickBoard(p_data, a_pos.x / p_data->gridSize[p_data->level - 1000], a_pos.y / p_data->gridSize[p_data->level - 1000]);
			checkClear(p_data);
		}
		drawBoard(p_data);
	}
}

// 마우스 왼쪽 버튼은 자주 사용하기 때문에 EasyWin32 시스템이 기본으로 제공하지만 오른쪽 버튼은 
// 제공되지 않아서 메시지를 직접 처리해야 합니다. 그래서 사용자가 직접 메시지를 처리하기 위해 함수를 추가함
int OnUserMsg(HWND ah_wnd, UINT a_message_id, WPARAM wParam, LPARAM lParam)
{
	pGameData p_data = (pGameData)GetAppData();

	int x_pos = GET_X_LPARAM(lParam);
	int y_pos = GET_Y_LPARAM(lParam);

	// 마우스 오른쪽 버튼을 누른 경우에 처리
	if (a_message_id == WM_RBUTTONDOWN) {
		if (p_data->game_step == PLAYGAME) {
			p_data->temp_pos.x = x_pos;
			p_data->temp_pos.y = y_pos;
			resetClickState(p_data);

			if (p_data->board_state[y_pos / p_data->gridSize[p_data->level - 1000]][x_pos / p_data->gridSize[p_data->level - 1000]] <= mine) {
				p_data->click_state[y_pos / p_data->gridSize[p_data->level - 1000]][x_pos / p_data->gridSize[p_data->level - 1000]] = CLICKED;
				drawBoard(p_data);

				return 1;
			}
		}
	}
	// 마우스 휠버튼, 2개 이상의 버튼을 누른 경우에 처리
	else if (a_message_id  == WM_MBUTTONDOWN || a_message_id == WM_MBUTTONDBLCLK) {
		if (p_data->game_step == PLAYGAME) {
			resetClickState(p_data);

			if (x_pos / p_data->gridSize[p_data->level - 1000] < p_data->x_count[p_data->level - 1000] &&
				y_pos / p_data->gridSize[p_data->level - 1000] < p_data->y_count[p_data->level - 1000]) {
				p_data->temp_pos.x = x_pos;
				p_data->temp_pos.y = y_pos;

				for (int i = y_pos / p_data->gridSize[p_data->level - 1000] - 1; i <= y_pos / p_data->gridSize[p_data->level - 1000] + 1; i++) {
					for (int j = x_pos / p_data->gridSize[p_data->level - 1000] - 1; j <= x_pos / p_data->gridSize[p_data->level - 1000] + 1; j++) {
						if (i < 0 || j < 0 || i >= p_data->y_count[p_data->level - 1000] || j >= p_data->x_count[p_data->level - 1000] ||
							(i == y_pos / p_data->gridSize[p_data->level - 1000] && j == x_pos / p_data->gridSize[p_data->level - 1000]) ||
							(p_data->board_state[i][j] >= nothing_open && p_data->board_state[i][j] <= mine_num8_open))
							continue;

						p_data->click_state[i][j] = CLICKED;
					}
				}
				drawBoard(p_data);

				return 1;
			}
		}
	}

	// 마우스 오른쪽 버튼을 땐 경우에 처리
	if (a_message_id == WM_RBUTTONUP) {
		if (p_data->game_step == PLAYGAME) {
			resetClickState(p_data);

			if (x_pos / p_data->gridSize[p_data->level - 1000] == p_data->temp_pos.x / p_data->gridSize[p_data->level - 1000] &&
				y_pos / p_data->gridSize[p_data->level - 1000] == p_data->temp_pos.y / p_data->gridSize[p_data->level - 1000]) {
				flagQuesBoard(p_data, x_pos / p_data->gridSize[p_data->level - 1000], y_pos / p_data->gridSize[p_data->level - 1000]);
			}
			drawBoard(p_data);

			return 1;
		}
	}
	// 마우스 휠버튼, 더블클릭, 양쪽 보튼 클릭 후 오른쪽 버튼을 땜
	else if (a_message_id == WM_MBUTTONUP || a_message_id == WM_LBUTTONDBLCLK) {
		if (p_data->game_step == PLAYGAME) {
			resetClickState(p_data);

			if (p_data->board_state[y_pos / p_data->gridSize[p_data->level - 1000]][x_pos / p_data->gridSize[p_data->level - 1000]] >= nothing_open &&
				p_data->board_state[y_pos / p_data->gridSize[p_data->level - 1000]][x_pos / p_data->gridSize[p_data->level - 1000]] <= mine_num8_open) {
				checkAndOpenBoard(p_data, x_pos / p_data->gridSize[p_data->level - 1000], y_pos / p_data->gridSize[p_data->level - 1000]);
				checkClear(p_data);
			}
			drawBoard(p_data);
				
			return 1;
		}
	}

	return 0;
}

// 마우스 왼쪽 버튼을 클릭, 해제 그리고 시스템이 기본으로 처리하지 않는 메시지를 처리하는 함수
ON_MESSAGE(OnMouseLeftDOWN, OnMouseLeftUP, NULL, NULL, NULL, OnUserMsg)

int main()
{
	GameData data = { { { 0, }, },    // 판 상태
					  { { 0, }, },    // 깃발과 물음표를 제외한 판 상태
					  { { 0, }, },    // 깃발과 물음표를 제외한 판 상태
					  {EASY_GRID_SIZE, NORMAL_GRID_SIZE, HARD_GRID_SIZE},    // 타일 하나의 크기 배열
					  {EASY_X_COUNT,   NORMAL_X_COUNT,   HARD_X_COUNT  },    // x축 타일의 개수
					  {EASY_Y_COUNT,   NORMAL_Y_COUNT,   HARD_Y_COUNT  },    // y축 타일의 개수
					  {EASY_MINE_NUM,  NORMAL_MINE_NUM,  HARD_MINE_NUM },    // 지뢰의 개수
					  0, SELECTLV, 0 };
	SetAppData(&data, sizeof(GameData));    // data를 내부변수로 설정

	SelectFontObject("굴림", 20, 1);    // 글씨체와 크기 설정
	selectLvButton();    // 난이도 선택 버튼 생성

	// 1초(1000ms)마다 함수를 호출
	SetTimer(1, 100, StopWatchProc);

	ShowDisplay();    // 화면에 출력
	return 0;
}

// 난이도 선택 버튼 생성
void selectLvButton()
{
	Clear();

	Rectangle(20, 10, 108, 38, ORANGE, ORANGE);     // 쉬움lv
	TextOut(43, 13, BLACK, "쉬움");

	Rectangle(120, 10, 208, 38, ORANGE, ORANGE);    // 보통lv
	TextOut(143, 13, BLACK, "보통");

	Rectangle(220, 10, 308, 38, ORANGE, ORANGE);    // 어려움lv
	TextOut(233, 13, BLACK, "어려움");

	ShowDisplay();
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

// 클릭 생태를 저장하는 판 초기화
void resetClickState(pGameData ap_data)
{
	for (int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
		for (int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
			ap_data->click_state[y][x] = 0;
		}
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

	Rectangle(150, 500, 190, 540, ORANGE, ORANGE);    // 재시작 버튼 생성
	Rectangle(5, 495, 50, 523, WHITE, WHITE);    // 숫자 지우는 용도
	TextOut(10, 500, BLACK, "%03d", ap_data->curr_time / 1000);    // 현재시간 출력
	TextOut(300, 500, BLACK, "%02d", ap_data->mineNum[ap_data->level - 1000] - ap_data->currFlagNum);    // 남은 깃발 개수 출력

	// 지뢰 개수, 빈칸, 깃발, 물음표 출력
	for (int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
		for (int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
			if (ap_data->click_state[y][x] == CLICKED)    // 클릭된 타일
				Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], RGB(0, 100, 200), RGB(0, 0, 128));

			switch (ap_data->board_state[y][x]) {
			case nothing_open:    // 그냥 열려있는 타일
				Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], BLACK, GRAY);
				break;
			case mine_num1_open: case mine_num2_open: case mine_num3_open:
			case mine_num4_open:					  case mine_num5_open:    // 주변 지뢰 개수가 적힌 열린 타일
			case mine_num6_open: case mine_num7_open: case mine_num8_open:
				Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], BLACK, GRAY);
				TextOut(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], WHITE, "%d", ap_data->board_state[y][x] - 10);
				break;
			case flag:    // 깃발
				Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], WHITE, RGB(128, 0, 0));
				break;
			case questionMark:    // 물음표
				Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], WHITE, BLACK);
				break;
			default:
				break;
			}
		}
	}
	
	if (ap_data->game_step == GAMEOVER) {
		for (int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
			for (int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
				if (ap_data->board_state[y][x] == mine)
					Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], BLACK, BLACK);
			}
		}

		TextOut(400, 500, RGB(255, 0, 0), "Game Over!");
	}

	if (ap_data->game_step == CLEARGME) {
		UINT64 minute = ap_data->curr_time / 60000;
		UINT64 sec = (ap_data->curr_time % 60000) / 1000;
		UINT64 mSec = ap_data->curr_time % 1000;

		TextOut(400, 500, RGB(100, 255, 100), "Game Clear!");
		TextOut(550, 500, BLACK, "Time : %02llu'%02llu\"%03llu", minute, sec, mSec);
	}

	ShowDisplay();
}

// 랜덤으로 지뢰 생성
void randMine(pGameData ap_data)
{
	srand((unsigned int)time(NULL));    // 현재 시간을 시드값으로 설정
	int tempX, tempY;    // 난수를 저장할 임시 변수
	int tempMineNum = 0;    // 현재 생성된 지뢰 개수

	while (tempMineNum != ap_data->mineNum[ap_data->level - 1000]) {    // 난이도에 따른 지뢰 개수만큼
		if (ap_data->board_state[tempY = (rand() % ap_data->y_count[ap_data->level - 1000])][tempX = (rand() % ap_data->x_count[ap_data->level - 1000])] != mine) {
			ap_data->board_state[tempY][tempX] = mine;    // 지뢰가 없으면 지뢰 생성
			tempMineNum++;

			// 지뢰가 생성되는 순간 주위 8곳에 1증가
			for (int y = tempY - 1; y <= tempY + 1; y++) {
				for (int x = tempX - 1; x <= tempX + 1; x++) {
					if (y < 0 || x < 0 || x >= ap_data->x_count[ap_data->level - 1000] || y >= ap_data->y_count[ap_data->level - 1000] || ap_data->board_state[y][x] == mine)
						continue;
					ap_data->board_state[y][x]++;
				}
			}
		}
	}
}

// 판 클릭시
void clickBoard(pGameData ap_data, int x, int y)
{
	if (x < ap_data->x_count[ap_data->level - 1000] && y < ap_data->y_count[ap_data->level - 1000]) {
		if (ap_data->board_state[y][x] == mine)    // 지뢰를 누르면 게임 오버, 지뢰 출력
			ap_data->game_step = GAMEOVER;

		// 아무것도 없는 곳을 누르면 열기
		if (ap_data->board_state[y][x] == nothing_closed)
			openNothingClosed(ap_data, x, y);    // 연쇄적으로 오픈
		else if (ap_data->board_state[y][x] <= mine_num8_closed)
			ap_data->board_state[y][x] += 10;    // 닫힌 숫자들에 10을 더해 열린 10으로 만듦
	}
}

// 게임 클리어 여부 확인
void checkClear(pGameData ap_data)
{
	int closedTile = 0;

	if (ap_data->game_step == PLAYGAME) {
		for (int i = 0; i < ap_data->y_count[ap_data->level - 1000]; i++) {
			for (int j = 0; j < ap_data->x_count[ap_data->level - 1000]; j++) {
				if (ap_data->board_state[i][j] <= 9 || ap_data->board_state[i][j] == flag || ap_data->board_state[i][j] == questionMark)
					closedTile++;
			}
		}

		if (closedTile == ap_data->mineNum[ap_data->level - 1000])
			ap_data->game_step = CLEARGME;
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
void flagQuesBoard(pGameData ap_data, int x, int y)
{
	if (x < ap_data->x_count[ap_data->level - 1000] && y < ap_data->y_count[ap_data->level - 1000]) {
		if (ap_data->board_state[y][x] <= mine) {    // 닫힌 칸에만 적용
			ap_data->board_temp[y][x] = ap_data->board_state[y][x];    // 원래 상태를 임시 판에 저장
			ap_data->board_state[y][x] = flag;    // 원래 상태를 깃발로 변경
			ap_data->currFlagNum++;

			checkClear(ap_data);    // 클리어 체크
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

// 올바르게 깃발을 놓고 휠을 누르면 근처 8개의판이 열림
void checkAndOpenBoard(pGameData ap_data, int x, int y)
{
	int rightMineNum = 0;
	int flagNum = 0;

	for (int i = y - 1; i <= y + 1; i++) {
		for (int j = x - 1; j <= x + 1; j++) {
			if (i < 0 || j < 0 || j >= ap_data->x_count[ap_data->level - 1000] || i >= ap_data->y_count[ap_data->level - 1000])
				continue;

			if (ap_data->board_state[i][j] == questionMark && ap_data->board_temp[i][j] == mine)
				return;

			if (ap_data->board_state[i][j] == flag && ap_data->board_temp[i][j] == mine)    // 깃발로 찾은 지뢰 개수
				rightMineNum++;
			if (ap_data->board_state[i][j] == flag)    // 깃발 개수
				flagNum++;
		}
	}

	if (flagNum == ap_data->board_state[y][x] - 10 && rightMineNum == ap_data->board_state[y][x] - 10) {
		for (int i = y - 1; i <= y + 1; i++) {
			for (int j = x - 1; j <= x + 1; j++) {
				if (i < 0 || j < 0 || j >= ap_data->x_count[ap_data->level - 1000] || i >= ap_data->y_count[ap_data->level - 1000] ||
					(ap_data->board_state[i][j] >= nothing_open && ap_data->board_state[i][j] <= mine_num8_open) || 
					ap_data->board_state[i][j] == flag || ap_data->board_state[i][j] == questionMark)
					continue;

				if (ap_data->board_state[i][j] == nothing_closed)
					openNothingClosed(ap_data, j, i);
				else if (ap_data->board_state[i][j] >= mine_num1_closed && ap_data->board_temp[i][j] <= mine_num8_closed)
					ap_data->board_state[i][j] += 10;
			}
		}
	}
	else if (flagNum == ap_data->board_state[y][x] - 10) {
		ap_data->game_step = GAMEOVER;
	}

	drawBoard(ap_data);
}