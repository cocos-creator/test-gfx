set(TARGET_NAME Core)
set(CMAKE_CXX_STANDARD 17)

set(CURRENT_DIR ${COCOS_SRC_PATH}/base/)

file(GLOB_RECURSE HEADER_FILES ${CURRENT_DIR}/*.h)
file(GLOB_RECURSE SOURCE_CPP_FILES ${CURRENT_DIR}/*.cpp ${CURRENT_DIR}/*.cc)
file(GLOB_RECURSE SOURCE_C_FILES ${CURRENT_DIR}/*.c)

file(GLOB_RECURSE MATH_HEADER_FILES ${COCOS_SRC_PATH}/math/*.h ${COCOS_SRC_PATH}/math/*.inl)
file(GLOB_RECURSE MATH_SOURCE_CPP_FILES ${COCOS_SRC_PATH}/math/*.cpp ${COCOS_SRC_PATH}/math/*.cc)
file(GLOB_RECURSE MATH_SOURCE_C_FILES ${COCOS_SRC_PATH}/math/*.c)

set(PLATFORM_SUPPORT_SOURCE
  ${COCOS_SRC_PATH}/platform/Image.h
  ${COCOS_SRC_PATH}/platform/Image.cpp
  ${COCOS_SRC_PATH}/platform/SAXParser.h
  ${COCOS_SRC_PATH}/platform/SAXParser.cpp
  ${COCOS_SRC_PATH}/platform/FileUtils.h
  ${COCOS_SRC_PATH}/platform/FileUtils.cpp
  ${COCOS_SRC_PATH}/platform/BasePlatform.h
  ${COCOS_SRC_PATH}/platform/BasePlatform.cpp
  ${COCOS_SRC_PATH}/platform/SDLHelper.h
  ${COCOS_SRC_PATH}/platform/SDLHelper.cpp
  ${COCOS_SRC_PATH}/platform/StdC.h
  ${COCOS_SRC_PATH}/platform/UniversalPlatform.h
  ${COCOS_SRC_PATH}/platform/UniversalPlatform.cpp
  ${COCOS_SRC_PATH}/platform/interfaces/modules/Device.cpp
  ${COCOS_SRC_PATH}/platform/interfaces/modules/Device.h
  ${COCOS_SRC_PATH}/platform/interfaces/modules/IAccelerometer.h
  ${COCOS_SRC_PATH}/platform/interfaces/modules/IBattery.h
  ${COCOS_SRC_PATH}/platform/interfaces/modules/IScreen.h
  ${COCOS_SRC_PATH}/platform/interfaces/modules/INetwork.h
  ${COCOS_SRC_PATH}/platform/interfaces/modules/ISystem.cpp
  ${COCOS_SRC_PATH}/platform/interfaces/modules/ISystem.h
  ${COCOS_SRC_PATH}/platform/interfaces/modules/ISystemWindow.h
  ${COCOS_SRC_PATH}/platform/interfaces/modules/IVibrator.h
  ${COCOS_SRC_PATH}/platform/interfaces/OSInterface.cpp
  ${COCOS_SRC_PATH}/platform/interfaces/OSInterface.h
  ${COCOS_SRC_PATH}/platform/interfaces/OSInterfaceManager.cpp
  ${COCOS_SRC_PATH}/platform/interfaces/OSInterfaceManager.h
  ${COCOS_SRC_PATH}/application/ApplicationManager.h
  ${COCOS_SRC_PATH}/application/ApplicationManager.cpp
  ${COCOS_ROOT_PATH}/src/MainHack.cpp
)

if(WINDOWS)
  list(APPEND PLATFORM_SUPPORT_SOURCE
    ${COCOS_SRC_PATH}/platform/win32/FileUtils-win32.h
    ${COCOS_SRC_PATH}/platform/win32/FileUtils-win32.cpp
    ${COCOS_SRC_PATH}/platform/win32/Utils-win32.h
    ${COCOS_SRC_PATH}/platform/win32/Utils-win32.cpp
    ${COCOS_SRC_PATH}/platform/win32/WindowsPlatform.h
    ${COCOS_SRC_PATH}/platform/win32/WindowsPlatform.cpp
    ${COCOS_SRC_PATH}/platform/win32/modules/Accelerometer.cpp
    ${COCOS_SRC_PATH}/platform/win32/modules/Accelerometer.h
    ${COCOS_SRC_PATH}/platform/win32/modules/Battery.cpp
    ${COCOS_SRC_PATH}/platform/win32/modules/Battery.h
    ${COCOS_SRC_PATH}/platform/win32/modules/Screen.cpp
    ${COCOS_SRC_PATH}/platform/win32/modules/Screen.h
    ${COCOS_SRC_PATH}/platform/win32/modules/Network.cpp
    ${COCOS_SRC_PATH}/platform/win32/modules/Network.h
    ${COCOS_SRC_PATH}/platform/win32/modules/Vibrator.cpp
    ${COCOS_SRC_PATH}/platform/win32/modules/Vibrator.h
    ${COCOS_SRC_PATH}/platform/win32/modules/System.cpp
    ${COCOS_SRC_PATH}/platform/win32/modules/System.h
    ${COCOS_SRC_PATH}/platform/win32/modules/SystemWindow.cpp
    ${COCOS_SRC_PATH}/platform/win32/modules/SystemWindow.h
  )
elseif(ANDROID)
  list(APPEND PLATFORM_SUPPORT_SOURCE
    ${COCOS_SRC_PATH}/platform/android/FileUtils-android.h
    ${COCOS_SRC_PATH}/platform/android/FileUtils-android.cpp
    ${COCOS_SRC_PATH}/platform/java/jni/JniHelper.h
    ${COCOS_SRC_PATH}/platform/java/jni/JniHelper.cpp
  )
elseif(APPLE)
  list(APPEND PLATFORM_SUPPORT_SOURCE
    ${COCOS_SRC_PATH}/platform/apple/FileUtils-apple.h
    ${COCOS_SRC_PATH}/platform/apple/FileUtils-apple.mm
  )
endif()

set(ALL_FILES
    ${HEADER_FILES}
    ${SOURCE_CPP_FILES}
    ${SOURCE_C_FILES}
    ${MATH_HEADER_FILES}
    ${MATH_SOURCE_CPP_FILES}
    ${MATH_SOURCE_C_FILES}
    ${CC_EXTERNAL_SOURCES}
    ${PLATFORM_SUPPORT_SOURCE}
)

if(COCOS_UNICODE)
    add_definitions("-DUNICODE -D_UNICODE")
endif()

if (COCOS_STATIC)
    add_library(${TARGET_NAME} STATIC ${HEADER_FILES} ${ALL_FILES})
else()
    add_library(${TARGET_NAME} SHARED ${HEADER_FILES} ${ALL_FILES})
endif()

if(MACOSX)
    find_library(PLATFORM_SUPPORT_LIBRARY AppKit)
endif()

target_link_libraries(${TARGET_NAME} ${CC_EXTERNAL_LIBS} ${PLATFORM_SUPPORT_LIBRARY} ${SE_PLATFORM_SPECIFIC_LIBS})

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME CCCore)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG CCCore_d)

message(STATUS "${TARGET_NAME} configuration completed.")

target_compile_definitions(${TARGET_NAME} PUBLIC
    $<$<CONFIG:Debug>:CC_DEBUG=1>
)
