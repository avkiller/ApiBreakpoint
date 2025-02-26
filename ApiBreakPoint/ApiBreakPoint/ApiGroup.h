#pragma once
//#define TAB_COUNT 15
//constexpr auto CHECK_COUNT = (25*3);
#include <string>
#include <vector>
#include <windows.h>

//#define TAB_COUNT 15
//#define CHECK_COUNT (25*3)

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
	) :
		dllName(_dllName),
		apiName(_apiName),
		description(_description),
		bWantToSetBp(false),
		bBpSet(false),
		bCmdSuccess(false)
	{
	}
};

struct ApiGroup
{
	std::wstring groupName;
	std::vector<ApiBreakPointInfo> apiList;
};

extern std::vector<ApiGroup> g_Api_Groups;