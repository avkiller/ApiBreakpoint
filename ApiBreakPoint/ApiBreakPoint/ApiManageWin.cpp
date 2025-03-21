﻿
#include "ApiManageWin.h"

namespace {
	constexpr int MAINWINDOW_WIDTH = 800;
	constexpr int MAINWINDOW_HEIGHT = 600;
	constexpr int CHECKBOX_COLUMNS = 3;
	constexpr int TAB_HEIGHT = 32;
	constexpr int CHECKBOX_HEIGHT = 25;
	constexpr int CHECKBOX_MINCHECKWIDTH = 180;
	constexpr int CHECKBOX_MAXCHECKWIDTH = 360;
	constexpr int MARGIN = 10;
	constexpr int IDC_TABCTRL = 1500;
	constexpr int IDC_CHECK_FIRST = 1500;
	constexpr int MAX_CHECKS_PER_TAB = 100;
	constexpr wchar_t CLASS_NAME[] = L"ApiBreakpointManagerWindow";
} // namespace

ApiBreakpointManager& ApiBreakpointManager::GetInstance() {
	static ApiBreakpointManager instance;
	return instance;
}

ApiBreakpointManager::ApiBreakpointManager() {
	Initialize();
}

ApiBreakpointManager::~ApiBreakpointManager() {
	Cleanup();
}

void ApiBreakpointManager::Initialize() {
	WNDCLASSW wc = { 0 };
	wc.lpfnWndProc = WndProc;
	wc.hInstance = g_hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	RegisterClassW(&wc);

	m_dpi.current = GetCurrentDPI();
	m_dpi.scaling = m_dpi.current / 96.0f;
	UpdateDPICache(m_dpi.current);
	RecreateFonts();
	InitCommonControls();
}

void ApiBreakpointManager::CenterWindow() {
	if (!m_hMainWnd) return;

	// 获取父窗口（调试器主窗口）
	HWND hParent = GetParent(m_hMainWnd);
	if (!hParent) hParent = GetDesktopWindow();



	// 获取窗口和父窗口的尺寸
	RECT rcWindow, rcParent, rc;
	GetWindowRect(m_hMainWnd, &rcWindow);
	GetWindowRect(hParent, &rcParent);
	CopyRect(&rc, &rcParent);

	OffsetRect(&rcWindow, -rcWindow.left, -rcWindow.top);
	OffsetRect(&rc, -rc.left, -rc.top);
	OffsetRect(&rc, -rcWindow.right, -rcWindow.bottom);

	SetWindowPos(m_hMainWnd,
		HWND_TOP,
		rcParent.left + (rc.right / 2),
		rcParent.top + (rc.bottom / 2),
		0, 0,          // Ignores size arguments. 
		SWP_NOSIZE);
}

