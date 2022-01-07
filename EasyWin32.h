#ifndef _EASY_WIN32_H_
#define _EASY_WIN32_H_

// 이 라이브러리는 C 언어로 Win32 프로그래밍을 쉽게 할수 있도록 도와주기 위해 만들었습니다.
// 이 방법을 사용해서 C 언어를 배우면서 Win32 프로그래밍으로 실습을 하면 됩니다.
//
// 제작자 : 김성엽 (tipsware@naver.com, https://blog.naver.com/tipsware, https://cafe.naver.com/easywin32)
//
// 개발 시작 : 2019년 9월 3일 화요일
// 최근 업데이트 : 2021년 1월 10일 일요일
//
// 이 라이브러리의 저작권은 '(주)팁스웨어'에 있습니다.
// 이 라이브러리는 C 언어를 공부하는 사람들을 위해 만들어졌습니다.
// 따라서 출처만 정확하게 밝혀주면 모든 상황에서 무료로 사용이 가능합니다.
// 이 라이브러리는 후원 하시는 분들의 도움으로 계속 발전하고 있습니다.
// 후원에 참여하실 분들은 아래의 링크를 참고하세요.
// https://blog.naver.com/tipsware/221552898585
// 여러분의 작은 관심이 개발에 큰 힘이 됩니다. 감사합니다!!


typedef void (*FP_MOUSE_MSG)(int a_mixed_key, POINT a_pos);
typedef void (*FP_COMMAND)(INT32 a_ctrl_id, INT32 a_notify_code, void *ap_ctrl);
typedef void (*FP_DESTROY)();
typedef int (*FP_USER_MESSAGE)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
  
struct TargetData
{
	HWND h_wnd, h_base_wnd;
	int win_x, win_y, win_cx, win_cy;
	int x, y, cx, cy;
};

#define IMAGE_BMP   0
#define IMAGE_JPEG  1
#define IMAGE_GIF   2
#define IMAGE_TIFF  3
#define IMAGE_PNG   4

#define PI     3.141592
#define H_PI   1.570796

#define GDI_COLOR(c) ((c >> 16) & 0x000000FF) | (c & 0x0000FF00) | ((c << 16) & 0x00FF0000)

class SystemMessageMap
{
protected:
	void *mp_app_data;
	FP_MOUSE_MSG mp_left_down, mp_left_up, mp_mouse_move;
	FP_COMMAND mp_command;
	FP_DESTROY mp_destroy;
	FP_USER_MESSAGE mp_user_message;

public:
	SystemMessageMap(FP_MOUSE_MSG ap_left_down, FP_MOUSE_MSG ap_left_up, FP_MOUSE_MSG ap_mouse_move, FP_COMMAND ap_command, FP_DESTROY ap_destroy, FP_USER_MESSAGE ap_user_message);
	virtual ~SystemMessageMap();

	void SetAppData(void *ap_data, int a_data_size);
	void *GetAppData();

	FP_MOUSE_MSG GetLeftBtnDown();
	FP_MOUSE_MSG GetLeftBtnUp();
	FP_MOUSE_MSG GetMouseMove();
	FP_COMMAND GetCommand();
	FP_DESTROY GetDestroy();
	FP_USER_MESSAGE GetUserMessage();
};

namespace EasyAPI_Tipsware
{
	void Rectangle(RECT *ap_rect);
	void Rectangle(int a_sx, int a_sy, int a_ex, int a_ey);
	void Rectangle(int a_sx, int a_sy, int a_ex, int a_ey, COLORREF a_color);
	void Rectangle(int a_sx, int a_sy, int a_ex, int a_ey, COLORREF a_line_color, COLORREF a_fill_color);

	void Ellipse(int a_sx, int a_sy, int a_ex, int a_ey);
	void Ellipse(int a_sx, int a_sy, int a_ex, int a_ey, COLORREF a_color);
	void Ellipse(int a_sx, int a_sy, int a_ex, int a_ey, COLORREF a_line_color, COLORREF a_fill_color);

