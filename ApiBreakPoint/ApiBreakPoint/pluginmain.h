﻿#pragma once

// Plugin information
constexpr auto ClASS_NAME = L"ApiBreakpoint";
constexpr auto PLUGIN_NAME = "ApiBreakpoint";
constexpr auto API_BREAKPOINT_VERSION_MAJOR = 2;
constexpr auto API_BREAKPOINT_VERSION_MINOR = 0;
constexpr auto API_BREAKPOINT_VERSION_PATCH = 0;
constexpr size_t PLUGIN_NAME_MAX_LEN = 64;

// 定义日志级别（通过编译选项或配置文件）
#define LOG_LEVEL_DEBUG   0
#define LOG_LEVEL_RELEASE 1
#define LOG_LEVEL_NONE    2

#pragma warning(push)
#pragma warning(disable: 4244)
#include "pluginsdk/bridgemain.h"
#include "pluginsdk/_plugins.h"
#pragma warning(pop)

#include "pluginsdk/_scriptapi_argument.h"
#include "pluginsdk/_scriptapi_assembler.h"
#include "pluginsdk/_scriptapi_bookmark.h"
#include "pluginsdk/_scriptapi_comment.h"
#include "pluginsdk/_scriptapi_debug.h"
#include "pluginsdk/_scriptapi_flag.h"
#include "pluginsdk/_scriptapi_function.h"
#include "pluginsdk/_scriptapi_gui.h"
#include "pluginsdk/_scriptapi_label.h"
#include "pluginsdk/_scriptapi_memory.h"
#include "pluginsdk/_scriptapi_misc.h"
#include "pluginsdk/_scriptapi_module.h"
#include "pluginsdk/_scriptapi_pattern.h"
#include "pluginsdk/_scriptapi_register.h"
#include "pluginsdk/_scriptapi_stack.h"
#include "pluginsdk/_scriptapi_symbol.h"

#include "pluginsdk/DeviceNameResolver/DeviceNameResolver.h"
#include "pluginsdk/jansson/jansson.h"
#include "pluginsdk/lz4/lz4file.h"
#include "pluginsdk/TitanEngine/TitanEngine.h"
#include "pluginsdk/XEDParse/XEDParse.h"

#include "Settings.h"
#include "Util.h"

#ifdef _WIN64
#pragma comment(lib, "pluginsdk/x64dbg.lib")
#pragma comment(lib, "pluginsdk/x64bridge.lib")
#pragma comment(lib, "pluginsdk/DeviceNameResolver/DeviceNameResolver_x64.lib")
#pragma comment(lib, "pluginsdk/jansson/jansson_x64.lib")
#pragma comment(lib, "pluginsdk/lz4/lz4_x64.lib")
#pragma comment(lib, "pluginsdk/TitanEngine/TitanEngine_x64.lib")
#pragma comment(lib, "pluginsdk/XEDParse/XEDParse_x64.lib")
#else
#pragma comment(lib, "pluginsdk/x32dbg.lib")
#pragma comment(lib, "pluginsdk/x32bridge.lib")
#pragma comment(lib, "pluginsdk/DeviceNameResolver/DeviceNameResolver_x86.lib")
#pragma comment(lib, "pluginsdk/jansson/jansson_x86.lib")
#pragma comment(lib, "pluginsdk/lz4/lz4_x86.lib")
#pragma comment(lib, "pluginsdk/TitanEngine/TitanEngine_x86.lib")
#pragma comment(lib, "pluginsdk/XEDParse/XEDParse_x86.lib")
#endif //_WIN64

//#define Cmd(x) DbgCmdExecDirect(x)
#define Cmd(x) DbgCmdExecDirect((x))
#define Eval(x) DbgValFromString(x)

// 设置当前日志级别（示例中设为RELEASE级别）
#ifndef CURRENT_LOG_LEVEL
#define CURRENT_LOG_LEVEL LOG_LEVEL_DEBUG
#endif

// 分级日志宏
#if CURRENT_LOG_LEVEL <= LOG_LEVEL_DEBUG
#define DEBUG_LOG(fmt, ...) _plugin_logprintf("[DEBUG] " fmt, ##__VA_ARGS__)
#else
#define DEBUG_LOG(fmt, ...) ((void)0)
#endif

#if CURRENT_LOG_LEVEL <= LOG_LEVEL_RELEASE
#define RELEASE_LOG(fmt, ...) _plugin_logprintf("[INFO] " fmt, ##__VA_ARGS__)
#else
#define RELEASE_LOG(fmt, ...) ((void)0)
#endif

#define PLUG_EXPORT extern "C" __declspec(dllexport)

//superglobal variables
extern int pluginHandle;
extern HWND hwndDlg;
extern int hMenu;
extern int hMenuDisasm;
extern int hMenuDump;
extern int hMenuStack;

extern HINSTANCE g_hInstance;