LRESULT CALLBACK ApiBreakpointManager::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	ApiBreakpointManager* pThis = nullptr;

	if (msg == WM_NCCREATE) {
		// 从创建参数获取实例指针
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pThis = static_cast<ApiBreakpointManager*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
		pThis->m_hMainWnd = hwnd;
	}
	else {
		pThis = reinterpret_cast<ApiBreakpointManager*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	if (pThis) {
		return pThis->HandleMessage(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT ApiBreakpointManager::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CREATE:
		Initialize();
		InitTooltip();
		return 0;

	//case WM_VSCROLL:{
	//	/*HandleScroll(hwnd, wParam);
	//	AdjustLayout();*/
	//	ULONGLONG s_lastScrollTime = 0;
	//	ULONGLONG now = GetTickCount64();
	//	if (now - s_lastScrollTime > 100) { // 50ms间隔
	//		HandleScroll(hwnd, wParam);
	//		AdjustLayout();
	//		s_lastScrollTime = now;
	//	}
	//	break;
	//
	//}
		
	case WM_MOUSEWHEEL: {
		HandleMouseWheel(wParam, hwnd);
		break;
	}

	case WM_SIZE:
		AdjustLayout();
		return 0;

	case WM_DPICHANGED: {
		m_dpi.current = LOWORD(wParam);
		HandleDpiChange();
		RECT* const rect = reinterpret_cast<RECT*>(lParam);
		SetWindowPos(m_hMainWnd, nullptr,
			rect->left, rect->top,
			rect->right - rect->left,
			rect->bottom - rect->top,
			SWP_NOZORDER | SWP_NOACTIVATE);
		return 0;
	}

	case WM_COMMAND:
		if (HIWORD(wParam) == BN_CLICKED) {
			HandleButtonClick(reinterpret_cast<HWND>(lParam));
		}
		return 0;

	case WM_NOTIFY:
		return HandleNotify(reinterpret_cast<NMHDR*>(lParam));

	case WM_PAINT: {
		HandlePaint(hwnd);
		return TRUE;
	}
		

	case WM_DRAWITEM: {
		LPDRAWITEMSTRUCT pDrawItem = (LPDRAWITEMSTRUCT)lParam;
		HWND hButton = pDrawItem->hwndItem;
		// 检查是否是复选框
		if (pDrawItem->CtlType != ODT_BUTTON)
			return DefWindowProc(m_hMainWnd, msg, wParam, lParam);

		DrawCheckbox(
			reinterpret_cast<HWND>(reinterpret_cast<DRAWITEMSTRUCT*>(lParam)->hwndItem),
			reinterpret_cast<DRAWITEMSTRUCT*>(lParam));
		return TRUE;
	}

	case WM_DESTROY:
		Cleanup();
		//PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		DestroyWindow(m_hMainWnd); // 仅销毁当前窗口
		return 0;

	default:
		break;
	}
	return DefWindowProc(m_hMainWnd, msg, wParam, lParam);
}

void ApiBreakpointManager::HandlePaint(HWND hwnd)
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	RECT rcClient;
	GetClientRect(hwnd, &rcClient);
	int cxClient = rcClient.right - rcClient.left;  // 客户区宽度
	int cyClient = rcClient.bottom - rcClient.top;
	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmMem = CreateCompatibleBitmap(hdc, cxClient, cyClient);
	SelectObject(hdcMem, hbmMem);

	// 所有绘制操作在内存DC进行

	HBRUSH hBgBrush = CreateSolidBrush(RGB(255, 255, 255));
	FillRect(hdcMem, &rcClient, hBgBrush);
	DeleteObject(hBgBrush);

	BitBlt(hdc, 0, 0, cxClient, cyClient, hdcMem, 0, 0, SRCCOPY);
	DeleteObject(hbmMem);
	DeleteDC(hdcMem);
	EndPaint(hwnd, &ps);
}

void ApiBreakpointManager::HandleMouseWheel(WPARAM& wParam, HWND hwnd)
{
	if (!m_needScroll) return;
	ULONGLONG s_lastScrollTime = 0;

	ULONGLONG now = GetTickCount64();

	if (now - s_lastScrollTime > 100) {
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		UINT scrollCmd = (delta > 0) ? SB_LINEUP : SB_LINEDOWN;
		HandleScroll(hwnd, MAKEWPARAM(scrollCmd, 0));
		_plugin_logprintf("[+] ApiBreakpoint: WM_MOUSEWHEEL CALL AdjustLayout\n");
		AdjustLayout();
		s_lastScrollTime = now;
	}

}

void ApiBreakpointManager::ShowMainWindow() {
	if (m_hMainWnd && IsWindowVisible(m_hMainWnd)) {
		SetForegroundWindow(m_hMainWnd);
		return;
	}

	m_hMainWnd = CreateWindowW(CLASS_NAME, L"API断点管理器",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT,
		MulDiv(MAINWINDOW_WIDTH, m_dpi.current, 96),
		MulDiv(MAINWINDOW_HEIGHT, m_dpi.current, 96),
		nullptr, nullptr, g_hInstance, this);

	CreateTabControl();
	::ShowWindow(m_hMainWnd, SW_SHOW);
	CenterWindow();
	UpdateWindow(m_hMainWnd);
}

void ApiBreakpointManager::HandleDpiChange() {
	m_dpi.current = GetCurrentDPI();
	m_dpi.scaling = m_dpi.current / 96.0f;
	RecreateFonts();
	CenterWindow();
	AdjustLayout();
}

