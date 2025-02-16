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

#define MENU_MAINWINDOW_POPUP 1

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
};
extern DpiState g_dpi;

const UINT IDC_TABCTRL = __COUNTER__ + 1500;



#define TAB_COUNT 15
#define CHECK_COUNT (25*3)

struct ApiBreakPointInfo
{
	std::wstring dllName;
	std::wstring apiName;
	std::wstring description;
	bool bWantToSetBp; // means weather user want bp set or not. true doesn't means bp really set, perhaps int3 erased or dll not loaded yet

	ApiBreakPointInfo(std::wstring _dllName, std::wstring _apiName, std::wstring _description)
		: dllName(_dllName), apiName(_apiName), description(_description), bWantToSetBp(false) {
	}
};

struct ApiGroup
{
	std::wstring groupName;
	std::vector<ApiBreakPointInfo> apiList;
};

// check box control ID
const UINT IDC_CHECKS[CHECK_COUNT] =
{
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
	__COUNTER__ + 1500,
};




