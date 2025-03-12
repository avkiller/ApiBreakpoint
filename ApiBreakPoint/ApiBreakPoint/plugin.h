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

#define IDC_TABCTRL 1500
#define IDC_CHECK_FIRST 1500
#define MAX_CHECKS_PER_TAB 100

//functions
bool pluginInit(PLUG_INITSTRUCT* initStruct);
void pluginStop();
void pluginSetup();


#define MAINWINDOW_WIDTH 800
#define MAINWINDOW_HEIGHT 600
#define CHECKBOX_COLUMNS         3
#define TAB_HEIGHT               35
#define CHECKBOX_HEIGHT          25
#define MARGIN_SIZE              10

// È«¾ÖDPI×´Ì¬
struct DpiState {
	int current = 96;
	float scaling = 1.0f;
	HFONT font = nullptr;
	HFONT tabfont = nullptr;
};
extern DpiState g_dpi;

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

