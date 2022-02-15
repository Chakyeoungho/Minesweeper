#include "pch.h"
#include <stdio.h>       // 파일 입출력, 로컬 랭킹 기록용
#include <sys/stat.h>    // 파일이 있는지 확인하기 위한 stat가 들어있는 헤더파일
#include "tipsware.h"
#include "Constant.h"    // 필요한 상수를 모아놓은 헤더파일
#include <Windowsx.h>    // lParam의 값을 x, y좌표로 바꾸기 위한 매크로가 들어있는 헤더파일
#include <time.h>        // 난수의 시드값을 설정하기 위한 헤더파일

#pragma pack(push, 1)
typedef struct _Rank
{
	UINT64 rank[3][10];    // 랭킹 저장할 배열, 정렬하기 편하게 가로 세로의 값을 정함
} Rank, *pRank;

typedef struct _GameButton
{
	void *p_select_ctrl[3];    // 난이도 선택 버튼 주소
	void *p_game_ctrl[2];      // 재시작, 타이틀 버튼 주소
	void *p_game_rank;         // 랭킹 버튼 주소
	void *p_game_rule;         // 게임 규칙 버튼 주소
	void *p_clear_rank;        // 랭킹 초기화 버튼 주소
} GameButton, *pGameButton;

typedef struct _GameData // 게임 플레이중 필요한 데이터
{
	BYTE board_state[HARD_Y_COUNT][HARD_X_COUNT];    // 가장 큰 사이즈의 보드만 있어도 모든 난이도의 게임을 만들 수 있다
	BYTE board_temp[HARD_Y_COUNT][HARD_X_COUNT];     // 깃발과 물음표를 사용할 때 원래 보드의 상태를 기억
	BYTE click_state[HARD_Y_COUNT][HARD_X_COUNT];    // 판 클릭 상태
	BYTE gridSize[3];     // 타일 하나의 크기 배열
	BYTE x_count[3];      // x축 타일의 개수
	BYTE y_count[3];      // y축 타일의 개수
	BYTE mineNum[3];      // 지뢰의 개수
	BYTE currFlagNum;     // 현재 깃발의 개수
	BYTE game_step;       // 현재 게임 단계
	BYTE rankB_toggle;    // 랭킹 버튼 토글용 변수
	WORD level;           // 선택한 난이도
	bool isClicked;       // 게임 시작후 유효한 첫 클릭을 했는지 체크할 변수
	UINT64 start_time;    // 시작 시간
	UINT64 curr_time;     // 현재 시간
	POINT temp_pos;       // 임시 커서 좌표
	GameButton button_adress;    // 게임 버튼 주소 모아놓은 구조체
} GameData, *pGameData;
#pragma pack(pop)

void CreateSelectLVButton();    // 난이도 선택 버튼 생성
void resetClickState(pGameData ap_data);    // 클릭 상태 초기화
void drawBoard(pGameData ap_data);    // 보드판 그리기
void randMine(pGameData ap_data);     // 랜덤으로 지뢰 생성
void clickBoard(pGameData ap_data, int x, int y);    // 판 클릭
void checkClear(pGameData ap_data);    // 클리어 확인
void writeRank(pGameData ap_data);    // 랭킹 작성
void rank_bubble_sort(UINT64 data[], int count);    // 랭킹 정렬
void openNothingClosed(pGameData ap_data, int x_pos, int y_pos);    // 연쇄적으로 판 오픈
void flagQuesBoard(pGameData ap_data, int x, int y);    // 깃발과 물음표 관리
void checkAndOpen8Board(pGameData ap_data, int x, int y);    // 주변 지뢰의 개수와 같게 깃발을 놓고 휠 클릭, 왼쪽 더블클릭을 하면 근처 8개의판이 열림

// 타이머가 0.1초마다 호출할 함수
TIMER StopWatchProc(NOT_USE_TIMER_DATA)
{
	pGameData ap_data = (pGameData)GetAppData();

	if (ap_data->game_step == PLAYGAME && ap_data->isClicked) {    // 게임 중, 첫 클릭을 했을 때만
		ap_data->curr_time = GetTickCount64() - ap_data->start_time;    // 현재 시간 구하기
		Rectangle(5, 495, 50, 533, WHITE, WHITE);    // 숫자 지우는 용도
		TextOut(10, 500, BLACK, "%03d", ap_data->curr_time / 1000);    // 현재 시간 출력
		ShowDisplay();    // 화면에 출력
	}
}

