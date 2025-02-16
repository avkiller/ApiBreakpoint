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
    _plugin_logprintf("Plugin name %s \n", PLUGIN_NAME);
    initStruct->pluginVersion = PLUGIN_VERSION;
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
	}
	return TRUE;
}
