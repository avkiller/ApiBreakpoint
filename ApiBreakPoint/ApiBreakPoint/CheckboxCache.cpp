#include "CheckboxCache.h"
#include "plugin.h"
#include "ApiManageWin.h"
std::unordered_map<CacheKey, SIZE> g_textSizeCache;

void CheckboxCache::UpdateDpiResources(int currentDpi) {
    if (dpi == currentDpi) return;

    dpi = currentDpi;
    checkSize = MulDiv(16, dpi, 96);
    margin = MulDiv(4, dpi, 96);
    penWidth = MulDiv(2, dpi, 96);

    checkPoints[0] = { MulDiv(3, dpi, 96), MulDiv(7, dpi, 96) };
    checkPoints[1] = { MulDiv(7, dpi, 96), MulDiv(11, dpi, 96) };
    checkPoints[2] = { MulDiv(13, dpi, 96), MulDiv(3, dpi, 96) };

    currentFont = nullptr; // 触发字体更新
}

void CheckboxCache::UpdateBrush(COLORREF newColor) {
    if (bgColor == newColor && bgBrush) return;

    if (bgBrush) {
        DeleteObject(bgBrush);
        bgBrush = nullptr;
    }
    bgBrush = CreateSolidBrush(newColor);
    bgColor = newColor;
}

void CheckboxCache::UpdateFontMetrics(HDC hdc) {
    HFONT hFont = reinterpret_cast<HFONT>(GetCurrentObject(hdc, OBJ_FONT));
    if (hFont != currentFont) {
        GetTextMetrics(hdc, &textMetric);
        fontHeight = textMetric.tmHeight;
        charWidth_ = textMetric.tmAveCharWidth; // 等宽字体的平均字符宽度
        charWidth_ = MulDiv(charWidth_, dpi, 96);
        isMonospace = (textMetric.tmPitchAndFamily & TMPF_FIXED_PITCH) == 0;
        currentFont = hFont;
    }
}

void CheckboxCache::Cleanup() {
    if (bgBrush) DeleteObject(bgBrush);
    if (checkPen) DeleteObject(checkPen);
    memset(this, 0, sizeof(*this));
}

int CheckboxCache::CalculateTextWidth(HDC hdc, HFONT font, const std::wstring& text)
{
    HDC hMemDC = CreateCompatibleDC(NULL);
    HBITMAP hBmp = CreateCompatibleBitmap(hMemDC, 1024, 100);
    SelectObject(hMemDC, hBmp);
    SelectObject(hMemDC, font); // 显式选择字体

    // 测量文本
    RECT rect = { 0, 0, 0, 0 };
    DrawText(hMemDC, text.c_str(), -1, &rect, DT_CALCRECT);
    DeleteObject(hBmp);
    DeleteDC(hMemDC);
    return rect.right;
}


// CacheKey 的 operator== 实现
bool CacheKey::operator==(const CacheKey& other) const {
    return text == other.text && dpiX == other.dpiX && Width == other.Width;
}

// std::hash 特化实现
namespace std {
    size_t hash<CacheKey>::operator()(const CacheKey& key) const {
        return hash<wstring>()(key.text) ^hash<int>()(key.dpiX)^hash<int>()(key.Width);
    }
}


// TextLayoutCache 成员函数实现
TextLayoutCache::TextLayoutCache(size_t maxSize) : m_maxSize(maxSize) {}

CacheValue TextLayoutCache::GetTextLayout(HDC hdc, const std::wstring& text, int Width=0) {

    auto& manager = ApiBreakpointManager::GetInstance();
    const int m_dpi = manager.GetApiWinDPI();
    const int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
   
    CacheKey key{ text, dpiX, Width};

    // LRU 缓存查询
    auto it = g_cache.find(key);
    if (it != g_cache.end()) {
        m_lruList.remove(key);
        m_lruList.push_front(key);
        return it->second;
    }


    // 缓存未命中，计算 RECT 和 SIZE
    CacheValue result{};
    RECT rc = { 0, 0, Width, 0 }; // 初始宽度为 maxWidth，高度自动扩展
    DrawTextW(hdc, text.c_str(), -1, &rc, DT_CALCRECT|DT_EXPANDTABS);

    // 转换为物理像素（若需要）
    if (dpiX!= m_dpi) {
        result.calcRect = {
       MulDiv(rc.left,   dpiX, 96),
       MulDiv(rc.top,    dpiX, 96),
       MulDiv(rc.right,  dpiX, 96),
       MulDiv(rc.bottom, dpiX, 96)
        };
    }
    else {
        result.calcRect = { rc.left,rc.top, rc.right, rc.bottom};
    }
   
    result.size = {
        result.calcRect.right - result.calcRect.left,
        result.calcRect.bottom - result.calcRect.top
    };


    // 更新缓存
    if (g_cache.size() >= m_maxSize) {
        g_cache.erase(m_lruList.back());
        m_lruList.pop_back();
    }
    g_cache[key] = result;
    m_lruList.push_front(key);

    return result;
}

void TextLayoutCache::Clear() {
    g_cache.clear();
    m_lruList.clear();
}