// 마우스 왼쪽 버튼을 눌렀을 때 호출될 함수
void OnMouseLeftDOWN(int a_mixed_key, POINT a_pos)
{
	pGameData p_data = (pGameData)GetAppData();
	p_data->temp_pos = a_pos;    // 눌렀을 때 좌표를 저장

	if (p_data->game_step == PLAYGAME) {
		resetClickState(p_data);    // 클릭 초기화

		if (a_pos.x > 0 && a_pos.y > 0 &&
			a_pos.x < p_data->gridSize[p_data->level - 1000] * p_data->x_count[p_data->level - 1000] &&
			a_pos.y < p_data->gridSize[p_data->level - 1000] * p_data->y_count[p_data->level - 1000] &&    // 마우스 좌표 볌위 확인
			p_data->board_state[a_pos.y / p_data->gridSize[p_data->level - 1000]][a_pos.x / p_data->gridSize[p_data->level - 1000]] <= mine) {
			p_data->click_state[a_pos.y / p_data->gridSize[p_data->level - 1000]][a_pos.x / p_data->gridSize[p_data->level - 1000]] = CLICKED;    // 클릭
			drawBoard(p_data);    // 판 그리기
		}
	}
}

// 마우스 왼쪽 버튼을 땠을 때 호출될 함수
void OnMouseLeftUP(int a_mixed_key, POINT a_pos)
{
	pGameData p_data = (pGameData)GetAppData();

	if (p_data->game_step == PLAYGAME) {
		resetClickState(p_data);     // 클릭 초기화

		// 좌클릭으로 판 열기
		if (a_pos.x / p_data->gridSize[p_data->level - 1000] == p_data->temp_pos.x / p_data->gridSize[p_data->level - 1000] &&
			a_pos.y / p_data->gridSize[p_data->level - 1000] == p_data->temp_pos.y / p_data->gridSize[p_data->level - 1000]) {
			// 첫 클릭 때 시간 초기화
			if (p_data->isClicked == false) {
				p_data->start_time = GetTickCount64();    // 시작 시간 재설정
				p_data->isClicked = true;    // 처음 우클릭이나 좌클릭을 했을 때 부터 시간을 재기 위한 변수
			}

			clickBoard(p_data, a_pos.x / p_data->gridSize[p_data->level - 1000], a_pos.y / p_data->gridSize[p_data->level - 1000]);    // 판 클릭
			checkClear(p_data);    // 클리어 했는지 확인
		}
		drawBoard(p_data);    // 판 그리기
	}
}

