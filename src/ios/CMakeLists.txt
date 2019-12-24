macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY}
                 ${XCODE_VALUE})
endmacro (set_xcode_property)

message(STATUS "App: ${APP_NAME}...")

# ---------------------------------------------
# set cocos platform, base, math, external files
set(CC_PLATFORM_SOURCES
    ${COCOS_SRC_PATH}/platform/CCApplication.h
    ${COCOS_SRC_PATH}/platform/CCDevice.h
    ${COCOS_SRC_PATH}/platform/CCFileUtils.h
    ${COCOS_SRC_PATH}/platform/CCGL.h
    ${COCOS_SRC_PATH}/platform/CCImage.h
    ${COCOS_SRC_PATH}/platform/CCPlatformConfig.h
    ${COCOS_SRC_PATH}/platform/CCPlatformDefine.h
    ${COCOS_SRC_PATH}/platform/CCSAXParser.h
    ${COCOS_SRC_PATH}/platform/CCStdC.h

    ${COCOS_SRC_PATH}/platform/CCFileUtils.cpp
    ${COCOS_SRC_PATH}/platform/CCImage.cpp
    ${COCOS_SRC_PATH}/platform/CCSAXParser.cpp
)

if(APPLE)
    list(APPEND CC_PLATFORM_SOURCES
        ${COCOS_SRC_PATH}/platform/apple/CCFileUtils-apple.h
        ${COCOS_SRC_PATH}/platform/apple/CCDevice-apple.mm
        ${COCOS_SRC_PATH}/platform/apple/CCFileUtils-apple.mm
        ${COCOS_SRC_PATH}/platform/apple/CCDevice-apple.h
    )
endif()

set(CC_BASE_HEADERS 
    ${COCOS_SRC_PATH}/base/base64.h
    ${COCOS_SRC_PATH}/base/CCAutoreleasePool.h
    ${COCOS_SRC_PATH}/base/ccCArray.h
    ${COCOS_SRC_PATH}/base/ccConfig.h
    ${COCOS_SRC_PATH}/base/CCConfiguration.h
    ${COCOS_SRC_PATH}/base/CCData.h
    ${COCOS_SRC_PATH}/base/CCGLUtils.h
    ${COCOS_SRC_PATH}/base/CCLog.h
    ${COCOS_SRC_PATH}/base/ccMacros.h
    ${COCOS_SRC_PATH}/base/CCMap.h
    ${COCOS_SRC_PATH}/base/ccRandom.h
    ${COCOS_SRC_PATH}/base/CCRef.h
    ${COCOS_SRC_PATH}/base/CCRefPtr.h
    # ${COCOS_SRC_PATH}/base/CCRenderTexture.h
    # ${COCOS_SRC_PATH}/base/CCScheduler.h
    # ${COCOS_SRC_PATH}/base/CCThreadPool.h
    ${COCOS_SRC_PATH}/base/ccTypes.h
    ${COCOS_SRC_PATH}/base/ccUTF8.h
    ${COCOS_SRC_PATH}/base/ccUtils.h
    ${COCOS_SRC_PATH}/base/CCValue.h
    ${COCOS_SRC_PATH}/base/CCVector.h
    ${COCOS_SRC_PATH}/base/etc1.h
    ${COCOS_SRC_PATH}/base/etc2.h
    ${COCOS_SRC_PATH}/base/pvr.h
    ${COCOS_SRC_PATH}/base/TGAlib.h
    ${COCOS_SRC_PATH}/base/uthash.h
    ${COCOS_SRC_PATH}/base/utlist.h
    ${COCOS_SRC_PATH}/base/ZipUtils.h
)

set(CC_BASE_SOURCES
    ${COCOS_SRC_PATH}/base/base64.cpp
    ${COCOS_SRC_PATH}/base/CCAutoreleasePool.cpp
    ${COCOS_SRC_PATH}/base/ccCArray.cpp
    ${COCOS_SRC_PATH}/base/CCConfiguration.cpp
    ${COCOS_SRC_PATH}/base/CCData.cpp
    ${COCOS_SRC_PATH}/base/CCGLUtils.cpp
    ${COCOS_SRC_PATH}/base/CCLog.cpp
    ${COCOS_SRC_PATH}/base/ccRandom.cpp
    ${COCOS_SRC_PATH}/base/CCRef.cpp
    # ${COCOS_SRC_PATH}/base/CCRenderTexture.cpp
    # ${COCOS_SRC_PATH}/base/CCScheduler.cpp
    # ${COCOS_SRC_PATH}/base/CCThreadPool.cpp
    ${COCOS_SRC_PATH}/base/ccTypes.cpp
    ${COCOS_SRC_PATH}/base/ccUTF8.cpp
    ${COCOS_SRC_PATH}/base/ccUtils.cpp
    ${COCOS_SRC_PATH}/base/CCValue.cpp
    ${COCOS_SRC_PATH}/base/csscolorparser.cpp
    ${COCOS_SRC_PATH}/base/etc1.cpp
    ${COCOS_SRC_PATH}/base/etc2.cpp
    ${COCOS_SRC_PATH}/base/pvr.cpp
    ${COCOS_SRC_PATH}/base/TGAlib.cpp
    ${COCOS_SRC_PATH}/base/ZipUtils.cpp
)

