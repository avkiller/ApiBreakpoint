#include "plugin.h"
#include "ApiGroup.h"
#include <windows.h>
#include <ShellScalingApi.h>

#include "pluginsdk\_plugins.h"
#include "pluginsdk\bridgemain.h"

#include "util.h"
#include "OptionsDialog.h"

#include "resource.h"

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
std::vector<std::vector<HWND>> g_tabChecks;

std::unordered_map<HWND, std::pair<size_t, size_t>> g_checkboxMap;

std::vector<ApiGroup> g_Api_Groups;

DWORD WINAPI MsgLoopThread(LPVOID);
LRESULT CALLBACK ApiBreakPointManageWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

void DrawCheckbox(HWND hButton, LPDRAWITEMSTRUCT pDrawItem, const ApiBreakPointInfo& apiInfo);

void CreateTabs(TCITEMW& ti, HWND& hwnd);

void UpdateCheckBoxs();
void RecreateFont();
void CleanupBreakpoints();
void AdjustLayout(HWND);

DpiState g_dpi = {
	.current = 96,    // 初始DPI值
	.scaling = 1.0f,  // 初始缩放因子
	.font = nullptr,   // 初始字体句柄
	.tabfont = nullptr   // 初始字体句柄
};

std::wstring g_PluginDir;
std::wstring g_settingIniPath;
std::wstring g_api_BreakPointConfigPath;
scl::Settings g_settings;

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
	HRESULT hr = SetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE);
	if (FAILED(hr))
		_plugin_logprintf("SetProcessDpiAwareness failed with error\n");
	return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{}

//Do GUI/Menu related things here.
void pluginSetup()
{
	g_settings.Load(g_settingIniPath.c_str());

	std::wstring s_Api_BreakPoint_FileName = g_settings.opts().apiGroupFileName;
	if (!s_Api_BreakPoint_FileName.empty()) {
		g_api_BreakPointConfigPath = g_PluginDir + s_Api_BreakPoint_FileName;
		if (!LoadApiGroupsFromJson(g_api_BreakPointConfigPath, g_Api_Groups)) {
			SetDefaultApiGroups();
			SaveDefaultApiGroupsToJson(L"default_api_groups.json");
		}

	}
	_plugin_menuaddentry(hMenu, MENU_MAINWINDOW_POPUP, "&ApiBreakpoint Manage");
	_plugin_menuaddentry(hMenu, MENU_OPTIONS, "&Options");

	int hApiConfig = _plugin_menuadd(hMenu, "&API Config");
	_plugin_menuaddentry(hApiConfig, MENU_CONFIG, "&Reload API CONFIG");

	int hProfile = _plugin_menuadd(hMenu, "&Load Profile");

	//add profiles to menu
	for (size_t i = 0; i < g_settings.profile_names().size(); i++)
	{
		std::wstring profile = g_settings.profile_names()[i];
		auto mbstrName = scl::WideToUtf8(profile);
		_plugin_menuaddentry(hProfile, (int)i + MENU_MAX, mbstrName.c_str());
	}
}


// 封装宽字符转 UTF-8
std::string WideToUTF8(const std::wstring& wstr)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string str(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
	return str;
}

// 兼容旧系统的GetCurrentDPI实现
UINT GetCurrentDPI() {
	// 尝试加载 GetDpiForSystem（Windows 10 及以上）
	static auto pGetDpiForSystem = [] {
		HMODULE hUser32 = LoadLibraryW(L"user32.dll");
		return hUser32 ? (UINT(WINAPI*)())GetProcAddress(hUser32, "GetDpiForSystem") : nullptr;
		}();

	if (pGetDpiForSystem) {
		return pGetDpiForSystem();
	}
	else {
		// 否则，使用 GetDeviceCaps 获取屏幕 DPI
		HDC hdc = GetDC(nullptr);
		if (hdc) {
			UINT dpi = GetDeviceCaps(hdc, LOGPIXELSX);
			ReleaseDC(nullptr, hdc);
			return dpi;
		}
	}

	// 默认 DPI
	return 96;
}

