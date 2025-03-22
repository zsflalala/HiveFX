@echo off

cls

for %%f in (app mylibrary system_test001_frameplaying system_test002_slidewindow system_test003_blending system_test004_snowstylization system_test005_2channelstexture system_test006_scrollrain system_test007_combinedframeplaying system_test008_buildsimplestapk) do (
        if not exist "%%f\src\main\cpp\build" (
		cd app\src\main\cpp && generateConanPkg.bat && cd ../../../..
	 ) else (
		echo %%f\src\main\cpp\build exists.
	)
)

echo 生成所有的build文件夹完成
pause