	void TextOut(int a_x, int a_y, const char *ap_format, ...);
	void TextOut(int a_x, int a_y, COLORREF a_color, const char *ap_format, ...);
	void TextCenterOut(RECT *ap_rect, const char *ap_format, ...);

	void Line(int a_sx, int a_sy, int a_ex, int a_ey);
	void Line(int a_sx, int a_sy, int a_ex, int a_ey, COLORREF a_color, int a_tick = 1, int a_style = PS_SOLID);
	void MoveTo(int a_x, int a_y);
	void LineTo(int a_x, int a_y);
	void Pie(int a_left, int a_top, int a_right, int a_bottom, int a_xr1, int a_yr1, int a_xr2, int a_yr2);
	void PieRadian(int a_left, int a_top, int a_right, int a_bottom, double a_s_radian, double a_e_radian);
	void PieDegree(int a_left, int a_top, int a_right, int a_bottom, int a_s_degree, int a_e_degree);

	void GetIntPosFromCircle(int a_center_x, int a_center_y, int a_radius, double a_radian, int *ap_x, int *ap_y);
	void GetIntPosFromCircle(int a_center_x, int a_center_y, int a_radius, int a_degree, int *ap_x, int *ap_y);

	void GetPosFromCircle(double a_center_x, double a_center_y, double a_radius, double a_radian, double *ap_x, double *ap_y);
	void GetPosFromCircle(double a_center_x, double a_center_y, double a_radius, int a_degree, double *ap_x, double *ap_y);

	void MakeArrowHeadPos(POINT a_start_pos, POINT a_end_pos, POINT *ap_pos_list, int a_head_size = 15);
	void DrawArrow(POINT a_start_pos, POINT a_end_pos, int a_type, int a_head_size = 15);
	void DrawArrow(int a_sx, int a_sy, int a_ex, int a_ey, int a_type, int a_head_size = 15);

	HGDIOBJ SelectStockObject(int a_stock_index);
	HGDIOBJ SelectPenObject(COLORREF a_color, int a_tick = 1, int a_style = PS_SOLID);
	HGDIOBJ SelectBrushObject(COLORREF a_color);
	HGDIOBJ SelectFontObject(const char *ap_name, short a_size, unsigned char a_style = 0);

	void SetSimpleColorMode(char a_use_pen = 1, char a_use_brush = 1);
	void ChangePenColor(COLORREF a_color);
	void ChangeBrushColor(COLORREF a_color);
	void ChangeObjectColor(COLORREF a_color);

	void SetTextColor(COLORREF a_color);

	void UseDisplay(int a_index, int a_cx = 800, int a_cy = 600, HWND ah_wnd = NULL);
	void SetWindowStartPos(int a_x, int a_y);
	void ChangeWorkSize(int a_cx, int a_cy, int a_auto_change_display_size = 1);
	void ShowDisplay(int a_index = 0);

	void *FindControl(int a_id);
	void *CreateButton(const char *ap_name, int a_x, int a_y, int a_width, int a_height, int a_id, int a_style = 0, int a_ex_syle = 0);
	void *CreateEdit(int a_x, int a_y, int a_width, int a_height, int a_id, int a_style, int a_ex_syle = 0);
	void *CreateListBox(int a_x, int a_y, int a_width, int a_height, int a_id, void *afp_draw = NULL, int a_style = 0, int a_ex_syle = 0);
	void *CreateComboBox(int a_x, int a_y, int a_width, int a_height, int a_id, void *afp_draw = NULL, int a_style = 0, int a_ex_syle = 0);
	void DestroyControl(void *ap_ctrl);
	void ChangeCtrlColor(void *ap_ctrl, COLORREF a_fill_color, COLORREF a_out_border_color, COLORREF a_in_border_color, COLORREF a_text_color);
	void ChangeEditColor(void *ap_ctrl, COLORREF a_fill_color, COLORREF a_text_color);
	void SetCtrlName(void *ap_ctrl, const char *ap_name);
	void GetCtrlName(void *ap_ctrl, char *ap_name, int a_limit_size);
	void ShowControl(void *ap_ctrl, int a_show_type);
	HWND GetCtrlHandle(void *ap_ctrl);
	void SetIntDataToControl(int a_ctrl_id, int a_value);
	int GetIntDataFromControl(int a_ctrl_id);
	void Invalidate(void *ap_ctrl, int a_bk_flag = 1);
	void SetCtrlFont(void *ap_ctrl, const char *ap_font_name, int a_font_size, int a_attr = 0);
	void SetFocus(void *ap_ctrl);
	void CallDrawItem(int a_id, LPARAM lParam);
	void CallMeasureItem(int a_id, LPARAM lParam);
	INT32 CallCtrlColor(WPARAM wParam, LPARAM lParam);

