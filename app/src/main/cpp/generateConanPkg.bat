cls
for %%f in (armv7 armv8 x86_64 x86) do (
    conan install . -pr:h=android -s build_type=Debug -s arch=%%f --build=missing -c tools.cmake.cmake_layout:build_folder_vars=['settings.arch']
    conan install . -pr:h=android -s build_type=Release -s arch=%%f --build=missing -c tools.cmake.cmake_layout:build_folder_vars=['settings.arch']
)
pause