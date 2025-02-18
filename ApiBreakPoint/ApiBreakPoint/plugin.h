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

// 全局DPI状态
struct DpiState {
	int current = 96;
	float scaling = 1.0f;
	HFONT font = nullptr;
	HFONT tabfont = nullptr;
};
extern DpiState g_dpi;

const UINT IDC_TABCTRL = __COUNTER__ + 1500;



#define TAB_COUNT 15
constexpr auto CHECK_COUNT = (25*3);

struct ApiBreakPointInfo
{
	std::wstring dllName;
	std::wstring apiName;
	std::wstring description;
	bool bWantToSetBp; // means weather user want bp set or not. true doesn't means bp really set, perhaps int3 erased or dll not loaded yet
	bool bBpSet;        // 实际断点是否设置成功
	bool bCmdSuccess; //BP执行是否成功

	ApiBreakPointInfo(
		std::wstring _dllName, 
		std::wstring _apiName, 
		std::wstring _description
	): 
		dllName(_dllName), 
		apiName(_apiName),
		description(_description), 
		bWantToSetBp(false), 
		bBpSet(false),
		bCmdSuccess(false)
		{}
};

struct ApiGroup
{
	std::wstring groupName;
	std::vector<ApiBreakPointInfo> apiList;
};

extern ApiGroup g_Api_Groups[TAB_COUNT];

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