LRESULT ApiBreakpointManager::HandleNotify(NMHDR* pHdr) {
	if (pHdr->code == TTN_GETDISPINFO) {
		NMTTDISPINFO* pInfo = reinterpret_cast<NMTTDISPINFO*>(pHdr);
		auto it = m_TruncatedInfos.find(pInfo->hdr.hwndFrom);
		if (it != m_TruncatedInfos.end()) {
			pInfo->lpszText = const_cast<LPWSTR>(it->second.fullText.c_str());
		}
		return 0;
	}

	if (pHdr->idFrom != IDC_TABCTRL)
		return 0;

	switch (pHdr->code) {
	case TCN_SELCHANGE:
		OnTabChanged();
		return TRUE;

	}


	return DefWindowProc(m_hMainWnd, WM_NOTIFY, 0, reinterpret_cast<LPARAM>(pHdr));
	//return 0;
}

void ApiBreakpointManager::HandleButtonClick(HWND hButton) {
	auto it = m_checkboxMap.find(hButton);
	if (it != m_checkboxMap.end()) {
		const auto& [tabIdx, itemIdx] = it->second;
		ApiBreakPointInfo& apiInfo = g_Api_Groups[tabIdx].apiList[itemIdx];
		ToggleBreakpoint(hButton, apiInfo);
	}
}

void ApiBreakpointManager::HandleScroll(HWND hWnd, WPARAM wParam) {

	SCROLLINFO si = { sizeof(SCROLLINFO), SIF_ALL };
	GetScrollInfo(hWnd, SB_VERT, &si);

	switch (LOWORD(wParam)) {
	case SB_LINEUP:   si.nPos -= 1; break;
	case SB_LINEDOWN: si.nPos += 1; break;
	case SB_PAGEUP:   si.nPos -= si.nPage; break;
	case SB_PAGEDOWN: si.nPos += si.nPage; break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		si.nPos = si.nTrackPos;
		break;
	}

	// 确保页数在有效范围内
	si.nPos = max(si.nMin, min(si.nPos, si.nMax - (int)si.nPage + 1));
	si.fMask = SIF_POS;

	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
	m_vScrollPage = si.nPos;
	if (m_vScrollPage == 1) {
		_plugin_logprintf("[+] ApiBreakpoint HandleScroll: do scroll \n", si.nPos);
	}

	_plugin_logprintf("[+] ApiBreakpoint HandleScroll: si.nPos: %d\n", si.nPos);
}

void ApiBreakpointManager::InitTooltip() {
	m_hTooltip = CreateWindowEx(
		WS_EX_TOPMOST, TOOLTIPS_CLASS, nullptr,
		WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		m_hMainWnd, nullptr, GetModuleHandle(nullptr), nullptr
	);
}

