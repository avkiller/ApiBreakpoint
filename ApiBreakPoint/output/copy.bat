::set x64dbgdir="D:\retools\snapshot_2022-10-18_22-09\release\"
set x64dbgdir="d:\Analyze\DebugTools\x64dbg\"
set batdir=%~dp0
echo %batdir%
if exist %batdir%\ApiBreakPoint.dp32 (
    ECHO "hello"
	COPY /Y %batdir%\ApiBreakPoint.dp32 %x64dbgdir%\x32\plugins\ 
	if errorlevel 1 (
        echo "Copy failed"
    ) else ( echo "Copy success" )
) ELSE (echo "File not exist")


if exist %batdir%\ApiBreakPoint.dp64 (
	COPY /Y %batdir%\ApiBreakPoint.dp64 %x64dbgdir%\x64\plugins\ 
	if errorlevel 1 (
        echo "Copy failed"
    ) else ( echo "Copy success" )
) ELSE (echo "File not exist")