set(CC_MATH_HEADERS
    ${COCOS_SRC_PATH}/math/CCGeometry.h
    ${COCOS_SRC_PATH}/math/CCMath.h
    ${COCOS_SRC_PATH}/math/CCMathBase.h
    ${COCOS_SRC_PATH}/math/CCVertex.h
    ${COCOS_SRC_PATH}/math/Mat4.h
    ${COCOS_SRC_PATH}/math/MathUtil.h
    ${COCOS_SRC_PATH}/math/Quaternion.h
    ${COCOS_SRC_PATH}/math/Vec2.h
    ${COCOS_SRC_PATH}/math/Vec3.h
    ${COCOS_SRC_PATH}/math/Vec4.h
)

set(CC_MATH_SOURCES
    ${COCOS_SRC_PATH}/math/CCGeometry.cpp
    ${COCOS_SRC_PATH}/math/CCVertex.cpp
    ${COCOS_SRC_PATH}/math/Mat4.cpp
    ${COCOS_SRC_PATH}/math/MathUtil.cpp
    ${COCOS_SRC_PATH}/math/Quaternion.cpp
    ${COCOS_SRC_PATH}/math/Vec2.cpp
    ${COCOS_SRC_PATH}/math/Vec3.cpp
    ${COCOS_SRC_PATH}/math/Vec4.cpp
)

include(${COCOS_EXTERNAL_PATH}/CMakeLists.txt)
include(${COCOS_ROOT_PATH}/tests/CMakeLists.txt)

# ---------------------------------------------------------------------
# set include files

set(INCLUDE_FILES
  ${GFX_EXTERNAL_PATH}/khronos
  ${COCOS_SRC_PATH}/renderer/core
  ${COCOS_SRC_PATH}/renderer/gfx-gles2
  ${COCOS_SRC_PATH}/renderer/gfx-gles3
  ./GFXTest
)

include_directories( ${INCLUDE_FILES} )

# ---------------------------------------------------------------------

file(GLOB_RECURSE HEADER_FILES *.h)
file(GLOB_RECURSE SOURCE_CPP_FILES *.cpp *.cc)
file(GLOB_RECURSE SOURCE_C_FILES *.c)
file(GLOB_RECURSE SOURCE_MM_FILES *.m *.mm)

set(APP_UI_RES
    ${CMAKE_CURRENT_LIST_DIR}/../Resources/uv_checker_01.jpg
)

set(ALL_FILES 
    ${HEADER_FILES} 
    ${SOURCE_CPP_FILES} 
    ${SOURCE_C_FILES} 
    ${SOURCE_MM_FILES} 
    ${GFX_TESTCASE_HEADER} 
    ${GFX_TESTCASE_SOURCE} 
    ${APP_UI_RES}
    ${CC_PLATFORM_SOURCES}
    ${CC_BASE_HEADERS} 
    ${CC_BASE_SOURCES}
    ${CC_MATH_HEADERS}
    ${CC_MATH_SOURCES}
    ${CC_EXTERNAL_SROUCES}
    ${cc_common_res}
    ${COCOS_SRC_PATH}/cocos2d.h
    ${COCOS_SRC_PATH}/cocos2d.cpp
    )

if(COCOS_UNICODE)
	add_definitions("-DUNICODE -D_UNICODE")
endif()

add_executable(${APP_NAME} ${ALL_FILES})

source_group(TREE ${COCOS_SRC_PATH}/platform PREFIX "Header Files/platform" FILES ${CC_PLATFORM_SOURCES})
source_group(TREE ${COCOS_SRC_PATH}/base PREFIX "Header Files/base" FILES ${CC_BASE_HEADERS})
source_group(TREE ${COCOS_SRC_PATH}/base PREFIX "Source Files/base" FILES ${CC_BASE_SOURCES})
source_group(TREE ${COCOS_SRC_PATH}/math PREFIX "Header Files/math" FILES ${CC_MATH_HEADERS})
source_group(TREE ${COCOS_SRC_PATH}/math PREFIX "Source Files/math" FILES ${CC_MATH_SOURCES})
source_group(TREE ${COCOS_EXTERNAL_PATH}/sources PREFIX "Header Files/external" FILES ${CC_EXTERNAL_SROUCES})

set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
set_xcode_property(${APP_NAME} INFOPLIST_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)

# Locate system libraries on iOS
find_library(UIKIT_LIBRARY UIKit)
find_library(OPENGLES_LIBRARY OpenGLES)
find_library(QUARTZCORE_LIBRARY QuartzCore)
find_library(METAL_LIBRARY Metal)
find_library(METALKIT_LIBRARY MetalKit)
list(APPEND PLATFORM_SPECIFIC_LIBS
     ${UIKIT_LIBRARY}
     ${OPENGLES_LIBRARY}
     ${QUARTZCORE_LIBRARY}
     ${METAL_LIBRARY}
     ${METAL_KIT}
)

target_link_libraries(${APP_NAME} 
    ${PLATFORM_SPECIFIC_LIBS} 
    Core 
    GFXGLES3 
    GFXGLES2
    ${CC_EXTERNAL_LIBS}
    z
)

target_include_directories(${APP_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}
    ${COCOS_SRC_PATH}/platform
    ${COCOS_EXTERNAL_PATH}/sources
    ${CC_EXTERNAL_INCLUDES}
    ${CC_EXTERNAL_INCLUDES}/v8
)

if(MACOSX)
    target_include_directories(${APP_NAME} PRIVATE 
        ##FIXME: bad dependency
        ${COCOS_SRC_PATH}/platform/ios
    )
endif()

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

message(STATUS "${APP_NAME} configuration completed.") 
