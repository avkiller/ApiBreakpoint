#pragma once
#include <vector>
#include <unordered_map>
#include <list>
#include <future>
#include <algorithm>

#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <ShellScalingApi.h>

#include "ApiGroup.h"
#include "CheckboxCache.h"
#include "plugin.h"

#pragma comment(lib, "comctl32.lib")


class ApiBreakpointManager {
public:
    struct DpiState {
        UINT current = 96;
        float scaling = 1.0f;
        HFONT font = nullptr;
        HFONT tabfont = nullptr;
    };

    using DpiInfo = DpiState;

    static ApiBreakpointManager& GetInstance();

    bool IsWindowCreated() const {
        return m_hMainWnd && ::IsWindow(m_hMainWnd); // 验证窗口句柄有效性
    }

    // 获取当前DPI缩放因子
    float GetScalingFactor() const { return m_dpi.scaling; }

    // 获取当前DPI值
    UINT GetApiWinDPI() const { return m_dpi.current; }

    // 获取主字体句柄（只读）
    HFONT GetMainFont() const { return m_dpi.font; }

    // 获取DPI完整状态（按值返回防止外部修改）
    DpiInfo GetDpiState() const;
    void ShowMainWindow();
    void Cleanup();

private:
    ApiBreakpointManager();
    ~ApiBreakpointManager();

    // 禁用拷贝和移动
    ApiBreakpointManager(const ApiBreakpointManager&) = delete;
    ApiBreakpointManager& operator=(const ApiBreakpointManager&) = delete;

    struct TabContext {
        std::vector<HWND> checkboxes;
    };

    // 窗口消息处理
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK TooltipProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
        UINT_PTR uIdSubclass, DWORD_PTR dwRefData
    );
    LRESULT HandleMsgTooltip(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,UINT_PTR uIdSubclass, DWORD_PTR dwRefData );

    // 功能方法
    void Initialize();
    void InitTooltip();
    void CreateTabControl();
    void CreateTabs();
    void CreateTabContent(int tabIndex);
    void UpdateCheckboxes(int tabIndex);
    void AdjustLayout();
    void CenterWindow();
    void HandleButtonClick(HWND hButton);
    void ToggleBreakpoint(HWND hButton, ApiBreakPointInfo& apiInfo);
    //void HandleScroll(HWND hWnd, WPARAM wParam);

    void RecreateFonts();
    void UpdateDPICache(UINT newDPI);
    void HandleDpiChange();
    LRESULT HandleNotify(NMHDR* pHdr);
    void OnTabChanged();
    void SwitchTabContent(int oldTabIndex, int newTabIndex);
    void DrawCheckbox(HWND hWnd, LPDRAWITEMSTRUCT pDrawItem);

    void CleanupBreakpoints();

    // 窗口相关
    HWND m_hMainWnd = nullptr;
    HWND m_hTabCtrl = nullptr;
    HWND m_hTooltip = nullptr;
    int m_currentTab = 0;
    int m_currentScrollPos = 0;
    int m_totalContentHeight = 0;
    DpiState m_dpi;
    UINT GetCurrentDPI();

    //缓存部分值
    int m_cachedDPI;
    int m_cachedTabHeight;
    int m_cachedMargin;
    int m_cachedCheckHeight;

    struct TruncatedCheckboxInfo {
        RECT rcTruncated{0, 0, 0, 0};       // 截断区域坐标
        std::wstring fullText;  // 完整描述文本
    };

    // 资源管理
    std::vector<TabContext> m_tabChecks;
    std::unordered_map<HWND, std::pair<int, int>> m_checkboxMap;// <HWND, <tabIndex, itemIndex>>
    std::map<HWND, TruncatedCheckboxInfo> m_TruncatedInfos;
    CheckboxCache m_checkboxCache;
    TextLayoutCache m_textCache;

    //std::vector<std::future<void>> m_asyncFutures;
};

