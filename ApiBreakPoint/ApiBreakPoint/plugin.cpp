#include "plugin.h"
#include <ShellScalingApi.h>

#include "pluginsdk\_plugins.h"
#include "pluginsdk\bridgemain.h"

//#ifdef _UNICODE
//#error "USE ASCII CODE PAGE"
//#endif

using namespace Script::Module;
using namespace Script::Symbol;
using namespace Script::Debug;
using namespace Script::Register;

// make sure only one window exists
bool bIsMainWindowShow = false;


HWND hTabControl;
HWND hMain;
int CurTab = 0;

// checkbox handle
HWND hChecks[CHECK_COUNT];


DWORD WINAPI MsgLoopThread(LPVOID);
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

void UpdateCheckBoxs();
void RecreateFont();
void CleanupBreakpoints();
void AdjustLayout(HWND);

DpiState g_dpi = {
	.current = 96,    // ��ʼDPIֵ
	.scaling = 1.0f,  // ��ʼ��������
	.font = nullptr,   // ��ʼ������
	.tabfont = nullptr   // ��ʼ������
};


//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
	HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	if (FAILED(hr))
		std::cerr << "SetProcessDpiAwareness failed with error: " << hr << std::endl;
	{
		SetProcessDPIAware();
	}
	return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{}

//Do GUI/Menu related things here.
void pluginSetup()
{
	_plugin_menuaddentry(hMenu, MENU_MAINWINDOW_POPUP, "Set Api Breakpoint");
}


// ��װ���ַ�ת UTF-8
std::string WideToUTF8(const std::wstring& wstr)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string str(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
	return str;
}


// doing msg loop like WinMain
DWORD WINAPI MsgLoopThread(LPVOID)
{
	MSG msg;
	WNDCLASSW wc = { 0 };
	HWND hwnd;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInstance;
	wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);

	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClASS_NAME;

	RegisterClassW(&wc);

	// ��ȡ��ǰDPI����������
	// ��ȡ��ʼDPI
	
	HDC hdc = GetDC(nullptr);
	g_dpi.current = GetDeviceCaps(hdc, LOGPIXELSX);
	ReleaseDC(nullptr, hdc);
	g_dpi.scaling = g_dpi.current / 96.0f;
	hwnd = CreateWindowW(
		ClASS_NAME, 
		L"API�ϵ������",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT,
		MulDiv(MAINWINDOW_WIDTH, g_dpi.current, 96),
		MulDiv(MAINWINDOW_HEIGHT, g_dpi.current, 96),
		nullptr,
		nullptr, 
		g_hInstance, 
		//GetModuleHandle(nullptr),
		nullptr);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	bIsMainWindowShow = false;

	return (int)msg.wParam;
}

VOID CenterDialog(HWND hDlg)
{
	HWND hwndOwner = NULL;
	RECT rcOwner, rcDlg, rc;
	// Get the owner window and dialog box rectangles. 			
	if ((hwndOwner = GetParent(hDlg)) == NULL)
	{
		hwndOwner = GetDesktopWindow();
	}
	GetWindowRect(hwndOwner, &rcOwner);
	GetWindowRect(hDlg, &rcDlg);
	CopyRect(&rc, &rcOwner);

	// Offset the owner and dialog box rectangles so that right and bottom 
	// values represent the width and height, and then offset the owner again 
	// to discard space taken up by the dialog box. 

	OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
	OffsetRect(&rc, -rc.left, -rc.top);
	OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

	// The new position is the sum of half the remaining space and the owner's 
	// original position. 

	SetWindowPos(hDlg,
		HWND_TOP,
		rcOwner.left + (rc.right / 2),
		rcOwner.top + (rc.bottom / 2),
		0, 0,          // Ignores size arguments. 
		SWP_NOSIZE);
}

// called when tab changed, update checkbox check(uncheck) and show(hide) state
void UpdateCheckBoxs()
{
	CurTab = TabCtrl_GetCurSel(hTabControl);
	size_t len = g_Api_Groups[CurTab].apiList.size();
	for (size_t i = 0; i < CHECK_COUNT; i++)
	{
		if (i < len)
		{
			SetWindowTextW(hChecks[i], std::wstring(g_Api_Groups[CurTab].apiList[i].apiName + L"  " + g_Api_Groups[CurTab].apiList[i].description).c_str());
			if (g_Api_Groups[CurTab].apiList[i].bWantToSetBp)
			{
				CheckDlgButton(hMain, IDC_CHECKS[i], BST_CHECKED);
			}
			else
			{
				CheckDlgButton(hMain, IDC_CHECKS[i], BST_UNCHECKED);
			}
			ShowWindow(hChecks[i], SW_SHOW);
		}
		else
		{
			ShowWindow(hChecks[i], SW_HIDE);
		}
	}
}

