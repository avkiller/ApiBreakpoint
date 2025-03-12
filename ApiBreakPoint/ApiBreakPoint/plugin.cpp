#include <windows.h>
#include <ShellScalingApi.h>

#include "pluginsdk\_plugins.h"
#include "pluginsdk\bridgemain.h"

#include "plugin.h"
#include "ApiGroup.h"
#include "util.h"
#include "OptionsDialog.h"
#include "ApiManageWin.h"

#include "resource.h"

//#ifdef _UNICODE
//#error "USE ASCII CODE PAGE"
//#endif

using namespace Script::Module;
using namespace Script::Symbol;
using namespace Script::Debug;
using namespace Script::Register;


std::vector<ApiGroup> g_Api_Groups;
std::wstring g_PluginDir;
std::wstring g_settingIniPath;
std::wstring g_api_BreakPointConfigPath;
scl::Settings g_settings;

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
	HRESULT hr = SetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE);
	if (FAILED(hr))
		_plugin_logprintf("SetProcessDpiAwareness failed with error\n");
	return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{}

//Do GUI/Menu related things here.
void pluginSetup()
{
	g_settings.Load(g_settingIniPath.c_str());

	std::wstring s_Api_BreakPoint_FileName = g_settings.opts().apiGroupFileName;
	if (!s_Api_BreakPoint_FileName.empty()) {
		g_api_BreakPointConfigPath = g_PluginDir + s_Api_BreakPoint_FileName;
		if (!LoadApiGroupsFromJson(g_api_BreakPointConfigPath, g_Api_Groups)) {
			SetDefaultApiGroups();
			//SaveDefaultApiGroupsToJson(L"default_api_groups.json");
		}

	}
	_plugin_menuaddentry(hMenu, MENU_MAINWINDOW_POPUP, "&ApiBreakpoint Manage");
	_plugin_menuaddentry(hMenu, MENU_OPTIONS, "&Options");

	int hApiConfig = _plugin_menuadd(hMenu, "&API Config");
	_plugin_menuaddentry(hApiConfig, MENU_CONFIG, "&Reload API CONFIG");

	int hProfile = _plugin_menuadd(hMenu, "&Load Profile");

	//add profiles to menu
	for (size_t i = 0; i < g_settings.profile_names().size(); i++)
	{
		std::wstring profile = g_settings.profile_names()[i];
		auto mbstrName = scl::WideToUtf8(profile);
		_plugin_menuaddentry(hProfile, (int)i + MENU_MAX, mbstrName.c_str());
	}
}


// set delay breakpoint when dll loaded
EXTERN_C __declspec(dllexport) void _cdecl CBLOADDLL(
	CBTYPE bType, // event type, one of the `cbType` CB_* values listed above
	PLUG_CB_LOADDLL* callbackInfo // pointer to a structure of information
)
{
	if (g_settings.opts().dllReloadBreakPoint) {
	//check if there are any breakpoint can be set
		for (size_t i = 0; i < g_Api_Groups.size(); i++)
		{
			for (size_t j = 0; j < g_Api_Groups[i].apiList.size(); j++)
			{
				auto& apiInfo = g_Api_Groups[i].apiList[j];  // 简化引用

				if (apiInfo.bBpSet) {
					_plugin_logprintf("BP Already Set\n");
					std::wstring wcmd = L"bp " + apiInfo.apiName;
					std::string cmd = scl::WideToUtf8(wcmd);
					bool bSuccess = DbgCmdExecDirect(cmd.c_str());
				}

				
			}
		 }
	}
}

// register menu to popup main window
extern "C" __declspec(dllexport) void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
	switch (info->hEntry)
	{
	case MENU_MAINWINDOW_POPUP:
	{
		if (!bIsMainWindowShow && DbgIsDebugging())
		{
			HANDLE hThread = CreateThread(
				NULL,                   // 安全属性
				0,                      // 默认堆栈大小
				MsgLoopThread,          // 线程函数
				NULL,                   // 参数
				0,                      // 创建标志
				NULL                    // 线程ID
			);

			if (hThread != NULL)
			{
				// 线程创建成功，可以关闭句柄（如果不需后续操作）
				CloseHandle(hThread);
				bIsMainWindowShow = true;
			}
			else
			{
				// 处理线程创建失败的情况
				_plugin_logprintf("Failed to create thread %s \n", MB_ICONERROR);
			}
		}
		break;
	}

	case MENU_CONFIG:
	{
		ReloadApiGroupsFromJson(g_api_BreakPointConfigPath);
		break;
	}

	case MENU_OPTIONS:
	{
		DialogBoxW(g_hInstance, MAKEINTRESOURCE(IDD_OPTIONS), hwndDlg, &OptionsDlgProc);
		break;
	}

	default: {
		auto profile_name = g_settings.profile_names()[info->hEntry - MENU_MAX].c_str();
		g_settings.SetProfile(profile_name);
		break;
	}



	}
}



