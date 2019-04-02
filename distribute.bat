copy Release\nsfplay.exe distribute\nsfplay.exe
mkdir distribute\plugins
copy Release\plugins\in_yansf.dll distribute\plugins\in_yansf.dll
copy Release\plugins\nsfplug_ui.dll distribute\plugins\nsfplug_ui.dll
del distribute\plugins\in_yansf.ini
@echo.
@echo distribute folder updated.
@echo.
@pause
