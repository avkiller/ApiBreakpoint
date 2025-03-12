#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <iterator>
#include <Windows.h>

#include "json.hpp"
#include "ApiGroup.h"
#include "Util.h"
#include <filesystem>

using namespace std;
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

std::mutex g_ApiGroupsMutex;

// 动态生成 IDC_CHECKS
std::vector<UINT> GenerateIDC_CHECKS(size_t checkCount) {
	std::vector<UINT> idcChecks(checkCount);
	for (UINT i = 0; i < checkCount; i++) {
		idcChecks[i] = __COUNTER__ + 1500 + i; // 动态生成 ID
	}
	return idcChecks;
}
//
//
//
//// 初始化动态ID
//void InitializeDynamicCheckIDs() {
//	g_tabCheckIDs.clear();
//	for (int tabIdx = 0; tabIdx < g_Api_Groups.size(); ++tabIdx) {
//		std::vector<UINT> tabIDs;
//		for (int chkIdx = 0; chkIdx < g_Api_Groups[tabIdx].apiList.size(); ++chkIdx) {
//			tabIDs.push_back(__COUNTER__ + 1500 + tabIdx * 100);  // 动态生成ID，按标签页偏移
//		}
//		g_tabCheckIDs.push_back(tabIDs);
//	}
//}

// JSON 加载函数
bool LoadApiGroupsFromJson(const std::wstring& filename, std::vector<ApiGroup>& apiGroups) {
	std::ifstream file{ std::filesystem::path(filename) };
	if (!file.is_open()) return false;

	try {
		json j;
		file >> j;

		for (const auto& groupJson : j) {
			/*ApiGroup group;
			group.groupName = scl::Utf8ToWide(groupJson["groupName"].get<std::string>());*/
			ApiGroup group(scl::Utf8ToWide(groupJson["groupName"].get<std::string>()));

			for (const auto& apiJson : groupJson["apiList"]) {
				std::string dllName = apiJson.value("dllName", "");
				std::string apiName = apiJson["apiName"].get<std::string>();
				std::string description = apiJson.value("description", "");

				group.apiList.emplace_back(
					scl::Utf8ToWide(dllName),
					scl::Utf8ToWide(apiName),
					scl::Utf8ToWide(description)
				);
			}
			//group.UpdateMaxApiNameLength();
			apiGroups.push_back(std::move(group));
		}

		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "JSON 错误: " << e.what() << std::endl;
		return false;
	}
}

// 序列化 ApiBreakPointInfo 到 JSON（字段顺序无关紧要）
void to_json(ordered_json& j, const ApiBreakPointInfo& api) {
	j = ordered_json{
		{"dllName", scl::WideToUtf8(api.dllName)},
		{"apiName", scl::WideToUtf8(api.apiName)},
		{"description", scl::WideToUtf8(api.description)}
	};
}

// 序列化 ApiGroup 到 JSON（保持字段顺序）
void to_json(ordered_json& j, const ApiGroup& group) {
	j = ordered_json{  // 使用 ordered_json 代替普通 json
		{"groupName", scl::WideToUtf8(group.groupName)},  // 第一个字段
		{"apiList", group.apiList}                     // 第二个字段
	};
}


// 保存默认配置到 JSON 文件
bool SaveDefaultApiGroupsToJson(const std::wstring& filename) {
	try {
		ordered_json  j = DefaultApiGroups; // 自动调用 to_json 序列化
		std::ofstream file(filename);
		file << j.dump(2); // 缩进 2 空格美化格式
		return true;
	}
	catch (...) {
		return false;
	}
}


bool ReloadApiGroupsFromJson(const std::wstring& filename) {
	std::vector<ApiGroup> new_groups;

	// 尝试加载新配置
	if (!LoadApiGroupsFromJson(filename, new_groups)) {
		return false;  // 加载失败，保留旧配置
	}

	// 加锁保护全局变量
	std::lock_guard<std::mutex> lock(g_ApiGroupsMutex);
	g_Api_Groups = std::move(new_groups);  // 原子性替换配置

	return true;
}