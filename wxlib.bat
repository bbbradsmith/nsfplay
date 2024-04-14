@REM Define the following environment variables to suppress actions:
@REM WXB_NOPAUSE - don't pause after completion
@REM WXB_NO64 - don't build 64-bit libraries
@REM WXB_NO32 - don't build 32-bit libraries
@REM WXB_NORELEASE - don't build release libraries
@REM WXB_NODEBUG - don't build debug libraries

@set WXDIR=wx
@set WXOUT=wxlib
@set WXREL=..\wx
@set WXCMAKE64=vc64
@set WXCMAKE32=vc32

@set WXCONF=%WXCONF% -DwxBUILD_USE_STATIC_RUNTIME=ON
@set WXCONF=%WXCONF% -DwxBUILD_SHARED=OFF
@set WXCONF=%WXCONF% -DwxUSE_UNICODE=ON
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
@set WXCONF=%WXCONF% -DwxUSE_LIBSDL=OFF
@echo set WXCONF=%WXCONF%

REM ensure wx exists
@if not exist "wx\CMakeLists.txt" (
@echo.
@echo Error: Unable to find wx\CmakeLists.txt
@echo Do you need to set up git submodules?
@echo    git submodule init
@echo    git submodule update --depth 1
@echo    cd wx
@echo    git submodule init
@echo    git submodule update --depth 1
@echo Alternative: manually place wxWidgets source in wx directory.
@echo.
@if not defined WXB_NOPAUSE pause
exit /b 1
)

REM locate visual studio vsdevcmd.bat to use cmake
@for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do (
@if exist "%%i\Common7\Tools\vsdevcmd.bat" (
@call "%%i\Common7\Tools\vsdevcmd.bat"
@goto build
)
)
@echo.
@echo Error: Unable to find Visual Studio installation: %ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe
@echo.
@if not defined WXB_NOPAUSE pause
exit /b 1

:build
REM build wxWidgets libraries

@if defined WXB_NO64 goto :no64
mkdir %WXOUT%\%WXCMAKE64%
@pushd .
@cd %WXOUT%\%WXCMAKE64%
cmake ..\%WXREL% %WXCONF% -DCMAKE_GENERATOR_PLATFORM=x64 || @goto error
@if defined WXB_NORELEASE goto :norelease64
cmake --build .               --config Release --verbose || @goto error
cmake --install . --prefix .. --config Release           || @goto error
:norelease64
@if defined WXB_NODEBUG goto :nodebug64
cmake --build .               --config Debug   --verbose || @goto error
cmake --install . --prefix .. --config Debug             || @goto error
:nodebug64
@popd
:no64

@if defined WXB_NO32 goto :no32
mkdir %WXOUT%\%WXCMAKE32%
@pushd .
@cd %WXOUT%\%WXCMAKE32%
cmake ..\%WXREL% %WXCONF% -DCMAKE_GENERATOR_PLATFORM=Win32 || @goto error
@if defined WXB_NORELEASE goto :norelease32
cmake --build .               --config Release --verbose || @goto error
cmake --install . --prefix .. --config Release           || @goto error
:norelease32
@if defined WXB_NODEBUG goto :nodebug32
cmake --build .               --config Debug   --verbose || @goto error
cmake --install . --prefix .. --config Debug             || @goto error
:nodebug32
@popd
:no32

@echo.
@echo Success.
@echo.
@if not defined WXB_NOPAUSE pause
exit /b 0

:error
@echo.
@echo Unable to build wxWidgets libraries. See error above.
@echo.
@if not defined WXB_NOPAUSE pause
exit /b 1
