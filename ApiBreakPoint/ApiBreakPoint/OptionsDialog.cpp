#include "OptionsDialog.h"
#include <CommCtrl.h>
#include <codecvt>
#include <locale>
#include "Settings.h"
#include "Util.h"
#include "pluginsdk/bridgemain.h"
#include "resource.h"
#include "plugin.h"
#include "ApiGroup.h"

extern DWORD ProcessId;
extern bool bHooked;

static void UpdateOptions(HWND hWnd, const scl::Settings *settings)
{
    auto opts = &settings->opts();

    CheckDlgButton(hWnd, IDC_CLOSE_CLEAR_APIBREAKPOINT, opts->closeClearBreakPoint);
    CheckDlgButton(hWnd, IDC_RELOAD_BREAKPOINT_DLL, opts->dllReloadBreakPoint);
    SetDlgItemTextW(hWnd, IDC_API_CONFIG_FILE_NAME, opts->apiGroupFileName.c_str());
}

void SaveOptions(HWND hWnd, scl::Settings *settings)
{
    auto opts = &settings->opts();

    opts->closeClearBreakPoint = (IsDlgButtonChecked(hWnd, IDC_CLOSE_CLEAR_APIBREAKPOINT) == BST_CHECKED);
    opts->dllReloadBreakPoint = (IsDlgButtonChecked(hWnd, IDC_RELOAD_BREAKPOINT_DLL) == BST_CHECKED);
    opts->apiGroupFileName = scl::GetDlgItemTextW(hWnd, IDC_API_CONFIG_FILE_NAME);
    settings->Save();
}


//options dialog proc
INT_PTR CALLBACK OptionsDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        // add current profile to options title
        auto wstrTitle = scl::fmtw(L"[APiBreakPoint Options] Profile: %s", g_settings.profile_name().c_str());
        SetWindowTextW(hDlg, wstrTitle.c_str());

        // fill combobox with profiles
        for (size_t i = 0; i < g_settings.profile_names().size(); i++)
        {
            SendDlgItemMessageW(hDlg, IDC_PROFILES, CB_ADDSTRING, 0, (LPARAM)g_settings.profile_names()[i].c_str());
            if (g_settings.profile_name() == g_settings.profile_names()[i])
                SendDlgItemMessageW(hDlg, IDC_PROFILES, CB_SETCURSEL, i, 0);
        }

        SendDlgItemMessageW(hDlg, IDC_RELOAD_APICONFIG, CB_SETCURSEL, 0, 0);

        UpdateOptions(hDlg, &g_settings);

        break;
    }
    case WM_CLOSE:
    {
        EndDialog(hDlg, NULL);
    }
    break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_PROFILES:
        {
            if (HIWORD(wParam) != CBN_SELCHANGE)
                break;

            auto profileIdx = (int)SendDlgItemMessageW(hDlg, IDC_PROFILES, CB_GETCURSEL, 0, 0);
            g_settings.SetProfile(g_settings.profile_names()[profileIdx].c_str());

            // update options title
            auto wstrTitle = scl::fmtw(L"[ApiBreakPoint Options] Profile: %s", g_settings.profile_name().c_str());
            SetWindowTextW(hDlg, wstrTitle.c_str());

            UpdateOptions(hDlg, &g_settings);
            break;
        }

        case IDC_SAVEPROFILE:
        {
            if (HIWORD(wParam) != BN_CLICKED)
                break;

            std::wstring wstrNewProfileName;

            std::string strNewProfileName;
            strNewProfileName.resize(GUI_MAX_LINE_SIZE);
            if (!GuiGetLineWindow("New profile name?", &strNewProfileName[0]))
                break;
            wstrNewProfileName = scl::Utf8ToWide(strNewProfileName);

            if (!g_settings.AddProfile(wstrNewProfileName.c_str()))
                break;
            g_settings.SetProfile(wstrNewProfileName.c_str());

            auto wstrTitle = scl::fmtw(L"[ApiBreakPoint Options] Profile: %s", g_settings.profile_name().c_str());
            SetWindowTextW(hDlg, wstrTitle.c_str());

            SendDlgItemMessageW(hDlg, IDC_PROFILES, CB_ADDSTRING, 0, (LPARAM)wstrNewProfileName.c_str());
            auto profileCount = (int)SendDlgItemMessageW(hDlg, IDC_PROFILES, CB_GETCOUNT, 0, 0);
            SendDlgItemMessageW(hDlg, IDC_PROFILES, CB_SETCURSEL, profileCount - 1, 0);

            UpdateOptions(hDlg, &g_settings);
            break;
        }

        case IDOK:
        {
            if (HIWORD(wParam) != BN_CLICKED)
                break;

            SaveOptions(hDlg, &g_settings);
            EndDialog(hDlg, NULL);
            break;
        }

        case IDC_APPLY:
        {
            if (HIWORD(wParam) != BN_CLICKED)
                break;

            SaveOptions(hDlg, &g_settings);
            break;
        }

       /* case IDC_RELOAD_APICONFIG:
        {
            if (HIWORD(wParam) != BN_CLICKED)
                break;

            ReloadApiGroupsFromJson(g_api_BreakPointConfigPath);
            break;
        }*/

        default:
            break;
        }

    }
    break;

    default:
    {
        return FALSE;
    }
    }

    return 0;
}
