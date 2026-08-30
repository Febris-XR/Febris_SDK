@echo off
rem Builds the C++ parity probe with plain cl against the FebrisCpp sources.
rem The SDK DLL exports nothing (there is no import library to link against),
rem so the probe compiles the sources it needs directly. Everything the probe
rem reaches is header-only after the port (StatementFactoring.h, XApiJson.h,
rem inline model ctors); .cpp files are only added here when a link error
rem proves a definition lives out-of-line.
setlocal
rem Locate VS2022 via vswhere (fixed path on every install, editions and CI
rem runners included) instead of hardcoding the Community path.
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
  for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do set "VSDIR=%%i"
)
if not defined VSDIR set "VSDIR=C:\Program Files\Microsoft Visual Studio\2022\Community"
call "%VSDIR%\Common7\Tools\VsDevCmd.bat" -no_logo -arch=amd64
set ROOT=%~dp0..\..\cpp
set OUT=%~dp0build
if not exist "%OUT%" mkdir "%OUT%"
cl /nologo /std:c++17 /EHsc /MD /W3 /O2 ^
  /D WIN32 /D NDEBUG /D _CONSOLE ^
  /I "%ROOT%" ^
  /I "%ROOT%\packages\boost.1.82.0\lib\native\include" ^
  /I "%ROOT%\packages\nlohmann.json.3.11.2\build\native\include" ^
  /I "%ROOT%\packages\rapidxml.1.13\build\native\include" ^
  /Fo"%OUT%\\" /Fe"%OUT%\parity_probe.exe" ^
  "%~dp0main.cpp"
endlocal
