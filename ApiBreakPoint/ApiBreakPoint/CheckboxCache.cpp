#include "CheckboxCache.h"
#include "plugin.h"
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

int CheckboxCache::CalculateTextWidth(HDC hdc, const std::wstring& text)
{
    RECT rc = { 0 };
    DrawTextW(hdc, text.c_str(), -1, &rc, DT_CALCRECT | DT_SINGLELINE);
    return rc.right - rc.left;
}


// CacheKey 的 operator== 实现
bool CacheKey::operator==(const CacheKey& other) const {
    return text == other.text && font == other.font &&
        dpiX == other.dpiX && format == other.format &&
        maxWidth == other.maxWidth;
}

// std::hash 特化实现
namespace std {
    size_t hash<CacheKey>::operator()(const CacheKey& key) const {
        return hash<wstring>()(key.text) ^ hash<HFONT>()(key.font) ^
            hash<int>()(key.dpiX) ^ hash<UINT>()(key.format) ^
            hash<int>()(key.maxWidth);
    }
}


// TextLayoutCache 成员函数实现
TextLayoutCache::TextLayoutCache(size_t maxSize) : m_maxSize(maxSize) {}

CacheValue TextLayoutCache::GetTextLayout(HDC hdc, const std::wstring& text, HFONT font, UINT format, int maxWidth) {
    const int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    CacheKey key{ text, font, dpiX, format, maxWidth };

    // LRU 缓存查询
    auto it = g_cache.find(key);
    if (it != g_cache.end()) {
        m_lruList.remove(key);
        m_lruList.push_front(key);
        return it->second;
    }


    // 缓存未命中，计算 RECT 和 SIZE
    CacheValue result{};
    RECT rc = { 0, 0, maxWidth, 0 }; // 初始宽度为 maxWidth，高度自动扩展
    DrawTextW(hdc, text.c_str(), -1, &rc, format | DT_CALCRECT);

    // 转换为物理像素（若需要）
    if (dpiX!= g_dpi.current) {
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