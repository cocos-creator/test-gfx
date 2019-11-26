@echo off

echo Specify the generator for this project
echo 1.Visual Studio 2017
echo 2.Visual Studio 2017 x64
echo 3.Android armv7a (NMake Makefiles)
echo 4.Android x86 (NMake Makefiles)

set /p a=select:
if %a%==1 goto vs15
if %a%==2 goto vs15_x64
if %a%==3 goto android_armv7a_nmake
if %a%==4 goto android_x86_nmake

:vs15
mkdir Build.vs15
cmake -E chdir Build.vs15 cmake -G "Visual Studio 15 2017" ^
	-DMSVC=1 ^
	..
goto end

:vs15_x64
mkdir Build.vs15x64
cmake -E chdir Build.vs15x64 cmake -G "Visual Studio 15 2017 Win64" ^
	-DMSVC=1 ^
	-DCPUX64=1 ^
	..
goto end

:android_armv7a_nmake
mkdir Build.armv7a
cmake -E chdir Build.armv7a cmake -G "NMake Makefiles" ^
	-DANDROID=1 ^
	-DCMAKE_TOOLCHAIN_FILE=External/cmake/android.toolchain.cmake ^
	-DCMAKE_BUILD_TYPE=None ^
	-DANDROID_TOOLCHAIN_NAME=llvm ^
	-DCMAKE_ANDROID_STL_TYPE=c++_static ^
	-DCMAKE_ANDROID_ARCH=armv7-a ^
	-DCMAKE_ANDROID_RTTI=OFF ^
	-DANDROID_NATIVE_API_LEVEL=14 ^
	..
goto end

:android_x86_nmake
mkdir Build.x86
cmake -E chdir Build.x86 cmake -G "NMake Makefiles" ^
	-DANDROID=1 ^
	-DCMAKE_TOOLCHAIN_FILE=External/cmake/android.toolchain.cmake ^
	-DCMAKE_BUILD_TYPE=None ^
	-DANDROID_TOOLCHAIN_NAME=llvm ^
	-DCMAKE_ANDROID_STL_TYPE=c++_static ^
	-DCMAKE_ANDROID_ARCH=x86 ^
	-DCMAKE_ANDROID_RTTI=OFF ^
	-DANDROID_NATIVE_API_LEVEL=14 ^
	..
goto end

:end

pause
