set WXDIR=wx
set WXOUT=output_wx
set WXREL=..\wx

@set WXCONF=%WXCONF% -G "Visual Studio 16"
@set WXCONF=%WXCONF% -DwxBUILD_USE_STATIC_RUNTIME=ON
@set WXCONF=%WXCONF% -DwxBUILD_SHARED=OFF
@set WXCONF=%WXCONF% -DwxUSE_AUI=OFF
@set WXCONF=%WXCONF% -DwxUSE_HTML=OFF
@set WXCONF=%WXCONF% -DwxUSE_MEDIACTRL=OFF
@set WXCONF=%WXCONF% -DwxUSE_OPENGL=OFF
@set WXCONF=%WXCONF% -DwxUSE_DEBUGREPORT=OFF
@set WXCONF=%WXCONF% -DwxUSE_RIBBON=OFF
@set WXCONF=%WXCONF% -DwxUSE_RICHTEXT=OFF
@set WXCONF=%WXCONF% -DwxUSE_STC=OFF
@set WXCONF=%WXCONF% -DwxUSE_WEBVIEW=OFF
@set WXCONF=%WXCONF% -DwxUSE_XRC=OFF
@set WXCONF=%WXCONF% -DwxUSE_STL=OFF
@set WXCONF=%WXCONF% -DwxUSE_REGEX=OFF
@set WXCONF=%WXCONF% -DwxUSE_ZLIB=builtin
@set WXCONF=%WXCONF% -DwxUSE_EXPAT=OFF
@set WXCONF=%WXCONF% -DwxUSE_LIBJPEG=OFF
@set WXCONF=%WXCONF% -DwxUSE_LIBPNG=builtin
@set WXCONF=%WXCONF% -DwxUSE_LIBTIFF=OFF
@set WXCONF=%WXCONF% -DwxUSE_NANOSVG=OFF
@set WXCONF=%WXCONF% -DwxUSE_LIBLZMA=OFF
@set WXCONF=%WXCONF% -DwxUSE_WXHTML_HELP=OFF
@echo set WXCONF=%WXCONF%

REM ensure wx exists
@if not exist "wx\CMakeLists.txt" (
@echo.
@echo Error: Unable to find wx\CmakeLists.txt
@echo Do you need to set up git submodules?
@echo    git submodule init
@echo    git submodule update --depth 1
@echo Alternative: manually place wxWidgets source in wx directory.
@echo.
@if not "%1" == "nopause" pause
exit /b 1
)

REM locate visual studio vsdevcmd.bat to use cmake
@for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do (
@if exist "%%i\Common7\Tools\vsdevcmd.bat" (
call "%%i\Common7\Tools\vsdevcmd.bat"

mkdir %WXOUT%\x64
mkdir %WXOUT%\x86

cd %WXOUT%\x64
cmake ..\%WXREL% %WXCONF% -DCMAKE_GENERATOR_PLATFORM=x64 || @goto error
cmake --build . --config Release || @goto error
cmake --build . --config Debug || @goto error

cd ..\x86
cmake ..\%WXREL% %WXCONF% -DCMAKE_GENERATOR_PLATFORM=Win32 || @goto error
cmake --build . --config Release || @goto error
cmake --build . --config Debug || @goto error

@echo.
@echo Success.
@echo.
@if not "%1" == "nopause" pause

exit /b 0
)
)

@echo.
@echo Error: Unable to find Visual Studio installation: %ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe
@echo.
@if not "%1" == "nopause" pause
exit /b 1

:error
@echo.
@echo Unable to build wxWidgets libraries. See error above.
@echo.
@if not "%1" == "nopause" pause
exit /b 1