LRESULT CALLBACK ApiBreakpointManager::TooltipProc(
	HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	// 通过 dwRefData 获取类实例
	if (auto pThis = reinterpret_cast<ApiBreakpointManager*>(dwRefData)) {
		return pThis->HandleMsgTooltip(
			hWnd, uMsg, wParam, lParam, uIdSubclass, dwRefData
		);
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT ApiBreakpointManager::HandleMsgTooltip(HWND hwnd, UINT uMsg,WPARAM wParam, 
	LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	static bool trackingMouseLeave = false;
	switch (uMsg) {
	    case WM_NCDESTROY:
			RemoveWindowSubclass(hwnd, TooltipProc, uIdSubclass);
			break;

		case WM_MOUSEMOVE: {
			auto it = m_TruncatedInfos.find(hwnd);
			if (it != m_TruncatedInfos.end()) {

				
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				//ClientToScreen(hwnd, &pt);

				if (PtInRect(&it->second.rcTruncated, pt)) {

					ClientToScreen(hwnd, &pt);

					TOOLINFO ti = { sizeof(TOOLINFO) };
					ti.hwnd = hwnd;
					ti.uId = 1;
					ti.lpszText = (LPWSTR)it->second.fullText.c_str();

					if (!SendMessage(m_hTooltip, TTM_GETTOOLINFO, 0, (LPARAM)&ti)) {
						SendMessage(m_hTooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
					}

					SendMessage(m_hTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
					SendMessage(m_hTooltip, TTM_TRACKPOSITION, 0, MAKELPARAM(pt.x + 16, pt.y));

					if (!trackingMouseLeave) {
						TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };
						tme.dwFlags = TME_LEAVE;
						tme.hwndTrack = hwnd;
						TrackMouseEvent(&tme);
						trackingMouseLeave = true;
					}
				}	
			}

			break;

		}

		case WM_MOUSELEAVE: {
			// 关闭 Tooltip
			TOOLINFO ti = { sizeof(TOOLINFO) };
			ti.hwnd = hwnd;
			ti.uId = 1;
			SendMessage(m_hTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
			trackingMouseLeave = false;
			break;
		}

		case WM_DESTROY: {
			// 清理资源
			RemoveWindowSubclass(hwnd, TooltipProc, uIdSubclass);
			break;
		}
	
	}
	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}


void ApiBreakpointManager::AdjustLayout() {
	if (!m_hMainWnd || !m_hTabCtrl) return;

	//获取主窗口的位置信息
	RECT rcMain;
	GetClientRect(m_hMainWnd, &rcMain);
	const int mainWidth = rcMain.right - rcMain.left;
	const int mainHeight = rcMain.bottom - rcMain.top;

	int availableWidth = mainWidth;
	int availableHeight = mainHeight;

	int tabIndex = TabCtrl_GetCurSel(m_hTabCtrl);
	int itemsCnt = static_cast<int>(m_tabChecks[tabIndex].checkboxes.size());

	// 调整Tab控件
	const int tabHeight = m_cachedTabHeight;
	SetWindowPos(m_hTabCtrl, nullptr, 0, 0, mainWidth, tabHeight, SWP_NOZORDER);

	// 复选框布局
	const int margin = m_cachedMargin;
	const int checkHeight = m_cachedCheckHeight;
	const int minCheckWidth = m_cachedMinCheckWidth;
	const int maxCheckWidth = m_cachedMaxCheckWidth;

	// 动态计算列数
	int columns = (availableWidth - margin) / (minCheckWidth + margin);
	columns = max(1, columns);
	const int balanceThreshold = 15; // 降低平衡阈值
	if (itemsCnt <= balanceThreshold) {
		columns = min(columns, 2);
	}
	else {
		columns = min(columns, static_cast<int>(sqrt(itemsCnt)) + 1);
	}
	columns = clamp(columns, 1, static_cast<int>(itemsCnt));

	// 计算复选框的宽度
	int checkWidth = (availableWidth - margin * (columns + 1)) / columns;
	checkWidth = clamp(checkWidth, minCheckWidth, maxCheckWidth);
	const int viewportHeight = mainHeight - tabHeight - margin * 2;
	const int rowHeight = checkHeight + margin;
	int rowsPerPage = viewportHeight / rowHeight;
	if (viewportHeight % rowHeight != 0) {
		rowsPerPage++;
	}

	// 计算实际需要的高度
	const int totalRows = (itemsCnt + columns - 1) / columns;
	m_rowsPerPage = rowsPerPage * columns;
	//const int totalCheckHeight = totalRows * rowHeight;
	//const int scrollRange = totalCheckHeight - viewportHeight;
	//const int scrollRange = (itemCount + ItemsPerPage - 1) / ItemsPerPage;
	const int scrollRange = totalRows-rowsPerPage-1;
	m_maxScrollPage = max(0, scrollRange);

	m_needScroll = m_maxScrollPage > 0 ? TRUE : FALSE;
	if (m_needScroll) {
		// 设置滚动区域
		SCROLLINFO info = { sizeof(SCROLLINFO) };
		info.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		info.nMin = 0;
		info.nMax = totalRows;
		info.nPage = rowsPerPage;
		info.nPos = clamp(m_vScrollPage, 0, m_maxScrollPage);
		SetScrollInfo(m_hMainWnd, SB_VERT, &info, TRUE);
		ShowScrollBar(m_hMainWnd, SB_VERT, TRUE);
	}
	else {
		m_vScrollPage = 0;
		ShowScrollBar(m_hMainWnd, SB_VERT, FALSE);
	}

	//计算坐标
	const int startIndex = m_vScrollPage * columns;
	const int endIndex = min(startIndex + m_rowsPerPage, itemsCnt);
	const int stepX = checkWidth + margin;
	const int stepY = checkHeight + margin;
	int startY = tabHeight + margin;

	auto& checkboxes = m_tabChecks[tabIndex].checkboxes;
	HDWP hdwp = BeginDeferWindowPos(static_cast<int>(itemsCnt));
	for (int i=startIndex; i < itemsCnt; i++) {
		int  col= static_cast<int>((i-startIndex) % columns);
		int  row = static_cast<int>((i - startIndex) / columns);
		int x = margin + col * stepX;
		int y = startY + row * stepY;
		hdwp = DeferWindowPos(hdwp, checkboxes[i], nullptr,
			x, y, checkWidth, checkHeight,
			SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOREDRAW| SWP_NOCOPYBITS);
	}

	EndDeferWindowPos(hdwp);
	//RedrawWindow(m_hMainWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW | RDW_ERASE);
	RedrawWindow(m_hMainWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN|RDW_FRAME);

}

void ApiBreakpointManager::CreateTabControl() {
	m_hTabCtrl = CreateWindowW(L"SysTabControl32", L"",
		WS_CHILD | WS_VISIBLE | TCS_FIXEDWIDTH,
		0, 0, 0, 0, m_hMainWnd,
		reinterpret_cast<HMENU>(static_cast<UINT_PTR>(IDC_TABCTRL)),
		g_hInstance, nullptr);

	SendMessage(m_hTabCtrl, WM_SETFONT, (WPARAM)m_dpi.tabfont, TRUE);
	CreateTabs();
	//AdjustLayout();
}

void ApiBreakpointManager::CreateTabs() {

	m_tabChecks.clear();
	m_checkboxMap.clear();

	size_t totalChecks = 0;
	for (const auto& group : g_Api_Groups) {
		totalChecks += group.apiList.size();
	}
	m_checkboxMap.reserve(totalChecks);
	m_tabChecks.reserve(g_Api_Groups.size());


	TCITEMW ti = { 0 };
	ti.mask = TCIF_TEXT;
	TabContext context;

	for (size_t tabIdx = 0; tabIdx < g_Api_Groups.size(); ++tabIdx) {

		if (g_Api_Groups[tabIdx].groupName.empty()) {
			_plugin_logprintf("failed：tab %d groupName empty\n", tabIdx);
			continue;
		}
		ti.pszText = const_cast<LPWSTR>(g_Api_Groups[tabIdx].groupName.c_str());
		TabCtrl_InsertItem(m_hTabCtrl, tabIdx, &ti);
		m_tabChecks.push_back(context);
	}
    //创建第一个选项卡的复选框
	CreateTabContent(0);
}

void ApiBreakpointManager::CreateTabContent(int tabIndex) {
	auto& curtab = m_tabChecks[tabIndex];
	const auto& apiGroup = g_Api_Groups[tabIndex];

	size_t checkCount = g_Api_Groups[tabIndex].apiList.size();

	auto& Hcheckboxes = curtab.checkboxes;
	if (!curtab.checkboxes.size()) {
		Hcheckboxes.reserve(checkCount);
	}
	
	for (size_t chkIdx = 0; chkIdx < checkCount; ++chkIdx) {
		HWND hCheck = CreateWindowW(L"BUTTON", L"",
			WS_CHILD |WS_VISIBLE | BS_OWNERDRAW,
			0, 0, 0, 0, 
			m_hMainWnd, 
			//nullptr,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(IDC_CHECK_FIRST + (tabIndex * MAX_CHECKS_PER_TAB) + chkIdx)),
			g_hInstance, nullptr);

		SetWindowSubclass(
			hCheck,
			&ApiBreakpointManager::TooltipProc,
			0,
			reinterpret_cast<DWORD_PTR>(this)
		);

		if (hCheck) {
			SendMessage(hCheck, WM_SETFONT, (WPARAM)m_dpi.font, TRUE);
			Hcheckboxes.push_back(std::move(hCheck));
			m_checkboxMap.emplace(hCheck, std::make_pair(tabIndex, static_cast<int>(chkIdx)));
		}
	
		
	}
	//UpdateCheckboxes(tabIndex);
}

void ApiBreakpointManager::UpdateCheckboxes(int tabIndex) {
	const auto& apiGroup = g_Api_Groups[tabIndex];
	auto& tab = m_tabChecks[tabIndex];

	for (size_t i = 0; i < apiGroup.apiList.size(); ++i) {
		const auto& apiInfo = apiGroup.apiList[i];
		HWND hCheck = tab.checkboxes[i];

		SetWindowTextW(hCheck,
			(apiInfo.apiName + L"  " + apiInfo.description).c_str());
		SendMessage(hCheck, BM_SETCHECK,
			apiInfo.bBpSet ? BST_CHECKED : BST_UNCHECKED, 0);
	}
}

void ApiBreakpointManager::OnTabChanged() {
	int newTab = TabCtrl_GetCurSel(m_hTabCtrl);
	SwitchTabContent(m_currentTab, newTab);
}

void ApiBreakpointManager::SwitchTabContent(int oldTabIndex, int newTabIndex) {
	// 验证选项卡索引有效性
	if (oldTabIndex < 0 || oldTabIndex >= static_cast<int>(m_tabChecks.size()) ||
		newTabIndex < 0 || newTabIndex >= static_cast<int>(m_tabChecks.size()))
	{
		_plugin_logprintf("[错误] 无效的选项卡索引: old=%d, new=%d\n", oldTabIndex, newTabIndex);
		return;
	}

	// 隐藏旧选项卡内容
	if (oldTabIndex >= 0 && oldTabIndex < static_cast<int>(m_tabChecks.size())) {
		auto& oldTab = m_tabChecks[oldTabIndex];
		for (HWND hCheck : oldTab.checkboxes) {
			//if (IsWindow(hCheck)) DestroyWindow(hCheck);
			ShowWindow(hCheck, SW_HIDE);
		}
		//oldTab.checkboxes.clear();
	}

	// 延迟创建新选项卡内容（首次访问时创建）
	auto& newTab = m_tabChecks[newTabIndex];
	if (newTab.checkboxes.empty()) {
		CreateTabContent(newTabIndex);
		AdjustLayout();
	}

	// 显示新选项卡内容并更新布局
	for (HWND hCheck : newTab.checkboxes) {
		ShowWindow(hCheck, SW_SHOW);
		//UpdateWindow(hCheck); // 立即更新防止闪烁
	}

	// 更新当前选项卡索引
	m_currentTab = newTabIndex;
	// 强制重绘防止残留
	RedrawWindow(m_hTabCtrl, nullptr, nullptr,RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
}

void ApiBreakpointManager::ToggleBreakpoint(HWND hButton, ApiBreakPointInfo& apiInfo) {
	bool isChecked = !apiInfo.bWantToSetBp;

	UINT newState = (isChecked) ? BST_CHECKED : BST_UNCHECKED;

	// 执行业务逻辑
	if (isChecked) {

		std::wstring wcmd = L"bp " + apiInfo.apiName;
		std::string cmd = scl::WideToUtf8(wcmd);
		bool bSuccess = DbgCmdExecDirect(cmd.c_str());

		apiInfo.bCmdSuccess = bSuccess;
		apiInfo.bWantToSetBp = isChecked;
		apiInfo.bBpSet = bSuccess ? isChecked : apiInfo.bBpSet;

	}
	else {
		// 清理已经设置成功的断点
		if (apiInfo.bBpSet) {
			std::wstring wcmd = L"bc " + apiInfo.apiName;
			std::string cmd = scl::WideToUtf8(wcmd);
			bool bSuccess = DbgCmdExecDirect(cmd.c_str());
			apiInfo.bCmdSuccess = bSuccess;
			apiInfo.bWantToSetBp = isChecked;
			apiInfo.bBpSet = bSuccess ? isChecked : apiInfo.bBpSet;
		}

	}

	// 更新状态
	RedrawWindow(hButton, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
}

void ApiBreakpointManager::DrawCheckbox(HWND hWnd, LPDRAWITEMSTRUCT pDrawItem)
{
	const auto& [tabIndex, itemIndex] = m_checkboxMap[hWnd];
	auto& apiInfo = g_Api_Groups[tabIndex].apiList[itemIndex];
	HDC hDC = pDrawItem->hDC;
	RECT rcItem = pDrawItem->rcItem;
	// 双缓冲绘制
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, rcItem.right - rcItem.left, rcItem.bottom - rcItem.top);
	HGDIOBJ hOldBitmap = SelectObject(hMemDC, hBitmap);

	// 更新 DPI 资源
	ApiBreakpointManager::m_checkboxCache.UpdateDpiResources(m_dpi.current);

	//LRESULT state = SendMessage(hButton, BM_GETCHECK, 0, 0);
	bool isChecked = apiInfo.bBpSet;

	// 背景画刷处理
	const COLORREF bgColor = GetSysColor(COLOR_WINDOW);
	m_checkboxCache.UpdateBrush(bgColor);
	FillRect(hMemDC, &rcItem, m_checkboxCache.bgBrush);

	// 字体度量更新
	m_checkboxCache.UpdateFontMetrics(hDC);

	// 绘制状态管理
	if (m_checkboxCache.lastTextColor != GetSysColor(COLOR_WINDOWTEXT)) {
		SetTextColor(hMemDC, m_checkboxCache.lastTextColor = GetSysColor(COLOR_WINDOWTEXT));
	}

	if (m_checkboxCache.lastBkMode != TRANSPARENT) {
		SetBkMode(hMemDC, m_checkboxCache.lastBkMode = TRANSPARENT);
	}
	HFONT font = m_checkboxCache.currentFont;

	// 计算复选框位置
	RECT rcCheck;
	GetClientRect(hWnd, &rcCheck);

	rcCheck.left += m_checkboxCache.margin;
	rcCheck.top += (rcCheck.bottom - rcCheck.top - m_checkboxCache.checkSize) / 2;
	rcCheck.right = rcCheck.left + m_checkboxCache.checkSize;
	rcCheck.bottom = rcCheck.top + m_checkboxCache.checkSize;

	// 6. 绘制复选框框架
	UINT frameState = DFCS_BUTTONCHECK;
	if (apiInfo.bBpSet) frameState |= DFCS_CHECKED;
	if (pDrawItem->itemState & ODS_DISABLED) frameState |= DFCS_INACTIVE;
	DrawFrameControl(hMemDC, &rcCheck, DFC_BUTTON, frameState);

	// 7. 绘制自定义对勾
	if (apiInfo.bBpSet) {
		if (!m_checkboxCache.checkPen) {
			m_checkboxCache.checkPen = CreatePen(PS_SOLID, m_checkboxCache.penWidth, RGB(0, 0, 0));
		}

		HGDIOBJ hOldPen = SelectObject(hMemDC, m_checkboxCache.checkPen);

		POINT dynamicPoints[3];
		for (int i = 0; i < 3; ++i) {
			dynamicPoints[i] = {
				rcCheck.left + m_checkboxCache.checkPoints[i].x,
				rcCheck.top + m_checkboxCache.checkPoints[i].y
			};
		}
		Polyline(hMemDC, dynamicPoints, 3);
		SelectObject(hMemDC, hOldPen);

		SetTextColor(hMemDC, RGB(0, 0, 255));
	}

	// 文本布局计算
	const int textMargin = m_checkboxCache.margin / 2;
	RECT rcText = pDrawItem->rcItem;
	rcText.left += m_checkboxCache.checkSize + textMargin;
	rcText.right -= textMargin;

	
	std::wstring dllName = apiInfo.dllName;
	std::wstring apiName = apiInfo.apiName;
	std::wstring description = apiInfo.description;

	HFONT hOldFont = (HFONT)SelectObject(hMemDC, font);
	
	// 分层次绘制文本
	const int textFlags = DT_SINGLELINE | DT_VCENTER | DT_NOCLIP;

	RECT rcDll = rcText;
	rcDll.left = rcText.left + textMargin;
	//  绘制DLL名称（左对齐）
	if (!dllName.empty()) {
		CacheValue apiCache = m_textCache.GetTextLayout(hDC, dllName);
		rcDll.right = rcDll.left + apiCache.size.cx;
		rcDll.top = rcText.top;
		rcDll.bottom = rcText.bottom;
		DrawTextW(hMemDC, dllName.c_str(), -1, &rcDll, DT_LEFT | textFlags);
	}
	else {
		rcDll.right = rcText.left;
	}

	
	// 绘制API名称
	RECT rcApi = rcText;
	rcApi.left = rcDll.right + textMargin;
	if (!apiName.empty()) {
		CacheValue apiCache = m_textCache.GetTextLayout(hDC, apiName);
		rcApi.right = rcApi.left + apiCache.size.cx;
		rcApi.top = rcText.top;
		rcApi.bottom = rcText.bottom;
		DrawTextW(hMemDC, apiName.c_str(), -1, &rcApi, DT_LEFT | textFlags);

	}
	else {
		rcApi.right = rcDll.right;
	}

	RECT rcDesc = rcText;
	rcDesc.left = rcApi.right + textMargin;
	// 绘制描述信息（右对齐）
	if (!description.empty()) {
		CacheValue apiCache = m_textCache.GetTextLayout(hDC, description);
		rcDesc.right = rcDesc.left + apiCache.size.cx;
		if (rcDesc.right < rcText.right) {
			DrawTextW(hMemDC, description.c_str(), -1, &rcText, DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
		}
		else {
			rcText.left = rcApi.right + textMargin;
			DrawTextW(hMemDC, description.c_str(), -1, &rcText, DT_LEFT | DT_SINGLELINE| DT_VCENTER | DT_END_ELLIPSIS);

			const int requiredWidth = apiCache.size.cx;
			const int availableWidth = rcText.right - rcText.left;
			if (requiredWidth > availableWidth) {
				/*_plugin_logprintf*/("ELLIPSIS str %s", description);
				m_TruncatedInfos.emplace(
					hWnd,
					TruncatedCheckboxInfo(rcText, description)
				);

			}

			

		
		}
		
		
	}

	BitBlt(hDC, rcItem.left, rcItem.top, rcItem.right - rcItem.left, rcItem.bottom - rcItem.top,
		hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hMemDC);

}

// 在程序退出或崩溃前的清理函数中
void ApiBreakpointManager::CleanupBreakpoints()
{
	_plugin_logprintf("[+] ApiBreakpointManager::CleanupBreakpoints Cleaning up breakpoints...\n");

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
void ApiBreakpointManager::RecreateFonts() {
	if (m_dpi.font) DeleteObject(m_dpi.font);
	if (m_dpi.tabfont) DeleteObject(m_dpi.tabfont);

	LOGFONTW lf = {
		.lfHeight = -MulDiv(12, m_dpi.current, 96),
		.lfWeight = FW_NORMAL,
		.lfCharSet = GB2312_CHARSET,
		.lfQuality = CLEARTYPE_QUALITY,
	};
	wcscpy_s(lf.lfFaceName, L"Microsoft YaHei UI");

	m_dpi.font = CreateFontIndirectW(&lf);

	// 创建 Tab 控件的字体
	LOGFONTW tabLf = lf;
	tabLf.lfHeight = -MulDiv(14, m_dpi.current, 96); // 设置 Tab 控件的字体大小
	m_dpi.tabfont = CreateFontIndirectW(&tabLf);


	// 更新所有控件的字体
	if (m_hMainWnd) {
		EnumChildWindows(m_hMainWnd, [](HWND hWnd, LPARAM lParam) {
			auto pThis = reinterpret_cast<ApiBreakpointManager*>(lParam);
			if (hWnd == pThis->m_hTabCtrl) {
				SendMessageW(hWnd, WM_SETFONT, (WPARAM)pThis->m_dpi.tabfont, TRUE);
			}
			else {
				SendMessageW(hWnd, WM_SETFONT, (WPARAM)pThis->m_dpi.font, TRUE);
			}
			return TRUE;
		}, reinterpret_cast<LPARAM>(this));
	}
}

UINT ApiBreakpointManager::GetCurrentDPI() {
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
// 正确限定作用域的实现
ApiBreakpointManager::DpiInfo ApiBreakpointManager::GetDpiState() const {
	return m_dpi; 
}

void ApiBreakpointManager::UpdateDPICache(UINT newDPI) {
	m_cachedDPI = newDPI;
	m_cachedTabHeight = MulDiv(TAB_HEIGHT, newDPI, 96);
	m_cachedMargin = MulDiv(MARGIN, newDPI, 96);
	m_cachedCheckHeight = MulDiv(CHECKBOX_HEIGHT, newDPI, 96);
	m_cachedMinCheckWidth = MulDiv(CHECKBOX_MINCHECKWIDTH, newDPI, 96);
	m_cachedMaxCheckWidth = MulDiv(CHECKBOX_MAXCHECKWIDTH, newDPI, 96);
}

void ApiBreakpointManager::Cleanup() {
	// 清理字体资源
	if (m_dpi.font) {
		DeleteObject(m_dpi.font);
		m_dpi.font = nullptr;
	}
	if (m_dpi.tabfont) {
		DeleteObject(m_dpi.tabfont);
		m_dpi.tabfont = nullptr;
	}

	// 清理复选框资源
	for (auto& tab : m_tabChecks) {
		for (HWND hCheck : tab.checkboxes) {
			DestroyWindow(hCheck);
		}
		tab.checkboxes.clear();
	}
	m_tabChecks.clear();
	m_checkboxMap.clear();

	// 清理其他资源
	m_checkboxCache.Cleanup();
	m_textCache.Clear();

	if (g_settings.opts().closeClearBreakPoint) {
		CleanupBreakpoints();
	}
}