	void ListBox_ChangeDrawFunc(void *ap_ctrl, void *afp_draw);
	int ListBox_AddString(void *ap_ctrl, const char *ap_string, UINT8 a_auto_select = 1);
	int ListBox_InsertString(void *ap_ctrl, INT32 a_index, const char *ap_string, UINT8 a_auto_select = 1);
	int ListBox_GetTextLength(void *ap_ctrl, INT32 a_index);
	int ListBox_GetText(void *ap_ctrl, INT32 a_index, char *ap_string, int a_max_len);
	void ListBox_SetCurSel(void *ap_ctrl, INT32 a_index);
	INT32 ListBox_GetCurSel(void *ap_ctrl);
	INT32 ListBox_GetCount(void *ap_ctrl);
	void ListBox_DeleteString(void *ap_ctrl, INT32 a_index);
	void ListBox_ResetContent(void *ap_ctrl);
	void ListBox_SetItemDataPtr(void *ap_ctrl, INT32 a_index, void *ap_data);
	void ListBox_SetItemDataPtrEx(void *ap_ctrl, INT32 a_index, const char *ap_string, void *ap_data, UINT8 a_auto_select = 1);
	void *ListBox_GetItemDataPtr(void *ap_ctrl, INT32 a_index);
	void ListBox_SetItemData(void *ap_ctrl, INT32 a_index, INT32 a_data);
	void ListBox_SetItemDataEx(void *ap_ctrl, INT32 a_index, const char *ap_string, INT32 a_data, UINT8 a_auto_select = 1);
	INT32 ListBox_GetItemData(void *ap_ctrl, INT32 a_index);
	INT32 ListBox_FindString(void *ap_ctrl, INT32 a_index, const char *ap_string);
	INT32 ListBox_FindStringExact(void *ap_ctrl, INT32 a_index, const char *ap_string);
	void ListBox_SetItemWidth(void *ap_ctrl, INT32 a_width);
	void ListBox_SetItemHeight(void *ap_ctrl, INT32 a_height);

	void ComboBox_ChangeDrawFunc(void *ap_ctrl, void *afp_draw);
	int ComboBox_AddString(void *ap_ctrl, const char *ap_string, UINT8 a_auto_select = 1);
	int ComboBox_InsertString(void *ap_ctrl, INT32 a_index, const char *ap_string, UINT8 a_auto_select = 1);
	int ComboBox_GetTextLength(void *ap_ctrl, INT32 a_index);
	int ComboBox_GetText(void *ap_ctrl, INT32 a_index, char *ap_string, int a_max_len);
	void ComboBox_SetCurSel(void *ap_ctrl, INT32 a_index);
	INT32 ComboBox_GetCurSel(void *ap_ctrl);
	INT32 ComboBox_GetCount(void *ap_ctrl);
	void ComboBox_DeleteString(void *ap_ctrl, INT32 a_index);
	void ComboBox_ResetContent(void *ap_ctrl);
	void ComboBox_SetItemDataPtr(void *ap_ctrl, INT32 a_index, void *ap_data);
	void ComboBox_SetItemDataPtrEx(void *ap_ctrl, INT32 a_index, const char *ap_string, void *ap_data, UINT8 a_auto_select = 1);
	void *ComboBox_GetItemDataPtr(void *ap_ctrl, INT32 a_index);
	void ComboBox_SetItemData(void *ap_ctrl, INT32 a_index, INT32 a_data);
	void ComboBox_SetItemDataEx(void *ap_ctrl, INT32 a_index, const char *ap_string, INT32 a_data, UINT8 a_auto_select = 1);
	INT32 ComboBox_GetItemData(void *ap_ctrl, INT32 a_index);
	INT32 ComboBox_FindString(void *ap_ctrl, INT32 a_index, const char *ap_string);
	INT32 ComboBox_FindStringExact(void *ap_ctrl, INT32 a_index, const char *ap_string);
	void ComboBox_SetItemHeight(void *ap_ctrl, INT32 a_height);

