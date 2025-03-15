#pragma once

#include "pluginmain.h"
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <commctrl.h>

#pragma comment(lib,"Comctl32.lib")

using namespace Script::Module;
using namespace Script::Symbol;
using namespace Script::Debug;
using namespace Script::Register;


//functions
bool pluginInit(PLUG_INITSTRUCT* initStruct);
void pluginStop();
void pluginSetup();


enum ApiBreakPointMenuItems : int {
	MENU_OPTIONS = 0,
	MENU_MAINWINDOW_POPUP = 1,
	MENU_CONFIG = 2,
	MENU_PROFILES,
	MENU_MAX
};

extern std::wstring g_PluginDir;
extern std::wstring g_settingIniPath;
extern std::wstring g_api_BreakPointConfigPath;
extern scl::Settings g_settings;