// doing msg loop like WinMain
DWORD WINAPI MsgLoopThread(LPVOID)
{
	MSG msg;
	WNDCLASSW wc = { 0 };
	HWND hwnd;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = ApiBreakPointManageWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInstance;
	wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);

	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClASS_NAME;

	RegisterClassW(&wc);

	// 获取当前DPI并进行缩放
	// 获取初始DPI
	g_dpi.current = GetCurrentDPI();
	g_dpi.scaling = g_dpi.current / 96.0f;

	hwnd = CreateWindowW(
		ClASS_NAME, 
		L"API断点管理器",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT,
		MulDiv(MAINWINDOW_WIDTH, g_dpi.current, 96),
		MulDiv(MAINWINDOW_HEIGHT, g_dpi.current, 96),
		nullptr,
		nullptr, 
		g_hInstance, 
		nullptr);

	if (!hwnd) {
		return 0; // 窗口创建失败
	}

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
	/*CurTab = TabCtrl_GetCurSel(hTabControl);
	size_t apiList_len = g_Api_Groups[CurTab].apiList.size();
	for (size_t i = 0; i < apiList_len; i++)
	{
		if (i < apiList_len)
		{
	CurTab = TabCtrl_GetCurSel(hTabControl);
			SetWindowTextW(g_tabChecks[CurTab][i], std::wstring(g_Api_Groups[CurTab].apiList[i].apiName + L"  " + g_Api_Groups[CurTab].apiList[i].description).c_str());
			if (g_Api_Groups[CurTab].apiList[i].bWantToSetBp)
			{
				CheckDlgButton(hMain, g_tabChecks[CurTab][i], BST_CHECKED);
			}
			else
			{
				CheckDlgButton(hMain, g_tabChecks[CurTab][i], BST_UNCHECKED);
			}
			ShowWindow(hChecks[i], SW_SHOW);
		}
		else
		{
			ShowWindow(hChecks[i], SW_HIDE);
		}
	}*/

	CurTab = TabCtrl_GetCurSel(hTabControl);
	for (size_t chkIdx = 0; chkIdx < g_tabChecks[CurTab].size(); chkIdx++) {
		auto& apiInfo = g_Api_Groups[CurTab].apiList[chkIdx];
		SetWindowTextW(g_tabChecks[CurTab][chkIdx], std::wstring(apiInfo.apiName + L"  " + apiInfo.description).c_str());
		bool desiredState = apiInfo.bBpSet;
		SendMessage(g_tabChecks[CurTab][chkIdx], BM_SETCHECK, desiredState ? BST_CHECKED : BST_UNCHECKED, 0);
		InvalidateRect(g_tabChecks[CurTab][chkIdx], NULL, TRUE);
	}
}

