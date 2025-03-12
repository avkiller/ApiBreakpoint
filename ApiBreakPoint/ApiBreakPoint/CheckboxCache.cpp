#include "CheckboxCache.h"
//#include "ApiGroup.h"
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


SIZE CheckboxCache::GetSmartTextExtent(HDC hdc, const std::wstring& text) {
    // 获取 DPI 和字体类型
    const int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
    const int dpiY = GetDeviceCaps(hdc, LOGPIXELSY);

    // 构造缓存键
    CacheKey key;
    key.font = currentFont;
    key.dpiX = dpiX;
    key.length = text.length();
    if (!isMonospace) {
        key.text = text; // 非等宽字体需存储完整文本
    }

    // 查询缓存
    auto it = g_textSizeCache.find(key);
    if (it != g_textSizeCache.end()) {
        return it->second;
    }

    // 缓存未命中，计算尺寸
    SIZE size = { 0, 0 };
    if (isMonospace) {
        // 等宽优化：字符数 × 平均宽度
        size.cx = static_cast<int>(text.length() * textMetric.tmAveCharWidth);
        size.cy = textMetric.tmHeight;
    }
    else {
        // 非等宽：精确计算
        GetTextExtentPoint32W(hdc, text.c_str(), text.length(), &size);
    }

    // DPI 缩放
    size.cx = MulDiv(size.cx, dpiX, 96);
    size.cy = MulDiv(size.cy, dpiY, 96);

    // 写入缓存
    g_textSizeCache[key] = size;
    return size;
}