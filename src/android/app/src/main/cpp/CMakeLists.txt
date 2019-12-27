#
# Copyright (C) The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.4.1)

# build native_app_glue as a static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS} -G")
add_library(native_app_glue STATIC
    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -G -std=gnu++11 -Wall")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
    "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

# ---------------------------------------------------------------------
# set include files

set(INCLUDE_FILES
        ${GFX_EXTERNAL_PATH}/khronos
        ${COCOS_SRC_PATH}/renderer/core
        ${COCOS_SRC_PATH}/renderer/gfx-gles2
        ${COCOS_SRC_PATH}/renderer/gfx-gles3
        ./
        ${COCOS_ROOT_PATH}
        )

include_directories( ${INCLUDE_FILES} )

# ---------------------------------------------------------------------

set(ALL_FILES
        ${GFX_TESTCASE_HEADER}
        ${GFX_TESTCASE_SOURCE}
        ${CC_PLATFORM_SOURCES}
        ${CC_BASE_HEADERS} 
        ${CC_BASE_SOURCES}
        ${CC_MATH_HEADERS}
        ${CC_MATH_SOURCES}
        ${CC_EXTERNAL_SROUCES}
        ${COCOS_SRC_PATH}/cocos2d.h
        ${COCOS_SRC_PATH}/cocos2d.cpp
        main.cpp
        JniImp.cpp
        JniImp.h
        )

if(COCOS_UNICODE)
    add_definitions("-DUNICODE -D_UNICODE")
endif()

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        ${APP_NAME}

        # Sets the library as a shared library.
        SHARED

        ${ALL_FILES})

target_include_directories(${APP_NAME} PRIVATE
    ${ANDROID_NDK}/sources/android/native_app_glue
    
)
target_include_directories(${APP_NAME} PUBLIC
    ${COCOS_SRC_PATH}/platform
    ${COCOS_EXTERNAL_PATH}/sources
    ${CC_EXTERNAL_INCLUDES}
    ${CC_EXTERNAL_INCLUDES}/v8
)
# add lib dependencies
target_link_libraries(${APP_NAME}
    android
    native_app_glue
    EGL
    GLESv2
    GLESv3
    log
    Core
    GFXGLES3
    GFXGLES2
    ${CC_EXTERNAL_LIBS}
)

# setup default flags
target_compile_definitions(${APP_NAME} PUBLIC 
    $<$<NOT:$<BOOL:${USE_VIDEO}>>:USE_VIDEO=0>
    $<$<NOT:$<BOOL:${USE_WEB_VIEW}>>:USE_WEB_VIEW=0>
    $<$<NOT:$<BOOL:${USE_AUDIO}>>:USE_AUDIO=0>
    $<$<NOT:$<BOOL:${USE_SOCKET}>>:USE_SOCKET=0>
    $<$<NOT:$<BOOL:${USE_MIDDLEWARE}>>:USE_MIDDLEWARE=0>
    $<$<NOT:$<BOOL:${USE_SPINE}>>:USE_SPINE=0>
    $<$<NOT:$<BOOL:${USE_DRAGONBONES}>>:USE_DRAGONBONES=0>
    $<$<NOT:$<BOOL:${USE_TIFF}>>:CC_USE_TIFF=0>
    $<$<BOOL:${USE_GFX_RENDERER}>:USE_GFX_RENDERER=1>
    $<$<NOT:$<BOOL:${USE_V8_DEBUGGER}>>:USE_V8_DEBUGGER=0>
)