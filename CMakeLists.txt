cmake_minimum_required(VERSION 3.6)

set(APP_NAME gfx_tests)
if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.19")
    cmake_policy(SET CMP0111 OLD)
endif()

project(${APP_NAME})

if(XCODE)
    if(NOT DEFINED CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET)
        SET (CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 12.0)
    endif()
endif()

set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS TRUE)

set(COCOS_ROOT_PATH "${PROJECT_SOURCE_DIR}")
get_filename_component(COCOS_2dx_LITE_PATH "${COCOS_ROOT_PATH}/../cocos-engine/native" ABSOLUTE)
set(COCOS_SRC_PATH "${COCOS_2dx_LITE_PATH}/cocos")
set(COCOS_BUILD_PATH "${PROJECT_BINARY_DIR}")
set(COCOS_EXTERNAL_PATH "${COCOS_2dx_LITE_PATH}/external")
set(COCOS_BIN_PATH "${COCOS_BUILD_PATH}")
set(COCOS_LIB_PATH "${COCOS_BUILD_PATH}/lib")
set(GFX_EXTERNAL_PATH "${COCOS_ROOT_PATH}/external")

# output message
message(STATUS "\${COCOS_ROOT_PATH}: ${COCOS_ROOT_PATH}")
message(STATUS "\${COCOS_SRC_PATH}: ${COCOS_SRC_PATH}")
message(STATUS "\${COCOS_BUILD_PATH}: ${COCOS_BUILD_PATH}")
message(STATUS "\${COCOS_ENGINE_PATH}: ${COCOS_ENGINE_PATH}")
message(STATUS "\${COCOS_EXTERNAL_PATH}: ${COCOS_EXTERNAL_PATH}")
message(STATUS "\${COCOS_BIN_PATH}: ${COCOS_BIN_PATH}")
message(STATUS "\${COCOS_LIB_PATH}: ${COCOS_LIB_PATH}")
message(STATUS "\${GFX_EXTERNAL_PATH}: ${GFX_EXTERNAL_PATH}")

# set(NX ON)
# set(CC_NX_WINDOWS ON)

include(${COCOS_2dx_LITE_PATH}/cmake/predefine.cmake)

if(MSVC)
    if(ANDROID)
        set(CMAKE_CROSSCOMPILING TRUE)
        set(CMAKE_SYSTEM_NAME "Linux")
        #set(CMAKE_SYSTEM_NAME Android)
        set(CMAKE_SYSTEM_PROCESSOR arm)

        set(CMAKE_ANDROID_API "19")

    elseif(WIN32)
        set(COCOS_STATIC TRUE)
        add_definitions("-DCC_STATIC")
        add_definitions("-DNOMINMAX")
        add_definitions("-DUNICODE -D_UNICODE")

        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")

        # compiler building setting
        # Flags used by the compiler during all build types.
        set(CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W4 /wd4819 /wd4100 /wd4201 /wd4127 /Zm256 /EHsc /GR- /fp:fast")
        # Flags used by the compiler during debug builds.
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /Zi /Ob0 /Od /RTC1 /Oi /MDd /D_DEBUG")
        # set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /Zi /Ob0 /Od /RTC1 /Oi /MD /D_ITERATOR_DEBUG_LEVEL=0")
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
    #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall" CACHE STRING "" FORCE)
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fno-rtti -std=c++17" CACHE STRING "" FORCE)
elseif(APPLE)
    if(IOS)
        set(CMAKE_SYSTEM_NAME iOS)
        set(CMAKE_OSX_SYSROOT iphoneos)
        set(CMAKE_IOS_INSTALL_COMBINED 1)
        set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    else()
        #set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
    endif()

    set(CMAKE_CXX_FLAGS "-std=c++17 -fstrict-aliasing -frtti -Wall -Wextra -Wno-comment -Wno-missing-field-initializers -Wno-expansion-to-defined -Wno-multichar -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-unused-volatile-lvalue -Wno-deprecated-writable-strings")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -msse3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -DFIPS_DEBUG=1 -ggdb")

    set(CMAKE_C_FLAGS "-fstrict-aliasing -Wall -Wextra -Wno-multichar -Wno-comment -Wno-missing-field-initializers -Wno-expansion-to-defined -Wno-unused-parameter -Wno-unknown-pragmas -Wno-ignored-qualifiers -Wno-long-long -Wno-overloaded-virtual -Wno-unused-volatile-lvalue  -Wno-deprecated-writable-strings")
    set(CMAKE_C_FLAGS_RELEASE "-O3 -msse3 -DNDEBUG")
    set(CMAKE_C_FLAGS_DEBUG "-O0 -D_DEBUG_ -D_DEBUG -DFIPS_DEBUG=1 -g")

    set(CMAKE_EXE_LINKER_FLAGS "-ObjC -dead_strip -lpthread")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "")

    # need to set some flags directly as Xcode attributes
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++17")
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

    set(CMAKE_XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")

    # disable ARC
    set(CMAKE_XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "NO")
    set(CMAKE_XCODE_ATTRIBUTE_GCC_ENABLE_CPP_RTTI "NO")
    set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:_DEBUG>)
else()
    message(STATUS "Platform: Linux")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${COCOS_BIN_PATH}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${COCOS_LIB_PATH}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${COCOS_LIB_PATH}")

macro(use_precompiled_header PrecompiledHeader PrecompiledSource Sources)
endmacro(use_precompiled_header)

set(USE_SE_V8 ON)
set(USE_V8_DEBUGGER ON)
add_definitions("-DUSE_JOB_SYSTEM_TASKFLOW")

include(${COCOS_EXTERNAL_PATH}/CMakeLists.txt)
list(REMOVE_ITEM CC_EXTERNAL_LIBS "glfw3")

set(GFX_INCLUDE_FILES
  ${COCOS_EXTERNAL_PATH}/sources/EGL
  ${COCOS_EXTERNAL_PATH}/sources/khronos
  ${COCOS_2dx_LITE_PATH}
  ${COCOS_SRC_PATH}
  ${COCOS_SRC_PATH}/renderer
  ${COCOS_ROOT_PATH}
  ${CC_EXTERNAL_INCLUDES}
  ${CC_EXTERNAL_PRIVATE_INCLUDES}
)

include_directories(${GFX_INCLUDE_FILES})

add_subdirectory(native/base)
add_subdirectory(native/bindings)
add_subdirectory(native/renderer)
add_subdirectory(src)
