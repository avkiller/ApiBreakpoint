#include "plugin.h"
#include "ApiGroup.h"
#include "ApiManageWin.h"
#include "CheckboxCache.h"

namespace {
	CheckboxCache s_cache; // 匿名命名空间限制作用域
	TextLayoutCache g_textCache;
}

// make sure only one window exists
bool bIsMainWindowShow = false;
HWND hTabControl;
HWND hMain;
int CurTab = 0;

std::vector<std::vector<HWND>> g_tabChecks;
std::unordered_map<HWND, std::pair<size_t, size_t>> g_checkboxMap;

DpiState g_dpi = {
	.current = 96,    // 初始DPI值
	.scaling = 1.0f,  // 初始缩放因子
	.font = nullptr,   // 初始字体句柄
	.tabfont = nullptr   // 初始字体句柄
};


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
					std::string cmd = scl::WideToUtf8(wcmd);
					bool bSuccess = DbgCmdExecDirect(cmd.c_str());

					apiInfo.bCmdSuccess = bSuccess;
					apiInfo.bWantToSetBp = isChecked;
					apiInfo.bBpSet = bSuccess ? isChecked : apiInfo.bBpSet;

				}
				else {
					// 清理已经设置成功的断点
					if (apiInfo.bBpSet) {
						std::wstring wcmd = L"bc " + g_Api_Groups[tabIdx].apiList[chkIdx].apiName;
						std::string cmd = scl::WideToUtf8(wcmd);
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
		const auto& groupInfo = g_Api_Groups[tabIdx];
		const auto& apiInfo = groupInfo.apiList[chkIdx];

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
		g_textCache.Clear();
		s_cache.Cleanup();
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
	HDC hDC = pDrawItem->hDC;
	// 更新 DPI 资源
	s_cache.UpdateDpiResources(g_dpi.current);

	//LRESULT state = SendMessage(hButton, BM_GETCHECK, 0, 0);
	bool isChecked = apiInfo.bBpSet;

	// 背景画刷处理
	const COLORREF bgColor = GetSysColor(COLOR_WINDOW);
	s_cache.UpdateBrush(bgColor);
	FillRect(hDC, &pDrawItem->rcItem, s_cache.bgBrush);

	// 字体度量更新
	s_cache.UpdateFontMetrics(hDC);

	// 绘制状态管理
	if (s_cache.lastTextColor != GetSysColor(COLOR_WINDOWTEXT)) {
		SetTextColor(hDC, s_cache.lastTextColor = GetSysColor(COLOR_WINDOWTEXT));
	}

	if (s_cache.lastBkMode != TRANSPARENT) {
		SetBkMode(hDC, s_cache.lastBkMode = TRANSPARENT);
	}
	HFONT font = s_cache.currentFont;

	// 计算复选框位置
	RECT rcCheck;
	GetClientRect(hButton, &rcCheck);

	rcCheck.left += s_cache.margin;
	rcCheck.top += (rcCheck.bottom - rcCheck.top - s_cache.checkSize) / 2;
	rcCheck.right = rcCheck.left + s_cache.checkSize;
	rcCheck.bottom = rcCheck.top + s_cache.checkSize;

	// 6. 绘制复选框框架
	UINT frameState = DFCS_BUTTONCHECK;
	if (apiInfo.bBpSet) frameState |= DFCS_CHECKED;
	if (pDrawItem->itemState & ODS_DISABLED) frameState |= DFCS_INACTIVE;
	DrawFrameControl(hDC, &rcCheck, DFC_BUTTON, frameState);

	// 7. 绘制自定义对勾
	if (apiInfo.bBpSet) {
		if (!s_cache.checkPen) {
			s_cache.checkPen = CreatePen(PS_SOLID, s_cache.penWidth, RGB(0, 0, 0));
		}

		HGDIOBJ hOldPen = SelectObject(hDC, s_cache.checkPen);

		POINT dynamicPoints[3];
		for (int i = 0; i < 3; ++i) {
			dynamicPoints[i] = {
				rcCheck.left + s_cache.checkPoints[i].x,
				rcCheck.top + s_cache.checkPoints[i].y
			};
		}
		Polyline(hDC, dynamicPoints, 3);
		SelectObject(hDC, hOldPen);
	}

	// 文本布局计算
	const int textMargin = s_cache.margin / 2;
	RECT rcText = pDrawItem->rcItem;
	rcText.left += s_cache.checkSize + textMargin;
	rcText.right -= textMargin;

	
	std::wstring dllName = apiInfo.dllName;
	std::wstring apiName = apiInfo.apiName;
	std::wstring description = apiInfo.description;
	
	// 分层次绘制文本
	const int textFlags = DT_SINGLELINE | DT_VCENTER | DT_NOCLIP;

	RECT rcDll = rcText;
	rcDll.left = rcText.left + textMargin;
	//  绘制DLL名称（左对齐）
	if (!dllName.empty()) {
		//DrawTextW(hDC, dllName.c_str(), -1, &rcDll, DT_CALCRECT); // 计算文本尺寸
		CacheValue apiCache = g_textCache.GetTextLayout(hDC, dllName, font, 0, 0);
		rcDll.right = rcDll.left + apiCache.size.cx;
		rcDll.top = rcText.top;
		rcDll.bottom = rcText.bottom;
		DrawTextW(hDC, dllName.c_str(), -1, &rcDll, DT_LEFT | textFlags);
	}
	else {
		rcDll.right = rcText.left;
	}

	
	// 绘制API名称
	RECT rcApi = rcText;
	rcApi.left = rcDll.right + textMargin;
	if (!apiName.empty()) {
		CacheValue apiCache = g_textCache.GetTextLayout(hDC, apiName, font, 0, 0);
		rcApi.right = rcApi.left + apiCache.size.cx;
		rcApi.top = rcText.top;
		rcApi.bottom = rcText.bottom;
		DrawTextW(hDC, apiName.c_str(), -1, &rcApi, DT_LEFT | textFlags);

	}
	else {
		rcApi.right = rcDll.right;
	}

	RECT rcDesc = rcText;
	rcDesc.left = rcApi.right + textMargin;
	// 9.3 绘制描述信息（右对齐）
	if (!description.empty()) {
		CacheValue apiCache = g_textCache.GetTextLayout(hDC, description, font, 0, 0);
		rcDesc.right = rcDesc.left + apiCache.size.cx;
		//DrawTextW(hDC, description.c_str(), -1, &rcDesc, DT_RIGHT | DT_SINGLELINE | DT_END_ELLIPSIS);
		if (rcDesc.right < rcText.right) {
			DrawTextW(hDC, description.c_str(), -1, &rcText, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
		}
		else {
			rcText.left = rcApi.right + textMargin;
			DrawTextW(hDC, description.c_str(), -1, &rcText, DT_LEFT | DT_SINGLELINE| DT_VCENTER | DT_END_ELLIPSIS);
		
		}
		
		
	}

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
				std::string cmd = "bc " + scl::WideToUtf8(api.apiName);
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

