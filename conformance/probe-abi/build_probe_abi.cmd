@echo off
rem Builds the ABI lifecycle probe. Deliberately does NOT link against the SDK
rem or include its headers -- the probe binds to Febris.CppSimulationLibrary.dll
rem at runtime via LoadLibrary/GetProcAddress, proving the extern "C" export
rem names exactly as a customer's P/Invoke or engine plugin would.
setlocal
rem Locate VS2022 via vswhere (fixed path on every install, editions and CI
rem runners included) instead of hardcoding the Community path.
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if exist "%VSWHERE%" (
  for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`) do set "VSDIR=%%i"
)
if not defined VSDIR set "VSDIR=C:\Program Files\Microsoft Visual Studio\2022\Community"
call "%VSDIR%\Common7\Tools\VsDevCmd.bat" -no_logo -arch=amd64
set OUT=%~dp0build
if not exist "%OUT%" mkdir "%OUT%"
cl /nologo /std:c++17 /EHsc /MD /W3 /O2 /D WIN32 /D NDEBUG /D _CONSOLE ^
  /Fo"%OUT%\\" /Fe"%OUT%\parity_lifecycle.exe" ^
  "%~dp0main.cpp"
endlocal