	UINT32 Edit_GetLength(void *ap_ctrl);
	INT32 Edit_ReadTextFromFile(void *ap_ctrl, const char *ap_file_path);
	INT32 Edit_SaveTextToFile(void *ap_ctrl, const char *ap_file_path);

	void EnableEnterKey(void *ap_ctrl);
	void Clear(int a_index = 0, COLORREF a_color = RGB(255, 255, 255));

	void SetAppData(void *ap_data, int a_data_size);
	void *GetAppData();

	HDC GetCurrentDC();
	HBITMAP GetCurrentBitmap();
	void *GetMemDC(int a_index);
	void *GetCurrentMemDC();
	void BitBltByIndex(int a_dest_index, int a_dest_x, int a_dest_y, int a_dest_width, int a_dest_height, int a_src_index, int a_src_x = 0, int a_src_y = 0);
	void BitBltByIndex(int a_dest_x, int a_dest_y, int a_src_index, int a_src_x = 0, int a_src_y = 0);

	void SetTimer(UINT a_timer_id, UINT a_elapse, void *ap_timer_proc);
	void SetTimer(UINT ah_wnd, UINT a_timer_id, UINT a_elapse, void *ap_timer_proc);

	void KillTimer(UINT a_timer_id);
	void KillTimer(UINT ah_wnd, UINT a_timer_id);

	// ...GP 는 GDI+ 관련 함수들입니다!
	// ap_image_path 경로에 있는 이미지 파일을 읽는 함수!
	void *LoadImageGP(const char *ap_image_path);
	// ap_image에 저장된 이미지 정보를 ap_image_path경로에 a_image_type 형식의 이미지 파일로 저장합니다.
	void SaveImageGP(void *ap_image, const char *ap_image_path, int a_image_type = IMAGE_PNG);
	void *CreateBitmapGP(int a_width, int a_height, DWORD a_flag = 0, int a_bpp = 32);
	void DeleteImageGP(void *ap_image);

	void DrawImageGP(void *ap_image, int a_x, int a_y);
	void DrawImageGP(void *ap_image, int a_x, int a_y, int a_width, int a_height);
	void DrawImageGP(void *ap_image, int a_x, int a_y, double a_cx_rate, double a_cy_rate);
	void TransparentDrawGP(void *ap_image, int a_x, int a_y, COLORREF a_remove_color);
	void TransparentDrawGP(void *ap_image, int a_x, int a_y, int a_width, int a_height, COLORREF a_remove_color);
	void TransparentDrawGP(void *ap_image, int a_x, int a_y, double a_cx_rate, double a_cy_rate, COLORREF a_remove_color);

	HDC GetDCGP(void *ap_image);
	HBITMAP GetBitmapGP(void *ap_image);
	int GetWidthGP(void *ap_image);
	int GetHeightGP(void *ap_image);
	int GetBppGP(void *ap_image);
	int GetPitchGP(void *ap_image);
	void *GetBitsGP(void *ap_image);