// 게임 버튼 관리
void OnCommand(INT32 a_ctrl_id, INT32 a_notify_code, void *ap_ctrl)
{
	pGameData p_data = (pGameData)GetAppData();

	switch (a_ctrl_id) {
	// 다시 시작 버튼
	case RESTART:
		p_data->game_step = PLAYGAME;    // 게임 스텝 게임중으로 변경
		p_data->currFlagNum = 0;    // 깃발 개수 초기화
		p_data->isClicked = false;    // 첫 클릭 안한것으로 수정
		memset(p_data, 0, sizeof(char) * 16 * 30 * 3);    // 게임정보 초기화

		randMine(p_data);    // 지뢰 랜덤으로 생성
		p_data->start_time = GetTickCount64();    // 시작 시간 재설정
		p_data->curr_time = GetTickCount64() - p_data->start_time;    // 현재 시간 구하기
		drawBoard(p_data);    // 판 그리기
		break;
	// 타이틀 버튼
	case TITLE:
		Clear();    // 화면 초기화
		TextOut(10, 10, BLACK, "Minesweeper");    // 제목

		// 재시작, 타이틀 버튼 숨기기
		ShowControl(p_data->button_adress.p_game_ctrl[0], SW_HIDE);
		ShowControl(p_data->button_adress.p_game_ctrl[1], SW_HIDE);
		// 난이도 선택 버튼 보이기
		ShowControl(p_data->button_adress.p_select_ctrl[0], SW_SHOW);
		ShowControl(p_data->button_adress.p_select_ctrl[1], SW_SHOW);
		ShowControl(p_data->button_adress.p_select_ctrl[2], SW_SHOW);
		// 게임 룰, 랭킹 버튼 보이기
		ShowControl(p_data->button_adress.p_game_rule, SW_SHOW);
		ShowControl(p_data->button_adress.p_game_rank, SW_SHOW);

		memset(p_data, 0, sizeof(char) * 16 * 30 * 2);    // 게임정보 초기화
		p_data->currFlagNum = 0;    // 깃발 개수 초기화
		p_data->isClicked = false;    // 첫 클릭 안한것으로 수정
		p_data->game_step = SELECTLV;    // 난이도 선택단계로 수정
		ShowDisplay();
		break;
	// 난이도 버튼
	case EASY: case NORMAL: case HARD:
		p_data->level = a_ctrl_id;    // 선택한 난이도 저장
		randMine(p_data);    // 지뢰 랜덤으로 생성
		p_data->game_step = PLAYGAME;    // 다음단계로 이동

		// 난이도 선택 버튼 숨기기
		ShowControl(p_data->button_adress.p_select_ctrl[0], SW_HIDE);
		ShowControl(p_data->button_adress.p_select_ctrl[1], SW_HIDE);
		ShowControl(p_data->button_adress.p_select_ctrl[2], SW_HIDE);
		// 게임 룰, 랭킹 버튼 숨기기
		ShowControl(p_data->button_adress.p_game_rule, SW_HIDE);
		ShowControl(p_data->button_adress.p_game_rank, SW_HIDE);
		// 재시작, 타이틀 버튼 보이기
		ShowControl(p_data->button_adress.p_game_ctrl[0], SW_SHOW);
		ShowControl(p_data->button_adress.p_game_ctrl[1], SW_SHOW);

		p_data->start_time = GetTickCount64();    // 시간 초기화
		p_data->curr_time = GetTickCount64() - p_data->start_time;    // 현재 시간 구하기
		drawBoard(p_data);    // 판 그리기
		break;
		// 룰 버튼
	case RULE:
		MessageBox(gh_main_wnd, "        **지뢰가 없는 칸을 모두 클릭하면 클리어 됩니다.**\n\n \
1. 마우스 왼쪽을 누르면 닫혀있는 칸이 열립니다.\n \
2. 마우스 오른쪽 버튼을 누르면 깃발, 물음표, 닫힌 타일 순으로     토글됩니다.\n \
3. 주변 지뢰의 개수만큼 깃발을 놓고 마우스 휠 클릭 or 왼쪽        더블클릭시 주변 8칸이 열립니다.\n \
4. 숫자가 적힌 타일은 주변 지뢰의 개수를 나타냅니다.", "규칙", MB_ICONINFORMATION | MB_OK);
		break;
	// 랭킹 버튼
	case RANK:
		switch (p_data->rankB_toggle) {
		case 0:
		{    // 중괄호로 감싸줘야 파일을 사용할 수 있읍
			Clear();
			// 난이도 선택 버튼 숨기기
			ShowControl(p_data->button_adress.p_select_ctrl[0], SW_HIDE);
			ShowControl(p_data->button_adress.p_select_ctrl[1], SW_HIDE);
			ShowControl(p_data->button_adress.p_select_ctrl[2], SW_HIDE);
			// 게임 룰 버튼 숨기기
			ShowControl(p_data->button_adress.p_game_rule, SW_HIDE);
			// 랭킹 초기화 버튼 보이기
			ShowControl(p_data->button_adress.p_clear_rank, SW_SHOW);

			Rank data;    // 랭킹 저장할 구조체 변수 선언
			FILE *fp = NULL;    // 파일 포인터 생성
			fopen_s(&fp, "MinesweeperRank.bin", "rb");    // 랭킹 파일을 바이너리 읽기 모드로 열기

			// data에 파일 읽기, 읽기 실패시 닫고 종료
			if (fread(&data, sizeof(Rank), 1, fp) < 1) {
				fclose(fp);    // 파일 닫기
				return;    // 종료
			}

			TextOut(120, 100, "Easy");      // 쉬움
			TextOut(320, 100, "Normal");    // 보통
			TextOut(520, 100, "Hard");      // 어려움
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 3; j++) {
					switch (data.rank[j][i]) {
					case ULLONG_MAX:    // 최대값이면 랭킹이 없는 것으로 간주
						TextOut(120 + (200 * j), 150 + (28 * i), "empty");
						break;
					default:    // 쵀대값이 아니면 시간을 구해 출력
						TextOut(120 + (200 * j), 150 + (28 * i), "%02llu'%02llu\"%03llu", data.rank[j][i] / 60000, (data.rank[j][i] % 60000) / 1000, data.rank[j][i] % 1000);
						break;
					}
				}
				TextOut(30, 150 + (28 * i), "%2d.", i + 1);    // 랭킹 순서 출력 -> 1 ~ 10
			}

			fclose(fp);    // 파일 닫기
			break;
		}    // 중괄호로 감싸줘야 파일을 사용할 수 있읍
		default:
			Clear();
			TextOut(10, 10, BLACK, "Minesweeper");    // 게임 제목
			// 난이도 선택 버튼 보이기
			ShowControl(p_data->button_adress.p_select_ctrl[0], SW_SHOW);
			ShowControl(p_data->button_adress.p_select_ctrl[1], SW_SHOW);
			ShowControl(p_data->button_adress.p_select_ctrl[2], SW_SHOW);
			// 게임 룰 버튼 보이기
			ShowControl(p_data->button_adress.p_game_rule, SW_SHOW);
			// 랭킹 초기화 버튼 숨기기
			ShowControl(p_data->button_adress.p_clear_rank, SW_HIDE);
			break;
		}

		p_data->rankB_toggle = !p_data->rankB_toggle;    // 랭킹버튼 토글
		ShowDisplay();
		break;
	// 랭킹 초기화
	case CLEARRANK:
		Clear();    // 화면 초기화

		Rank data;    // 랭킹 저장할 구조체 변수 선언
		FILE *fp = NULL;    // 파일 포인터 생성
		fopen_s(&fp, "MinesweeperRank.bin", "wb");    // 랭킹 파일을 바이너리 쓰기 모드로 열기

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 10; j++) {
				data.rank[i][j] = ULLONG_MAX;    // 자료형의 최대값으로 초기화
			}
		}

		TextOut(120, 100, "Easy");      // 쉬움
		TextOut(320, 100, "Normal");    // 보통
		TextOut(520, 100, "Hard");      // 어려움
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 3; j++) {
				TextOut(120 + (200 * j), 150 + (28 * i), "empty");    // 모두 empty로 출력
			}
			TextOut(30, 150 + (28 * i), "%2d.", i + 1);    // 랭킹 순서 출력 -> 1 ~ 10
		}

		// data의 내용 파일에 쓰기, 쓰기 실패시 닫고 종료
		if (fwrite(&data, sizeof(Rank), 1, fp) < 1) {    // 초기화 후 파일 작성
			fclose(fp);    // 파일 닫기
			return;    // 종료
		}

		fclose(fp);    // 파일 닫기

		ShowDisplay();    // 화면에 출력
		break;
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
			// 눌렀을 때 좌표를 저장
			p_data->temp_pos.x = x_pos;
			p_data->temp_pos.y = y_pos;
			resetClickState(p_data);    // 클릭 초기화

			if (x_pos > 0 && y_pos > 0 &&
				x_pos < p_data->gridSize[p_data->level - 1000] * p_data->x_count[p_data->level - 1000] &&
				y_pos < p_data->gridSize[p_data->level - 1000] * p_data->y_count[p_data->level - 1000] &&    // 마우스 범위 확인
				p_data->board_state[y_pos / p_data->gridSize[p_data->level - 1000]][x_pos / p_data->gridSize[p_data->level - 1000]] <= mine) {
				p_data->click_state[y_pos / p_data->gridSize[p_data->level - 1000]][x_pos / p_data->gridSize[p_data->level - 1000]] = CLICKED;    // 클릭
				drawBoard(p_data);    // 판 그리기
			}
		}

		return 1;    // 종료
	}

	// 마우스 휠 버튼을 누른 경우에 처리
	if (a_message_id == WM_MBUTTONDOWN || a_message_id == WM_MBUTTONDBLCLK) {
		if (p_data->game_step == PLAYGAME) {
			resetClickState(p_data);    // 클릭 초기화

			if (x_pos > 0 && y_pos > 0 &&
				x_pos < p_data->gridSize[p_data->level - 1000] * p_data->x_count[p_data->level - 1000] &&
				y_pos < p_data->gridSize[p_data->level - 1000] * p_data->y_count[p_data->level - 1000]) {    // 마우스 범위 확인
				// 눌렀을 때 좌표를 저장
				p_data->temp_pos.x = x_pos;
				p_data->temp_pos.y = y_pos;

				// 휠을 클릭 했을 때 주변 8칸을 클릭
				for (int i = y_pos / p_data->gridSize[p_data->level - 1000] - 1; i <= y_pos / p_data->gridSize[p_data->level - 1000] + 1; i++) {
					for (int j = x_pos / p_data->gridSize[p_data->level - 1000] - 1; j <= x_pos / p_data->gridSize[p_data->level - 1000] + 1; j++) {
						if (i < 0 || j < 0 || i >= p_data->y_count[p_data->level - 1000] || j >= p_data->x_count[p_data->level - 1000] ||
							(i == y_pos / p_data->gridSize[p_data->level - 1000] && j == x_pos / p_data->gridSize[p_data->level - 1000]) ||
							(p_data->board_state[i][j] >= nothing_open && p_data->board_state[i][j] <= mine_num8_open))
							continue;    // 열린 것들이나 범위를 벗어나면 건너뛰기

						p_data->click_state[i][j] = CLICKED;    // 클릭
					}
				}
				drawBoard(p_data);    // 판 그리기
			}
		}

		return 1;    // 종료
	}

	// 마우스 오른쪽 버튼을 땐 경우에 처리
	if (a_message_id == WM_RBUTTONUP) {
		if (p_data->game_step == PLAYGAME) {
			resetClickState(p_data);     // 클릭 초기화

			if (x_pos / p_data->gridSize[p_data->level - 1000] == p_data->temp_pos.x / p_data->gridSize[p_data->level - 1000] && 
				y_pos / p_data->gridSize[p_data->level - 1000] == p_data->temp_pos.y / p_data->gridSize[p_data->level - 1000]) {
				// 첫 클릭 때 시간 초기화
				if (p_data->isClicked == false) {
					p_data->start_time = GetTickCount64();    // 시작 시간 재설정
					p_data->isClicked = true;    // 처음 우클릭이나 좌클릭을 했을 때 부터 시간을 재기 위한 변수
				}

				flagQuesBoard(p_data, x_pos / p_data->gridSize[p_data->level - 1000], y_pos / p_data->gridSize[p_data->level - 1000]);    // 깃발, 물음표
			}
			drawBoard(p_data);    // 판 그리기
		}

		return 1;    // 종료
	}

	// 마우스 휠버튼, 더블클릭, 양쪽 보튼 클릭 후 오른쪽 버튼을 땜
	if (a_message_id == WM_MBUTTONUP || a_message_id == WM_LBUTTONDBLCLK) {
		if (p_data->game_step == PLAYGAME) {
			resetClickState(p_data);    // 클릭 초기화

			if (p_data->board_state[y_pos / p_data->gridSize[p_data->level - 1000]][x_pos / p_data->gridSize[p_data->level - 1000]] >= nothing_open &&
				p_data->board_state[y_pos / p_data->gridSize[p_data->level - 1000]][x_pos / p_data->gridSize[p_data->level - 1000]] <= mine_num8_open) {
				checkAndOpen8Board(p_data, x_pos / p_data->gridSize[p_data->level - 1000], y_pos / p_data->gridSize[p_data->level - 1000]);
				checkClear(p_data);    // 게임 클리어 확인
			}
			drawBoard(p_data);    // 판 그리기
		}

		return 1;    // 종료
	}

	return 0;    // 종료
}

