cmake_minimum_required(VERSION 3.6)

set(APP_NAME gfx_tests)

project(${APP_NAME})

if(XCODE)
    if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
        SET (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 8.0)
    endif()
endif()

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

message(STATUS "### Cocos 3D Build System ###")
message(STATUS "Configuring build system...")
message(STATUS "CMake version: ${CMAKE_VERSION}")

set(COCOS_PLATFORM_WIN32 FALSE)
set(COCOS_PLATFORM_ANDROID FALSE)
set(COCOS_PLATFORM_IOS FALSE)
set(COCOS_PLATFORM_OSX FALSE)
set(COCOS_PLATFORM_LINUX FALSE)

if(CPUX64)
    set(COCOS_CPU_X64 TRUE)
else()
	set(COCOS_CPU_X64 FALSE)
endif()

if(MSVC)
	if(ANDROID)
		set(COCOS_PLATFORM_ANDROID TRUE)
		message(STATUS "Platform: MSVC Android")

		set(CMAKE_CROSSCOMPILING TRUE)
		set(CMAKE_SYSTEM_NAME "Linux")
		#set(CMAKE_SYSTEM_NAME Android)
		set(CMAKE_SYSTEM_PROCESSOR arm)
		
		set(CMAKE_ANDROID_API "19")

	elseif(WIN32)
		set(COCOS_PLATFORM_WIN32 TRUE)
		message(STATUS "Platform: Win32")
		set(COCOS_UNICODE TRUE)
		# compiler building setting
		# Flags used by the compiler during all build types.
		set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W4 /wd4819 /wd4100 /wd4201 /wd4127 /Zm256 /EHsc /GR- /fp:fast")
		# Flags used by the compiler during debug builds.
		set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /D_DEBUG /MDd /Zi /Ob0 /Od /RTC1 /Oi")
		# Flags used by the compiler during release minsize builds.
		set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS} /MD /O1 /Ob1 /DNDEBUG")
		# Flags used by the compiler during release builds (/MD /Ob1 /Oi /Ot /Oy /Gs will produce slightly less optimized but smaller files).
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /MD /O2 /Ob2 /DNDEBUG")
		# Flags used by the compiler during Release with Debug Info builds.
		set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} /MD /Zi /O2 /Ob1 /DNDEBUG")
		# Flags used by the compiler during all build types.
		set(CMAKE_C_FLAGS "/DWIN32 /D_WINDOWS /W3 /wd4819 /Zm256")
		# Flags used by the compiler during debug builds.
		set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS} /D_DEBUG /MDd /Zi /Ob0 /Od /RTC1")
		# Flags used by the compiler during release minsize builds.
		set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS} /MD /O1 /Ob1 /DNDEBUG")
		# Flags used by the compiler during release builds (/MD /Ob1 /Oi /Ot /Oy /Gs will produce slightly less optimized but smaller files).
		set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS} /MD /O2 /Ob2 /DNDEBUG")
		# Flags used by the compiler during Release with Debug Info builds.
		set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS} /MD /Zi /O2 /Ob1 /DNDEBUG")
			
		# Enable intrinsics on MSVC in debug mode
		if (CMAKE_CL_64)
			# Visual Studio bails out on debug builds in 64bit mode unless
			# this flag is set...
			set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /bigobj")
			set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /bigobj")
		endif ()
		
		# This seems to break Xcode projects so definitely don't enable on Apple builds
		set(CMAKE_USE_RELATIVE_PATHS true)
		# set(CMAKE_SUPPRESS_REGENERATION true)
		
	else()
		message(STATUS "Platform: Unknown")
	endif()