	void *CaptureScreenGP(int a_sx, int a_sy, int a_ex, int a_ey);
	void *CaptureScreenGP();

	void StartSocketSystem();
	void StopSocketSystem();

	// [[[ 서버 소켓 관련 ]]]
	void *CreateServerSocket(unsigned int a_user_data_size, void *ap_accept_user, void *ap_proc_net_msg, void *ap_close_user,
								int a_index = 0, int a_max_user_count = 100, unsigned char a_key = 0x51);
	void DeleteServerSocket(void *ap_object);
	void DeleteServerSockets();
	int GetServerSocketIndex(void *ap_object);

	void *GetUsersData(void *ap_object);
	void *GetUserDataByHandle(void *ap_object, unsigned int ah_socket);
	void *GetUserDataByIndex(void *ap_object, int a_index);

	void StartListenService(void *ap_object, const char *ap_ip_address, int a_port);
	void SendFrameDataToClient(void *ap_object, unsigned int ah_socket, char a_msg_id, const void *ap_data, int a_size);
	void BroadcastFrameData(void *ap_object, char a_msg_id, const void *ap_data, int a_size);
	void *GetProcessServerData(void *ap_object);
	void CloseSocketInServer(void *ap_object, unsigned int ah_socket, char *ap_close_user);
	void DestroySocketsInServer(void *ap_object);

	// [[[ 클라이언트 소켓 관련 ]]]
	void *CreateClientSocket(void *ap_connect, void *ap_net_msg, void *ap_close, int a_index = 0, unsigned char a_key = 0x51);
	int GetClientSocketIndex(void *ap_object);
	void DeleteClientSocket(void *ap_object);
	void DeleteClientSockets();
	void *GetProcessClientData(void *ap_object);
	int IsConnect(void *ap_object);
	void ConnectToServer(void *ap_object, const char *ap_ip_address, int a_port);
	void CloseSocket(void *ap_object);
	void SendFrameDataToServer(void *ap_object, char a_msg_id, const void *ap_data, int a_size);

	// [[ 문자열 처리 관련 일반 유틸리티 ]]
	char *GetNextString(char *ap_src_str, char a_delimiter, char *ap_buffer);
	char *GetNextStringEx(char *ap_src_str, char a_delimiter, char *ap_buffer, char a_remove_prefix = ' ');
	char* ConvertTabToSpace(const char* ap_src_string);
	// UTF8 기반의 문자열을 ASCII 형식의 문자열로 변환하는 함수.
	// 이 함수는 내부적으로 메모리를 할당해서 ap_dest_ascii_str 포인터에 주소를 대입해 줌
	// 따라서 이 함수를 사용하는 개발자가 free 함수를 사용하여 ap_dest_ascii_str 주소를 해제해야함!
	// 이 함수의 반환 값은 변환된 문자열의 길이입니다. (NULL 문자 포함 길이)
	int Utf8ToAscii(char **ap_dest_ascii_str, char *ap_src_utf8_str);

	// [[ 연산 수식 분석기 관련 ]]
	void *CreateExpParsor();
	const char *GetExpString(void *ap_parsor);
	void InitExpParser(void *ap_parsor);
	void SetExpString(void *ap_parsor, const char *ap_string);
	void RunExpParsing(void *ap_parsor);   // 사용자가 지정한 수식을 분석한다