// 마우스 왼쪽 버튼을 클릭, 해제 그리고 시스템이 기본으로 처리하지 않는 메시지를 처리하는 함수
ON_MESSAGE(OnMouseLeftDOWN, OnMouseLeftUP, NULL, OnCommand, NULL, OnUserMsg)

int main()
{
	// 글꼴, 글자 크기 적용
	SelectFontObject("굴림", 20, 1);

	GameData data = { { { 0, }, },    // 판 상태
					  { { 0, }, },    // 깃발과 물음표를 제외한 판 상태
					  { { 0, }, },    // 판 클릭 상태
					  { EASY_GRID_SIZE, NORMAL_GRID_SIZE, HARD_GRID_SIZE },    // 타일 하나의 크기 배열
					  { EASY_X_COUNT,   NORMAL_X_COUNT,   HARD_X_COUNT   },    // x축 타일의 개수
					  { EASY_Y_COUNT,   NORMAL_Y_COUNT,   HARD_Y_COUNT   },    // y축 타일의 개수
					  { EASY_MINE_NUM,  NORMAL_MINE_NUM,  HARD_MINE_NUM  },    // 지뢰의 개수
					  0, SELECTLV, 0, 0, false };
	SetAppData(&data, sizeof(GameData));    // data를 내부변수로 설정
	
	FILE *fp = NULL;    // 파일 포인터 생성
	Rank temp;    // 임시 변수
	struct stat buffer;

	if (stat("MinesweeperRank.bin", &buffer) == -1) {
		// 랭킹 파일이 없으면 파일을 만들고 모두 UINT64의 최대값으로 초기화
		// 파일이 있으면 0을 반환
		fopen_s(&fp, "MinesweeperRank.bin", "wb");    // 랭킹 파일을 바이너리 쓰기 형식으로 열기

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 10; j++) {
				temp.rank[i][j] = ULLONG_MAX;    // 자료형의 최대값으로 초기화
			}
		}

		// data의 내용 파일에 쓰기, 쓰기 실패시 닫고 종료
		if (fwrite(&temp, sizeof(Rank), 1, fp) < 1) {    // 초기화 후 파일 작성
			fclose(fp);    // 파일 닫기
			return 0;    // 종료
		}

		fclose(fp);    // 파일 닫기
	}
	
	TextOut(10, 10, BLACK, "Minesweeper");    // 게임 제목
	CreateSelectLVButton();    // 난이도 선택 버튼 생성
	SetTimer(1, 100, StopWatchProc);    // 0.1초(100ms)마다 함수를 호출

	ShowDisplay();    // 화면에 출력
	return 0;
}