elseif(ANDROID)
	set(COCOS_PLATFORM_ANDROID TRUE)
	message(STATUS "Platform: Android")
	set(COCOS_STATIC TRUE CACHE BOOL "Forcing static build for android" FORCE)
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall" CACHE STRING "" FORCE)
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti -std=c++14" CACHE STRING "" FORCE)
elseif(APPLE)
	if(IOS)
		set(COCOS_PLATFORM_IOS TRUE)
		message(STATUS "Platform: Mac iOS")
		set(COCOS_STATIC TRUE CACHE BOOL "Forcing static build for iPhone" FORCE)
		
		set(CMAKE_SYSTEM_NAME iOS)
		set(CMAKE_OSX_SYSROOT iphoneos)
		set(CMAKE_IOS_INSTALL_COMBINED 1)
		set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
	else()
		set(COCOS_PLATFORM_OSX TRUE)
		#set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
		message(STATUS "Platform: Mac OSX")
	endif()

	set(CMAKE_CXX_FLAGS "-std=c++14 -fstrict-aliasing -Wno-expansion-to-defined -Wno-multichar -Wall -Wextra -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-unused-volatile-lvalue -Wno-deprecated-writable-strings")
	set(CMAKE_CXX_FLAGS_RELEASE "-O3 -msse3 -DNDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -DFIPS_DEBUG=1 -ggdb")

	set(CMAKE_C_FLAGS "-fstrict-aliasing -Wno-multichar -Wall -Wextra -Wno-expansion-to-defined -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-unused-volatile-lvalue  -Wno-deprecated-writable-strings")
	set(CMAKE_C_FLAGS_RELEASE "-O3 -msse3 -DNDEBUG")
	set(CMAKE_C_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -DFIPS_DEBUG=1 -g")        	

	set(CMAKE_EXE_LINKER_FLAGS "-ObjC -dead_strip -lpthread")
	set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")

	# need to set some flags directly as Xcode attributes
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++14")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

	set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "NO") 

	# disable ARC
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "NO")
	set(CMAKE_XCODE_ATTRIBUTE_GCC_ENABLE_CPP_RTTI "NO")
	set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:_DEBUG>)
else()
	message(STATUS "Platform: Linux")
endif()

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(COCOS_ROOT_PATH "${PROJECT_SOURCE_DIR}")
set(COCOS_2dx_LITE_PATH "${COCOS_ROOT_PATH}/cocos2d-x-lite")
set(COCOS_SRC_PATH "${COCOS_2dx_LITE_PATH}/cocos")
set(COCOS_BUILD_PATH "${PROJECT_BINARY_DIR}")
set(COCOS_EXTERNAL_PATH "${COCOS_2dx_LITE_PATH}/external")
set(COCOS_BIN_PATH "${COCOS_BUILD_PATH}")
set(COCOS_LIB_PATH "${COCOS_BUILD_PATH}/lib")
set(GFX_EXTERNAL_PATH "${COCOS_ROOT_PATH}/external")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${COCOS_BIN_PATH}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${COCOS_LIB_PATH}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${COCOS_LIB_PATH}")

set(COCOS_BIN_DEBUG "${COCOS_BUILD_PATH}/Debug")
set(COCOS_BIN_MINSIZEREL "${COCOS_BUILD_PATH}/MinSizeRel")
set(COCOS_BIN_RELEASE "${COCOS_BUILD_PATH}/Release")
set(COCOS_BIN_RELWITHDEBINFO "${COCOS_BUILD_PATH}/RelWithDebInfo")
set(COCOS_LIB_DEBUG "${COCOS_BUILD_PATH}/Lib/Debug")
set(COCOS_LIB_MINSIZEREL "${COCOS_BUILD_PATH}/Lib/MinSizeRel")
set(COCOS_LIB_RELEASE "${COCOS_BUILD_PATH}/Lib/Release")
set(COCOS_LIB_RELWITHDEBINFO "${COCOS_BUILD_PATH}/Lib/RelWithDebInfo")

# output message
message(STATUS "\${COCOS_ROOT_PATH}: ${COCOS_ROOT_PATH}")
message(STATUS "\${COCOS_SRC_PATH}: ${COCOS_SRC_PATH}")
message(STATUS "\${COCOS_BUILD_PATH}: ${COCOS_BUILD_PATH}")
message(STATUS "\${COCOS_ENGINE_PATH}: ${COCOS_ENGINE_PATH}")
message(STATUS "\${COCOS_EXTERNAL_PATH}: ${COCOS_EXTERNAL_PATH}")
message(STATUS "\${COCOS_BIN_PATH}: ${COCOS_BIN_PATH}")
message(STATUS "\${COCOS_LIB_PATH}: ${COCOS_LIB_PATH}")
message(STATUS "\${GFX_EXTERNAL_PATH}: ${GFX_EXTERNAL_PATH}")

macro(use_precompiled_header PrecompiledHeader PrecompiledSource Sources)
endmacro(use_precompiled_header)

# ad dependencies
set(COCOS_EXTERNAL_TARGETS
	tommyds)
if(COCOS_PLATFORM_WIN32 OR COCOS_PLATFORM_ANDROID)
	list(APPEND COCOS_EXTERNAL_TARGETS jemalloc)
endif()
foreach(TARGET ${COCOS_EXTERNAL_TARGETS})
	add_subdirectory(${GFX_EXTERNAL_PATH}/${TARGET})
endforeach()

add_subdirectory("cocos2d-x-lite/cocos/renderer")
add_subdirectory(src)