	int GetExpVariableCount(void *ap_parsor);
	const char *GetExpVariableName(void *ap_parsor, int a_index);
	// 지정된 변수에 값을 대입한다.
	void SetExpVarValue(void *ap_parsor, int a_index, double a_value);
	// 지정된 변수가 참조할 값이 있는 위치를 주소로 알려준다
	void SetExpVariablePointer(void *ap_parsor, int a_index, double *ap_data);
	// 변수값을 주소로 참조하도록 되어 있는 변수가 있다면 해당 주소에서 값을 읽어 변수값을 업데이트 한다.
	void UpdateExpVariableValue(void *ap_parsor);
	// 이미 수식이 parsing되었다면 다시 parsing할 필요가 없고 이함수를 호출한후, ExecExp 함수를 호출하면 된다
	char RestoreExpParserInfo(void *ap_parsor);
	char GetExpReadyFlag(void *ap_parsor);   // Parsing 이 되어있는지를 알려준다 ( 0 -> 파싱안됨, 1 -> 파싱됨 )
	double ExecExp(void *ap_parsor, unsigned int a_prev_depth = 0);  // Parsing된 수식을 실행하여 수식의 결과값을 얻는다
	void DestroyExpParsor(void *ap_parsor);

	void *CreateExpParsorEx();
	const char *GetExpStringEx(void *ap_parsor);
	void InitExpParserEx(void *ap_parsor);
	void SetExpStringEx(void *ap_parsor, const char *ap_string);
	void RunExpParsingEx(void *ap_parsor);   // 사용자가 지정한 수식을 분석한다

	int GetExpVariableCountEx(void *ap_parsor);
	const char *GetExpVariableNameEx(void *ap_parsor, int a_index);
	// 지정된 변수가 참조할 값이 있는 위치를 주소로 알려준다
	void SetExpVariablePointerEx(void *ap_parsor, int a_index, unsigned char a_data_type, double *ap_data);
	// 변수값을 주소로 참조하도록 되어 있는 변수가 있다면 해당 주소에서 값을 읽어 변수값을 업데이트 한다.
	void UpdateExpVariableValueEx(void *ap_parsor);
	// 이미 수식이 parsing되었다면 다시 parsing할 필요가 없고 이함수를 호출한후, ExecExp 함수를 호출하면 된다
	char RestoreExpParserInfoEx(void *ap_parsor);
	char GetExpReadyFlagEx(void *ap_parsor);   // Parsing 이 되어있는지를 알려준다 ( 0 -> 파싱안됨, 1 -> 파싱됨 )
	ExpDataType ExecExpEx(void *ap_parsor, unsigned int a_prev_depth = 0);  // Parsing된 수식을 실행하여 수식의 결과값을 얻는다
	void DestroyExpParsorEx(void *ap_parsor);

	// [ 매크로 관련 함수들 ]
	// 시스템 키를 입력하는 함수. 
	void SystemKeyWrite(char a_key, int a_delay = 250);  // (Key Down & Up을 한 번에 처리)
	void SystemKeyDown(char a_key);
	void SystemKeyUp(char a_key);
	// 일반 키를 입력하는 함수. Shift 키를 사용하는 경우에는 a_is_shift에 1을 입력해야 한다. 
	void InputNormalChar(char a_is_shift, char a_key);  // (Key Down & Up을 한 번에 처리)
	void InputNormalCharDown(char a_is_shift, char a_key);
	void InputNormalCharUp(char a_is_shift, char a_key);
	// ap_string에 전달된 키보드로 입력한다.
	char InputNormalString(const char *ap_string, char a_is_han_mode, int a_delay = 100);
	void WaitInputSystem(int a_delay);

	void MouseMoveWrite(int a_x, int a_y);  // 마우스 이동!
	void MouseClickWrite(int a_x, int a_y, int a_is_left = 1, int a_delay = 0);
	void MouseClickDown(int a_x, int a_y, int a_is_left = 1);
	void MouseClickUp(int a_x, int a_y, int a_is_left = 1);

