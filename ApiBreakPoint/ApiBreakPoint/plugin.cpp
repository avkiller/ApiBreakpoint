#include "plugin.h"
#include <ShellScalingApi.h>

#include "pluginsdk\_plugins.h"
#include "pluginsdk\bridgemain.h"

//#ifdef _UNICODE
//#error "USE ASCII CODE PAGE"
//#endif

using namespace Script::Module;
using namespace Script::Symbol;
using namespace Script::Debug;
using namespace Script::Register;

// make sure only one window exists
bool bIsMainWindowShow = false;


HWND hTabControl;
HWND hMain;
int CurTab = 0;

// checkbox handle
HWND hChecks[CHECK_COUNT];



/*
 * Add API Breakpoint here.
 * You can change display text language here if you want.
 * Dll name are not necessary so far cause we are using "bp" rather than SetBPX()
 * If the function name appears in multiple dlls, the behavior depends on the bp instruction
 */
#define CHINESE
#ifdef CHINESE
ApiGroup Groups[TAB_COUNT] =
{
	{
		L"样本调试",
		{
			ApiBreakPointInfo(L"",L"LoadLibraryA",L"加载DLL"),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExW",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L"查找模块"),
			ApiBreakPointInfo(L"",L"GetModuleHandleW",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleExA",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleExW",L""),
			ApiBreakPointInfo(L"",L"CreateFileA",L"创建或打开文件"),
			ApiBreakPointInfo(L"",L"CreateFileW",L""),
			ApiBreakPointInfo(L"",L"ReadFile",L"读文件"),
			ApiBreakPointInfo(L"",L"WriteFile",L"写文件"),
			ApiBreakPointInfo(L"",L"DeleteFileA",L"删除文件"),
			ApiBreakPointInfo(L"",L"DeleteFileW",L""),
			ApiBreakPointInfo(L"",L"CreateThread",L"创建线程"),
			ApiBreakPointInfo(L"",L"CreateRemoteThread",L""),
			ApiBreakPointInfo(L"",L"OpenProcess",L"打开进程"),
			ApiBreakPointInfo(L"",L"WriteProcessMemory",L"跨进程写内存"),
			ApiBreakPointInfo(L"",L"ReadProcessMemory",L"跨进程读内存"),
			ApiBreakPointInfo(L"",L"CreateMutexA",L"创建互斥体"),
			ApiBreakPointInfo(L"",L"CreateMutexW",L""),
			ApiBreakPointInfo(L"",L"OpenMutexA",L"打开互斥体"),
			ApiBreakPointInfo(L"",L"OpenMutexW",L""),
			ApiBreakPointInfo(L"",L"CreateToolhelp32SnapShot",L"枚举进程"),
			ApiBreakPointInfo(L"",L"Heap32ListFirst",L""),
			ApiBreakPointInfo(L"",L"Heap32ListNext",L""),
			ApiBreakPointInfo(L"",L"Heap32First",L""),
			ApiBreakPointInfo(L"",L"Heap32Next",L""),
			ApiBreakPointInfo(L"",L"Module32First",L"枚举模块"),
			ApiBreakPointInfo(L"",L"Module32Next",L""),
			ApiBreakPointInfo(L"",L"Module32FirstW",L""),
			ApiBreakPointInfo(L"",L"Module32NextW",L""),
			ApiBreakPointInfo(L"",L"Process32First",L"枚举进程"),
			ApiBreakPointInfo(L"",L"Process32FirstW",L""),
			ApiBreakPointInfo(L"",L"Process32Next",L""),
			ApiBreakPointInfo(L"",L"Process32NextW",L""),
			ApiBreakPointInfo(L"",L"PostQuitMessage",L""),
			ApiBreakPointInfo(L"",L"Toolhelp32ReadProcessMemory",L""),
			ApiBreakPointInfo(L"",L"ZwQueryInformationProcess",L""),
			ApiBreakPointInfo(L"",L"SetTimer",L""),
			ApiBreakPointInfo(L"",L"GetTempPathA",L""),
			ApiBreakPointInfo(L"",L"GetTempPathW",L""),
			ApiBreakPointInfo(L"",L"CreateProcessA",L"创建进程"),
			ApiBreakPointInfo(L"",L"CreateProcessW",L""),
			ApiBreakPointInfo(L"",L"ShellExecuteA",L""),
			ApiBreakPointInfo(L"",L"ShellExecuteW",L""),
			ApiBreakPointInfo(L"",L"WinExec",L""),
			ApiBreakPointInfo(L"",L"UrlDownloadToFileA",L"下载文件"),
			ApiBreakPointInfo(L"",L"UrlDownloadToFileW",L""),
			ApiBreakPointInfo(L"",L"GetDesktopWindow",L""),
		}
	},
	{
		// https://anti-debug.checkpoint.com/
		L"对抗分析",
		{
			ApiBreakPointInfo(L"",L"IsDebuggerPresent",L"检测调试器"),
			ApiBreakPointInfo(L"",L"CheckRemoteDebuggerPresent",L"检测调试器"),
			ApiBreakPointInfo(L"",L"NtQueryInformationProcess",L"多个标志位检测调试器，检测父进程"),
			ApiBreakPointInfo(L"",L"UnhandledExceptionFilter",L""),
			ApiBreakPointInfo(L"",L"OutputDebugStringA",L"检查错误码探测调试器"),
			ApiBreakPointInfo(L"",L"OutputDebugStringW",L""),
			ApiBreakPointInfo(L"",L"GetThreadContext",L"探测硬件断点"),
			ApiBreakPointInfo(L"",L"GetProcessHeap",L"LFH:低碎片堆"),
			ApiBreakPointInfo(L"",L"GetProcAddress",L"检测HOOK"),
			ApiBreakPointInfo(L"",L"NtSetInformationThread",L"向调试器隐藏线程"),
			ApiBreakPointInfo(L"",L"NtYieldExecution",L"检测调试器"),
			ApiBreakPointInfo(L"",L"ReadProcessMemory",L"int3断点检测"),
			ApiBreakPointInfo(L"",L"VirtualProtect",L"修改内存权限触发异常检测调试器"),
			ApiBreakPointInfo(L"",L"VirtualProtectEx",L"修改内存权限触发异常检测调试器"),
			ApiBreakPointInfo(L"",L"RtlQueryProcessHeapInformation",L"堆标志位检测调试器"),
			ApiBreakPointInfo(L"",L"RtlQueryProcessDebugInformation",L"堆标志位检测调试器"),
			ApiBreakPointInfo(L"",L"NtQuerySystemInformation",L"检测调试器"),
			ApiBreakPointInfo(L"",L"OpenProcess",L"打开csrss.exe成功表示有调试器"),
			ApiBreakPointInfo(L"",L"CreateFileA",L"独占方式打开文件，失败表示有调试器"),
			ApiBreakPointInfo(L"",L"CreateFileW",L""),
			ApiBreakPointInfo(L"",L"NtClose",L"捕获EXCEPTION_INVALID_HANDLE异常表示存在调试器"),
			ApiBreakPointInfo(L"",L"CloseHandle",L""),
			ApiBreakPointInfo(L"",L"NtQueryObject",L"探测调试对象"),
			ApiBreakPointInfo(L"",L"SetUnhandledExceptionFilter",L"如果存在调试器，则回调不调用"),
			ApiBreakPointInfo(L"",L"RaiseException",L"抛出DBG_CONTROL_C等特殊异常，未接管则有调试器"),
			ApiBreakPointInfo(L"",L"RtlRaiseException",L"抛出DBG_CONTROL_C等特殊异常，未接管则有调试器"),
			ApiBreakPointInfo(L"",L"GetLocalTime",L"执行耗时估算，超时认为被调试"),
			ApiBreakPointInfo(L"",L"GetSystemTime",L"执行耗时估算，超时认为被调试"),
			ApiBreakPointInfo(L"",L"GetTickCount",L"执行耗时估算，超时认为被调试"),
			ApiBreakPointInfo(L"",L"NtGetTickCount",L"执行耗时估算，超时认为被调试"),
			ApiBreakPointInfo(L"",L"QueryPerformanceCounter",L"执行耗时估算，超时认为被调试"),
			ApiBreakPointInfo(L"",L"timeGetTime",L"执行耗时估算，超时认为被调试"),
			ApiBreakPointInfo(L"",L"NtQueryVirtualMemory",L"内存断点检测"),
			ApiBreakPointInfo(L"",L"AddVectoredExceptionHandler",L"注册VEH"),
			ApiBreakPointInfo(L"",L"RemoveVectoredExceptionHandler",L"移除VEH"),
			ApiBreakPointInfo(L"",L"DebugActiveProcess",L"创建子进程调试自己"),
			ApiBreakPointInfo(L"",L"DbgUiDebugActiveProcess",L"创建子进程调试自己"),
			ApiBreakPointInfo(L"",L"NtDebugActiveProcess",L"创建子进程调试自己"),
			ApiBreakPointInfo(L"",L"SetConsoleCtrlHandler",L"拦截CTRL_C_EVENT异常"),
			ApiBreakPointInfo(L"",L"BlockInput",L"一直返回TRUE表明被HOOK"),
			ApiBreakPointInfo(L"",L"EnumWindows",L"检测父进程窗口标题"),
			ApiBreakPointInfo(L"",L"SwitchDesktop",L"切换桌面反调试"),
			ApiBreakPointInfo(L"",L"FindWindowA",L"探测调试器窗口类名"),
			ApiBreakPointInfo(L"",L"FindWindowW",L"探测调试器窗口类名"),
			ApiBreakPointInfo(L"",L"FindWindowExA",L"探测调试器窗口类名"),
			ApiBreakPointInfo(L"",L"FindWindowExW",L"探测调试器窗口类名"),
			ApiBreakPointInfo(L"",L"DbgSetDebugFilterState",L""),
			ApiBreakPointInfo(L"",L"SwitchToThread",L""),
			ApiBreakPointInfo(L"",L"GetWriteWatch",L""),
		}
	},
	{
		L"注入",
		{
		}
	},
	{
		L"网络",
		{
			ApiBreakPointInfo(L"",L"send",L""),
			ApiBreakPointInfo(L"",L"sendto",L""),
			ApiBreakPointInfo(L"",L"WSASend",L""),
			ApiBreakPointInfo(L"",L"WSASendTo",L""),
			ApiBreakPointInfo(L"",L"recv",L""),
			ApiBreakPointInfo(L"",L"inet_addr",L""),
			ApiBreakPointInfo(L"",L"connect",L""),
			ApiBreakPointInfo(L"",L"UrlDownloadToFileA",L""),
			ApiBreakPointInfo(L"",L"UrlDownloadToFileW",L""),
		}
	},
	{
		L"对话框",
		{
			ApiBreakPointInfo(L"",L"MessageBoxA",L"弹对话框"),
			ApiBreakPointInfo(L"",L"MessageBoxW",L""),
			ApiBreakPointInfo(L"",L"MessageBoxExA",L""),
			ApiBreakPointInfo(L"",L"MessageBoxExW",L""),
			ApiBreakPointInfo(L"",L"MessageBoxIndirectA",L""),
			ApiBreakPointInfo(L"",L"MessageBoxIndirectW",L""),
			ApiBreakPointInfo(L"",L"CreateWindowExA",L"创建窗口"),
			ApiBreakPointInfo(L"",L"CreateWindowExW",L""),
			ApiBreakPointInfo(L"",L"CreateWindowA",L""),
			ApiBreakPointInfo(L"",L"CreateWindowW",L""),
			ApiBreakPointInfo(L"",L"DestroyWindow",L"销毁窗口"),
			ApiBreakPointInfo(L"",L"DialogBoxA",L"模态对话框"),
			ApiBreakPointInfo(L"",L"DialogBoxW",L""),
			ApiBreakPointInfo(L"",L"DialogBoxParamA",L""),
			ApiBreakPointInfo(L"",L"DialogBoxParamW",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectA",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectW",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectParamA",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectParamW",L""),
			ApiBreakPointInfo(L"",L"EndDialog",L"清除模态对话框"),
			ApiBreakPointInfo(L"",L"GetWindowTextA",L"取控件文本"),
			ApiBreakPointInfo(L"",L"GetWindowTextW",L""),
			ApiBreakPointInfo(L"",L"SetWindowTextA",L"设置控件文本"),
			ApiBreakPointInfo(L"",L"SetWindowTextW",L""),
			ApiBreakPointInfo(L"",L"GetDlgItemTextA",L"取控件文本"),
			ApiBreakPointInfo(L"",L"GetDlgItemTextW",L""),
			ApiBreakPointInfo(L"",L"SetDlgItemTextA",L"设置控件文本"),
			ApiBreakPointInfo(L"",L"SetDlgItemTextW",L""),
			ApiBreakPointInfo(L"",L"GetDlgItemInt",L"取控件中整数"),
			ApiBreakPointInfo(L"",L"SetDlgItemInt",L"设置控件中整数"),
			ApiBreakPointInfo(L"",L"SendMessageA",L"发消息"),
			ApiBreakPointInfo(L"",L"SendMessageW",L"发消息"),
			ApiBreakPointInfo(L"",L"SendDlgItemMessage",L"发消息"),
			ApiBreakPointInfo(L"",L"IsDialogMessageA",L""),
			ApiBreakPointInfo(L"",L"IsDialogMessageW",L""),
			ApiBreakPointInfo(L"",L"DispatchMessageA",L"消息派发"),
			ApiBreakPointInfo(L"",L"DispatchMessageW",L""),
			ApiBreakPointInfo(L"",L"CallWindowProcA",L"调用窗口过程"),
			ApiBreakPointInfo(L"",L"CallWindowProcW",L""),
			ApiBreakPointInfo(L"",L"DefWindowProcA",L"默认消息处理"),
			ApiBreakPointInfo(L"",L"DefWindowProcW",L""),
			ApiBreakPointInfo(L"",L"MessageBeep",L"系统警告声"),
			ApiBreakPointInfo(L"",L"DrawTextA",L"文本描绘到矩形中"),
			ApiBreakPointInfo(L"",L"DrawTextW",L""),
			ApiBreakPointInfo(L"",L"DrawTextExA",L""),
			ApiBreakPointInfo(L"",L"DrawTextExW",L""),
			ApiBreakPointInfo(L"",L"FindWindowA",L"获取窗口句柄"),
			ApiBreakPointInfo(L"",L"FindWindowW",L""),
			ApiBreakPointInfo(L"",L"GetClipboardData",L"获取剪贴板数据"),
			ApiBreakPointInfo(L"",L"CoInitialize",L"初始化COM库"),
			ApiBreakPointInfo(L"",L"CoCreateInstance",L"创建COM对象"),
			ApiBreakPointInfo(L"",L"LoadIconA",L"载入图标资源"),
			ApiBreakPointInfo(L"",L"LoadIconW",L""),
			ApiBreakPointInfo(L"",L"SetWindowLongA",L"设置窗口属性"),
			ApiBreakPointInfo(L"",L"SetWindowLongW",L""),
			ApiBreakPointInfo(L"",L"SetWindowLongPtrA",L""),
			ApiBreakPointInfo(L"",L"SetWindowLongPtrW",L""),
			ApiBreakPointInfo(L"",L"ShowWindow",L"显示窗口"),
			ApiBreakPointInfo(L"",L"UpdateWindow",L"更新窗口"),
			ApiBreakPointInfo(L"",L"TranslateMessage",L"消息队列"),
			ApiBreakPointInfo(L"",L"GetMessageA",L"取消息队列"),
			ApiBreakPointInfo(L"",L"GetMessageW",L""),
			ApiBreakPointInfo(L"",L"PeekMessageA",L"查消息队列"),
			ApiBreakPointInfo(L"",L"PeekMessageW",L""),
			ApiBreakPointInfo(L"",L"SetMenu",L"建菜单"),
			ApiBreakPointInfo(L"",L"DestroyMenu",L"删除菜单"),
			ApiBreakPointInfo(L"",L"DeleteMenu",L"删除菜单项"),
			ApiBreakPointInfo(L"",L"EnableMenuItem",L"菜单有无变灰"),
			ApiBreakPointInfo(L"",L"EnableWindow",L"禁止键盘鼠标输入"),
		}
	},
	{
		L"文件处理",
		{
			ApiBreakPointInfo(L"",L"CreateFileA",L"创建或打开文件"),
			ApiBreakPointInfo(L"",L"CreateFileW",L""),
			ApiBreakPointInfo(L"",L"OpenFile",L"打开文件"),
			ApiBreakPointInfo(L"",L"ReadFile",L"读文件"),
			ApiBreakPointInfo(L"",L"WriteFile",L"写文件"),
			ApiBreakPointInfo(L"",L"GetFileSize",L"取文件大小"),
			ApiBreakPointInfo(L"",L"FindFirstFileA",L"查找文件"),
			ApiBreakPointInfo(L"",L"FindFirstFileW",L""),
			ApiBreakPointInfo(L"",L"GetModuleFileNameA",L"查模块路径"),
			ApiBreakPointInfo(L"",L"GetModuleFileNameW",L""),
			ApiBreakPointInfo(L"",L"OpenFileMappingA",L"打开文件映射对象"),
			ApiBreakPointInfo(L"",L"OpenFileMappingW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryA",L"加载DLL"),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExW",L""),
			ApiBreakPointInfo(L"",L"CreateFileMappingA",L"创建文件对象"),
			ApiBreakPointInfo(L"",L"CreateFileMappingW",L""),
			ApiBreakPointInfo(L"",L"CopyFileA",L"复制文件"),
			ApiBreakPointInfo(L"",L"CopyFileW",L""),
			ApiBreakPointInfo(L"",L"CopyFileExA",L""),
			ApiBreakPointInfo(L"",L"CopyFileExW",L""),
			ApiBreakPointInfo(L"",L"SetFilePointer",L"设置文件读写位置"),
			ApiBreakPointInfo(L"",L"MoveFileA",L"移动文件"),
			ApiBreakPointInfo(L"",L"MoveFileW",L""),
			ApiBreakPointInfo(L"",L"MoveFileExA",L""),
			ApiBreakPointInfo(L"",L"MoveFileExW",L""),
			ApiBreakPointInfo(L"",L"DeleteFileA",L""),
			ApiBreakPointInfo(L"",L"DeleteFileW",L"删除文件"),
			ApiBreakPointInfo(L"",L"LoadCursorFromFileA",L"创建文件光标"),
			ApiBreakPointInfo(L"",L"LoadCursorFromFileW",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringA",L"INI重启验证常用"),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringW",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileIntA",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileIntW",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringA",L"取字符串"),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringW",L""),
			ApiBreakPointInfo(L"",L"WritePrivateProfileStringA",L"设置字符串"),
			ApiBreakPointInfo(L"",L"WritePrivateProfileStringW",L""),
		}
	},
	{
		L"时间处理",
		{
			ApiBreakPointInfo(L"",L"SetSystemTime",L"设置系统时间"),
			ApiBreakPointInfo(L"",L"TimerProc",L"回调函数"),//what is this
			ApiBreakPointInfo(L"",L"SetLocalTime",L"设置本地时间"),
			ApiBreakPointInfo(L"",L"GetLocalTime",L"获取本地时间"),
			ApiBreakPointInfo(L"",L"GetSystemTime",L"获取系统时间"),
			ApiBreakPointInfo(L"",L"GetCurrentTime",L"获取系统时间"),
			ApiBreakPointInfo(L"",L"GetFileTime",L"获取文件时间"),
			ApiBreakPointInfo(L"",L"GetTickCount",L"系统启动后所经过的毫秒数"),
			ApiBreakPointInfo(L"",L"CompareFileTime",L"比较文件时间"),
			ApiBreakPointInfo(L"",L"SetTimer",L"创建定时器"),
			ApiBreakPointInfo(L"",L"KillTimer",L"移除定时器"),
			ApiBreakPointInfo(L"",L"timeSetEvent",L"多媒体定时器"),
		}
	},
	{
		L"进程函数",
		{
			ApiBreakPointInfo(L"",L"CreateThread",L"创建线程"),
			ApiBreakPointInfo(L"",L"CreateRemoteThread",L"创建远程线程"),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L"查模块基址"),
			ApiBreakPointInfo(L"",L"GetModuleHandleW",L""),
			ApiBreakPointInfo(L"",L"OpenMutexA",L"打开互斥体"),
			ApiBreakPointInfo(L"",L"OpenMutexW",L""),
			ApiBreakPointInfo(L"",L"WriteProcessMemory",L"写内存"),
			ApiBreakPointInfo(L"",L"ZwWriteVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"ReadProcessMemory",L"读内存"),
			ApiBreakPointInfo(L"",L"ZwReadVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"CreateProcessA",L"创建进程"),
			ApiBreakPointInfo(L"",L"CreateProcessW",L""),
			ApiBreakPointInfo(L"",L"OpenProcess",L"打开进程"),
			ApiBreakPointInfo(L"",L"ExitProcess",L"结束进程"),
			ApiBreakPointInfo(L"",L"ExitThread",L"结束当前线程"),
			ApiBreakPointInfo(L"",L"TerminateProcess",L"终止进程"),
			ApiBreakPointInfo(L"",L"TerminateThread",L"终止线程"),
			ApiBreakPointInfo(L"",L"CreateToolhelp32Snapshot",L"进程快照"),
			ApiBreakPointInfo(L"",L"Process32First",L"取进程句柄"),
			ApiBreakPointInfo(L"",L"Process32FirstW",L""),
			ApiBreakPointInfo(L"",L"Process32Next",L""),
			ApiBreakPointInfo(L"",L"Process32NextW",L""),
			ApiBreakPointInfo(L"",L"Module32Next",L"取模块句柄"),
			ApiBreakPointInfo(L"",L"Module32NextW",L""),
			ApiBreakPointInfo(L"",L"Module32First",L""),
			ApiBreakPointInfo(L"",L"Module32FirstW",L""),
			ApiBreakPointInfo(L"",L"Heap32ListFirst",L"取堆句柄"),
			ApiBreakPointInfo(L"",L"Heap32ListNext",L"取堆句柄"),
			ApiBreakPointInfo(L"",L"GetProcessHeap",L"取堆句柄"),
			ApiBreakPointInfo(L"",L"Heap32First",L"取进程堆相关信息"),
			ApiBreakPointInfo(L"",L"Heap32Next",L"取进程堆相关信息"),
			ApiBreakPointInfo(L"",L"PostQuitMessage",L"终止线程"),
			ApiBreakPointInfo(L"",L"IsDebuggerPresent",L"检测调试器"),
			ApiBreakPointInfo(L"",L"OpenProcessToken",L"获取进程访问令牌句柄"),
			ApiBreakPointInfo(L"",L"OpenThreadToken",L""),
			ApiBreakPointInfo(L"",L"ZwQueryInformationProcess",L""),
		}
	},
	{
		L"内存",
		{
			ApiBreakPointInfo(L"",L"FindWindow",L"取窗口句柄"),
			ApiBreakPointInfo(L"",L"GetWindowThreadProcessId",L"获取窗口进程ID"),
			ApiBreakPointInfo(L"",L"OpenProcess",L"打开进程"),
			ApiBreakPointInfo(L"",L"VirtualAllocEx",L"申请内存"),
			ApiBreakPointInfo(L"",L"WriteProcessMemory",L"写内存"),
			ApiBreakPointInfo(L"",L"ReadProcessMemory",L"读内存"),
			ApiBreakPointInfo(L"",L"CreateRemoteThread",L"创建远程线程"),
			ApiBreakPointInfo(L"",L"CreateMutexA",L"创建互斥体"),
			ApiBreakPointInfo(L"",L"CreateMutexW",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L"查模块基址"),
			ApiBreakPointInfo(L"",L"GetModuleHandleW",L""),
			ApiBreakPointInfo(L"",L"GetProcAddress",L"查函数地址"),
			ApiBreakPointInfo(L"",L"VirtualAlloc",L"申请内存"),
			ApiBreakPointInfo(L"",L"HeapAlloc",L""),
			ApiBreakPointInfo(L"",L"VirtualFree",L"释放内存"),
			ApiBreakPointInfo(L"",L"HeapFree",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryA",L"载入DLL"),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"GetVersion",L"获取系统版本"),
			ApiBreakPointInfo(L"",L"ZwAllocateVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"ZwWriteVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"ZwReadVirtualMemory",L""),
		}
	},
	{
		L"磁盘",
		{
			ApiBreakPointInfo(L"",L"GetDiskFreeSpaceA",L"获取磁盘信息"),
			ApiBreakPointInfo(L"",L"GetDiskFreeSpaceExA",L""),
			ApiBreakPointInfo(L"",L"GetDiskFreeSpaceExW",L""),
			ApiBreakPointInfo(L"",L"GetDriveTypeA",L"判断磁盘驱动器类型"),
			ApiBreakPointInfo(L"",L"GetDriveTypeW",L""),
			ApiBreakPointInfo(L"",L"GetLogicalDrives",L"取驱动器字母"),
			ApiBreakPointInfo(L"",L"GetLogicalDriveStringsA",L"取驱动器路径"),
			ApiBreakPointInfo(L"",L"GetLogicalDriveStringsW",L""),
		}
	},
	{
		L"注册表",
		{
			ApiBreakPointInfo(L"",L"RegOpenKeyA",L"打开注册表项"),
			ApiBreakPointInfo(L"",L"RegOpenKeyW",L""),
			ApiBreakPointInfo(L"",L"RegOpenKeyExA",L"重启验证常用"),
			ApiBreakPointInfo(L"",L"RegOpenKeyExW",L""),
			ApiBreakPointInfo(L"",L"RegCreateKeyExA",L""),
			ApiBreakPointInfo(L"",L"RegCreateKeyExW",L""),
			ApiBreakPointInfo(L"",L"RegQueryValueA",L"取注册表键值"),
			ApiBreakPointInfo(L"",L"RegQueryValueW",L""),
			ApiBreakPointInfo(L"",L"RegQueryValueExA",L""),
			ApiBreakPointInfo(L"",L"RegQueryValueExW",L""),
			ApiBreakPointInfo(L"",L"RegCreateKeyA",L"创建新项"),
			ApiBreakPointInfo(L"",L"RegCreateKeyW",L""),
			ApiBreakPointInfo(L"",L"RegCloseKey",L"关闭注册表"),
			ApiBreakPointInfo(L"",L"RegDeleteKeyA",L"删除键值"),
			ApiBreakPointInfo(L"",L"RegDeleteKeyW",L""),
			ApiBreakPointInfo(L"",L"RegEnumKeyExA",L"枚举子项"),
			ApiBreakPointInfo(L"",L"RegEnumKeyExW",L""),
			ApiBreakPointInfo(L"",L"RegSetValueA",L"设置默认值"),
			ApiBreakPointInfo(L"",L"RegSetValueW",L""),
			ApiBreakPointInfo(L"",L"RegSetValueExA",L"设置指定项的值"),
			ApiBreakPointInfo(L"",L"RegSetValueExW",L""),
		}
	},
	{
		L"VB",
		{
			ApiBreakPointInfo(L"",L"FindResourceA",L"VB校验"),
			ApiBreakPointInfo(L"",L"CreateFileA",L"VB校验"),
			ApiBreakPointInfo(L"",L"GetVolumeInformation",L"磁盘卷"),
			ApiBreakPointInfo(L"",L"MultiByteToWideChar",L"A转U字符串"),
			ApiBreakPointInfo(L"",L"WideCharToMultiByte",L"U转A字符串"),
			ApiBreakPointInfo(L"",L"GetFileSize",L"获取文件大小"),
			ApiBreakPointInfo(L"",L"VarCyFromStr",L"字符串转整型"),
			ApiBreakPointInfo(L"",L"arBstrFromI2",L"整型转字符串"),
			ApiBreakPointInfo(L"",L"rtcFileLen",L"VB自校验"),
			ApiBreakPointInfo(L"",L"rtcFileLenth",L""),
			ApiBreakPointInfo(L"",L"rtcMsgBox",L"显示对话框"),
			ApiBreakPointInfo(L"",L"rtcInputBox",L"输入框"),
			ApiBreakPointInfo(L"",L"GetMemStr",L""),
			ApiBreakPointInfo(L"",L"rtcR8ValFromBstr",L""),
			ApiBreakPointInfo(L"",L"rtcGetPresendDate",L""),
			ApiBreakPointInfo(L"",L"rtcBeep",L""),
			ApiBreakPointInfo(L"",L"rtcTrimBstr",L"字符串去除空格"),
			ApiBreakPointInfo(L"",L"rtcMidCharVar",L"字符串中取字符"),
			ApiBreakPointInfo(L"",L"rtcLeftCharVar",L"字符串左边取字符"),
			ApiBreakPointInfo(L"",L"rtcRightCharVar",L"字符串右边取字符"),
			ApiBreakPointInfo(L"",L"StrConv",L"转换字符串"),
			ApiBreakPointInfo(L"",L"rtcT8ValFromBstr",L"字符串转浮点数"),
			ApiBreakPointInfo(L"",L"__vbaI2Str",L"字符转1字节数值"),
			ApiBreakPointInfo(L"",L"__vbaI4Str",L"字符转4字节数值"),
			ApiBreakPointInfo(L"",L"__vbar4Str",L"字符转4浮点型"),
			ApiBreakPointInfo(L"",L"__vbar8Str",L"字符转8浮点型"),
			ApiBreakPointInfo(L"",L"__vbavaradd",L"变量值相加"),
			ApiBreakPointInfo(L"",L"__vbavarsub",L"变量值相减"),
			ApiBreakPointInfo(L"",L"__vbavarmul",L"变量值相乘"),
			ApiBreakPointInfo(L"",L"__vbavaridiv",L"变量值相除"),
			ApiBreakPointInfo(L"",L"__vbavarxor",L"变量值异或"),
			ApiBreakPointInfo(L"",L"__vbavarcat",L"变量值相连"),
			ApiBreakPointInfo(L"",L"__vbaStrCat",L"字符串相连"),
			ApiBreakPointInfo(L"",L"__vbaVarCat",L"连接字串"),
			ApiBreakPointInfo(L"",L"__vbaStrCmp",L"字符串比较"),
			ApiBreakPointInfo(L"",L"__vbaStrComp",L"字符串比较"),
			ApiBreakPointInfo(L"",L"__vbaVarTstEq",L"比较变量"),
			ApiBreakPointInfo(L"",L"__vbaFreeStr",L""),
			ApiBreakPointInfo(L"",L"__vbaStrCopy",L""),
			ApiBreakPointInfo(L"",L"__vbaLenBstr",L""),
			ApiBreakPointInfo(L"",L"__vbavarfornext",L"循环结构"),
			ApiBreakPointInfo(L"",L"__vbafreeobj",L"释放对象"),
		}
	},
	{
		L"VC/MFC",
		{
			ApiBreakPointInfo(L"",L"lstrcmpA",L"字符串比较"),
			ApiBreakPointInfo(L"",L"DefWindowProcA",L"调用默认窗口过程"),
			ApiBreakPointInfo(L"",L"DefWindowProcW",L""),
			ApiBreakPointInfo(L"",L"RegisterClassA",L"注册窗口类"),
			ApiBreakPointInfo(L"",L"RegisterClassW",L""),
			ApiBreakPointInfo(L"",L"RegisterClassExA",L""),
			ApiBreakPointInfo(L"",L"RegisterClassExW",L""),
			ApiBreakPointInfo(L"",L"FindResourceA",L"查找资源"),
			ApiBreakPointInfo(L"",L"FindResourceW",L""),
			ApiBreakPointInfo(L"",L"LoadResource",L"加载资源"),
			ApiBreakPointInfo(L"",L"SetHandleCount",L"取变量地址"),
			ApiBreakPointInfo(L"",L"IsWindowEnabled",L"可否输入键鼠"),
		}
	},
	{
		L"加密狗/穿山甲",
		{
			ApiBreakPointInfo(L"",L"CreateThread",L"寻找OEP"),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L"魔幻跳转"),
			ApiBreakPointInfo(L"",L"OpenMutexA",L"1转单进程两次改跳"),
			ApiBreakPointInfo(L"",L"GetSystemTime",L"补丁KEY"),
			ApiBreakPointInfo(L"",L"VirtualProtect",L"2EAX为401000返回"),
			ApiBreakPointInfo(L"",L"CreateFileMappingA",L"辅助"),
			ApiBreakPointInfo(L"",L"LoadLibraryA",L"载入DLL"),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExW",L""),
			ApiBreakPointInfo(L"",L"RefreshDeviceList",L"深思3"),
			ApiBreakPointInfo(L"",L"DeviceIoControl",L"操作设备"),
			ApiBreakPointInfo(L"",L"Prestochangoselector",L""),
			ApiBreakPointInfo(L"",L"FreeEnvironmentStringsA",L"释放环境字串块"),
			ApiBreakPointInfo(L"",L"GetLogicalDriveStringsA",L"取列表"),
			ApiBreakPointInfo(L"",L"GetLogicalDrives",L"取驱动器列表"),
			ApiBreakPointInfo(L"",L"GetDriveTypeA",L"判断驱动器类型"),
			ApiBreakPointInfo(L"",L"CreateFileA",L"读狗驱动"),
			ApiBreakPointInfo(L"",L"FindFirstFileA",L"查找文件"),
		}
	},
	{
		L"其他",
		{
			ApiBreakPointInfo(L"",L"FindFirstFileA",L"查找文件"),
			ApiBreakPointInfo(L"",L"ExitProcess",L"程序退出"),
			ApiBreakPointInfo(L"",L"GetStartupInfoA",L"获取启动信息"),
			ApiBreakPointInfo(L"",L"GetFileSize",L"获取文件大小"),
			ApiBreakPointInfo(L"",L"VirtualProtect",L"Vmp脱壳"),
			ApiBreakPointInfo(L"",L"ExitWindowsEx",L"关机断点"),
			ApiBreakPointInfo(L"",L"CreateFileA",L"自校验"),
			ApiBreakPointInfo(L"",L"GetVersion",L"易语言脱壳"),
			ApiBreakPointInfo(L"",L"send",L"发送封包"),
			ApiBreakPointInfo(L"",L"WSASend",L"发送封包"),
			ApiBreakPointInfo(L"",L"recv",L"接收封包"),
			ApiBreakPointInfo(L"",L"RtlAdjustPrivilege",L"易语言快速关机"),
			ApiBreakPointInfo(L"",L"SHFormatDrive",L"格盘API"),
			ApiBreakPointInfo(L"",L"RemoveDirectoryA",L"删除目录"),
			ApiBreakPointInfo(L"",L"DeleteFileA",L"删除文件"),
			ApiBreakPointInfo(L"",L"GetLastError",L"返回错误码"),
		}
	},
};

#else

ApiGroup Groups[TAB_COUNT] =
{
	{
		"Common",
		{
			ApiBreakPointInfo(L"",L"FindFirstFileA",L"search file"),
			ApiBreakPointInfo(L"",L"ExitProcess",L""),
			ApiBreakPointInfo(L"",L"GetStartupInfoA",L""),
			ApiBreakPointInfo(L"",L"GetFileSize",L""),
			ApiBreakPointInfo(L"",L"VirtualProtect",L"modify memory protect"),
			ApiBreakPointInfo(L"",L"ExitWindowsEx",L"shutdown breakpoint"),
			ApiBreakPointInfo(L"",L"CreateFileA",L""),
			ApiBreakPointInfo(L"",L"GetVersion",L""),
			ApiBreakPointInfo(L"",L"send",L""),
			ApiBreakPointInfo(L"",L"WSASend",L""),
			ApiBreakPointInfo(L"",L"recv",L""),
			ApiBreakPointInfo(L"",L"RtlAdjustPrivilege",L""),
			ApiBreakPointInfo(L"",L"SHFormatDrive",L"Format Disk"),
			ApiBreakPointInfo(L"",L"RemoveDirectoryA",L""),
			ApiBreakPointInfo(L"",L"DeleteFileA",L""),
			ApiBreakPointInfo(L"",L"GetLastError",L""),
		}
	},
	{
		"Dialog",
		{
			ApiBreakPointInfo(L"",L"MessageBoxA",L"popup a message box"),
			ApiBreakPointInfo(L"",L"MessageBoxW",L""),
			ApiBreakPointInfo(L"",L"MessageBoxExA",L""),
			ApiBreakPointInfo(L"",L"MessageBoxExW",L""),
			ApiBreakPointInfo(L"",L"MessageBoxIndirectA",L""),
			ApiBreakPointInfo(L"",L"MessageBoxIndirectW",L""),
			ApiBreakPointInfo(L"",L"GetWindowTextA",L"get control text"),
			ApiBreakPointInfo(L"",L"GetWindowTextW",L""),
			ApiBreakPointInfo(L"",L"SetWindowTextA",L"set control text"),
			ApiBreakPointInfo(L"",L"SetWindowTextW",L""),
			ApiBreakPointInfo(L"",L"GetDlgItemTextA",L""),
			ApiBreakPointInfo(L"",L"GetDlgItemTextW",L""),
			ApiBreakPointInfo(L"",L"SetDlgItemTextA",L"set control text"),
			ApiBreakPointInfo(L"",L"SetDlgItemTextW",L""),
			ApiBreakPointInfo(L"",L"GetDlgItemInt",L""),
			ApiBreakPointInfo(L"",L"SetDlgItemInt",L""),
			ApiBreakPointInfo(L"",L"SendMessageA",L""),
			ApiBreakPointInfo(L"",L"SendMessageW",L""),
			ApiBreakPointInfo(L"",L"SendDlgItemMessage",L""),
			ApiBreakPointInfo(L"",L"IsDialogMessageA",L""),
			ApiBreakPointInfo(L"",L"IsDialogMessageW",L""),
			ApiBreakPointInfo(L"",L"DispatchMessageA",L""),
			ApiBreakPointInfo(L"",L"DispatchMessageW",L""),
			ApiBreakPointInfo(L"",L"CallWindowProcA",L""),
			ApiBreakPointInfo(L"",L"CallWindowProcW",L""),
			ApiBreakPointInfo(L"",L"DefWindowProcA",L""),
			ApiBreakPointInfo(L"",L"DefWindowProcW",L""),
			ApiBreakPointInfo(L"",L"MessageBeep",L""),
			ApiBreakPointInfo(L"",L"DrawTextA",L""),
			ApiBreakPointInfo(L"",L"DrawTextW",L""),
			ApiBreakPointInfo(L"",L"DrawTextExA",L""),
			ApiBreakPointInfo(L"",L"DrawTextExW",L""),
			ApiBreakPointInfo(L"",L"FindWindowA",L""),
			ApiBreakPointInfo(L"",L"FindWindowW",L""),
			ApiBreakPointInfo(L"",L"GetClipboardData",L""),
			ApiBreakPointInfo(L"",L"CoInitialize",L"initialize COM library"),
			ApiBreakPointInfo(L"",L"CoCreateInstance",L"create COM instance"),
			ApiBreakPointInfo(L"",L"LoadIconA",L""),
			ApiBreakPointInfo(L"",L"LoadIconW",L""),
			ApiBreakPointInfo(L"",L"SetWindowLongA",L"change window attribute"),
			ApiBreakPointInfo(L"",L"SetWindowLongW",L""),
			ApiBreakPointInfo(L"",L"SetWindowLongPtrA",L""),
			ApiBreakPointInfo(L"",L"SetWindowLongPtrW",L""),
			ApiBreakPointInfo(L"",L"ShowWindow",L""),
			ApiBreakPointInfo(L"",L"UpdateWindow",L""),
			ApiBreakPointInfo(L"",L"CreateWindowExA",L""),
			ApiBreakPointInfo(L"",L"CreateWindowExW",L""),
			ApiBreakPointInfo(L"",L"CreateWindowA",L""),
			ApiBreakPointInfo(L"",L"CreateWindowW",L""),
			ApiBreakPointInfo(L"",L"DestroyWindow",L""),
			ApiBreakPointInfo(L"",L"DialogBoxA",L""),
			ApiBreakPointInfo(L"",L"DialogBoxW",L""),
			ApiBreakPointInfo(L"",L"DialogBoxParamA",L""),
			ApiBreakPointInfo(L"",L"DialogBoxParamW",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectA",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectW",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectParamA",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectParamW",L""),
			ApiBreakPointInfo(L"",L"EndDialog",L""),
			ApiBreakPointInfo(L"",L"TranslateMessage",L""),
			ApiBreakPointInfo(L"",L"GetMessageA",L""),
			ApiBreakPointInfo(L"",L"GetMessageW",L""),
			ApiBreakPointInfo(L"",L"PeekMessageA",L""),
			ApiBreakPointInfo(L"",L"PeekMessageW",L""),
			ApiBreakPointInfo(L"",L"SetMenu",L""),
			ApiBreakPointInfo(L"",L"DestroyMenu",L""),
			ApiBreakPointInfo(L"",L"DeleteMenu",L""),
			ApiBreakPointInfo(L"",L"EnableMenuItem",L""),
			ApiBreakPointInfo(L"",L"EnableWindow",L""),
		}
	},
	{
		"File",
		{
			ApiBreakPointInfo(L"",L"CreateFileA",L"create or open file"),
			ApiBreakPointInfo(L"",L"CreateFileW",L""),
			ApiBreakPointInfo(L"",L"OpenFile",L""),
			ApiBreakPointInfo(L"",L"ReadFile",L""),
			ApiBreakPointInfo(L"",L"WriteFile",L""),
			ApiBreakPointInfo(L"",L"GetFileSize",L""),
			ApiBreakPointInfo(L"",L"FindFirstFileA",L""),
			ApiBreakPointInfo(L"",L"FindFirstFileW",L""),
			ApiBreakPointInfo(L"",L"GetModuleFileNameA",L""),
			ApiBreakPointInfo(L"",L"GetModuleFileNameW",L""),
			ApiBreakPointInfo(L"",L"OpenFileMappingA",L""),
			ApiBreakPointInfo(L"",L"OpenFileMappingW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExW",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleW",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleExA",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleExW",L""),
			ApiBreakPointInfo(L"",L"CreateFileMappingA",L""),
			ApiBreakPointInfo(L"",L"CreateFileMappingW",L""),
			ApiBreakPointInfo(L"",L"CopyFileA",L""),
			ApiBreakPointInfo(L"",L"CopyFileW",L""),
			ApiBreakPointInfo(L"",L"CopyFileExA",L""),
			ApiBreakPointInfo(L"",L"CopyFileExW",L""),
			ApiBreakPointInfo(L"",L"SetFilePointer",L""),
			ApiBreakPointInfo(L"",L"MoveFileA",L""),
			ApiBreakPointInfo(L"",L"MoveFileW",L""),
			ApiBreakPointInfo(L"",L"MoveFileExA",L""),
			ApiBreakPointInfo(L"",L"MoveFileExW",L""),
			ApiBreakPointInfo(L"",L"DeleteFileA",L""),
			ApiBreakPointInfo(L"",L"DeleteFileW",L""),
			ApiBreakPointInfo(L"",L"LoadCursorFromFileA",L""),
			ApiBreakPointInfo(L"",L"LoadCursorFromFileW",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringA",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringW",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileIntA",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileIntW",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringA",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringW",L""),
			ApiBreakPointInfo(L"",L"WritePrivateProfileStringA",L""),
			ApiBreakPointInfo(L"",L"WritePrivateProfileStringW",L""),
		}
	},
	{
		"Register",
		{
			ApiBreakPointInfo(L"",L"RegOpenKeyA",L""),
			ApiBreakPointInfo(L"",L"RegOpenKeyW",L""),
			ApiBreakPointInfo(L"",L"RegOpenKeyExA",L""),
			ApiBreakPointInfo(L"",L"RegOpenKeyExW",L""),
			ApiBreakPointInfo(L"",L"RegCreateKeyExA",L""),
			ApiBreakPointInfo(L"",L"RegCreateKeyExW",L""),
			ApiBreakPointInfo(L"",L"RegQueryValueA",L""),
			ApiBreakPointInfo(L"",L"RegQueryValueW",L""),
			ApiBreakPointInfo(L"",L"RegQueryValueExA",L""),
			ApiBreakPointInfo(L"",L"RegQueryValueExW",L""),
			ApiBreakPointInfo(L"",L"RegCreateKeyA",L""),
			ApiBreakPointInfo(L"",L"RegCreateKeyW",L""),
			ApiBreakPointInfo(L"",L"RegCloseKey",L""),
			ApiBreakPointInfo(L"",L"RegDeleteKeyA",L""),
			ApiBreakPointInfo(L"",L"RegDeleteKeyW",L""),
			ApiBreakPointInfo(L"",L"RegEnumKeyExA",L""),
			ApiBreakPointInfo(L"",L"RegEnumKeyExW",L""),
			ApiBreakPointInfo(L"",L"RegSetValueA",L""),
			ApiBreakPointInfo(L"",L"RegSetValueW",L""),
			ApiBreakPointInfo(L"",L"RegSetValueExA",L""),
			ApiBreakPointInfo(L"",L"RegSetValueExW",L""),
		}
	},
	{
		"Time/Date",
		{
			ApiBreakPointInfo(L"",L"SetSystemTime",L""),
			ApiBreakPointInfo(L"",L"TimerProc",L""),//what is this
			ApiBreakPointInfo(L"",L"SetLocalTime",L""),
			ApiBreakPointInfo(L"",L"GetLocalTime",L""),
			ApiBreakPointInfo(L"",L"GetSystemTime",L""),
			ApiBreakPointInfo(L"",L"GetCurrentTime",L""),
			ApiBreakPointInfo(L"",L"GetFileTime",L""),
			ApiBreakPointInfo(L"",L"GetTickCount",L""),
			ApiBreakPointInfo(L"",L"CompareFileTime",L""),
			ApiBreakPointInfo(L"",L"SetTimer",L""),
			ApiBreakPointInfo(L"",L"KillTimer",L""),
			ApiBreakPointInfo(L"",L"timeSetEvent",L""),
		}
	},
	{
		"Process",
		{
			ApiBreakPointInfo(L"",L"CreateThread",L""),
			ApiBreakPointInfo(L"",L"CreateRemoteThread",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleW",L""),
			ApiBreakPointInfo(L"",L"OpenMutexA",L""),
			ApiBreakPointInfo(L"",L"OpenMutexW",L""),
			ApiBreakPointInfo(L"",L"WriteProcessMemory",L""),
			ApiBreakPointInfo(L"",L"ZwWriteVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"ReadProcessMemory",L""),
			ApiBreakPointInfo(L"",L"ZwReadVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"CreateProcessA",L""),
			ApiBreakPointInfo(L"",L"CreateProcessW",L""),
			ApiBreakPointInfo(L"",L"OpenProcess",L""),
			ApiBreakPointInfo(L"",L"ExitProcess",L""),
			ApiBreakPointInfo(L"",L"ExitThread",L""),
			ApiBreakPointInfo(L"",L"TerminateProcess",L""),
			ApiBreakPointInfo(L"",L"TerminateThread",L""),
			ApiBreakPointInfo(L"",L"CreateToolhelp32Snapshot",L"enum process"),
			ApiBreakPointInfo(L"",L"Process32First",L""),
			ApiBreakPointInfo(L"",L"Process32FirstW",L""),
			ApiBreakPointInfo(L"",L"Process32Next",L""),
			ApiBreakPointInfo(L"",L"Process32NextW",L""),
			ApiBreakPointInfo(L"",L"Module32Next",L""),
			ApiBreakPointInfo(L"",L"Module32NextW",L""),
			ApiBreakPointInfo(L"",L"Module32First",L""),
			ApiBreakPointInfo(L"",L"Module32FirstW",L""),
			ApiBreakPointInfo(L"",L"Heap32ListFirst",L""),
			ApiBreakPointInfo(L"",L"Heap32ListNext",L""),
			ApiBreakPointInfo(L"",L"GetProcessHeap",L""),
			ApiBreakPointInfo(L"",L"Heap32First",L""),
			ApiBreakPointInfo(L"",L"Heap32Next",L""),
			ApiBreakPointInfo(L"",L"PostQuitMessage",L""),
			ApiBreakPointInfo(L"",L"IsDebuggerPresent",L""),
			ApiBreakPointInfo(L"",L"OpenProcessToken",L""),
			ApiBreakPointInfo(L"",L"OpenThreadToken",L""),
			ApiBreakPointInfo(L"",L"ZwQueryInformationProcess",L""),
		}
	},
	{
		"Network",
		{
			ApiBreakPointInfo(L"",L"send",L""),
			ApiBreakPointInfo(L"",L"sendto",L""),
			ApiBreakPointInfo(L"",L"WSASend",L""),
			ApiBreakPointInfo(L"",L"WSASendTo",L""),
			ApiBreakPointInfo(L"",L"recv",L""),
			ApiBreakPointInfo(L"",L"inet_addr",L""),
			ApiBreakPointInfo(L"",L"connect",L""),
			ApiBreakPointInfo(L"",L"UrlDownloadToFileA",L""),
			ApiBreakPointInfo(L"",L"UrlDownloadToFileW",L""),
		}
	},
	{
		"Memory",
		{
			ApiBreakPointInfo(L"",L"FindWindow",L""),
			ApiBreakPointInfo(L"",L"GetWindowThreadProcessId",L""),
			ApiBreakPointInfo(L"",L"OpenProcess",L""),
			ApiBreakPointInfo(L"",L"VirtualAllocEx",L""),
			ApiBreakPointInfo(L"",L"WriteProcessMemory",L""),
			ApiBreakPointInfo(L"",L"ReadProcessMemory",L""),
			ApiBreakPointInfo(L"",L"CreateRemoteThread",L""),
			ApiBreakPointInfo(L"",L"CreateMutexA",L""),
			ApiBreakPointInfo(L"",L"CreateMutexW",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleW",L""),
			ApiBreakPointInfo(L"",L"GetProcAddress",L""),
			ApiBreakPointInfo(L"",L"VirtualAlloc",L""),
			ApiBreakPointInfo(L"",L"HeapAlloc",L""),
			ApiBreakPointInfo(L"",L"VirtualFree",L""),
			ApiBreakPointInfo(L"",L"HeapFree",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"GetVersion",L""),
			ApiBreakPointInfo(L"",L"ZwAllocateVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"ZwWriteVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"ZwReadVirtualMemory",L""),
		}
	},
	{
		"Disk",
		{
			ApiBreakPointInfo(L"",L"GetDiskFreeSpaceA",L""),
			ApiBreakPointInfo(L"",L"GetDiskFreeSpaceExA",L""),
			ApiBreakPointInfo(L"",L"GetDiskFreeSpaceExW",L""),
			ApiBreakPointInfo(L"",L"GetDriveTypeA",L""),
			ApiBreakPointInfo(L"",L"GetDriveTypeW",L""),
			ApiBreakPointInfo(L"",L"GetLogicalDrives",L""),
			ApiBreakPointInfo(L"",L"GetLogicalDriveStringsA",L""),
			ApiBreakPointInfo(L"",L"GetLogicalDriveStringsW",L""),
		}
	},
	{
		"Malware",
		{
			ApiBreakPointInfo(L"",L"CreateFileA",L""),
			ApiBreakPointInfo(L"",L"CreateFileW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExW",L""),
			ApiBreakPointInfo(L"",L"CreateThread",L""),
			ApiBreakPointInfo(L"",L"CreateProcessA",L""),
			ApiBreakPointInfo(L"",L"CreateProcessW",L""),
			ApiBreakPointInfo(L"",L"CreateRemoteThread",L""),
			ApiBreakPointInfo(L"",L"WriteProcessMemory",L""),
			ApiBreakPointInfo(L"",L"ReadProcessMemory",L""),
			ApiBreakPointInfo(L"",L"OpenMutexA",L""),
			ApiBreakPointInfo(L"",L"OpenMutexW",L""),
			ApiBreakPointInfo(L"",L"CreateToolhelp32SnapShot",L""),
			ApiBreakPointInfo(L"",L"Heap32ListFirst",L""),
			ApiBreakPointInfo(L"",L"Heap32ListNext",L""),
			ApiBreakPointInfo(L"",L"Heap32First",L""),
			ApiBreakPointInfo(L"",L"Heap32Next",L""),
			ApiBreakPointInfo(L"",L"Module32First",L""),
			ApiBreakPointInfo(L"",L"Module32Next",L""),
			ApiBreakPointInfo(L"",L"Module32FirstW",L""),
			ApiBreakPointInfo(L"",L"Module32NextW",L""),
			ApiBreakPointInfo(L"",L"Process32First",L""),
			ApiBreakPointInfo(L"",L"Process32FirstW",L""),
			ApiBreakPointInfo(L"",L"Process32Next",L""),
			ApiBreakPointInfo(L"",L"Process32NextW",L""),
			ApiBreakPointInfo(L"",L"PostQuitMessage",L""),
			ApiBreakPointInfo(L"",L"Toolhelp32ReadProcessMemory",L""),
			ApiBreakPointInfo(L"",L"ZwQueryInformationProcess",L""),
			ApiBreakPointInfo(L"",L"SetTimer",L""),
			ApiBreakPointInfo(L"",L"GetTempPathA",L""),
			ApiBreakPointInfo(L"",L"GetTempPathW",L""),
			ApiBreakPointInfo(L"",L"ReadFile",L""),
			ApiBreakPointInfo(L"",L"WriteFile",L""),
			ApiBreakPointInfo(L"",L"ShellExecuteA",L""),
			ApiBreakPointInfo(L"",L"ShellExecuteW",L""),
			ApiBreakPointInfo(L"",L"WinExec",L""),
			ApiBreakPointInfo(L"",L"DeleteFileA",L""),
			ApiBreakPointInfo(L"",L"DeleteFileW",L""),
			ApiBreakPointInfo(L"",L"UrlDownloadToFileA",L""),
			ApiBreakPointInfo(L"",L"UrlDownloadToFileW",L""),
			ApiBreakPointInfo(L"",L"GetDesktopWindow",L""),
		}
	},
	{
		"VB",
		{
			ApiBreakPointInfo(L"",L"FindResourceA",L""),
			ApiBreakPointInfo(L"",L"CreateFileA",L""),
			ApiBreakPointInfo(L"",L"GetVolumeInformation",L""),
			ApiBreakPointInfo(L"",L"MultiByteToWideChar",L""),
			ApiBreakPointInfo(L"",L"WideCharToMultiByte",L""),
			ApiBreakPointInfo(L"",L"GetFileSize",L""),
			ApiBreakPointInfo(L"",L"VarCyFromStr",L""),
			ApiBreakPointInfo(L"",L"arBstrFromI2",L""),
			ApiBreakPointInfo(L"",L"rtcFileLen",L""),
			ApiBreakPointInfo(L"",L"rtcFileLenth",L""),
			ApiBreakPointInfo(L"",L"rtcMsgBox",L""),
			ApiBreakPointInfo(L"",L"rtcInputBox",L""),
			ApiBreakPointInfo(L"",L"GetMemStr",L""),
			ApiBreakPointInfo(L"",L"rtcR8ValFromBstr",L""),
			ApiBreakPointInfo(L"",L"rtcGetPresendDate",L""),
			ApiBreakPointInfo(L"",L"rtcBeep",L""),
			ApiBreakPointInfo(L"",L"rtcTrimBstr",L""),
			ApiBreakPointInfo(L"",L"rtcMidCharVar",L""),
			ApiBreakPointInfo(L"",L"rtcLeftCharVar",L""),
			ApiBreakPointInfo(L"",L"rtcRightCharVar",L""),
			ApiBreakPointInfo(L"",L"StrConv",L"转换字符串"),
			ApiBreakPointInfo(L"",L"rtcT8ValFromBstr",L""),
			ApiBreakPointInfo(L"",L"__vbaI2Str",L""),
			ApiBreakPointInfo(L"",L"__vbaI4Str",L""),
			ApiBreakPointInfo(L"",L"__vbar4Str",L""),
			ApiBreakPointInfo(L"",L"__vbar8Str",L""),
			ApiBreakPointInfo(L"",L"__vbavaradd",L""),
			ApiBreakPointInfo(L"",L"__vbavarsub",L""),
			ApiBreakPointInfo(L"",L"__vbavarmul",L""),
			ApiBreakPointInfo(L"",L"__vbavaridiv",L""),
			ApiBreakPointInfo(L"",L"__vbavarxor",L""),
			ApiBreakPointInfo(L"",L"__vbavarcat",L""),
			ApiBreakPointInfo(L"",L"__vbaStrCat",L""),
			ApiBreakPointInfo(L"",L"__vbaVarCat",L""),
			ApiBreakPointInfo(L"",L"__vbaStrCmp",L""),
			ApiBreakPointInfo(L"",L"__vbaStrComp",L""),
			ApiBreakPointInfo(L"",L"__vbaVarTstEq",L""),
			ApiBreakPointInfo(L"",L"__vbaFreeStr",L""),
			ApiBreakPointInfo(L"",L"__vbaStrCopy",L""),
			ApiBreakPointInfo(L"",L"__vbaLenBstr",L""),
			ApiBreakPointInfo(L"",L"__vbavarfornext",L""),
			ApiBreakPointInfo(L"",L"__vbafreeobj",L""),
		}
	},
	{
		"VC/MFC",
		{
			ApiBreakPointInfo(L"",L"lstrcmpA",L""),
			ApiBreakPointInfo(L"",L"DefWindowProcA",L""),
			ApiBreakPointInfo(L"",L"DefWindowProcW",L""),
			ApiBreakPointInfo(L"",L"RegisterClassA",L""),
			ApiBreakPointInfo(L"",L"RegisterClassW",L""),
			ApiBreakPointInfo(L"",L"RegisterClassExA",L""),
			ApiBreakPointInfo(L"",L"RegisterClassExW",L""),
			ApiBreakPointInfo(L"",L"FindResourceA",L""),
			ApiBreakPointInfo(L"",L"FindResourceW",L""),
			ApiBreakPointInfo(L"",L"LoadResource",L""),
			ApiBreakPointInfo(L"",L"SetHandleCount",L""),
			ApiBreakPointInfo(L"",L"IsWindowEnabled",L""),
		}
	},
	{
		"Unpack",
		{
			ApiBreakPointInfo(L"",L"CreateThread",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L""),
			ApiBreakPointInfo(L"",L"OpenMutexA",L""),
			ApiBreakPointInfo(L"",L"GetSystemTime",L""),
			ApiBreakPointInfo(L"",L"VirtualProtect",L""),
			ApiBreakPointInfo(L"",L"CreateFileMappingA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExW",L""),
			ApiBreakPointInfo(L"",L"RefreshDeviceList",L""),
			ApiBreakPointInfo(L"",L"DeviceIoControl",L""),
			ApiBreakPointInfo(L"",L"Prestochangoselector",L""),
			ApiBreakPointInfo(L"",L"FreeEnvironmentStringsA",L""),
			ApiBreakPointInfo(L"",L"GetLogicalDriveStringsA",L""),
			ApiBreakPointInfo(L"",L"GetLogicalDrives",L""),
			ApiBreakPointInfo(L"",L"GetDriveTypeA",L""),
			ApiBreakPointInfo(L"",L"CreateFileA",L""),
			ApiBreakPointInfo(L"",L"FindFirstFileA",L""),
		}
	},
	{
		"Reserve",
		{
		}
	},
	{
		"Reserve2",
		{
		}
	},
};


#endif




DWORD WINAPI MsgLoopThread(LPVOID);
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam);

void UpdateCheckBoxs();
void RecreateFont();
void CleanupBreakpoints();
void AdjustLayout(HWND);

DpiState g_dpi = {
	.current = 96,    // 初始DPI值
	.scaling = 1.0f,  // 初始缩放因子
	.font = nullptr   // 初始字体句柄
};


//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
	HRESULT hr = SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	if (FAILED(hr))
		std::cerr << "SetProcessDpiAwareness failed with error: " << hr << std::endl;
	{
		SetProcessDPIAware();
	}
	return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{
}

//Do GUI/Menu related things here.
void pluginSetup()
{
	_plugin_menuaddentry(hMenu, MENU_MAINWINDOW_POPUP, "Set Api Breakpoint");
}


// 封装宽字符转 UTF-8
std::string WideToUTF8(const std::wstring& wstr)
{
	int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);
	std::string str(size, 0);
	WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, nullptr, nullptr);
	return str;
}


// doing msg loop like WinMain
DWORD WINAPI MsgLoopThread(LPVOID)
{
	MSG msg;
	WNDCLASSW wc = { 0 };
	HWND hwnd;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInstance;
	wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);

	wc.lpszMenuName = NULL;
	wc.lpszClassName = ClASS_NAME;

	RegisterClassW(&wc);

	// 获取当前DPI并进行缩放
	// 获取初始DPI
	
	HDC hdc = GetDC(nullptr);
	g_dpi.current = GetDeviceCaps(hdc, LOGPIXELSX);
	ReleaseDC(nullptr, hdc);
	g_dpi.scaling = g_dpi.current / 96.0f;
	hwnd = CreateWindowW(
		ClASS_NAME, 
		L"API断点管理器",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT,
		MulDiv(MAINWINDOW_WIDTH, g_dpi.current, 96),
		MulDiv(MAINWINDOW_HEIGHT, g_dpi.current, 96),
		nullptr,
		nullptr, 
		g_hInstance, 
		//GetModuleHandle(nullptr),
		nullptr);

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	bIsMainWindowShow = false;

	return (int)msg.wParam;
}

VOID CenterDialog(HWND hDlg)
{
	HWND hwndOwner = NULL;
	RECT rcOwner, rcDlg, rc;
	// Get the owner window and dialog box rectangles. 			
	if ((hwndOwner = GetParent(hDlg)) == NULL)
	{
		hwndOwner = GetDesktopWindow();
	}
	GetWindowRect(hwndOwner, &rcOwner);
	GetWindowRect(hDlg, &rcDlg);
	CopyRect(&rc, &rcOwner);

	// Offset the owner and dialog box rectangles so that right and bottom 
	// values represent the width and height, and then offset the owner again 
	// to discard space taken up by the dialog box. 

	OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
	OffsetRect(&rc, -rc.left, -rc.top);
	OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

	// The new position is the sum of half the remaining space and the owner's 
	// original position. 

	SetWindowPos(hDlg,
		HWND_TOP,
		rcOwner.left + (rc.right / 2),
		rcOwner.top + (rc.bottom / 2),
		0, 0,          // Ignores size arguments. 
		SWP_NOSIZE);
}

// called when tab changed, update checkbox check(uncheck) and show(hide) state
void UpdateCheckBoxs()
{
	CurTab = TabCtrl_GetCurSel(hTabControl);
	size_t len = Groups[CurTab].apiList.size();
	for (size_t i = 0; i < CHECK_COUNT; i++)
	{
		if (i < len)
		{
			SetWindowTextW(hChecks[i], std::wstring(Groups[CurTab].apiList[i].apiName + L"  " + Groups[CurTab].apiList[i].description).c_str());
			if (Groups[CurTab].apiList[i].bWantToSetBp)
			{
				CheckDlgButton(hMain, IDC_CHECKS[i], BST_CHECKED);
			}
			else
			{
				CheckDlgButton(hMain, IDC_CHECKS[i], BST_UNCHECKED);
			}
			ShowWindow(hChecks[i], SW_SHOW);
		}
		else
		{
			ShowWindow(hChecks[i], SW_HIDE);
		}
	}
}

// initialize main window, register tab control and checkbox
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	TCITEMW ti;
	static HFONT hFont = NULL;
	hMain = hwnd;

	switch (iMsg)
	{
	case WM_CREATE:
	{
		RecreateFont();
		//defaultFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

		InitCommonControls();

		hTabControl = CreateWindowW(
			L"SysTabControl32",
			L"",
			WS_CHILD | WS_VISIBLE | TCS_FIXEDWIDTH,
			0,
			0, 
			0,
			0, 
			hwnd, 
			//(HMENU)IDC_TABCTRL,
			reinterpret_cast<HMENU>(static_cast<UINT_PTR>(IDC_TABCTRL)),
			g_hInstance, nullptr);

		for (int i = 0; i < TAB_COUNT; i++)
		{
			ti.mask = TCIF_TEXT;
			ti.pszText = (LPWSTR)Groups[i].groupName.c_str();
			TabCtrl_InsertItem(hTabControl, i, &ti);
		}
		// 创建复选框
		for (int i = 0; i < CHECK_COUNT; i++) {
			hChecks[i] = CreateWindowW(
				L"BUTTON",
				L"",
				WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_MULTILINE,
				0, 0, 0, 0,
				hwnd,
				//(HMENU)IDC_CHECKS[i], 
				reinterpret_cast<HMENU>(static_cast<UINT_PTR>(IDC_CHECKS[i])),
				g_hInstance, nullptr);
			SendMessageW(hChecks[i], WM_SETFONT, (WPARAM)g_dpi.font, TRUE);
		}

		CenterDialog(hwnd);
		AdjustLayout(hwnd);
		UpdateCheckBoxs();
	}
	return 0;
	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{

		case TCN_SELCHANGE:
		{
			UpdateCheckBoxs();

			return TRUE;
		}
		}
	}
	return 0;
	case WM_COMMAND:
		for (int i = 0; i < CHECK_COUNT; i++)
		{
			if (LOWORD(wParam) == IDC_CHECKS[i])
			{
				if (IsDlgButtonChecked(hwnd, IDC_CHECKS[i]))
				{
					// checked, means user want to set bp here, then we'll try to use bp instruction set one.
					Groups[CurTab].apiList[i].bWantToSetBp = true;
					std::wstring wcmd = L"bp " + Groups[CurTab].apiList[i].apiName;
					std::string cmd = WideToUTF8(wcmd); // 封装编码转换
					Cmd(cmd.c_str());
				}
				else
				{
					Groups[CurTab].apiList[i].bWantToSetBp = false;
					std::wstring wcmd = L"bc " + Groups[CurTab].apiList[i].apiName;
					std::string cmd = WideToUTF8(wcmd); // 封装编码转换
					Cmd(cmd.c_str());
				}
			}
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
		return 0;
	case WM_SIZE:
		AdjustLayout(hwnd);
		return 0;

	case WM_DESTROY:
		if (g_dpi.font) DeleteObject(g_dpi.font);
		CleanupBreakpoints();
		PostQuitMessage(0);
		return 0;
	case WM_DPICHANGED:
	{
		g_dpi.current = HIWORD(wParam);
		g_dpi.scaling = g_dpi.current / 96.0f;
		RecreateFont();

		// 调整窗口位置和大小
		RECT* rc = (RECT*)lParam;
		SetWindowPos(hwnd, nullptr,
			rc->left, rc->top,
			rc->right - rc->left,
			rc->bottom - rc->top,
			SWP_NOZORDER | SWP_NOACTIVATE);

		AdjustLayout(hwnd);
		return 0;
	}
	}

	return DefWindowProcW(hwnd, iMsg, wParam, lParam);
}

// 布局调整
void AdjustLayout(HWND hwnd) {
	RECT rc;
	GetClientRect(hwnd, &rc);
	const int width = rc.right - rc.left;
	const int height = rc.bottom - rc.top;

	// Tab控件
	const int tabHeight = MulDiv(TAB_HEIGHT, g_dpi.current, 96);
	SetWindowPos(hTabControl, nullptr, 0, 0, width, tabHeight, SWP_NOZORDER);

	// 复选框布局
	const int margin = MulDiv(MARGIN_SIZE, g_dpi.current, 96);
	const int checkWidth = (width - margin * (CHECKBOX_COLUMNS + 1)) / CHECKBOX_COLUMNS;
	const int checkHeight = MulDiv(CHECKBOX_HEIGHT, g_dpi.current, 96);

	// 调整Tab控件
	SetWindowPos(hTabControl, NULL,
		0, 0, rc.right, tabHeight,
		SWP_NOZORDER | SWP_NOACTIVATE);

	for (int i = 0; i < CHECK_COUNT; i++) {
		int col = i % CHECKBOX_COLUMNS;
		int row = i / CHECKBOX_COLUMNS;
		int x = margin + col * (checkWidth + margin);
		int y = tabHeight + margin + row * (checkHeight + margin);

		SetWindowPos(hChecks[i], nullptr,
			x, y, checkWidth, checkHeight,
			SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

// 在程序退出或崩溃前的清理函数中
void CleanupBreakpoints()
{
	_plugin_logprintf("[+] Cleaning up breakpoints...\n");

	for (auto& group : Groups)
	{
		for (auto& api : group.apiList)
		{
			if (api.bWantToSetBp)
			{
				std::string cmd = "bc " + WideToUTF8(api.apiName);
				Cmd(cmd.c_str());
				api.bWantToSetBp = false;
			}
				
		}
	}
}

// 字体管理
void RecreateFont() {
	if (g_dpi.font) DeleteObject(g_dpi.font);

	LOGFONTW lf = {
		.lfHeight = -MulDiv(12, g_dpi.current, 96),
		.lfWeight = FW_NORMAL,
		.lfCharSet = GB2312_CHARSET,
		.lfQuality = CLEARTYPE_QUALITY,
	};
	wcscpy_s(lf.lfFaceName, L"Microsoft YaHei UI");

	g_dpi.font = CreateFontIndirectW(&lf);

	// 更新所有控件字体
	EnumChildWindows(hMain, [](HWND hwnd, LPARAM) -> BOOL {
		SendMessageW(hwnd, WM_SETFONT, (WPARAM)g_dpi.font, TRUE);
		return TRUE;
		}, 0);
}




// set delay breakpoint when dll loaded
EXTERN_C __declspec(dllexport) void _cdecl CBLOADDLL(
	CBTYPE bType, // event type, one of the `cbType` CB_* values listed above
	PLUG_CB_LOADDLL* callbackInfo // pointer to a structure of information
)
{
	// check if there are any breakpoint can be set
	for (size_t i = 0; i < TAB_COUNT; i++)
	{
		for (size_t j = 0; j < Groups[i].apiList.size(); j++)
		{
			if (Groups[i].apiList[j].bWantToSetBp)
			{
				std::wstring wcmd = L"bp " + Groups[i].apiList[j].apiName;
				int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wcmd.c_str(), -1, NULL, 0, NULL, NULL);
				std::string cmd(sizeNeeded, 0);
				WideCharToMultiByte(CP_UTF8, 0, wcmd.c_str(), -1, &cmd[0], sizeNeeded, NULL, NULL);
				Cmd(cmd.c_str());
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
}



