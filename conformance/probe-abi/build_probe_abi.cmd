@echo off
rem Builds the ABI lifecycle probe. Deliberately does NOT link against the SDK
rem or include its headers -- the probe binds to Febris.CppSimulationLibrary.dll
rem at runtime via LoadLibrary/GetProcAddress, proving the extern "C" export
rem names exactly as a customer's P/Invoke or engine plugin would.
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -no_logo -arch=amd64
set OUT=%~dp0build
if not exist "%OUT%" mkdir "%OUT%"
cl /nologo /std:c++17 /EHsc /MD /W3 /O2 /D WIN32 /D NDEBUG /D _CONSOLE ^
  /Fo"%OUT%\\" /Fe"%OUT%\parity_lifecycle.exe" ^
  "%~dp0main.cpp"
endlocal