// 선택 버튼 생성
void CreateSelectLVButton() {
	pGameData ap_data = (pGameData)GetAppData();

	// 난이도 선택 버튼 만들고 주소 저장
	ap_data->button_adress.p_select_ctrl[0] = CreateButton("Easy", 10, 100, 98, 120, EASY);         // 쉬움
	ap_data->button_adress.p_select_ctrl[1] = CreateButton("Normal", 110, 100, 98, 120, NORMAL);    // 보통
	ap_data->button_adress.p_select_ctrl[2] = CreateButton("Hard", 210, 100, 98, 120, HARD);        // 어려움
	// 랭킹, 랭킹 초기화 버튼 만들고 주소 저장
	ap_data->button_adress.p_game_rank = CreateButton("Rank", 700, 50, 50, 50, RANK);          // 랭킹
	ap_data->button_adress.p_clear_rank = CreateButton("Clear", 700, 500, 50, 50, CLEARRANK);    // 랭킹 초기화
	// 게임 룰 버튼 만들고 주소 저장
	ap_data->button_adress.p_game_rule = CreateButton("Rule", 310, 100, 50, 50, RULE);    // 규칙
	// 재시작, 타이틀 버튼 만들고 주소 저장
	ap_data->button_adress.p_game_ctrl[0] = CreateButton("Restart", 70, 490, 100, 40, RESTART);    // 재시작
	ap_data->button_adress.p_game_ctrl[1] = CreateButton("Title", 180, 490, 100, 40, TITLE);       // 타이틀

	// 재시작, 타이틀 버튼 숨기기
	ShowControl(ap_data->button_adress.p_game_ctrl[0], SW_HIDE);
	ShowControl(ap_data->button_adress.p_game_ctrl[1], SW_HIDE);
	// 랭킹 초기화 버튼 숨기기
	ShowControl(ap_data->button_adress.p_clear_rank, SW_HIDE);
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

	Rectangle(5, 495, 67, 523, WHITE, WHITE);    // 숫자 지우는 용도
	TextOut(10, 500, BLACK, "%03d", ap_data->curr_time / 1000);    // 현재 시간 출력
	TextOut(300, 500, BLACK, "%02d", ap_data->mineNum[ap_data->level - 1000] - ap_data->currFlagNum);    // 남은 깃발 개수 출력

	// 지뢰 개수, 빈칸, 깃발, 물음표 출력
	for (int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
		for (int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
			// 판 그리기
			Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], RGB(0, 0, 128), RGB(0, 100, 200));

			if (ap_data->click_state[y][x] == CLICKED)    // 클릭된 타일
				Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], RGB(0, 100, 200), RGB(0, 0, 128));

			switch (ap_data->board_state[y][x]) {
			case nothing_open:    // 그냥 열려있는 타일
				Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], BLACK, GRAY);
				break;
			case mine_num1_open: case mine_num2_open: case mine_num3_open:
			case mine_num4_open:					  case mine_num5_open:    // 주변의 지뢰 개수가 적힌 열린 타일
			case mine_num6_open: case mine_num7_open: case mine_num8_open:
				Rectangle(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], BLACK, GRAY);
				TextOut(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], WHITE, "%d", ap_data->board_state[y][x] - 10);
				break;
			case flag:    // 깃발
				Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], WHITE, RGB(128, 0, 0));
				break;
			case questionMark:    // 물음표
				Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], WHITE, YELLOW);
				break;
			default:
				break;
			}
		}
	}

	// 게임 클리어시 걸린 시간 출력
	if (ap_data->game_step == CLEARGME) {
		UINT64 minute = ap_data->curr_time / 60000;
		UINT64 sec = (ap_data->curr_time % 60000) / 1000;
		UINT64 mSec = ap_data->curr_time % 1000;

		TextOut(400, 500, RGB(100, 255, 100), "Game Clear!");
		TextOut(550, 500, BLACK, "Time : %02llu'%02llu\"%03llu", minute, sec, mSec);
	}

	// 게임오버시 지뢰의 위치와 깃발로 잘못 찾은 지뢰 출력
	if (ap_data->game_step == GAMEOVER) {
		for (int y = 0; y < ap_data->y_count[ap_data->level - 1000]; y++) {
			for (int x = 0; x < ap_data->x_count[ap_data->level - 1000]; x++) {
				if (ap_data->board_state[y][x] == mine)    // 지뢰 출력
					Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], BLACK, BLACK);
				else if (ap_data->board_state[y][x] == flag && ap_data->board_temp[y][x] != mine)    // 지뢰 잘못 찾은 깃발 출력
					Ellipse(x * ap_data->gridSize[ap_data->level - 1000], y * ap_data->gridSize[ap_data->level - 1000], (x + 1) * ap_data->gridSize[ap_data->level - 1000], (y + 1) * ap_data->gridSize[ap_data->level - 1000], BLACK, GREEN);
			}
		}

		TextOut(400, 500, RGB(255, 0, 0), "Game Over!");
	}

	ShowDisplay();    // 화면에 출력
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
					if (y < 0 || x < 0 || x >= ap_data->x_count[ap_data->level - 1000] || y >= ap_data->y_count[ap_data->level - 1000] ||
						ap_data->board_state[y][x] == mine)    // 범위, 지뢰 체크 
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
			ap_data->game_step = GAMEOVER;    // 게임 단계를 게임 오버로 수정

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
	int closedTile = 0;    // 닫힌 타일

	for (int i = 0; i < ap_data->y_count[ap_data->level - 1000]; i++) {
		for (int j = 0; j < ap_data->x_count[ap_data->level - 1000]; j++) {
			if (ap_data->board_state[i][j] <= 9 || ap_data->board_state[i][j] == flag || ap_data->board_state[i][j] == questionMark)
				closedTile++;    // 닫힌 타일 1증가
		}
	}

	if (closedTile == ap_data->mineNum[ap_data->level - 1000]) {    // 닫힌 타일의 개수가 지뢰의 개수와 같으면
		ap_data->game_step = CLEARGME;    // 게임 단계를 클리어로 수정
		writeRank(ap_data);
	}
}

