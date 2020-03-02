# add -lite external here for test project

# include modules
if(WIN32)
    set(CMAKE_MODULE_PATH ${GFX_EXTERNAL_PATH}/cmake)
    include(CocosBuildSet)
endif()

# ---------------------------------------------
# set cocos platform, base, math, external files
set(CC_PLATFORM_SOURCES
    ${COCOS_SRC_PATH}/platform/CCApplication.h
    ${COCOS_SRC_PATH}/platform/CCDevice.h
    ${COCOS_SRC_PATH}/platform/CCFileUtils.h
    ${COCOS_SRC_PATH}/platform/CCImage.h
    ${COCOS_SRC_PATH}/platform/CCPlatformConfig.h
    ${COCOS_SRC_PATH}/platform/CCPlatformDefine.h
    ${COCOS_SRC_PATH}/platform/CCSAXParser.h

    ${COCOS_SRC_PATH}/platform/CCFileUtils.cpp
    ${COCOS_SRC_PATH}/platform/CCImage.cpp
    ${COCOS_SRC_PATH}/platform/CCSAXParser.cpp
)
if(WIN32)
    list(APPEND CC_PLATFORM_SOURCES
        ${COCOS_SRC_PATH}/platform/win32/CCFileUtils-win32.h
        ${COCOS_SRC_PATH}/platform/win32/CCPlatformDefine-win32.h
        ${COCOS_SRC_PATH}/platform/win32/CCUtils-win32.h
       
        ${COCOS_SRC_PATH}/platform/win32/CCDevice-win32.cpp
        ${COCOS_SRC_PATH}/platform/win32/CCFileUtils-win32.cpp
        ${COCOS_SRC_PATH}/platform/win32/CCUtils-win32.cpp
    )
elseif(ANDROID)
    list(APPEND CC_PLATFORM_SOURCES
        ${COCOS_SRC_PATH}/platform/android/CCFileUtils-android.h
        ${COCOS_SRC_PATH}/platform/android/CCPlatformDefine-android.h
         ${COCOS_SRC_PATH}/platform/android/jni/JniHelper.h
#         ${COCOS_SRC_PATH}/platform/android/jni/JniImp.h

        # ${COCOS_SRC_PATH}/platform/android/CCDevice-android.cpp
        ${COCOS_SRC_PATH}/platform/android/CCFileUtils-android.cpp 
        # ${COCOS_SRC_PATH}/platform/android/CCApplication-android.cpp
        # ${COCOS_SRC_PATH}/platform/android/CCCanvasRenderingContext2D-android.cpp
#         ${COCOS_SRC_PATH}/platform/android/jni/JniImp.cpp
         ${COCOS_SRC_PATH}/platform/android/jni/JniHelper.cpp
    )
elseif(APPLE)
    list(APPEND CC_PLATFORM_SOURCES
        ${COCOS_SRC_PATH}/platform/apple/CCFileUtils-apple.h
        
        ${COCOS_SRC_PATH}/platform/apple/CCFileUtils-apple.mm
        
        
    )
endif()

set(CC_BASE_HEADERS 
    ${COCOS_SRC_PATH}/base/base64.h
    ${COCOS_SRC_PATH}/base/CCAutoreleasePool.h
    ${COCOS_SRC_PATH}/base/ccCArray.h
    ${COCOS_SRC_PATH}/base/ccConfig.h
    ${COCOS_SRC_PATH}/base/CCData.h
    # ${COCOS_SRC_PATH}/base/CCGLUtils.h
    ${COCOS_SRC_PATH}/base/CCLog.h
    ${COCOS_SRC_PATH}/base/ccMacros.h
    ${COCOS_SRC_PATH}/base/CCMap.h
    ${COCOS_SRC_PATH}/base/ccRandom.h
    ${COCOS_SRC_PATH}/base/CCRef.h
    ${COCOS_SRC_PATH}/base/CCRefPtr.h
    # ${COCOS_SRC_PATH}/base/CCRenderTexture.h
    # ${COCOS_SRC_PATH}/base/CCScheduler.h
    # ${COCOS_SRC_PATH}/base/CCThreadPool.h
    ${COCOS_SRC_PATH}/base/ccUTF8.h
    ${COCOS_SRC_PATH}/base/ccUtils.h
    ${COCOS_SRC_PATH}/base/CCValue.h
    ${COCOS_SRC_PATH}/base/CCVector.h
    ${COCOS_SRC_PATH}/base/etc1.h
    ${COCOS_SRC_PATH}/base/etc2.h
    ${COCOS_SRC_PATH}/base/pvr.h
    ${COCOS_SRC_PATH}/base/uthash.h
    ${COCOS_SRC_PATH}/base/utlist.h
    ${COCOS_SRC_PATH}/base/ZipUtils.h
)

set(CC_BASE_SOURCES
    ${COCOS_SRC_PATH}/base/base64.cpp
    ${COCOS_SRC_PATH}/base/CCAutoreleasePool.cpp
    ${COCOS_SRC_PATH}/base/ccCArray.cpp
    ${COCOS_SRC_PATH}/base/CCData.cpp
    # ${COCOS_SRC_PATH}/base/CCGLUtils.cpp
    ${COCOS_SRC_PATH}/base/CCLog.cpp
    ${COCOS_SRC_PATH}/base/ccRandom.cpp
    ${COCOS_SRC_PATH}/base/CCRef.cpp
   
    ${COCOS_SRC_PATH}/base/ccUTF8.cpp
    ${COCOS_SRC_PATH}/base/ccUtils.cpp
    ${COCOS_SRC_PATH}/base/CCValue.cpp
    ${COCOS_SRC_PATH}/base/csscolorparser.cpp
    ${COCOS_SRC_PATH}/base/etc1.cpp
    ${COCOS_SRC_PATH}/base/etc2.cpp
    ${COCOS_SRC_PATH}/base/pvr.cpp
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
list(REMOVE_ITEM CC_EXTERNAL_LIBS "glfw3")

# resources
set(GAME_RES_FOLDER "${CMAKE_CURRENT_SOURCE_DIR}/Resources")
if(VS)
    cocos_mark_multi_resources(cc_common_res RES_TO "Resources" FOLDERS ${GAME_RES_FOLDER})
endif()
set(APP_UI_RES
    "${GAME_RES_FOLDER}/uv_checker_01.jpg"
    "${GAME_RES_FOLDER}/uv_checker_02.jpg"
    "${GAME_RES_FOLDER}/stencil.jpg"
    "${GAME_RES_FOLDER}/background.png"
    "${GAME_RES_FOLDER}/sprite0.png"
)

# add test cases
include(${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt)

if(WIN32)
    add_definitions("-DCC_STATIC")
    add_subdirectory(win)
elseif(IOS)
	add_subdirectory(ios)
elseif(ANDROID)
    add_subdirectory(android/app/src/main/cpp)
elseif(APPLE)
    add_subdirectory(mac)
endif()