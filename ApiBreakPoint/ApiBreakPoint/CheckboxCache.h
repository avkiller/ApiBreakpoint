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
    int charWidth_;
    bool isMonospace;
    POINT checkPoints[3];

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
    SIZE GetSmartTextExtent(HDC hdc, const std::wstring& text);
};

struct CacheKey {
    std::wstring text;   // 文本内容（等宽字体时可为空）
    HFONT font;           // 字体句柄
    int dpiX;            // 水平DPI
    size_t length;       // 文本长度（等宽字体时使用）

    bool operator==(const CacheKey& other) const {
        return text == other.text && font == other.font &&
            dpiX == other.dpiX && length == other.length;
    }
};


// 哈希函数特化
namespace std {
    template<> struct hash<CacheKey> {
        size_t operator()(const CacheKey& key) const {
            return hash<std::wstring>()(key.text) ^
                hash<HFONT>()(key.font) ^
                hash<int>()(key.dpiX) ^
                hash<size_t>()(key.length);
        }
    };
}

extern  std::unordered_map<CacheKey, SIZE> g_textSizeCache;