// 게임 클리어 시간 랭킹 작성
void writeRank(pGameData ap_data)
{
	Rank data;    // 랭킹 저장할 구조체 변수 선언
	FILE *fp;    // 

	// data로 파일 읽기
	fopen_s(&fp, "MinesweeperRank.bin", "rb");    // 랭킹 파일을 읽기 용도로 열기

	if (fread(&data, sizeof(Rank), 1, fp) < 1) {    // rank 구조체로 파일 읽기
		fclose(fp);    // 파일 닫기
		return;    // 종료
	}

	fclose(fp);    // 파일 닫기

	// 랭킹 업데이트
	if (ap_data->curr_time < data.rank[ap_data->level - 1000][9]) {    // 랭킹 10위 이내에 들면
		fopen_s(&fp, "MinesweeperRank.bin", "wb");    // 랭킹 파일을 쓰기 용도로 열기

		data.rank[ap_data->level - 1000][9] = ap_data->curr_time;
		rank_bubble_sort(data.rank[ap_data->level - 1000], 10);

		if (fwrite(&data, sizeof(Rank), 1, fp) < 1) {    // rank 구조체로 파일 읽기
			fclose(fp);    // 파일 닫기
			return;    // 파일 포인터 생성
		}

		fclose(fp);    // 파일 닫기
	}
}

