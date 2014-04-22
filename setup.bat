@echo This batch file sets up in_yansf.ini for debugging.
md Debug
md Debug\plugins
cd ..
md Release
md Release\plugins
cd ..
copy distribute\plugins\in_yansf.ini Debug\plugins\in_yansf.ini
copy distribute\plugins\in_yansf.ini Release\plugins\in_yansf.ini
pause