// initialize main window, register tab control and checkbox
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	TCITEMW ti;
	static HFONT hFont = NULL;
	hMain = hwnd;

	switch (iMsg)
	{
	case WM_CREATE:
	{
		RecreateFont();
		//defaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

		InitCommonControls();

		hTabControl = CreateWindowW(
			L"SysTabControl32",
			L"",
			WS_CHILD | WS_VISIBLE | TCS_FIXEDWIDTH,
			0,
			0, 
			0,
			0, 
			hwnd, 
			//(HMENU)IDC_TABCTRL,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(IDC_TABCTRL)),
			g_hInstance, nullptr);

		//// ����ѡ���Ĭ������,Ŀǰ�Ϳؼ�һ��
		SendMessage(hTabControl, WM_SETFONT, (WPARAM)g_dpi.tabfont, TRUE);


		for (int i = 0; i < TAB_COUNT; i++)
		{
			ti.mask = TCIF_TEXT;
			ti.pszText = (LPWSTR)g_Api_Groups[i].groupName.c_str();
			TabCtrl_InsertItem(hTabControl, i, &ti);
		}
		// ������ѡ��
		for (int i = 0; i < CHECK_COUNT; i++) {
			hChecks[i] = CreateWindowW(
				L"BUTTON",
				L"",
				WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_MULTILINE,
				0, 0, 0, 0,
				hwnd,
				//(HMENU)IDC_CHECKS[i], 
				reinterpret_cast<HMENU>(static_cast<UINT_PTR>(IDC_CHECKS[i])),
				g_hInstance, nullptr);
			SendMessageW(hChecks[i], WM_SETFONT, (WPARAM)g_dpi.font, TRUE);
		}

		CenterDialog(hwnd);
		AdjustLayout(hwnd);
		UpdateCheckBoxs();
	}
	return 0;
	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{

		case TCN_SELCHANGE:
		{
			UpdateCheckBoxs();

			return TRUE;
		}
		}
	}
	return 0;
	case WM_COMMAND:
		for (int i = 0; i < CHECK_COUNT; i++) 
		{
			if (LOWORD(wParam) == IDC_CHECKS[i]) 
			{
				bool bNewState = IsDlgButtonChecked(hwnd, IDC_CHECKS[i]) == BST_CHECKED;
				auto& apiInfo = g_Api_Groups[CurTab].apiList[i];  // ������

				// ״̬δ�仯���Ѿ�ִ�гɹ�������
				if (apiInfo.bWantToSetBp == bNewState && apiInfo.bCmdSuccess) {
					//_plugin_logprintf("BP break\n");
					break;
				}

				// ��������
				//_plugin_logprintf("BP cmd start\n");
				std::wstring wcmd = bNewState ? L"bp " + apiInfo.apiName
					: L"bc " + apiInfo.apiName;

				// ת��Ϊ UTF-8
				std::string cmd = WideToUTF8(wcmd);

				// ִ�������¼���
				bool bSuccess = DbgCmdExecDirect(cmd.c_str());

				

				// ����״̬
				apiInfo.bCmdSuccess = bSuccess;
				apiInfo.bWantToSetBp = bNewState;
				apiInfo.bBpSet = bSuccess ? bNewState : apiInfo.bBpSet;  // ���ɹ�ʱ����ʵ��״̬

				// ������
				if (!bSuccess) {
					_plugin_logprintf("Failed to %s breakpoint %s\n", bNewState ? "set" : "clear", cmd.c_str());
				}
				else {
					_plugin_logprintf("Successfully %s breakpoint %s\n", bNewState ? "set" : "cleared", cmd.c_str());
				}
			}
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_SIZE:
		AdjustLayout(hwnd);
		return 0;

	case WM_DESTROY:
		if (g_dpi.font) DeleteObject(g_dpi.font);
		//CleanupBreakpoints();
		PostQuitMessage(0);
		return 0;
	case WM_DPICHANGED:
	{
		g_dpi.current = HIWORD(wParam);
		g_dpi.scaling = g_dpi.current / 96.0f;
		RecreateFont();

		// ��������λ�úʹ�С
		RECT* rc = (RECT*)lParam;
		SetWindowPos(hwnd, nullptr,
			rc->left, rc->top,
			rc->right - rc->left,
			rc->bottom - rc->top,
			SWP_NOZORDER | SWP_NOACTIVATE);

		AdjustLayout(hwnd);
		return 0;
	}
	}

	return DefWindowProcW(hwnd, iMsg, wParam, lParam);
}

