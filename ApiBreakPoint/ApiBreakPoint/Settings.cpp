#include "Settings.h"
#include <algorithm>

#include "Util.h"

#define API_BREAkPOINT_SETTINGS_SECTION                L"SETTINGS"
#define API_BREAkPOINT_SETTINGS_CURRENT_PROFILE_KEY    L"CurrentProfile"
#define API_BREAkPOINT_SETTINGS_DEFAULT_PROFILE        L"default"

const wchar_t scl::Settings::kFileName[] = L"ApiBreakPoint.ini";

void scl::Settings::Load(const wchar_t *ini_path)
{
    ini_path_ = ini_path;
    profile_names_ = IniLoadSectionNames(ini_path);
    profile_names_.erase(std::remove(profile_names_.begin(), profile_names_.end(), API_BREAkPOINT_SETTINGS_SECTION), profile_names_.end());

    profile_name_ = IniLoadString(ini_path, API_BREAkPOINT_SETTINGS_SECTION, API_BREAkPOINT_SETTINGS_CURRENT_PROFILE_KEY, API_BREAkPOINT_SETTINGS_DEFAULT_PROFILE);
    LoadProfile(ini_path_.c_str(), profile_name_.c_str(), &profile_);
}

bool scl::Settings::Save() const
{
    if (!FileExistsW(ini_path_.c_str()))
    {
        WORD wBOM = 0xFEFF; // UTF16-LE
        DWORD NumberOfBytesWritten;

        auto hFile = CreateFileW(ini_path_.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (!hFile)
            return false;
        WriteFile(hFile, &wBOM, sizeof(WORD), &NumberOfBytesWritten, nullptr);
        CloseHandle(hFile);
    }

    return SaveProfile(ini_path_.c_str(), profile_name_.c_str(), &profile_);
}

bool scl::Settings::AddProfile(const wchar_t *name)
{
    if (std::find(profile_names_.begin(), profile_names_.end(), name) != profile_names_.end())
        return false;

    profile_names_.push_back(name);
    return true;
}

void scl::Settings::SetProfile(const wchar_t *name)
{
    if (profile_name_ == name)
        return;

    profile_name_ = name;
    IniSaveString(ini_path_.c_str(), API_BREAkPOINT_SETTINGS_SECTION, API_BREAkPOINT_SETTINGS_CURRENT_PROFILE_KEY, name);

    LoadProfile(ini_path_.c_str(), name, &profile_);
}


void scl::Settings::LoadProfile(const wchar_t *file, const wchar_t *name, Profile *profile)
{
    profile->closeClearBreakPoint = IniLoadNum(file, name, L"closeClearBreakPoint", 1);
    profile->dllReloadBreakPoint = IniLoadNum(file, name, L"dllReloadBreakPoint", 0);
    profile->apiGroupFileName = IniLoadString(file, name, L"apiGroupFileName", L"api_breakpoint_cn.json");
}

bool scl::Settings::SaveProfile(const wchar_t *file, const wchar_t *name, const Profile *profile)
{
    auto success = true;

    success &= IniSaveNum(file, name, L"closeClearBreakPoint", profile->closeClearBreakPoint);
    success &= IniSaveNum(file, name, L"dllReloadBreakPoint", profile->dllReloadBreakPoint);
    success &= IniSaveString(file, name, L"apiGroupFileName", profile->apiGroupFileName.c_str());

    return success;
}
