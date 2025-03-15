#pragma once
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <list>
#include <future>
#include "ApiGroup.h"
#include "CheckboxCache.h"

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

    // 功能方法
    void Initialize();
    void CreateTabControl();
    void CreateTabs();
    void CreateTabContent(int tabIndex);
    void UpdateCheckboxes(int tabIndex);
    void AdjustLayout();
    void CenterWindow();
    void HandleButtonClick(HWND hButton);
    void ToggleBreakpoint(HWND hButton, ApiBreakPointInfo& apiInfo);

    void RecreateFonts();
    void HandleDpiChange();
    LRESULT HandleNotify(NMHDR* pHdr);
    void OnTabChanged();
    void SwitchTabContent(int oldTabIndex, int newTabIndex);
    void DrawCheckbox(HWND hWnd, LPDRAWITEMSTRUCT pDrawItem);

    void CleanupBreakpoints();

    // 窗口相关
    HWND m_hMainWnd = nullptr;
    HWND m_hTabCtrl = nullptr;
    int m_currentTab = 0;
    DpiState m_dpi;
    UINT GetCurrentDPI();

    // 资源管理
    std::vector<TabContext> m_tabChecks;
    std::unordered_map<HWND, std::pair<int, int>> m_checkboxMap; // <HWND, <tabIndex, itemIndex>>
    CheckboxCache m_checkboxCache;
    TextLayoutCache m_textCache;

    //std::vector<std::future<void>> m_asyncFutures;
};

