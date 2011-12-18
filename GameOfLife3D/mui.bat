rem @echo off
pushd .

REM DESTDIR=Debug/Release
SET DESTDIR=%1

REM LANG_LCID=0x0409/0x0411/...
SET LANG_LCID=%2

REM LANG_NAME=en-US/ja-JP/...
SET LANG_NAME=%3

REM DLL_POSTFIX=en_us/ja_jp/...
SET DLL_POSTFIX=%4

SET LN_MODULE_NAME=.\GameOfLife3D_MUI.dll
IF %5 EQU 0 goto DO_LOC
SET LN_MODULE_NAME=.\GameOfLife3D_MUI_dummy.dll

:DO_LOC
cd %DESTDIR%
mkdir %LANG_NAME%
muirct.exe -q ..\GameOfLife3D\mui_rcconfig.xml -v 2 -x %LANG_LCID% -g 0x0409 GameOfLife3D_MUI_%DLL_POSTFIX%.dll %LN_MODULE_NAME% .\%LANG_NAME%\GameOfLife3D_MUI.dll.mui
muirct.exe -c .\GameOfLife3D_MUI.dll -v 2 -e .\%LANG_NAME%\GameOfLife3D_MUI.dll.mui

IF EXIST .\GameOfLife3D_MUI_dummy.dll del .\GameOfLife3D_MUI_dummy.dll

popd