// initialize main window, register tab control and checkbox
LRESULT CALLBACK ApiBreakPointManageWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
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
				g_hInstance, nullptr
			);

			// 设置选项卡的默认字体,目前和控件一致
			SendMessage(hTabControl, WM_SETFONT, (WPARAM)g_dpi.tabfont, TRUE);
			CreateTabs(ti, hwnd);
			CenterDialog(hwnd);
			AdjustLayout(hwnd);
		}
		return 0;

		case WM_NOTIFY:
		{
			switch (((LPNMHDR)lParam)->code)
			{

				case TCN_SELCHANGE:
				{
				
					int newTab = TabCtrl_GetCurSel(hTabControl);
					// 修改：切换时隐藏非当前标签页的复选框
					for (size_t tabIdx = 0; tabIdx < g_tabChecks.size(); tabIdx++) {
						bool show = (tabIdx == newTab);
						for (HWND hCheck : g_tabChecks[tabIdx]) {
							ShowWindow(hCheck, show ? SW_SHOW : SW_HIDE);
						}
					}
					AdjustLayout(hwnd);
					//UpdateCheckBoxs();
					return TRUE;
				}
			}
		}
		return 0;
		case WM_COMMAND:
		{
			if (HIWORD(wParam) == BN_CLICKED) {
				UINT ctrlID = LOWORD(wParam);
				HWND hCheck = (HWND)lParam; // 获取控件句柄
				// 新增：通过映射表快速查找
				auto it = g_checkboxMap.find(hCheck);
				if (it != g_checkboxMap.end())
				{
					size_t tabIdx = it->second.first;
					size_t chkIdx = it->second.second;

					auto& apiInfo = g_Api_Groups[tabIdx].apiList[chkIdx];
					bool isChecked = !apiInfo.bWantToSetBp;

					UINT newState = (isChecked) ? BST_CHECKED : BST_UNCHECKED;

					// 执行业务逻辑
					if (isChecked) {

						std::wstring wcmd = L"bp " + g_Api_Groups[tabIdx].apiList[chkIdx].apiName;
						std::string cmd = WideToUTF8(wcmd);
						bool bSuccess = DbgCmdExecDirect(cmd.c_str());

						apiInfo.bCmdSuccess = bSuccess;
						apiInfo.bWantToSetBp = isChecked;
						apiInfo.bBpSet = bSuccess ? isChecked : apiInfo.bBpSet;

					} else {
						// 清理已经设置成功的断点
						if (apiInfo.bBpSet) {
							std::wstring wcmd = L"bc " + g_Api_Groups[tabIdx].apiList[chkIdx].apiName;
							std::string cmd = WideToUTF8(wcmd);
							bool bSuccess = DbgCmdExecDirect(cmd.c_str());
							apiInfo.bCmdSuccess = bSuccess;
							apiInfo.bWantToSetBp = isChecked;
							apiInfo.bBpSet = bSuccess ? isChecked : apiInfo.bBpSet;
						}

					}

					// 更新状态
					RedrawWindow(hCheck, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);

					/*_plugin_logprintf("WM_COMMAND: Checkboxes ID %d, tabIdx%d, chkIdx%d, isChecked, %s, bBpSet,%s\n",
						ctrlID,
						tabIdx,
						chkIdx,
						isChecked ? "check" : "uncheck",
						apiInfo.bBpSet ? "success" : "failed");*/
				}

			
				return 0;
			}
	
		
		
		
			
		}
		return 0;

		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
		return 0;

		case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT pDrawItem = (LPDRAWITEMSTRUCT)lParam;
			HWND hButton = pDrawItem->hwndItem;
			// 检查是否是复选框
			if (pDrawItem->CtlType != ODT_BUTTON)
				return DefWindowProc(hwnd, iMsg, wParam, lParam);

			// 修改：通过映射表直接获取数据
			auto it = g_checkboxMap.find(hButton);
			if (it == g_checkboxMap.end())
				return DefWindowProc(hwnd, iMsg, wParam, lParam);

			size_t tabIdx = it->second.first;
			size_t chkIdx = it->second.second;
			const auto& apiInfo = g_Api_Groups[tabIdx].apiList[chkIdx];

			DrawCheckbox(hButton, pDrawItem, apiInfo);

			return DefWindowProc(hwnd, iMsg, wParam, lParam);
		}
       
		case WM_SIZE: {
	
			AdjustLayout(hwnd);
			break;
			return 0;
		}
		case WM_DESTROY:
			g_checkboxMap.clear();
			if (g_dpi.font) DeleteObject(g_dpi.font);
			if (g_dpi.tabfont) DeleteObject(g_dpi.tabfont);
			for (auto& tab : g_tabChecks) {
				for (HWND hCheck : tab) {
					DestroyWindow(hCheck);
				}
			}

			if (g_settings.opts().closeClearBreakPoint) {
				CleanupBreakpoints();
			}
			PostQuitMessage(0);
			return 0;
		case WM_DPICHANGED:
		{
			g_dpi.current = HIWORD(wParam);
			g_dpi.scaling = g_dpi.current / 96.0f;
			RecreateFont();

			// 调整窗口位置和大小
			RECT* rc = (RECT*)lParam;
			SetWindowPos(hwnd, nullptr,
				rc->left, rc->top,
				rc->right - rc->left,
				rc->bottom - rc->top,
				SWP_NOZORDER | SWP_NOACTIVATE);

			AdjustLayout(hwnd);
			// 强制重绘所有控件
			for (const auto& tabIndex : g_tabChecks)
			{
				for (HWND hCheck : tabIndex)
				{
					if (hCheck)
						InvalidateRect(hCheck, NULL, TRUE);
				}
			}
			return 0;
		}
		}

		return DefWindowProcW(hwnd, iMsg, wParam, lParam);
}