// ���ֵ���
void AdjustLayout(HWND hwnd) {
	RECT rc;
	GetClientRect(hwnd, &rc);
	const int width = rc.right - rc.left;
	const int height = rc.bottom - rc.top;

	// Tab�ؼ�
	const int tabHeight = MulDiv(TAB_HEIGHT, g_dpi.current, 96);
	SetWindowPos(hTabControl, nullptr, 0, 0, width, tabHeight, SWP_NOZORDER);

	// ��ѡ�򲼾�
	const int margin = MulDiv(MARGIN_SIZE, g_dpi.current, 96);
	const int checkWidth = (width - margin * (CHECKBOX_COLUMNS + 1)) / CHECKBOX_COLUMNS;
	const int checkHeight = MulDiv(CHECKBOX_HEIGHT, g_dpi.current, 96);

	// ����Tab�ؼ�
	SetWindowPos(hTabControl, NULL,
		0, 0, rc.right, tabHeight,
		SWP_NOZORDER | SWP_NOACTIVATE);

	for (int i = 0; i < CHECK_COUNT; i++) {
		int col = i % CHECKBOX_COLUMNS;
		int row = i / CHECKBOX_COLUMNS;
		int x = margin + col * (checkWidth + margin);
		int y = tabHeight + margin + row * (checkHeight + margin);

		SetWindowPos(hChecks[i], nullptr,
			x, y, checkWidth, checkHeight,
			SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

// �ڳ����˳������ǰ����������
void CleanupBreakpoints()
{
	_plugin_logprintf("[+] Cleaning up breakpoints...\n");

	for (auto& group : g_Api_Groups)
	{
		for (auto& api : group.apiList)
		{
			if (api.bWantToSetBp)
			{
				std::string cmd = "bc " + WideToUTF8(api.apiName);
				Cmd(cmd.c_str());
				api.bWantToSetBp = false;
			}
				
		}
	}
}

// �������
void RecreateFont() {
	if (g_dpi.font) DeleteObject(g_dpi.font);
	if (g_dpi.tabfont) DeleteObject(g_dpi.tabfont);

	LOGFONTW lf = {
		.lfHeight = -MulDiv(12, g_dpi.current, 96),
		.lfWeight = FW_NORMAL,
		.lfCharSet = GB2312_CHARSET,
		.lfQuality = CLEARTYPE_QUALITY,
	};
	wcscpy_s(lf.lfFaceName, L"Microsoft YaHei UI");

	g_dpi.font = CreateFontIndirectW(&lf);

	// ���� Tab �ؼ�������
	LOGFONTW tabLf = lf;
	tabLf.lfHeight = -MulDiv(14, g_dpi.current, 96); // ���� Tab �ؼ��������С
	g_dpi.tabfont = CreateFontIndirectW(&tabLf);

	// ���� Tab �ؼ�������
	if (hTabControl) {
		SendMessage(hTabControl, WM_SETFONT, (WPARAM)g_dpi.tabfont, TRUE);
	}

	// ���������ؼ�����
	EnumChildWindows(hMain, [](HWND hwnd, LPARAM) -> BOOL {
		if (hwnd != hTabControl) { // �ų� Tab �ؼ�
			SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_dpi.font, TRUE);
		}
		return TRUE;
		}, 0);
}




// set delay breakpoint when dll loaded
EXTERN_C __declspec(dllexport) void _cdecl CBLOADDLL(
	CBTYPE bType, // event type, one of the `cbType` CB_* values listed above
	PLUG_CB_LOADDLL* callbackInfo // pointer to a structure of information
)
{
	// check if there are any breakpoint can be set
	//for (size_t i = 0; i < TAB_COUNT; i++)
	//{
	//	for (size_t j = 0; j < Groups[i].apiList.size(); j++)
	//	{
	//		auto& apiInfo = Groups[CurTab].apiList[i];  // ������

	//		if (apiInfo.bBpSet && apiInfo.bCmdSuccess) {
	//			_plugin_logprintf("BP Already Set\n");
	//			break;
	//		}

	//		std::wstring wcmd = L"bp " + apiInfo.apiName;
	//	    std::string cmd = WideToUTF8(wcmd);
	//	    bool bSuccess = DbgCmdExecDirect(cmd.c_str());
	//	}
	//}
}

// register menu to popup main window
extern "C" __declspec(dllexport) void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
	switch (info->hEntry)
	{
	case MENU_MAINWINDOW_POPUP:
		if (!bIsMainWindowShow && DbgIsDebugging())
		{
			HANDLE hThread = CreateThread(
				NULL,                   // ��ȫ����
				0,                      // Ĭ�϶�ջ��С
				MsgLoopThread,          // �̺߳���
				NULL,                   // ����
				0,                      // ������־
				NULL                    // �߳�ID
			);

			if (hThread != NULL)
			{
				// �̴߳����ɹ������Թرվ��������������������
				CloseHandle(hThread);
				bIsMainWindowShow = true;
			}
			else
			{
				// �����̴߳���ʧ�ܵ����
				_plugin_logprintf("Failed to create thread %s \n", MB_ICONERROR);
			}
		}
		break;
	}
}



