#ifndef __CONSTANT_H__
#define __CONSTANT_H__

/*
*        < 보드판 상태 >
* 
*        0 : 아무것도 없음 (타일 닫힘)
*    1 ~ 8 : 주변 지뢰의 개수 (타일 닫힘)
*        9 : 지뢰 (타일 닫힘)
*       10 : 아무것도 없음 (타일 열림)
*  11 ~ 18 : 주변 지뢰의 개수 (타일 열림)
*       19 : 지뢰 (타일 열림)
*       20 : 깃발
*       21 : 물음표
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
	mine_closed,
	nothing_open = 10,
	mine_num1_open,
	mine_num2_open,
	mine_num3_open,
	mine_num4_open,
	mine_num5_open,
	mine_num6_open,
	mine_num7_open,
	mine_num8_open,
	mine_open,
	flag = 20,
	questionMark
};

// 게임 난이도
#define EASY   1000    // 난이도 쉬움
#define NORMAL 1001    // 난이도 보통
#define HARD   1002    // 난이도 어려움

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

// 총 타일 개수
#define EASY_BOARD_NUM   (( 9) * ( 9))    // 쉬움 난이도 총 타일 개수
#define NORMAL_BOARD_NUM ((16) * (16))    // 보통 난이도 총 타일 개수
#define HARD_BOARD_NUM   ((30) * (16))    // 어려움 난이도 총 타일 개수

// 지뢰의 총 개수
#define EASY_MINE_NUM   10    // 쉬움 난이도 총 지뢰 개수
#define NORMAL_MINE_NUM 40    // 보통 난이도 총 지뢰 개수
#define HARD_MINE_NUM   99    // 어려움 난이도 총 지뢰 개수

// 마우스 클릭 상태
#define LBUTTON 100    // 왼쪽 클릭
#define RBUTTON 101    // 오른쪽 클릭

// 자주 사용하는 색상
#define BLACK  RGB(0, 0, 0)          // 검은색
#define WHITE  RGB(255, 255, 255)    // 흰색
#define ORANGE RGB(200, 150, 30)     // 주황색

// 현재 개임 스텝(난이도 선택화면, 게임 플레이 화면)
#define SELECTLV 0    // 난이도 선택 화면
#define PLAYGAME 1    // 게임중 화면
#define GAMEOVER 2    // 게임 오버

#endif // !__CONSTANT_H__
