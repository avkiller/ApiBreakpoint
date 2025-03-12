#pragma once
//#define TAB_COUNT 15
//constexpr auto CHECK_COUNT = (25*3);
#include <string>
#include <vector>
#include <windows.h>
#include <mutex> 

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
    //size_t maxApiNameLength = 0;  // 直接存储最大长度
    //size_t maxDllNameLength = 0;  // 直接存储最大长度
    // 新增构造函数：仅接受 groupName
    //ApiGroup(const std::wstring& name) : groupName(name) {}

    //// 保留原有构造函数（如果其他地方用到）
    //ApiGroup(const std::wstring& name, std::initializer_list<ApiBreakPointInfo> apis)
    //    : groupName(name), apiList(apis)
    //{
    //    UpdateMaxApiNameLength(); // 计算最大长度
    //}

    // 新增方法：动态更新最大长度
   /* void UpdateMaxApiNameLength()
    {
        maxApiNameLength = 0;
        maxDllNameLength = 0;
        for (const auto& api : apiList) {
            maxApiNameLength = max(maxApiNameLength, api.apiName.length());
            maxDllNameLength = max(maxDllNameLength, api.dllName.length());
        }
    }*/
};

extern std::vector<ApiGroup> g_Api_Groups;

bool LoadApiGroupsFromJson(
    const std::wstring& filename,
    std::vector<ApiGroup>& apiGroups
);

bool SaveDefaultApiGroupsToJson(const std::wstring& filename);

void SetDefaultApiGroups();
extern std::vector<ApiGroup> DefaultApiGroups;

extern std::mutex g_ApiGroupsMutex;

bool ReloadApiGroupsFromJson(const std::wstring& filename);