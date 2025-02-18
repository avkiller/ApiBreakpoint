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
		L"��������",
		{
			ApiBreakPointInfo(L"",L"LoadLibraryA",L"����DLL"),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExW",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L"����ģ��"),
			ApiBreakPointInfo(L"",L"GetModuleHandleW",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleExA",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleExW",L""),
			ApiBreakPointInfo(L"",L"CreateFileA",L"��������ļ�"),
			ApiBreakPointInfo(L"",L"CreateFileW",L""),
			ApiBreakPointInfo(L"",L"ReadFile",L"���ļ�"),
			ApiBreakPointInfo(L"",L"WriteFile",L"д�ļ�"),
			ApiBreakPointInfo(L"",L"DeleteFileA",L"ɾ���ļ�"),
			ApiBreakPointInfo(L"",L"DeleteFileW",L""),
			ApiBreakPointInfo(L"",L"CreateThread",L"�����߳�"),
			ApiBreakPointInfo(L"",L"CreateRemoteThread",L""),
			ApiBreakPointInfo(L"",L"OpenProcess",L"�򿪽���"),
			ApiBreakPointInfo(L"",L"WriteProcessMemory",L"�����д�ڴ�"),
			ApiBreakPointInfo(L"",L"ReadProcessMemory",L"����̶��ڴ�"),
			ApiBreakPointInfo(L"",L"CreateMutexA",L"����������"),
			ApiBreakPointInfo(L"",L"CreateMutexW",L""),
			ApiBreakPointInfo(L"",L"OpenMutexA",L"�򿪻�����"),
			ApiBreakPointInfo(L"",L"OpenMutexW",L""),
			ApiBreakPointInfo(L"",L"CreateToolhelp32SnapShot",L"ö�ٽ���"),
			ApiBreakPointInfo(L"",L"Heap32ListFirst",L""),
			ApiBreakPointInfo(L"",L"Heap32ListNext",L""),
			ApiBreakPointInfo(L"",L"Heap32First",L""),
			ApiBreakPointInfo(L"",L"Heap32Next",L""),
			ApiBreakPointInfo(L"",L"Module32First",L"ö��ģ��"),
			ApiBreakPointInfo(L"",L"Module32Next",L""),
			ApiBreakPointInfo(L"",L"Module32FirstW",L""),
			ApiBreakPointInfo(L"",L"Module32NextW",L""),
			ApiBreakPointInfo(L"",L"Process32First",L"ö�ٽ���"),
			ApiBreakPointInfo(L"",L"Process32FirstW",L""),
			ApiBreakPointInfo(L"",L"Process32Next",L""),
			ApiBreakPointInfo(L"",L"Process32NextW",L""),
			ApiBreakPointInfo(L"",L"PostQuitMessage",L""),
			ApiBreakPointInfo(L"",L"Toolhelp32ReadProcessMemory",L""),
			ApiBreakPointInfo(L"",L"ZwQueryInformationProcess",L""),
			ApiBreakPointInfo(L"",L"SetTimer",L""),
			ApiBreakPointInfo(L"",L"GetTempPathA",L""),
			ApiBreakPointInfo(L"",L"GetTempPathW",L""),
			ApiBreakPointInfo(L"",L"CreateProcessA",L"��������"),
			ApiBreakPointInfo(L"",L"CreateProcessW",L""),
			ApiBreakPointInfo(L"",L"ShellExecuteA",L""),
			ApiBreakPointInfo(L"",L"ShellExecuteW",L""),
			ApiBreakPointInfo(L"",L"WinExec",L""),
			ApiBreakPointInfo(L"",L"UrlDownloadToFileA",L"�����ļ�"),
			ApiBreakPointInfo(L"",L"UrlDownloadToFileW",L""),
			ApiBreakPointInfo(L"",L"GetDesktopWindow",L""),
		}
	},
	{
		// https://anti-debug.checkpoint.com/
		L"�Կ�����",
		{
			ApiBreakPointInfo(L"",L"IsDebuggerPresent",L"��������"),
			ApiBreakPointInfo(L"",L"CheckRemoteDebuggerPresent",L"��������"),
			ApiBreakPointInfo(L"",L"NtQueryInformationProcess",L"�����־λ������������⸸����"),
			ApiBreakPointInfo(L"",L"UnhandledExceptionFilter",L""),
			ApiBreakPointInfo(L"",L"OutputDebugStringA",L"��������̽�������"),
			ApiBreakPointInfo(L"",L"OutputDebugStringW",L""),
			ApiBreakPointInfo(L"",L"GetThreadContext",L"̽��Ӳ���ϵ�"),
			ApiBreakPointInfo(L"",L"GetProcessHeap",L"LFH:����Ƭ��"),
			ApiBreakPointInfo(L"",L"GetProcAddress",L"���HOOK"),
			ApiBreakPointInfo(L"",L"NtSetInformationThread",L"������������߳�"),
			ApiBreakPointInfo(L"",L"NtYieldExecution",L"��������"),
			ApiBreakPointInfo(L"",L"ReadProcessMemory",L"int3�ϵ���"),
			ApiBreakPointInfo(L"",L"VirtualProtect",L"�޸��ڴ�Ȩ�޴����쳣��������"),
			ApiBreakPointInfo(L"",L"VirtualProtectEx",L"�޸��ڴ�Ȩ�޴����쳣��������"),
			ApiBreakPointInfo(L"",L"RtlQueryProcessHeapInformation",L"�ѱ�־λ��������"),
			ApiBreakPointInfo(L"",L"RtlQueryProcessDebugInformation",L"�ѱ�־λ��������"),
			ApiBreakPointInfo(L"",L"NtQuerySystemInformation",L"��������"),
			ApiBreakPointInfo(L"",L"OpenProcess",L"��csrss.exe�ɹ���ʾ�е�����"),
			ApiBreakPointInfo(L"",L"CreateFileA",L"��ռ��ʽ���ļ���ʧ�ܱ�ʾ�е�����"),
			ApiBreakPointInfo(L"",L"CreateFileW",L""),
			ApiBreakPointInfo(L"",L"NtClose",L"����EXCEPTION_INVALID_HANDLE�쳣��ʾ���ڵ�����"),
			ApiBreakPointInfo(L"",L"CloseHandle",L""),
			ApiBreakPointInfo(L"",L"NtQueryObject",L"̽����Զ���"),
			ApiBreakPointInfo(L"",L"SetUnhandledExceptionFilter",L"������ڵ���������ص�������"),
			ApiBreakPointInfo(L"",L"RaiseException",L"�׳�DBG_CONTROL_C�������쳣��δ�ӹ����е�����"),
			ApiBreakPointInfo(L"",L"RtlRaiseException",L"�׳�DBG_CONTROL_C�������쳣��δ�ӹ����е�����"),
			ApiBreakPointInfo(L"",L"GetLocalTime",L"ִ�к�ʱ���㣬��ʱ��Ϊ������"),
			ApiBreakPointInfo(L"",L"GetSystemTime",L"ִ�к�ʱ���㣬��ʱ��Ϊ������"),
			ApiBreakPointInfo(L"",L"GetTickCount",L"ִ�к�ʱ���㣬��ʱ��Ϊ������"),
			ApiBreakPointInfo(L"",L"NtGetTickCount",L"ִ�к�ʱ���㣬��ʱ��Ϊ������"),
			ApiBreakPointInfo(L"",L"QueryPerformanceCounter",L"ִ�к�ʱ���㣬��ʱ��Ϊ������"),
			ApiBreakPointInfo(L"",L"timeGetTime",L"ִ�к�ʱ���㣬��ʱ��Ϊ������"),
			ApiBreakPointInfo(L"",L"NtQueryVirtualMemory",L"�ڴ�ϵ���"),
			ApiBreakPointInfo(L"",L"AddVectoredExceptionHandler",L"ע��VEH"),
			ApiBreakPointInfo(L"",L"RemoveVectoredExceptionHandler",L"�Ƴ�VEH"),
			ApiBreakPointInfo(L"",L"DebugActiveProcess",L"�����ӽ��̵����Լ�"),
			ApiBreakPointInfo(L"",L"DbgUiDebugActiveProcess",L"�����ӽ��̵����Լ�"),
			ApiBreakPointInfo(L"",L"NtDebugActiveProcess",L"�����ӽ��̵����Լ�"),
			ApiBreakPointInfo(L"",L"SetConsoleCtrlHandler",L"����CTRL_C_EVENT�쳣"),
			ApiBreakPointInfo(L"",L"BlockInput",L"һֱ����TRUE������HOOK"),
			ApiBreakPointInfo(L"",L"EnumWindows",L"��⸸���̴��ڱ���"),
			ApiBreakPointInfo(L"",L"SwitchDesktop",L"�л����淴����"),
			ApiBreakPointInfo(L"",L"FindWindowA",L"̽���������������"),
			ApiBreakPointInfo(L"",L"FindWindowW",L"̽���������������"),
			ApiBreakPointInfo(L"",L"FindWindowExA",L"̽���������������"),
			ApiBreakPointInfo(L"",L"FindWindowExW",L"̽���������������"),
			ApiBreakPointInfo(L"",L"DbgSetDebugFilterState",L""),
			ApiBreakPointInfo(L"",L"SwitchToThread",L""),
			ApiBreakPointInfo(L"",L"GetWriteWatch",L""),
		}
	},
	{
		L"ע��",
		{
		}
	},
	{
		L"����",
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
		L"�Ի���",
		{
			ApiBreakPointInfo(L"",L"MessageBoxA",L"���Ի���"),
			ApiBreakPointInfo(L"",L"MessageBoxW",L""),
			ApiBreakPointInfo(L"",L"MessageBoxExA",L""),
			ApiBreakPointInfo(L"",L"MessageBoxExW",L""),
			ApiBreakPointInfo(L"",L"MessageBoxIndirectA",L""),
			ApiBreakPointInfo(L"",L"MessageBoxIndirectW",L""),
			ApiBreakPointInfo(L"",L"CreateWindowExA",L"��������"),
			ApiBreakPointInfo(L"",L"CreateWindowExW",L""),
			ApiBreakPointInfo(L"",L"CreateWindowA",L""),
			ApiBreakPointInfo(L"",L"CreateWindowW",L""),
			ApiBreakPointInfo(L"",L"DestroyWindow",L"���ٴ���"),
			ApiBreakPointInfo(L"",L"DialogBoxA",L"ģ̬�Ի���"),
			ApiBreakPointInfo(L"",L"DialogBoxW",L""),
			ApiBreakPointInfo(L"",L"DialogBoxParamA",L""),
			ApiBreakPointInfo(L"",L"DialogBoxParamW",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectA",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectW",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectParamA",L""),
			ApiBreakPointInfo(L"",L"DialogBoxIndirectParamW",L""),
			ApiBreakPointInfo(L"",L"EndDialog",L"���ģ̬�Ի���"),
			ApiBreakPointInfo(L"",L"GetWindowTextA",L"ȡ�ؼ��ı�"),
			ApiBreakPointInfo(L"",L"GetWindowTextW",L""),
			ApiBreakPointInfo(L"",L"SetWindowTextA",L"���ÿؼ��ı�"),
			ApiBreakPointInfo(L"",L"SetWindowTextW",L""),
			ApiBreakPointInfo(L"",L"GetDlgItemTextA",L"ȡ�ؼ��ı�"),
			ApiBreakPointInfo(L"",L"GetDlgItemTextW",L""),
			ApiBreakPointInfo(L"",L"SetDlgItemTextA",L"���ÿؼ��ı�"),
			ApiBreakPointInfo(L"",L"SetDlgItemTextW",L""),
			ApiBreakPointInfo(L"",L"GetDlgItemInt",L"ȡ�ؼ�������"),
			ApiBreakPointInfo(L"",L"SetDlgItemInt",L"���ÿؼ�������"),
			ApiBreakPointInfo(L"",L"SendMessageA",L"����Ϣ"),
			ApiBreakPointInfo(L"",L"SendMessageW",L"����Ϣ"),
			ApiBreakPointInfo(L"",L"SendDlgItemMessage",L"����Ϣ"),
			ApiBreakPointInfo(L"",L"IsDialogMessageA",L""),
			ApiBreakPointInfo(L"",L"IsDialogMessageW",L""),
			ApiBreakPointInfo(L"",L"DispatchMessageA",L"��Ϣ�ɷ�"),
			ApiBreakPointInfo(L"",L"DispatchMessageW",L""),
			ApiBreakPointInfo(L"",L"CallWindowProcA",L"���ô��ڹ���"),
			ApiBreakPointInfo(L"",L"CallWindowProcW",L""),
			ApiBreakPointInfo(L"",L"DefWindowProcA",L"Ĭ����Ϣ����"),
			ApiBreakPointInfo(L"",L"DefWindowProcW",L""),
			ApiBreakPointInfo(L"",L"MessageBeep",L"ϵͳ������"),
			ApiBreakPointInfo(L"",L"DrawTextA",L"�ı���浽������"),
			ApiBreakPointInfo(L"",L"DrawTextW",L""),
			ApiBreakPointInfo(L"",L"DrawTextExA",L""),
			ApiBreakPointInfo(L"",L"DrawTextExW",L""),
			ApiBreakPointInfo(L"",L"FindWindowA",L"��ȡ���ھ��"),
			ApiBreakPointInfo(L"",L"FindWindowW",L""),
			ApiBreakPointInfo(L"",L"GetClipboardData",L"��ȡ����������"),
			ApiBreakPointInfo(L"",L"CoInitialize",L"��ʼ��COM��"),
			ApiBreakPointInfo(L"",L"CoCreateInstance",L"����COM����"),
			ApiBreakPointInfo(L"",L"LoadIconA",L"����ͼ����Դ"),
			ApiBreakPointInfo(L"",L"LoadIconW",L""),
			ApiBreakPointInfo(L"",L"SetWindowLongA",L"���ô�������"),
			ApiBreakPointInfo(L"",L"SetWindowLongW",L""),
			ApiBreakPointInfo(L"",L"SetWindowLongPtrA",L""),
			ApiBreakPointInfo(L"",L"SetWindowLongPtrW",L""),
			ApiBreakPointInfo(L"",L"ShowWindow",L"��ʾ����"),
			ApiBreakPointInfo(L"",L"UpdateWindow",L"���´���"),
			ApiBreakPointInfo(L"",L"TranslateMessage",L"��Ϣ����"),
			ApiBreakPointInfo(L"",L"GetMessageA",L"ȡ��Ϣ����"),
			ApiBreakPointInfo(L"",L"GetMessageW",L""),
			ApiBreakPointInfo(L"",L"PeekMessageA",L"����Ϣ����"),
			ApiBreakPointInfo(L"",L"PeekMessageW",L""),
			ApiBreakPointInfo(L"",L"SetMenu",L"���˵�"),
			ApiBreakPointInfo(L"",L"DestroyMenu",L"ɾ���˵�"),
			ApiBreakPointInfo(L"",L"DeleteMenu",L"ɾ���˵���"),
			ApiBreakPointInfo(L"",L"EnableMenuItem",L"�˵����ޱ��"),
			ApiBreakPointInfo(L"",L"EnableWindow",L"��ֹ�����������"),
		}
	},
	{
		L"�ļ�����",
		{
			ApiBreakPointInfo(L"",L"CreateFileA",L"��������ļ�"),
			ApiBreakPointInfo(L"",L"CreateFileW",L""),
			ApiBreakPointInfo(L"",L"OpenFile",L"���ļ�"),
			ApiBreakPointInfo(L"",L"ReadFile",L"���ļ�"),
			ApiBreakPointInfo(L"",L"WriteFile",L"д�ļ�"),
			ApiBreakPointInfo(L"",L"GetFileSize",L"ȡ�ļ���С"),
			ApiBreakPointInfo(L"",L"FindFirstFileA",L"�����ļ�"),
			ApiBreakPointInfo(L"",L"FindFirstFileW",L""),
			ApiBreakPointInfo(L"",L"GetModuleFileNameA",L"��ģ��·��"),
			ApiBreakPointInfo(L"",L"GetModuleFileNameW",L""),
			ApiBreakPointInfo(L"",L"OpenFileMappingA",L"���ļ�ӳ�����"),
			ApiBreakPointInfo(L"",L"OpenFileMappingW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryA",L"����DLL"),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExW",L""),
			ApiBreakPointInfo(L"",L"CreateFileMappingA",L"�����ļ�����"),
			ApiBreakPointInfo(L"",L"CreateFileMappingW",L""),
			ApiBreakPointInfo(L"",L"CopyFileA",L"�����ļ�"),
			ApiBreakPointInfo(L"",L"CopyFileW",L""),
			ApiBreakPointInfo(L"",L"CopyFileExA",L""),
			ApiBreakPointInfo(L"",L"CopyFileExW",L""),
			ApiBreakPointInfo(L"",L"SetFilePointer",L"�����ļ���дλ��"),
			ApiBreakPointInfo(L"",L"MoveFileA",L"�ƶ��ļ�"),
			ApiBreakPointInfo(L"",L"MoveFileW",L""),
			ApiBreakPointInfo(L"",L"MoveFileExA",L""),
			ApiBreakPointInfo(L"",L"MoveFileExW",L""),
			ApiBreakPointInfo(L"",L"DeleteFileA",L""),
			ApiBreakPointInfo(L"",L"DeleteFileW",L"ɾ���ļ�"),
			ApiBreakPointInfo(L"",L"LoadCursorFromFileA",L"�����ļ����"),
			ApiBreakPointInfo(L"",L"LoadCursorFromFileW",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringA",L"INI������֤����"),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringW",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileIntA",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileIntW",L""),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringA",L"ȡ�ַ���"),
			ApiBreakPointInfo(L"",L"GetPrivateProfileStringW",L""),
			ApiBreakPointInfo(L"",L"WritePrivateProfileStringA",L"�����ַ���"),
			ApiBreakPointInfo(L"",L"WritePrivateProfileStringW",L""),
		}
	},
	{
		L"ʱ�䴦��",
		{
			ApiBreakPointInfo(L"",L"SetSystemTime",L"����ϵͳʱ��"),
			ApiBreakPointInfo(L"",L"TimerProc",L"�ص�����"),//what is this
			ApiBreakPointInfo(L"",L"SetLocalTime",L"���ñ���ʱ��"),
			ApiBreakPointInfo(L"",L"GetLocalTime",L"��ȡ����ʱ��"),
			ApiBreakPointInfo(L"",L"GetSystemTime",L"��ȡϵͳʱ��"),
			ApiBreakPointInfo(L"",L"GetCurrentTime",L"��ȡϵͳʱ��"),
			ApiBreakPointInfo(L"",L"GetFileTime",L"��ȡ�ļ�ʱ��"),
			ApiBreakPointInfo(L"",L"GetTickCount",L"ϵͳ�������������ĺ�����"),
			ApiBreakPointInfo(L"",L"CompareFileTime",L"�Ƚ��ļ�ʱ��"),
			ApiBreakPointInfo(L"",L"SetTimer",L"������ʱ��"),
			ApiBreakPointInfo(L"",L"KillTimer",L"�Ƴ���ʱ��"),
			ApiBreakPointInfo(L"",L"timeSetEvent",L"��ý�嶨ʱ��"),
		}
	},
	{
		L"���̺���",
		{
			ApiBreakPointInfo(L"",L"CreateThread",L"�����߳�"),
			ApiBreakPointInfo(L"",L"CreateRemoteThread",L"����Զ���߳�"),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L"��ģ���ַ"),
			ApiBreakPointInfo(L"",L"GetModuleHandleW",L""),
			ApiBreakPointInfo(L"",L"OpenMutexA",L"�򿪻�����"),
			ApiBreakPointInfo(L"",L"OpenMutexW",L""),
			ApiBreakPointInfo(L"",L"WriteProcessMemory",L"д�ڴ�"),
			ApiBreakPointInfo(L"",L"ZwWriteVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"ReadProcessMemory",L"���ڴ�"),
			ApiBreakPointInfo(L"",L"ZwReadVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"CreateProcessA",L"��������"),
			ApiBreakPointInfo(L"",L"CreateProcessW",L""),
			ApiBreakPointInfo(L"",L"OpenProcess",L"�򿪽���"),
			ApiBreakPointInfo(L"",L"ExitProcess",L"��������"),
			ApiBreakPointInfo(L"",L"ExitThread",L"������ǰ�߳�"),
			ApiBreakPointInfo(L"",L"TerminateProcess",L"��ֹ����"),
			ApiBreakPointInfo(L"",L"TerminateThread",L"��ֹ�߳�"),
			ApiBreakPointInfo(L"",L"CreateToolhelp32Snapshot",L"���̿���"),
			ApiBreakPointInfo(L"",L"Process32First",L"ȡ���̾��"),
			ApiBreakPointInfo(L"",L"Process32FirstW",L""),
			ApiBreakPointInfo(L"",L"Process32Next",L""),
			ApiBreakPointInfo(L"",L"Process32NextW",L""),
			ApiBreakPointInfo(L"",L"Module32Next",L"ȡģ����"),
			ApiBreakPointInfo(L"",L"Module32NextW",L""),
			ApiBreakPointInfo(L"",L"Module32First",L""),
			ApiBreakPointInfo(L"",L"Module32FirstW",L""),
			ApiBreakPointInfo(L"",L"Heap32ListFirst",L"ȡ�Ѿ��"),
			ApiBreakPointInfo(L"",L"Heap32ListNext",L"ȡ�Ѿ��"),
			ApiBreakPointInfo(L"",L"GetProcessHeap",L"ȡ�Ѿ��"),
			ApiBreakPointInfo(L"",L"Heap32First",L"ȡ���̶������Ϣ"),
			ApiBreakPointInfo(L"",L"Heap32Next",L"ȡ���̶������Ϣ"),
			ApiBreakPointInfo(L"",L"PostQuitMessage",L"��ֹ�߳�"),
			ApiBreakPointInfo(L"",L"IsDebuggerPresent",L"��������"),
			ApiBreakPointInfo(L"",L"OpenProcessToken",L"��ȡ���̷������ƾ��"),
			ApiBreakPointInfo(L"",L"OpenThreadToken",L""),
			ApiBreakPointInfo(L"",L"ZwQueryInformationProcess",L""),
		}
	},
	{
		L"�ڴ�",
		{
			ApiBreakPointInfo(L"",L"FindWindow",L"ȡ���ھ��"),
			ApiBreakPointInfo(L"",L"GetWindowThreadProcessId",L"��ȡ���ڽ���ID"),
			ApiBreakPointInfo(L"",L"OpenProcess",L"�򿪽���"),
			ApiBreakPointInfo(L"",L"VirtualAllocEx",L"�����ڴ�"),
			ApiBreakPointInfo(L"",L"WriteProcessMemory",L"д�ڴ�"),
			ApiBreakPointInfo(L"",L"ReadProcessMemory",L"���ڴ�"),
			ApiBreakPointInfo(L"",L"CreateRemoteThread",L"����Զ���߳�"),
			ApiBreakPointInfo(L"",L"CreateMutexA",L"����������"),
			ApiBreakPointInfo(L"",L"CreateMutexW",L""),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L"��ģ���ַ"),
			ApiBreakPointInfo(L"",L"GetModuleHandleW",L""),
			ApiBreakPointInfo(L"",L"GetProcAddress",L"�麯����ַ"),
			ApiBreakPointInfo(L"",L"VirtualAlloc",L"�����ڴ�"),
			ApiBreakPointInfo(L"",L"HeapAlloc",L""),
			ApiBreakPointInfo(L"",L"VirtualFree",L"�ͷ��ڴ�"),
			ApiBreakPointInfo(L"",L"HeapFree",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryA",L"����DLL"),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"GetVersion",L"��ȡϵͳ�汾"),
			ApiBreakPointInfo(L"",L"ZwAllocateVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"ZwWriteVirtualMemory",L""),
			ApiBreakPointInfo(L"",L"ZwReadVirtualMemory",L""),
		}
	},
	{
		L"����",
		{
			ApiBreakPointInfo(L"",L"GetDiskFreeSpaceA",L"��ȡ������Ϣ"),
			ApiBreakPointInfo(L"",L"GetDiskFreeSpaceExA",L""),
			ApiBreakPointInfo(L"",L"GetDiskFreeSpaceExW",L""),
			ApiBreakPointInfo(L"",L"GetDriveTypeA",L"�жϴ�������������"),
			ApiBreakPointInfo(L"",L"GetDriveTypeW",L""),
			ApiBreakPointInfo(L"",L"GetLogicalDrives",L"ȡ��������ĸ"),
			ApiBreakPointInfo(L"",L"GetLogicalDriveStringsA",L"ȡ������·��"),
			ApiBreakPointInfo(L"",L"GetLogicalDriveStringsW",L""),
		}
	},
	{
		L"ע���",
		{
			ApiBreakPointInfo(L"",L"RegOpenKeyA",L"��ע�����"),
			ApiBreakPointInfo(L"",L"RegOpenKeyW",L""),
			ApiBreakPointInfo(L"",L"RegOpenKeyExA",L"������֤����"),
			ApiBreakPointInfo(L"",L"RegOpenKeyExW",L""),
			ApiBreakPointInfo(L"",L"RegCreateKeyExA",L""),
			ApiBreakPointInfo(L"",L"RegCreateKeyExW",L""),
			ApiBreakPointInfo(L"",L"RegQueryValueA",L"ȡע����ֵ"),
			ApiBreakPointInfo(L"",L"RegQueryValueW",L""),
			ApiBreakPointInfo(L"",L"RegQueryValueExA",L""),
			ApiBreakPointInfo(L"",L"RegQueryValueExW",L""),
			ApiBreakPointInfo(L"",L"RegCreateKeyA",L"��������"),
			ApiBreakPointInfo(L"",L"RegCreateKeyW",L""),
			ApiBreakPointInfo(L"",L"RegCloseKey",L"�ر�ע���"),
			ApiBreakPointInfo(L"",L"RegDeleteKeyA",L"ɾ����ֵ"),
			ApiBreakPointInfo(L"",L"RegDeleteKeyW",L""),
			ApiBreakPointInfo(L"",L"RegEnumKeyExA",L"ö������"),
			ApiBreakPointInfo(L"",L"RegEnumKeyExW",L""),
			ApiBreakPointInfo(L"",L"RegSetValueA",L"����Ĭ��ֵ"),
			ApiBreakPointInfo(L"",L"RegSetValueW",L""),
			ApiBreakPointInfo(L"",L"RegSetValueExA",L"����ָ�����ֵ"),
			ApiBreakPointInfo(L"",L"RegSetValueExW",L""),
		}
	},
	{
		L"VB",
		{
			ApiBreakPointInfo(L"",L"FindResourceA",L"VBУ��"),
			ApiBreakPointInfo(L"",L"CreateFileA",L"VBУ��"),
			ApiBreakPointInfo(L"",L"GetVolumeInformation",L"���̾�"),
			ApiBreakPointInfo(L"",L"MultiByteToWideChar",L"AתU�ַ���"),
			ApiBreakPointInfo(L"",L"WideCharToMultiByte",L"UתA�ַ���"),
			ApiBreakPointInfo(L"",L"GetFileSize",L"��ȡ�ļ���С"),
			ApiBreakPointInfo(L"",L"VarCyFromStr",L"�ַ���ת����"),
			ApiBreakPointInfo(L"",L"arBstrFromI2",L"����ת�ַ���"),
			ApiBreakPointInfo(L"",L"rtcFileLen",L"VB��У��"),
			ApiBreakPointInfo(L"",L"rtcFileLenth",L""),
			ApiBreakPointInfo(L"",L"rtcMsgBox",L"��ʾ�Ի���"),
			ApiBreakPointInfo(L"",L"rtcInputBox",L"�����"),
			ApiBreakPointInfo(L"",L"GetMemStr",L""),
			ApiBreakPointInfo(L"",L"rtcR8ValFromBstr",L""),
			ApiBreakPointInfo(L"",L"rtcGetPresendDate",L""),
			ApiBreakPointInfo(L"",L"rtcBeep",L""),
			ApiBreakPointInfo(L"",L"rtcTrimBstr",L"�ַ���ȥ���ո�"),
			ApiBreakPointInfo(L"",L"rtcMidCharVar",L"�ַ�����ȡ�ַ�"),
			ApiBreakPointInfo(L"",L"rtcLeftCharVar",L"�ַ������ȡ�ַ�"),
			ApiBreakPointInfo(L"",L"rtcRightCharVar",L"�ַ����ұ�ȡ�ַ�"),
			ApiBreakPointInfo(L"",L"StrConv",L"ת���ַ���"),
			ApiBreakPointInfo(L"",L"rtcT8ValFromBstr",L"�ַ���ת������"),
			ApiBreakPointInfo(L"",L"__vbaI2Str",L"�ַ�ת1�ֽ���ֵ"),
			ApiBreakPointInfo(L"",L"__vbaI4Str",L"�ַ�ת4�ֽ���ֵ"),
			ApiBreakPointInfo(L"",L"__vbar4Str",L"�ַ�ת4������"),
			ApiBreakPointInfo(L"",L"__vbar8Str",L"�ַ�ת8������"),
			ApiBreakPointInfo(L"",L"__vbavaradd",L"����ֵ���"),
			ApiBreakPointInfo(L"",L"__vbavarsub",L"����ֵ���"),
			ApiBreakPointInfo(L"",L"__vbavarmul",L"����ֵ���"),
			ApiBreakPointInfo(L"",L"__vbavaridiv",L"����ֵ���"),
			ApiBreakPointInfo(L"",L"__vbavarxor",L"����ֵ���"),
			ApiBreakPointInfo(L"",L"__vbavarcat",L"����ֵ����"),
			ApiBreakPointInfo(L"",L"__vbaStrCat",L"�ַ�������"),
			ApiBreakPointInfo(L"",L"__vbaVarCat",L"�����ִ�"),
			ApiBreakPointInfo(L"",L"__vbaStrCmp",L"�ַ����Ƚ�"),
			ApiBreakPointInfo(L"",L"__vbaStrComp",L"�ַ����Ƚ�"),
			ApiBreakPointInfo(L"",L"__vbaVarTstEq",L"�Ƚϱ���"),
			ApiBreakPointInfo(L"",L"__vbaFreeStr",L""),
			ApiBreakPointInfo(L"",L"__vbaStrCopy",L""),
			ApiBreakPointInfo(L"",L"__vbaLenBstr",L""),
			ApiBreakPointInfo(L"",L"__vbavarfornext",L"ѭ���ṹ"),
			ApiBreakPointInfo(L"",L"__vbafreeobj",L"�ͷŶ���"),
		}
	},
	{
		L"VC/MFC",
		{
			ApiBreakPointInfo(L"",L"lstrcmpA",L"�ַ����Ƚ�"),
			ApiBreakPointInfo(L"",L"DefWindowProcA",L"����Ĭ�ϴ��ڹ���"),
			ApiBreakPointInfo(L"",L"DefWindowProcW",L""),
			ApiBreakPointInfo(L"",L"RegisterClassA",L"ע�ᴰ����"),
			ApiBreakPointInfo(L"",L"RegisterClassW",L""),
			ApiBreakPointInfo(L"",L"RegisterClassExA",L""),
			ApiBreakPointInfo(L"",L"RegisterClassExW",L""),
			ApiBreakPointInfo(L"",L"FindResourceA",L"������Դ"),
			ApiBreakPointInfo(L"",L"FindResourceW",L""),
			ApiBreakPointInfo(L"",L"LoadResource",L"������Դ"),
			ApiBreakPointInfo(L"",L"SetHandleCount",L"ȡ������ַ"),
			ApiBreakPointInfo(L"",L"IsWindowEnabled",L"�ɷ��������"),
		}
	},
	{
		L"���ܹ�/��ɽ��",
		{
			ApiBreakPointInfo(L"",L"CreateThread",L"Ѱ��OEP"),
			ApiBreakPointInfo(L"",L"GetModuleHandleA",L"ħ����ת"),
			ApiBreakPointInfo(L"",L"OpenMutexA",L"1ת���������θ���"),
			ApiBreakPointInfo(L"",L"GetSystemTime",L"����KEY"),
			ApiBreakPointInfo(L"",L"VirtualProtect",L"2EAXΪ401000����"),
			ApiBreakPointInfo(L"",L"CreateFileMappingA",L"����"),
			ApiBreakPointInfo(L"",L"LoadLibraryA",L"����DLL"),
			ApiBreakPointInfo(L"",L"LoadLibraryW",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExA",L""),
			ApiBreakPointInfo(L"",L"LoadLibraryExW",L""),
			ApiBreakPointInfo(L"",L"RefreshDeviceList",L"��˼3"),
			ApiBreakPointInfo(L"",L"DeviceIoControl",L"�����豸"),
			ApiBreakPointInfo(L"",L"Prestochangoselector",L""),
			ApiBreakPointInfo(L"",L"FreeEnvironmentStringsA",L"�ͷŻ����ִ���"),
			ApiBreakPointInfo(L"",L"GetLogicalDriveStringsA",L"ȡ�б�"),
			ApiBreakPointInfo(L"",L"GetLogicalDrives",L"ȡ�������б�"),
			ApiBreakPointInfo(L"",L"GetDriveTypeA",L"�ж�����������"),
			ApiBreakPointInfo(L"",L"CreateFileA",L"��������"),
			ApiBreakPointInfo(L"",L"FindFirstFileA",L"�����ļ�"),
		}
	},
	{
		L"����",
		{
			ApiBreakPointInfo(L"",L"FindFirstFileA",L"�����ļ�"),
			ApiBreakPointInfo(L"",L"ExitProcess",L"�����˳�"),
			ApiBreakPointInfo(L"",L"GetStartupInfoA",L"��ȡ������Ϣ"),
			ApiBreakPointInfo(L"",L"GetFileSize",L"��ȡ�ļ���С"),
			ApiBreakPointInfo(L"",L"VirtualProtect",L"Vmp�ѿ�"),
			ApiBreakPointInfo(L"",L"ExitWindowsEx",L"�ػ��ϵ�"),
			ApiBreakPointInfo(L"",L"CreateFileA",L"��У��"),
			ApiBreakPointInfo(L"",L"GetVersion",L"�������ѿ�"),
			ApiBreakPointInfo(L"",L"send",L"���ͷ��"),
			ApiBreakPointInfo(L"",L"WSASend",L"���ͷ��"),
			ApiBreakPointInfo(L"",L"recv",L"���շ��"),
			ApiBreakPointInfo(L"",L"RtlAdjustPrivilege",L"�����Կ��ٹػ�"),
			ApiBreakPointInfo(L"",L"SHFormatDrive",L"����API"),
			ApiBreakPointInfo(L"",L"RemoveDirectoryA",L"ɾ��Ŀ¼"),
			ApiBreakPointInfo(L"",L"DeleteFileA",L"ɾ���ļ�"),
			ApiBreakPointInfo(L"",L"GetLastError",L"���ش�����"),
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
			ApiBreakPointInfo(L"",L"StrConv",L"ת���ַ���"),
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
	.current = 96,    // ��ʼDPIֵ
	.scaling = 1.0f,  // ��ʼ��������
	.font = nullptr   // ��ʼ������
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
{}

//Do GUI/Menu related things here.
void pluginSetup()
{
	_plugin_menuaddentry(hMenu, MENU_MAINWINDOW_POPUP, "Set Api Breakpoint");
}


// ��װ���ַ�ת UTF-8
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

	// ��ȡ��ǰDPI����������
	// ��ȡ��ʼDPI
	
	HDC hdc = GetDC(nullptr);
	g_dpi.current = GetDeviceCaps(hdc, LOGPIXELSX);
	ReleaseDC(nullptr, hdc);
	g_dpi.scaling = g_dpi.current / 96.0f;
	hwnd = CreateWindowW(
		ClASS_NAME, 
		L"API�ϵ������",
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
		// ������ѡ��
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
				bool bNewState = IsDlgButtonChecked(hwnd, IDC_CHECKS[i]) == BST_CHECKED;
				auto& apiInfo = Groups[CurTab].apiList[i];  // ������

				// ״̬δ�仯���Ѿ�ִ�гɹ�������
				if (apiInfo.bWantToSetBp == bNewState && apiInfo.bCmdSuccess) {
					//_plugin_logprintf("BP break\n");
					break;
				}

				// ��������
				//_plugin_logprintf("BP cmd start\n");
				std::wstring wcmd = bNewState ? L"bp " + apiInfo.apiName
					: L"bc " + apiInfo.apiName;

				// ת��Ϊ UTF-8
				std::string cmd = WideToUTF8(wcmd);

				// ִ�������¼���
				bool bSuccess = DbgCmdExecDirect(cmd.c_str());

				

				// ����״̬
				apiInfo.bCmdSuccess = bSuccess;
				apiInfo.bWantToSetBp = bNewState;
				apiInfo.bBpSet = bSuccess ? bNewState : apiInfo.bBpSet;  // ���ɹ�ʱ����ʵ��״̬

				// ������
				if (!bSuccess) {
					_plugin_logprintf("Failed to %s breakpoint %s\n", bNewState ? "set" : "clear", cmd.c_str());
				}
				else {
					_plugin_logprintf("Successfully %s breakpoint %s\n", bNewState ? "set" : "cleared", cmd.c_str());
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
		//CleanupBreakpoints();
		PostQuitMessage(0);
		return 0;
	case WM_DPICHANGED:
	{
		g_dpi.current = HIWORD(wParam);
		g_dpi.scaling = g_dpi.current / 96.0f;
		RecreateFont();

		// ��������λ�úʹ�С
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

// ���ֵ���
void AdjustLayout(HWND hwnd) {
	RECT rc;
	GetClientRect(hwnd, &rc);
	const int width = rc.right - rc.left;
	const int height = rc.bottom - rc.top;

	// Tab�ؼ�
	const int tabHeight = MulDiv(TAB_HEIGHT, g_dpi.current, 96);
	SetWindowPos(hTabControl, nullptr, 0, 0, width, tabHeight, SWP_NOZORDER);

	// ��ѡ�򲼾�
	const int margin = MulDiv(MARGIN_SIZE, g_dpi.current, 96);
	const int checkWidth = (width - margin * (CHECKBOX_COLUMNS + 1)) / CHECKBOX_COLUMNS;
	const int checkHeight = MulDiv(CHECKBOX_HEIGHT, g_dpi.current, 96);

	// ����Tab�ؼ�
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

// �ڳ����˳������ǰ����������
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

// �������
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

	// �������пؼ�����
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
	//for (size_t i = 0; i < TAB_COUNT; i++)
	//{
	//	for (size_t j = 0; j < Groups[i].apiList.size(); j++)
	//	{
	//		auto& apiInfo = Groups[CurTab].apiList[i];  // ������

	//		if (apiInfo.bBpSet && apiInfo.bCmdSuccess) {
	//			_plugin_logprintf("BP Already Set\n");
	//			break;
	//		}

	//		std::wstring wcmd = L"bp " + apiInfo.apiName;
	//	    std::string cmd = WideToUTF8(wcmd);
	//	    bool bSuccess = DbgCmdExecDirect(cmd.c_str());
	//	}
	//}
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
				NULL,                   // ��ȫ����
				0,                      // Ĭ�϶�ջ��С
				MsgLoopThread,          // �̺߳���
				NULL,                   // ����
				0,                      // ������־
				NULL                    // �߳�ID
			);

			if (hThread != NULL)
			{
				// �̴߳����ɹ������Թرվ��������������������
				CloseHandle(hThread);
				bIsMainWindowShow = true;
			}
			else
			{
				// �����̴߳���ʧ�ܵ����
				_plugin_logprintf("Failed to create thread %s \n", MB_ICONERROR);
			}
		}
		break;
	}
}



