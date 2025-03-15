#include "pluginmain.h"
#include "plugin.h"


int pluginHandle;
HWND hwndDlg;
int hMenu;
int hMenuDisasm;
int hMenuDump;
int hMenuStack;

HINSTANCE g_hInstance;

PLUG_EXPORT bool pluginit(PLUG_INITSTRUCT* initStruct)
{
    //_plugin_logprintf("Plugin name %s \n", PLUGIN_NAME);
    initStruct->pluginVersion = (API_BREAKPOINT_VERSION_MAJOR * 100) + (API_BREAKPOINT_VERSION_MINOR * 10) + API_BREAKPOINT_VERSION_PATCH;
    initStruct->sdkVersion = PLUG_SDKVERSION;
    //wcsncpy_s(initStruct->pluginName, PLUGIN_NAME_MAX_LEN, PLUGIN_NAME, _TRUNCATE);
    strncpy_s(initStruct->pluginName, PLUGIN_NAME_MAX_LEN, PLUGIN_NAME, _TRUNCATE);
    pluginHandle = initStruct->pluginHandle;
    return pluginInit(initStruct);
}

PLUG_EXPORT bool plugstop()
{
    pluginStop();
    return true;
}

PLUG_EXPORT void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
    hwndDlg = setupStruct->hwndDlg;
    hMenu = setupStruct->hMenu;
    hMenuDisasm = setupStruct->hMenuDisasm;
    hMenuDump = setupStruct->hMenuDump;
    hMenuStack = setupStruct->hMenuStack;
    pluginSetup();
}

EXTERN_C _declspec(dllexport) BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		g_hInstance = hInst; // save instance handle for future use

        g_PluginDir = scl::GetModuleFileNameW(g_hInstance);
        g_PluginDir.resize(g_PluginDir.find_last_of(L'\\') + 1);
        g_settingIniPath = g_PluginDir + scl::Settings::kFileName;
	}
	return TRUE;
}
