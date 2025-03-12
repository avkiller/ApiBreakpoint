#pragma once





DWORD WINAPI MsgLoopThread(LPVOID);
LRESULT CALLBACK ApiBreakPointManageWndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

void DrawCheckbox(HWND hButton, LPDRAWITEMSTRUCT pDrawItem, const ApiBreakPointInfo& apiInfo);

void CreateTabs(TCITEMW& ti, HWND& hwnd);

void UpdateCheckBoxs();
void RecreateFont();
void CleanupBreakpoints();
void AdjustLayout(HWND);
extern bool bIsMainWindowShow;
extern size_t g_curTabmaxApiNameLength;
extern size_t g_curTabmaxDllNameLength;