	void GetMousePosFromTarget(TargetData *ap_target, int *ap_x, int *ap_y, int a_target_x, int a_target_y);
	void UpdateTargetImage();
	void UpdateTargetPos(TargetData *ap_target);
	// a_is_area_type (0: Client Area, 1: Window Area, 2: Screen Area) - 32Bit Color Only
	unsigned int GetTargetColor(TargetData *ap_target, int a_x, int a_y, int a_is_area_type = 0);
	TargetData *FindTargetImage(int a_index, const char *ap_class_name, const char *ap_title_name);
	TargetData *FindTargetImage(int a_index, int a_x, int a_y);
	void ShowTargetImage(TargetData *ap_target, int a_x, int a_y, int a_is_area_type);
	void ShowTargetImageEx(TargetData *ap_target, int a_x, int a_y, int a_cx, int a_cy, int a_is_area_type, int a_src_x, int a_src_y);
	void GetTargetImageData(unsigned int *ap_image_data, TargetData *ap_target, int a_is_area_type);
	void GetTargetImageDataEx(unsigned int *ap_image_data, TargetData *ap_target, int a_is_area_type, int a_x, int a_y, int a_cx, int a_cy);
	// 지정한 좌표(a_x, a_y)에 있는 색상(a_org_color)을 기준으로 주위 색상을 체크하여 대상의 영역를 계산한다.
	// 체크할 이미지는 ap_image_data이고 찾은 영역의 좌표는 p_target_rect에 사각형 좌표로 저장된다.
	// ap_image_data에 저장된 이미지 패턴의 폭은 a_image_width이고 높이는 a_image_height이다.
	void CheckOBjectPos(RECT *p_target_rect, int a_x, int a_y, unsigned int a_org_color, unsigned int a_change_color, unsigned int *ap_image_data, int a_image_width, int a_image_height);

	// 클립보드 관련 함수
	int CopyTextToClipboard(const char *ap_string);
	int CopyTextFromClipboard(char **ap_string, char a_is_clear);
	HBITMAP MakeDupBitmap(HBITMAP ah_bitmap);
	HBITMAP MakeDupBitmap(HBITMAP ah_bitmap, unsigned int *ap_image_data);
	HBITMAP CopyBitmapFromClipBoard();
	void CopyBitmapToClipBoard(HBITMAP ah_bitmap);

	// Common Dialog 관련 함수
	int ChooseOpenFileName(char *ap_open_path, int a_path_size, const char *ap_filter = "All\0*.*\0Text\0*.txt\0", int a_filter_index = 1, const char *ap_init_path = NULL, int a_add_attr = 0);
	int ChooseSaveFileName(char *ap_open_path, int a_path_size, const char *ap_filter = "All\0*.*\0Text\0*.txt\0", int a_filter_index = 1, const char *ap_init_path = NULL, int a_add_attr = 0);
	int ChooseColorDlg(COLORREF a_init_color, COLORREF *ap_selected_color);
	int ChooseFontDlg(char *ap_font_name, int *ap_font_size, int *ap_font_style, COLORREF a_init_color = RGB(0, 0, 0), COLORREF *ap_selected_color = NULL);

	// 시리얼 통신 관련 함수
	void *CreateSerialObject();
	int Serial_OpenPort(void *ap_object, int a_port_number, HWND ah_notify_wnd, void(*ap_modify_dcb)(DCB *ap_dcb) = NULL, int a_notify_msg_id = 21000);
	int Serial_IsOpenPort(void *ap_object);  // 포트가 오픈되어 있지 않으면 0, 오픈되어있다면 포트 번호가 반환됨. COM1이면 1이 반환!
	int Serial_GetRecvDataSize(void *ap_object);
	char *Serial_GetRecvDataBuffer(void *ap_object);
	void Serial_RemoveDataFromBuffer(void *ap_object, int a_remove_size);
	void Serial_ResumeWatchThread(void *ap_object);
	void Serial_SetRecvTimeoutInfo(void *ap_object, int a_count = 5, int a_interval = 300);
	int Serial_WriteCommData(void *ap_object, const char *ap_data, int a_length, int(*ap_error_process)(DWORD, COMSTAT) = NULL);
	void Serial_ClearEvent(void *ap_object, int a_tx = 1, int a_rx = 1);
	void DestroySerialObject(void *ap_object);
}

extern const char *gp_app_name;
extern const char *gp_window_title;
extern HWND gh_main_wnd;
#endif