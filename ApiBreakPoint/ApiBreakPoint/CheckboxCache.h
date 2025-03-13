#pragma once
#include <windows.h>
#include <string>
#include <unordered_map>

struct CheckboxCache {
    // DPI 相关
    int dpi = 0;
    int checkSize = 0;
    int fontHeight = 16;
    int margin = 0;
    int penWidth = 0;
    int charWidth_ = 0;
    bool isMonospace = false;
    POINT checkPoints[3] = {0};

    // GDI 资源
    HBRUSH bgBrush = nullptr;
    COLORREF bgColor = CLR_INVALID;
    HPEN checkPen = nullptr;

    // 字体度量
    TEXTMETRIC textMetric = { 0 };
    HFONT currentFont = nullptr;

    // 状态跟踪
    COLORREF lastTextColor = CLR_INVALID;
    int lastBkMode = -1;

    void UpdateDpiResources(int currentDpi);
    void UpdateBrush(COLORREF newColor);
    void UpdateFontMetrics(HDC hdc);
    void Cleanup();
    int CalculateTextWidth(HDC hdc, const std::wstring& text);
};

struct CacheKey {
    std::wstring text;
    HFONT font;
    int dpiX;
    UINT format;
    int maxWidth;

    bool operator==(const CacheKey& other) const;
};

struct CacheValue {
    RECT calcRect;  // 计算后的矩形（包含 left/right/top/bottom）
    SIZE size;      // 等效尺寸（cx = right - left, cy = bottom - top）
};


namespace std {
    template<> struct hash<CacheKey> {
        size_t operator()(const CacheKey& key) const;
    };
}


class TextLayoutCache {
private:
    std::unordered_map<CacheKey, CacheValue> g_cache;
    std::list<CacheKey> m_lruList;
    const size_t m_maxSize;

public:
    explicit TextLayoutCache(size_t maxSize = 1000);
    CacheValue GetTextLayout(HDC hdc, const std::wstring& text, HFONT font, UINT format, int maxWidth);
    void Clear();
};