void DrawCheckbox(HWND hButton, LPDRAWITEMSTRUCT pDrawItem, const ApiBreakPointInfo& apiInfo)
{
	//LRESULT state = SendMessage(hButton, BM_GETCHECK, 0, 0);
	bool isChecked = apiInfo.bBpSet;

	// 设置背景颜色
	HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_WINDOW));
	FillRect(pDrawItem->hDC, &pDrawItem->rcItem, hBrush);
	DeleteObject(hBrush);

	// 设置文本颜色
	SetTextColor(pDrawItem->hDC, GetSysColor(COLOR_WINDOWTEXT));
	SetBkMode(pDrawItem->hDC, TRANSPARENT);

	// 计算尺寸
	const int checkSize = MulDiv(16, g_dpi.current, 96);
	const int margin = MulDiv(4, g_dpi.current, 96);

	// 获取字体高度
	TEXTMETRIC tm;
	GetTextMetrics(pDrawItem->hDC, &tm);
	int checkBoxSize = tm.tmHeight;

	// 计算文本区域
	RECT rcText = pDrawItem->rcItem;
	rcText.left += checkBoxSize + 5; // 左边距（为复选框留出空间）
	rcText.right -= 5; // 右边距

	RECT rcCheck;
	GetClientRect(hButton, &rcCheck);

	// 计算复选框位置
	rcCheck.left += margin;
	rcCheck.top += (rcCheck.bottom - rcCheck.top - checkSize) / 2;
	rcCheck.right = rcCheck.left + checkSize;
	rcCheck.bottom = rcCheck.top + checkSize;

	UINT uState = DFCS_BUTTONCHECK;
	if (isChecked) uState |= DFCS_CHECKED;
	if (pDrawItem->itemState & ODS_DISABLED) uState |= DFCS_INACTIVE;
	DrawFrameControl(pDrawItem->hDC, &rcCheck, DFC_BUTTON, uState);

	// 绘制对勾
	if (isChecked) {
		HPEN hPen = CreatePen(PS_SOLID, MulDiv(2, g_dpi.current, 96), RGB(0, 0, 0));
		HGDIOBJ hOldPen = SelectObject(pDrawItem->hDC, hPen);

		const POINT pts[] = {
			{ rcCheck.left + MulDiv(3, g_dpi.current, 96), rcCheck.top + MulDiv(7, g_dpi.current, 96) },
			{ rcCheck.left + MulDiv(7, g_dpi.current, 96), rcCheck.top + MulDiv(11, g_dpi.current, 96) },
			{ rcCheck.left + MulDiv(13, g_dpi.current, 96), rcCheck.top + MulDiv(3, g_dpi.current, 96) }
		};
		Polyline(pDrawItem->hDC, pts, 3);

		SelectObject(pDrawItem->hDC, hOldPen);
		DeleteObject(hPen);
	}

	// 绘制 dllName
	if (!apiInfo.dllName.empty()) {
		DrawTextW(pDrawItem->hDC, apiInfo.dllName.c_str(), -1, &rcText, DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}
	// 绘制 apiName
	RECT rcApiName = rcText;
	rcApiName.left = rcCheck.right + margin; // apiName 的起始位置
	DrawTextW(pDrawItem->hDC, apiInfo.apiName.c_str(), -1, &rcApiName, DT_LEFT | DT_SINGLELINE | DT_VCENTER);

	// 绘制 description（右对齐）
	RECT rcDescription = rcText;
	DrawTextW(pDrawItem->hDC, apiInfo.description.c_str(), -1, &rcDescription, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
}

void CreateTabs(TCITEMW& ti, HWND& hwnd)
{
	if (!IsWindow(hwnd)) {
		_plugin_logprintf("failed：Parent Window handdle invalid\n");
		return;
	}

	g_tabChecks.clear();
	g_tabChecks.reserve(g_Api_Groups.size());
	for (size_t tabIdx = 0; tabIdx < g_Api_Groups.size(); tabIdx++)
	{
		if (g_Api_Groups[tabIdx].groupName.empty()) {
			_plugin_logprintf("failed：tab %d groupName empty\n", tabIdx);
			continue;
		}
		ti.mask = TCIF_TEXT;
		ti.pszText = (LPWSTR)g_Api_Groups[tabIdx].groupName.c_str();
		TabCtrl_InsertItem(hTabControl, tabIdx, &ti);

		size_t checkCount = g_Api_Groups[tabIdx].apiList.size();

		if (g_Api_Groups[tabIdx].apiList.empty()) {
			_plugin_logprintf("failed：tab %d  apiList empty\n", tabIdx);
			continue;
		}

		std::vector<HWND> Hchecks;
		Hchecks.reserve(checkCount);
		for (size_t chkIdx = 0; chkIdx < checkCount; chkIdx++) {

			//UINT_PTR controlID = IDC_CHECK_FIRST + tabIdx * MAX_CHECKS_PER_TAB + chkIdx;
			HWND hCheck = CreateWindowW(
				L"BUTTON",
				WC_BUTTON,
				WS_CHILD | WS_VISIBLE | BS_MULTILINE | BS_OWNERDRAW,
				0, 0, 0, 0,
				hwnd,
				reinterpret_cast<HMENU>(static_cast<UINT_PTR>(IDC_CHECK_FIRST + (tabIdx * MAX_CHECKS_PER_TAB) + chkIdx)),
				g_hInstance,
				nullptr  // 参数11
			);

			//_plugin_logprintf("Create tabs %d,TabcheckCount ID: %d controlID: %d\n", tabIdx, checkCount, IDC_CHECK_FIRST + tabIdx * MAX_CHECKS_PER_TAB + chkIdx);

			if (hCheck) {
				SendMessageW(hCheck, WM_SETFONT, (WPARAM)g_dpi.font, TRUE);
				g_checkboxMap[hCheck] = std::make_pair(tabIdx, chkIdx);
				Hchecks.push_back(hCheck);
			}

		}
		g_tabChecks.push_back(Hchecks);

	}
}

// 布局调整
void AdjustLayout(HWND hwnd) {
	RECT rc;
	GetClientRect(hwnd, &rc);
	const int width = rc.right - rc.left;
	const int height = rc.bottom - rc.top;

	// Tab控件
	const int tabHeight = MulDiv(TAB_HEIGHT, g_dpi.current, 96);
	SetWindowPos(hTabControl, nullptr, 0, 0, width, tabHeight, SWP_NOZORDER);

	// 复选框布局
	const int margin = MulDiv(MARGIN_SIZE, g_dpi.current, 96);
	const int checkWidth = (width - margin * (CHECKBOX_COLUMNS + 1)) / CHECKBOX_COLUMNS;
	const int checkHeight = MulDiv(CHECKBOX_HEIGHT, g_dpi.current, 96);

	// 调整Tab控件
	SetWindowPos(hTabControl, NULL,
		0, 0, rc.right, tabHeight,
		SWP_NOZORDER | SWP_NOACTIVATE);

	// 计算每列的宽度和每行的高度
	int totalMargin = margin * (CHECKBOX_COLUMNS + 1);
	int availableWidth = width - totalMargin;
	int availableHeight = height - tabHeight;

	//调整复选框
	CurTab = TabCtrl_GetCurSel(hTabControl);
	if (CurTab == -1) {
		return; // 没有选中的选项卡
	}

	const auto& checkBoxes = g_tabChecks[CurTab];

	for (size_t i = 0; i < checkBoxes.size(); i++) {
		int col = i % CHECKBOX_COLUMNS;
		int row = i / CHECKBOX_COLUMNS;
		int x = margin + col * (checkWidth + margin);
		int y = tabHeight + margin + row * (checkHeight + margin);

		if (checkBoxes[i]) {
			SetWindowPos(checkBoxes[i], nullptr, x, y, checkWidth, checkHeight, SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}
}

// 在程序退出或崩溃前的清理函数中
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

// 字体管理
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

	// 创建 Tab 控件的字体
	LOGFONTW tabLf = lf;
	tabLf.lfHeight = -MulDiv(14, g_dpi.current, 96); // 设置 Tab 控件的字体大小
	g_dpi.tabfont = CreateFontIndirectW(&tabLf);

	// 设置 Tab 控件的字体
	if (hTabControl) {
		SendMessage(hTabControl, WM_SETFONT, (WPARAM)g_dpi.tabfont, TRUE);
	}

	// 更新其他控件字体
	EnumChildWindows(hMain, [](HWND hwnd, LPARAM) -> BOOL {
		if (hwnd != hTabControl) { // 排除 Tab 控件
			SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_dpi.font, TRUE);
		}
		return TRUE;
		}, 0
	);
}




// set delay breakpoint when dll loaded
EXTERN_C __declspec(dllexport) void _cdecl CBLOADDLL(
	CBTYPE bType, // event type, one of the `cbType` CB_* values listed above
	PLUG_CB_LOADDLL* callbackInfo // pointer to a structure of information
)
{
	if (g_settings.opts().dllReloadBreakPoint) {
	//check if there are any breakpoint can be set
		for (size_t i = 0; i < g_Api_Groups.size(); i++)
		{
			for (size_t j = 0; j < g_Api_Groups[i].apiList.size(); j++)
			{
				auto& apiInfo = g_Api_Groups[i].apiList[j];  // 简化引用

				if (apiInfo.bBpSet) {
					_plugin_logprintf("BP Already Set\n");
					std::wstring wcmd = L"bp " + apiInfo.apiName;
					std::string cmd = WideToUTF8(wcmd);
					bool bSuccess = DbgCmdExecDirect(cmd.c_str());
				}

				
			}
		 }
	}
}

// register menu to popup main window
extern "C" __declspec(dllexport) void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
	switch (info->hEntry)
	{
	case MENU_MAINWINDOW_POPUP:
	{
		if (!bIsMainWindowShow && DbgIsDebugging())
		{
			HANDLE hThread = CreateThread(
				NULL,                   // 安全属性
				0,                      // 默认堆栈大小
				MsgLoopThread,          // 线程函数
				NULL,                   // 参数
				0,                      // 创建标志
				NULL                    // 线程ID
			);

			if (hThread != NULL)
			{
				// 线程创建成功，可以关闭句柄（如果不需后续操作）
				CloseHandle(hThread);
				bIsMainWindowShow = true;
			}
			else
			{
				// 处理线程创建失败的情况
				_plugin_logprintf("Failed to create thread %s \n", MB_ICONERROR);
			}
		}
		break;
	}

	case MENU_CONFIG:
	{
		ReloadApiGroupsFromJson(g_api_BreakPointConfigPath);
		break;
	}

	case MENU_OPTIONS:
	{
		DialogBoxW(g_hInstance, MAKEINTRESOURCE(IDD_OPTIONS), hwndDlg, &OptionsDlgProc);
		break;
	}

	default: {
		auto profile_name = g_settings.profile_names()[info->hEntry - MENU_MAX].c_str();
		g_settings.SetProfile(profile_name);
		break;
	}



	}
}



