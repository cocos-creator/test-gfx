# add -lite external here for test project

# include modules
if(WIN32)
    set(CMAKE_MODULE_PATH ${GFX_EXTERNAL_PATH}/cmake)
    include(CocosBuildSet)
endif()

# ---------------------------------------------
# set cocos platform, base, math, external files
set(CC_PLATFORM_SOURCES
    ${COCOS_SRC_PATH}/platform/Application.h
    ${COCOS_SRC_PATH}/platform/Device.h
    ${COCOS_SRC_PATH}/platform/FileUtils.h
    ${COCOS_SRC_PATH}/platform/Image.h
    ${COCOS_SRC_PATH}/platform/SAXParser.h

    ${COCOS_SRC_PATH}/platform/FileUtils.cpp
    ${COCOS_SRC_PATH}/platform/Image.cpp
    ${COCOS_SRC_PATH}/platform/SAXParser.cpp
)
if(WIN32)
    list(APPEND CC_PLATFORM_SOURCES
        ${COCOS_SRC_PATH}/platform/win32/FileUtils-win32.h
        ${COCOS_SRC_PATH}/platform/win32/Utils-win32.h

        ${COCOS_SRC_PATH}/platform/win32/Device-win32.cpp
        ${COCOS_SRC_PATH}/platform/win32/FileUtils-win32.cpp
        ${COCOS_SRC_PATH}/platform/win32/Utils-win32.cpp
    )
elseif(ANDROID)
    list(APPEND CC_PLATFORM_SOURCES
        ${COCOS_SRC_PATH}/platform/android/FileUtils-android.h
        ${COCOS_SRC_PATH}/platform/android/jni/JniHelper.h
        # ${COCOS_SRC_PATH}/platform/android/jni/JniImp.h

        # ${COCOS_SRC_PATH}/platform/android/Device-android.cpp
        ${COCOS_SRC_PATH}/platform/android/FileUtils-android.cpp
        # ${COCOS_SRC_PATH}/platform/android/Application-android.cpp
        # ${COCOS_SRC_PATH}/platform/android/CanvasRenderingContext2D-android.cpp
        # ${COCOS_SRC_PATH}/platform/android/jni/JniImp.cpp
        ${COCOS_SRC_PATH}/platform/android/jni/JniHelper.cpp
    )
elseif(APPLE)
    list(APPEND CC_PLATFORM_SOURCES
        ${COCOS_SRC_PATH}/platform/apple/FileUtils-apple.h

        ${COCOS_SRC_PATH}/platform/apple/FileUtils-apple.mm
    )
endif()

set(CC_BASE_HEADERS
    ${COCOS_SRC_PATH}/base/base64.h
    ${COCOS_SRC_PATH}/base/AutoreleasePool.h
    ${COCOS_SRC_PATH}/base/CachedArray.h
    ${COCOS_SRC_PATH}/base/Config.h
    ${COCOS_SRC_PATH}/base/Data.h
    ${COCOS_SRC_PATH}/base/Log.h
    ${COCOS_SRC_PATH}/base/Macros.h
    ${COCOS_SRC_PATH}/base/Map.h
    ${COCOS_SRC_PATH}/base/Random.h
    ${COCOS_SRC_PATH}/base/Ref.h
    # ${COCOS_SRC_PATH}/base/Scheduler.h
    # ${COCOS_SRC_PATH}/base/ThreadPool.h
    ${COCOS_SRC_PATH}/base/StringUtil.h
    ${COCOS_SRC_PATH}/base/UTFString.h
    ${COCOS_SRC_PATH}/base/UTF8.h
    ${COCOS_SRC_PATH}/base/Utils.h
    ${COCOS_SRC_PATH}/base/Value.h
    ${COCOS_SRC_PATH}/base/Vector.h
    ${COCOS_SRC_PATH}/base/etc1.h
    ${COCOS_SRC_PATH}/base/etc2.h
    ${COCOS_SRC_PATH}/base/uthash.h
    ${COCOS_SRC_PATH}/base/utlist.h
    ${COCOS_SRC_PATH}/base/ZipUtils.h
)

set(CC_BASE_SOURCES
    ${COCOS_SRC_PATH}/base/base64.cpp
    ${COCOS_SRC_PATH}/base/AutoreleasePool.cpp
    ${COCOS_SRC_PATH}/base/Data.cpp
    ${COCOS_SRC_PATH}/base/Log.cpp
    ${COCOS_SRC_PATH}/base/Random.cpp
    ${COCOS_SRC_PATH}/base/Ref.cpp

    ${COCOS_SRC_PATH}/base/StringUtil.cpp
    ${COCOS_SRC_PATH}/base/UTFString.cpp
    ${COCOS_SRC_PATH}/base/UTF8.cpp
    ${COCOS_SRC_PATH}/base/Utils.cpp
    ${COCOS_SRC_PATH}/base/Value.cpp
    ${COCOS_SRC_PATH}/base/csscolorparser.cpp
    ${COCOS_SRC_PATH}/base/etc1.cpp
    ${COCOS_SRC_PATH}/base/etc2.cpp
    ${COCOS_SRC_PATH}/base/ZipUtils.cpp
)

set(CC_MATH_HEADERS
    ${COCOS_SRC_PATH}/math/Geometry.h
    ${COCOS_SRC_PATH}/math/MathBase.h
    ${COCOS_SRC_PATH}/math/Math.h
    ${COCOS_SRC_PATH}/math/Vertex.h
    ${COCOS_SRC_PATH}/math/Mat4.h
    ${COCOS_SRC_PATH}/math/MathUtil.h
    ${COCOS_SRC_PATH}/math/Quaternion.h
    ${COCOS_SRC_PATH}/math/Vec2.h
    ${COCOS_SRC_PATH}/math/Vec3.h
    ${COCOS_SRC_PATH}/math/Vec4.h
)

set(CC_MATH_SOURCES
    ${COCOS_SRC_PATH}/math/Geometry.cpp
    ${COCOS_SRC_PATH}/math/Vertex.cpp
    ${COCOS_SRC_PATH}/math/Mat4.cpp
    ${COCOS_SRC_PATH}/math/MathUtil.cpp
    ${COCOS_SRC_PATH}/math/Quaternion.cpp
    ${COCOS_SRC_PATH}/math/Vec2.cpp
    ${COCOS_SRC_PATH}/math/Vec3.cpp
    ${COCOS_SRC_PATH}/math/Vec4.cpp
    ${COCOS_SRC_PATH}/math/Math.cpp
)

set(USE_SE_V8 ON)
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

    # add_subdirectory(mac-gles)
endif()
