// Microsoft Visual C++ generated resource script.
//

#include "resource.h"

#define APSTUDIO_READONLY_SYMBOLS
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 2 resource.
//
#include "winres.h"

/////////////////////////////////////////////////////////////////////////////
#undef APSTUDIO_READONLY_SYMBOLS

/////////////////////////////////////////////////////////////////////////////
// Chinese (Simplified, China) resources

#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_CHS)
LANGUAGE LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED

#ifdef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// TEXTINCLUDE
//

1 TEXTINCLUDE 
BEGIN
    "resource.h\0"
END

2 TEXTINCLUDE 
BEGIN
    "#include ""winres.h""\r\n"
    "\0"
END

3 TEXTINCLUDE 
BEGIN
    "\r\n"
    "\0"
END

#endif    // APSTUDIO_INVOKED


/////////////////////////////////////////////////////////////////////////////
//
// Dialog
//

IDD_OPTIONS DIALOGEX 0, 0, 366, 202
STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU
CAPTION "[ApiBreakPoint Options]"
FONT 8, "MS Shell Dlg", 400, 0, 0x1
BEGIN
    CONTROL         "关闭窗口清理断点", IDC_CLOSE_CLEAR_APIBREAKPOINT, "Button", BS_AUTOCHECKBOX | SS_LEFT | WS_TABSTOP, 12, 48, 104, 10
    CONTROL         "重新加载时DLL设置断点", IDC_RELOAD_BREAKPOINT_DLL, "Button", BS_AUTOCHECKBOX | SS_LEFT | WS_TABSTOP, 12, 58, 104, 10
    GROUPBOX        "配置",IDC_STATIC,6,4,360,28
    COMBOBOX        IDC_PROFILES,40,14,228,74,CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP
    LTEXT           "加载:",IDC_STATIC,12,14,27,13,SS_CENTERIMAGE
    PUSHBUTTON      "新建一个配置...",IDC_SAVEPROFILE,272,12,88,16
    LTEXT           "API断点列表配置文件",IDC_STATIC,201,46,38,14,SS_CENTERIMAGE
    EDITTEXT        IDC_API_CONFIG_FILE_NAME,244,46,104,14,ES_AUTOHSCROLL
END


#endif    // English (United States) resources
/////////////////////////////////////////////////////////////////////////////



#ifndef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// Generated from the TEXTINCLUDE 3 resource.
//


/////////////////////////////////////////////////////////////////////////////
#endif    // not APSTUDIO_INVOKED