// 랭킹 정렬
void rank_bubble_sort(UINT64 arr[], int count)
{
	UINT64 temp;

	for (int i = 0; i < count; i++) {
		for (int j = 0; j < count - 1 - i; j++) {
			if (arr[j] >= arr[j + 1]) {
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
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
				openNothingClosed(ap_data, x, y);    // 재귀
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
		else if (ap_data->board_state[y][x] == flag) {    // 깃발 이면
			ap_data->board_state[y][x] = questionMark;    // 깃발을 물음표로 변경
			ap_data->currFlagNum--;
		}
		else if (ap_data->board_state[y][x] == questionMark) {    // 물음표 이면
			ap_data->board_state[y][x] = ap_data->board_temp[y][x];    // 물음표를 원래 상태로 변경
		}

		drawBoard(ap_data);    // 판 그리기
	}
}

// 주변 지뢰의 개수와 같게 깃발을 놓고 휠 클릭, 왼쪽 더블클릭을 하면 근처 8개의판이 열림
void checkAndOpen8Board(pGameData ap_data, int x, int y)
{
	int rightFlagNum = 0;    // 주변 8칸의 올바르게 놓은 깃발의 개수
	int totalFlagNum = 0;    // 주변 8칸의 깃발의 개수

	for (int i = y - 1; i <= y + 1; i++) {
		for (int j = x - 1; j <= x + 1; j++) {
			if (i < 0 || j < 0 || j >= ap_data->x_count[ap_data->level - 1000] || i >= ap_data->y_count[ap_data->level - 1000])
				continue;    // 범위 확인

			if (ap_data->board_state[i][j] == flag && ap_data->board_temp[i][j] == mine)
				rightFlagNum++;    // 올바르게 놓은 깃발의 개수 1증가
			if (ap_data->board_state[i][j] == flag)
				totalFlagNum++;    // 깃발 개수 1증가
		}
	}

	// 총 깃발 개수와 올바르게 찾은 깃발의 개수가 주변 8자리 지뢰의 수와 같으면
	if (totalFlagNum == ap_data->board_state[y][x] - 10 && rightFlagNum == ap_data->board_state[y][x] - 10) {
		for (int i = y - 1; i <= y + 1; i++) {
			for (int j = x - 1; j <= x + 1; j++) {
				if (i < 0 || j < 0 || j >= ap_data->x_count[ap_data->level - 1000] || i >= ap_data->y_count[ap_data->level - 1000] ||
					(ap_data->board_state[i][j] >= nothing_open && ap_data->board_state[i][j] <= mine_num8_open) ||
					ap_data->board_state[i][j] == flag || ap_data->board_state[i][j] == questionMark)
					continue;    // 범위, 깃발, 물음표 확인

				if (ap_data->board_state[i][j] == nothing_closed)
					openNothingClosed(ap_data, j, i);    // 판 열기
				else if (ap_data->board_state[i][j] >= mine_num1_closed && ap_data->board_temp[i][j] <= mine_num8_closed)
					ap_data->board_state[i][j] += 10;    // 판 열기
			}
		}
	}
	// 잘못 찾고 총 깃발의 개수와 주변 8칸의 지뢰의 개수가 같으면
	else if (totalFlagNum == ap_data->board_state[y][x] - 10) {
		ap_data->game_step = GAMEOVER;
	}

	drawBoard(ap_data);    // 판 그리기
}