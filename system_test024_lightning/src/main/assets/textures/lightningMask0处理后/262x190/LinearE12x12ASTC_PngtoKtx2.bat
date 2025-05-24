@echo off
setlocal enabledelayedexpansion

:: Get current directory name (e.g. SmallRain_fore)
for %%A in ("%cd%") do set "folder_name=%%~nxA"
set "output_dir=!folder_name!ASTC12x12_Linear"

echo Batch converting PNG to KTX2 (ASTC 12x12)...
echo Output folder: !output_dir!

:: Create target folder if it doesn't exist
if not exist "!output_dir!" (
    mkdir "!output_dir!"
    echo Folder created: !output_dir!
)

:: Batch convert PNG to KTX2
for %%f in (*.png) do (
    set "filename=%%~nf"
    echo Processing: %%f
    toktx --encode astc --astc_blk_d 12x12  --astc_quality exhaustive --t2 --target_type RGBA --assign_oetf linear "!output_dir!\!filename!.ktx2" "%%f"
)

echo Conversion complete! All files saved to [!output_dir!] folder.
pause