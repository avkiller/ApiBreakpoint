#pragma once

#include <Windows.h>
#include <codecvt>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

namespace scl
{

    std::wstring GetModuleFileNameW(HMODULE hModule = nullptr);

    bool FileExistsW(const wchar_t *wszPath);

    std::vector<std::wstring> IniLoadSectionNames(const wchar_t *file);

    std::wstring IniLoadString(const wchar_t *file, const wchar_t *section, const wchar_t *key, const wchar_t *default_value);

    bool IniSaveString(const wchar_t *file, const wchar_t *section, const wchar_t *key, const wchar_t *value);

    template<int BASE = 10, typename VALUE_TYPE>
    VALUE_TYPE IniLoadNum(const wchar_t *file, const wchar_t *section, const wchar_t *key, VALUE_TYPE default_value)
    {
        static_assert((BASE == 8) || (BASE == 10) || (BASE == 16), "invalid base");

        std::wstringstream ss;

        if (BASE == 8)
            ss << std::oct;
        else if (BASE == 16)
            ss << std::hex;

        std::wstring default_value_str;
        ss << default_value;
        ss >> default_value_str;
        ss.str(L"");
        ss.clear();

        VALUE_TYPE value;
        ss << IniLoadString(file, section, key, default_value_str.c_str());
        ss >> value;
        return value;
    }

    template<int BASE = 10, typename VALUE_TYPE>
    bool IniSaveNum(const wchar_t *file, const wchar_t *section, const wchar_t *key, VALUE_TYPE value)
    {
        static_assert((BASE == 8) || (BASE == 10) || (BASE == 16), "invalid base");

        std::wstringstream ss;

        if (BASE == 8)
            ss << std::oct;
        else if (BASE == 16)
            ss << std::hex;

        ss << value;

        return IniSaveString(file, section, key, ss.str().c_str());
    }

    std::string wstring_to_utf8(const std::wstring& wstr);
    std::wstring utf8_to_wstring(const std::string& str);
    std::string wstring_to_utf8(const std::wstring& wstr);
    std::wstring utf8_to_wstring(const std::string& str);

    std::wstring GetDlgItemTextW(HWND hDlg, int nIDDlgItem);
    std::wstring fmtw(const wchar_t* fmt, ...);
    std::wstring vfmtw(const wchar_t* fmt, va_list ap);
};
