cd /d "%~dp0"
call distribute.bat
copy distribute\plugins\*.* "C:\Program Files (x86)\Winamp\Plugins"
@echo.
@echo Review the log above for errors. This batch file should be run as administrator.
@echo.
@pause
