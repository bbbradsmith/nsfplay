@REM Define the following environment variables to suppress actions:
@REM PAB_NOPAUSE - don't pause after completion
@REM PAB_NO64 - don't build 64-bit libraries
@REM PAB_NO32 - don't build 32-bit libraries
@REM PAB_NORELEASE - don't build release libraries
@REM PAB_NODEBUG - don't build debug libraries

@set PADIR=portaudio
@set PAOUT=portaudiolib
@set PAREL=..\..\portaudio

@set PACMAKE64=vc64_cmake
@set PACMAKE32=vc32_cmake
@set PAOUT64=..\vc64
@set PAOUT32=..\vc32

@set PACMAKE64D=vc64_cmaked
@set PACMAKE32D=vc32_cmaked
@set PAOUT64D=..\vc64d
@set PAOUT32D=..\vc32d

@set PACONF=%PACONF% -DCMAKE_POLICY_DEFAULT_CMP0091=NEW
@set PACONF=%PACONF% -DPA_BUILD_SHARED_LIBS=OFF
@set PACONF=%PACONF% -DPA_BUILD_TESTS=OFF
@set PACONF=%PACONF% -DPA_BUILD_EXAMPLES=OFF
@set PACONFD=%PACONF%
@set PACONF=%PACONF% -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreaded
@set PACONFD=%PACONFD% -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug
@echo set PACONF=%PACONF%
@echo set PACONFD=%PACONFD%

REM ensure portaudio exists
@if not exist "portaudio\CMakeLists.txt" (
@echo.
@echo Error: Unable to find portaudio\CmakeLists.txt
@echo Do you need to set up git submodules?
@echo    git submodule init
@echo    git submodule update --depth 1
@echo Alternative: manually place portaudio source in portaudio directory.
@echo.
@if not defined PAB_NOPAUSE pause
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
@if not defined PAB_NOPAUSE pause
exit /b 1

:build
REM build portaudio libraries

@if defined PAB_NO64 goto :no64
@if defined PAB_NORELEASE goto :norelease64
@pushd .
mkdir %PAOUT%\%PACMAKE64%
@cd %PAOUT%\%PACMAKE64%
cmake %PAREL% %PACONF%  -DCMAKE_GENERATOR_PLATFORM=x64           || @goto error
cmake --build .                       --config Release --verbose || @goto error
cmake --install . --prefix %PAOUT64%  --config Release           || @goto error
@popd
:norelease64
@if defined PAB_NODEBUG goto :nodebug64
@pushd .
mkdir %PAOUT%\%PACMAKE64D%
@cd %PAOUT%\%PACMAKE64D%
cmake %PAREL% %PACONFD% -DCMAKE_GENERATOR_PLATFORM=x64           || @goto error
cmake --build .                       --config Debug   --verbose || @goto error
cmake --install . --prefix %PAOUT64D% --config Debug             || @goto error
copy Debug\*.pdb %PAOUT64D%\lib
@popd
:nodebug64
:no64

@if defined PAB_NO32 goto :no32
@if defined PAB_NORELEASE goto :norelease32
@pushd .
mkdir %PAOUT%\%PACMAKE32%
@cd %PAOUT%\%PACMAKE32%
cmake %PAREL% %PACONF%  -DCMAKE_GENERATOR_PLATFORM=Win32         || @goto error
cmake --build .                       --config Release --verbose || @goto error
cmake --install . --prefix %PAOUT32%  --config Release           || @goto error
@popd
:norelease32
@if defined PAB_NODEBUG goto :nodebug32
@pushd .
mkdir %PAOUT%\%PACMAKE32D%
@cd %PAOUT%\%PACMAKE32D%
cmake %PAREL% %PACONFD% -DCMAKE_GENERATOR_PLATFORM=Win32         || @goto error
cmake --build .                       --config Debug   --verbose || @goto error
cmake --install . --prefix %PAOUT32D% --config Debug             || @goto error
copy Debug\*.pdb %PAOUT32D%\lib
@popd
:nodebug32
:no32

@echo.
@echo Success.
@echo.
@if not defined PAB_NOPAUSE pause
exit /b 0

:error
@echo.
@echo Unable to build portaudio libraries. See error above.
@echo.
@if not defined PAB_NOPAUSE pause
exit /b 1
