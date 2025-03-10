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

// ��̬���� IDC_CHECKS
std::vector<UINT> GenerateIDC_CHECKS(size_t checkCount) {
	std::vector<UINT> idcChecks(checkCount);
	for (UINT i = 0; i < checkCount; i++) {
		idcChecks[i] = __COUNTER__ + 1500 + i; // ��̬���� ID
	}
	return idcChecks;
}
//
//
//
//// ��ʼ����̬ID
//void InitializeDynamicCheckIDs() {
//	g_tabCheckIDs.clear();
//	for (int tabIdx = 0; tabIdx < g_Api_Groups.size(); ++tabIdx) {
//		std::vector<UINT> tabIDs;
//		for (int chkIdx = 0; chkIdx < g_Api_Groups[tabIdx].apiList.size(); ++chkIdx) {
//			tabIDs.push_back(__COUNTER__ + 1500 + tabIdx * 100);  // ��̬����ID������ǩҳƫ��
//		}
//		g_tabCheckIDs.push_back(tabIDs);
//	}
//}

// JSON ���غ���
bool LoadApiGroupsFromJson(const std::wstring& filename, std::vector<ApiGroup>& apiGroups) {
	std::ifstream file{ std::filesystem::path(filename) };
	if (!file.is_open()) return false;

	try {
		json j;
		file >> j;

		for (const auto& groupJson : j) {
			ApiGroup group;
			group.groupName = scl::Utf8ToWide(groupJson["groupName"].get<std::string>());

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

			apiGroups.push_back(std::move(group));
		}

		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "JSON ����: " << e.what() << std::endl;
		return false;
	}
}

// ���л� ApiBreakPointInfo �� JSON���ֶ�˳���޹ؽ�Ҫ��
void to_json(ordered_json& j, const ApiBreakPointInfo& api) {
	j = ordered_json{
		{"dllName", scl::WideToUtf8(api.dllName)},
		{"apiName", scl::WideToUtf8(api.apiName)},
		{"description", scl::WideToUtf8(api.description)}
	};
}

// ���л� ApiGroup �� JSON�������ֶ�˳��
void to_json(ordered_json& j, const ApiGroup& group) {
	j = ordered_json{  // ʹ�� ordered_json ������ͨ json
		{"groupName", scl::WideToUtf8(group.groupName)},  // ��һ���ֶ�
		{"apiList", group.apiList}                     // �ڶ����ֶ�
	};
}


// ����Ĭ�����õ� JSON �ļ�
bool SaveDefaultApiGroupsToJson(const std::wstring& filename) {
	try {
		ordered_json  j = DefaultApiGroups; // �Զ����� to_json ���л�
		std::ofstream file(filename);
		file << j.dump(2); // ���� 2 �ո�������ʽ
		return true;
	}
	catch (...) {
		return false;
	}
}


bool ReloadApiGroupsFromJson(const std::wstring& filename) {
	std::vector<ApiGroup> new_groups;

	// ���Լ���������
	if (!LoadApiGroupsFromJson(filename, new_groups)) {
		return false;  // ����ʧ�ܣ�����������
	}

	// ��������ȫ�ֱ���
	std::lock_guard<std::mutex> lock(g_ApiGroupsMutex);
	g_Api_Groups = std::move(new_groups);  // ԭ�����滻����

	return true;
}