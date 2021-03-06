#ifndef __CONSTANT_H__
#define __CONSTANT_H__

/*
*       < 보드판 상태 >
*
*        0 : 아무것도 없음 (타일 닫힘)
*    1 ~ 8 : 주변 지뢰의 개수 (타일 닫힘)
*        9 : 지뢰 (타일 닫힘)
*       10 : 아무것도 없음 (타일 열림)
*  11 ~ 18 : 주변 지뢰의 개수 (타일 열림)
*       19 : 지뢰 (타일 열림)
*       20 : 깃발
*       21 : 물음표
*       22 : 깃발로 찾은 지뢰
*/
enum boardState {
	nothing_closed,
	mine_num1_closed,
	mine_num2_closed,
	mine_num3_closed,
	mine_num4_closed,
	mine_num5_closed,
	mine_num6_closed,
	mine_num7_closed,
	mine_num8_closed,
	mine,
	nothing_open,
	mine_num1_open,
	mine_num2_open,
	mine_num3_open,
	mine_num4_open,
	mine_num5_open,
	mine_num6_open,
	mine_num7_open,
	mine_num8_open,
	flag,
	questionMark
};

// 게임 난이도, 버튼 id
#define EASY   1000    // 난이도 쉬움
#define NORMAL 1001    // 난이도 보통
#define HARD   1002    // 난이도 어려움

// 게임 버튼 id
#define RESTART   1010    // 게임 재시작
#define TITLE     1011    // 레벨 선택
#define RULE      1020    // 게임 룰
#define RANK      1030    // 랭킹
#define CLEARRANK 1031    // 랭킹 초기화

// 보드판 x, y축 크기
#define EASY_X_COUNT    9    // 쉬움 난이도 x축 개수
#define EASY_Y_COUNT    9    // 쉬움 난이도 y축 개수
#define NORMAL_X_COUNT 16    // 보통 난이도 x축 개수
#define NORMAL_Y_COUNT 16    // 보통 난이도 y축 개수
#define HARD_X_COUNT   30    // 어려움 난이도 x축 개수
#define HARD_Y_COUNT   16    // 어려움 난이도 y축 개수

// 타일 하나의 크기
#define EASY_GRID_SIZE   40    // 쉬움 난이도 사각형 크기
#define NORMAL_GRID_SIZE 30    // 보통 난이도 사각형 크기
#define HARD_GRID_SIZE   25    // 어려움 난이도 사각형 크기

// 타일 전체 이동 도트 개수
#define X_MOVING (0)
#define Y_MOVING (60)

// 총 타일 개수
#define EASY_BOARD_NUM   (( 9) * ( 9))    // 쉬움 난이도 총 타일 개수
#define NORMAL_BOARD_NUM ((16) * (16))    // 보통 난이도 총 타일 개수
#define HARD_BOARD_NUM   ((30) * (16))    // 어려움 난이도 총 타일 개수

// 지뢰의 총 개수
#define EASY_MINE_NUM   10    // 쉬움 난이도 총 지뢰 개수
#define NORMAL_MINE_NUM 40    // 보통 난이도 총 지뢰 개수
#define HARD_MINE_NUM   99    // 어려움 난이도 총 지뢰 개수

// 자주 사용하는 색상
#define ORANGE    RGB(200, 150, 30)     // 주황색
#define YELLOW    RGB(255, 255, 0)      // 노랑
#define GREEN     RGB(0, 255, 0)        // 초록
#define BLACK     RGB(0, 0, 0)          // 검은색
#define GRAY      RGB(200, 200, 200)    // 회색
#define DARKGRAY  RGB(150, 150, 150)    // 어두운 회색
#define WHITE     RGB(255, 255, 255)    // 흰색
#define LIGHTBLUE RGB(143, 206, 255)    // 하늘색

// 현재 개임 단계
#define SELECTLV 0    // 난이도 선택 화면
#define PLAYGAME 1    // 게임중 화면
#define CLEARGME 2    // 게임 클리어
#define GAMEOVER 3    // 게임 오버

// 마우스 클릭 상태
#define CLICKED 100    // 마우스 클릭

#endif // !__CONSTANT_H__