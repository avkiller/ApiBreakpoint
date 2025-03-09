#include "Util.h"
//#include <cstdio>
#include <stdarg.h>
#include <stdio.h>
std::wstring scl::GetModuleFileNameW(HMODULE hModule)
{
    std::wstring wstrFileName;
    DWORD copied = 0;
    do {
        wstrFileName.resize(wstrFileName.size() + MAX_PATH);
        copied = ::GetModuleFileNameW(hModule, &wstrFileName[0], (DWORD)wstrFileName.size());
    } while (copied >= wstrFileName.size());

    wstrFileName.resize(copied);
    return wstrFileName;
}


bool scl::FileExistsW(const wchar_t *wszPath)
{
    auto dwAttrib = ::GetFileAttributesW(wszPath);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES) && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}


std::vector<std::wstring> scl::IniLoadSectionNames(const wchar_t *file)
{
    std::wstring buf;
    DWORD ret = 0;
    while (((DWORD)buf.size() - ret) < 3) {
        buf.resize(buf.size() + MAX_PATH);
        ret = ::GetPrivateProfileSectionNamesW(&buf[0], (DWORD)buf.size(), file);
    }

    std::vector<std::wstring> sections;
    auto data = buf.c_str();
    for (; data[0]; data += lstrlenW(data) + 1) {
        sections.push_back(data);
    }

    return sections;
}

std::wstring scl::IniLoadString(const wchar_t *file, const wchar_t *section, const wchar_t *key, const wchar_t *default_value)
{
    std::wstring buf;
    DWORD ret = 0;

    while (((DWORD)buf.size() - ret) < 3) {
        buf.resize(buf.size() + MAX_PATH);
        ret = ::GetPrivateProfileStringW(section, key, default_value, &buf[0], (DWORD)buf.size(), file);
    }
    buf.resize(ret);

    return buf;
}

bool scl::IniSaveString(const wchar_t *file, const wchar_t *section, const wchar_t *key, const wchar_t *value)
{
    return WritePrivateProfileStringW(section, key, value, file) == TRUE;
}

std::string scl::wstring_to_utf8(const std::wstring& wstr)
{
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(len, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], len, nullptr, nullptr);
    return str;
}


std::wstring scl::utf8_to_wstring(const std::string& str) {
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
    return wstr;
}


//// 使用C++11的局部静态变量保证线程安全
//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>& wstr_conv() {
//    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
//    return conv;
//}
//
//// UTF-8 → wstring
//std::wstring utf8_to_wstring(const std::string& str) {
//    return wstr_conv().from_bytes(str);
//}
//
//// wstring → UTF-8
//std::string wstring_to_utf8(const std::wstring& wstr) {
//    return wstr_conv().to_bytes(wstr);
//}

constexpr int MAX_CTRL_TEXT_LENGTH = 1024 * 4;

std::wstring scl::GetDlgItemTextW(HWND hDlg, int nIDDlgItem) {
    // 获取控件句柄并验证类型
    HWND hCtrl = ::GetDlgItem(hDlg, nIDDlgItem);
    DWORD dwStyle = GetWindowLongW(hCtrl, GWL_STYLE);
    if (!hCtrl || !(dwStyle & (ES_AUTOHSCROLL | ES_LEFT | ES_RIGHT))) {
        return L"";
    }

    // 获取文本长度
    int len = GetWindowTextLengthW(hCtrl);
    if (len == 0 || len > MAX_CTRL_TEXT_LENGTH) {
        return L"";
    }

    // 动态分配缓冲区并获取文本
    std::unique_ptr<wchar_t[]> buffer(new wchar_t[len + 1] {L'\0'});
    if (!GetWindowTextW(hCtrl, buffer.get(), len + 1)) {
        return L"";
    }

    // 返回结果
    return std::wstring(buffer.get(), len);
}

std::wstring scl::fmtw(const wchar_t* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    auto str = vfmtw(fmt, ap);
    va_end(ap);

    return str;
}

std::wstring scl::vfmtw(const wchar_t* fmt, va_list ap)
{
    va_list vap;

    va_copy(vap, ap);
    auto size = ::_vsnwprintf_s(nullptr, 0, 0, fmt, vap);
    va_end(vap);

    std::wstring wstr;
    wstr.resize(size);

    va_copy(vap, ap);
    ::_vsnwprintf_s(&wstr[0], static_cast<size_t>(wstr.size() - 1), 0, fmt, vap);
    va_end(vap);

    return wstr;
}