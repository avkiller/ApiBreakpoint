#include "Util.h"
#include <Windows.h>
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

// 封装宽字符转 UTF-8
std::string WideToUTF8(const std::wstring& wstr)
{
    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
    return str;
}

std::string scl::WideToUtf8(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();

    // 计算所需UTF-8字节数
    int requiredSize = WideCharToMultiByte(
        CP_UTF8, 
        0,
        wstr.c_str(), 
        static_cast<int>(wstr.size()),
        nullptr,
        0,
        nullptr, 
        nullptr);
    if (requiredSize == 0) {
        throw std::runtime_error("WideCharToMultiByte failed to calculate size.");
    }

    // 分配内存并转换
    std::string str(requiredSize, '\0');
    int result = WideCharToMultiByte(
        CP_UTF8, 
        0,
        wstr.c_str(), 
        static_cast<int>(wstr.size()),
        &str[0], 
        requiredSize, 
        nullptr,
        nullptr);
    if (result != requiredSize) {
        throw std::runtime_error("WideCharToMultiByte conversion failed.");
    }
    str.resize(result);
    return str;
}


std::wstring scl::Utf8ToWide(const std::string& str) {
    if (str.empty()) 
        return L"";
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);
    return wstr;
}

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