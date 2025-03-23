#pragma once

#include <Windows.h>
#include <string>
#include <vector>



namespace scl
{
    class Settings
    {
    public:
        struct Profile
        {
            BOOL closeClearBreakPoint;
            BOOL dllReloadBreakPoint;
            std::wstring apiGroupFileName;
            std::wstring uiFontName;
        };

        static const wchar_t kFileName[];

        void Load(const wchar_t *ini_file);
        bool Save() const;

        bool AddProfile(const wchar_t *name);
        void SetProfile(const wchar_t *name);


        const std::vector<std::wstring> &profile_names() const
        {
            return profile_names_;
        }

        const std::wstring &profile_name() const
        {
            return profile_name_;
        }

        const Profile &opts() const
        {
            return profile_;
        }

        Profile &opts()
        {
            return profile_;
        }

    protected:
        static void LoadProfile(const wchar_t *file, const wchar_t *name, Profile *profile);
        static bool SaveProfile(const wchar_t *file, const wchar_t *name, const Profile *profile);

    private:
        std::wstring ini_path_;
        std::vector<std::wstring> profile_names_;
        std::wstring profile_name_;
        Profile profile_{};
    };
}


extern scl::Settings g_settings;