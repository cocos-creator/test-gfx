set(TARGET_NAME ScriptEngine)
set(CMAKE_CXX_STANDARD 17)

set(CURRENT_DIR ${COCOS_SRC_PATH}/bindings/jswrapper)

file(GLOB HEADER_FILES ${CURRENT_DIR}/*.h)
file(GLOB SOURCE_CPP_FILES ${CURRENT_DIR}/*.cpp  ${CURRENT_DIR}/*.cc)
file(GLOB SOURCE_C_FILES ${CURRENT_DIR}/*.c)

file(GLOB_RECURSE V8_HEADER_FILES ${CURRENT_DIR}/v8/*.h)
file(GLOB_RECURSE V8_SOURCE_CPP_FILES ${CURRENT_DIR}/v8/*.cpp ${CURRENT_DIR}/v8/*.cc)
file(GLOB_RECURSE V8_SOURCE_C_FILES ${CURRENT_DIR}/v8/*.c)

set(GENERAL_FILES
  ${COCOS_SRC_PATH}/bindings/manual/jsb_classtype.cpp
  ${COCOS_SRC_PATH}/bindings/manual/jsb_classtype.h
  ${COCOS_SRC_PATH}/bindings/manual/jsb_conversions.cpp
  ${COCOS_SRC_PATH}/bindings/manual/jsb_conversions.h
  ${COCOS_SRC_PATH}/bindings/manual/jsb_helper.cpp
  ${COCOS_SRC_PATH}/bindings/manual/jsb_helper.h
  ${COCOS_SRC_PATH}/bindings/manual/jsb_global_init.cpp
  ${COCOS_SRC_PATH}/bindings/manual/jsb_global_init.h
  ${COCOS_SRC_PATH}/bindings/manual/jsb_platform.h

  ${COCOS_SRC_PATH}/bindings/auto/jsb_gfx_auto.cpp
  ${COCOS_SRC_PATH}/bindings/auto/jsb_gfx_auto.h
  ${COCOS_SRC_PATH}/bindings/manual/jsb_gfx_manual.cpp
  ${COCOS_SRC_PATH}/bindings/manual/jsb_gfx_manual.h

  ${COCOS_SRC_PATH}/bindings/dop/BufferPool.cpp
  ${COCOS_SRC_PATH}/bindings/dop/BufferPool.h
  ${COCOS_SRC_PATH}/bindings/dop/ObjectPool.cpp
  ${COCOS_SRC_PATH}/bindings/dop/ObjectPool.h
  ${COCOS_SRC_PATH}/bindings/dop/jsb_dop.cpp
  ${COCOS_SRC_PATH}/bindings/dop/jsb_dop.h
  ${COCOS_SRC_PATH}/bindings/dop/PoolType.h
  ${COCOS_SRC_PATH}/bindings/dop/BufferAllocator.h
  ${COCOS_SRC_PATH}/bindings/dop/BufferAllocator.cpp

  ${COCOS_SRC_PATH}/bindings/event/CustomEventTypes.h
  ${COCOS_SRC_PATH}/bindings/event/EventDispatcher.cpp
  ${COCOS_SRC_PATH}/bindings/event/EventDispatcher.h
)

if(COCOS_PLATFORM_WIN32)
  set(PLATFORM_SPECIFIC_FILES
    ${COCOS_SRC_PATH}/bindings/manual/jsb_platfrom_win32.cpp
  )
elseif(COCOS_PLATFORM_ANDROID)
  set(PLATFORM_SPECIFIC_FILES
    ${COCOS_SRC_PATH}/bindings/manual/jsb_platform_android.cpp
    ${COCOS_SRC_PATH}/bindings/manual/JavaScriptJavaBridge.cpp
    ${COCOS_SRC_PATH}/bindings/manual/JavaScriptJavaBridge.h
  )
elseif(COCOS_PLATFORM_IOS OR COCOS_PLATFORM_OSX)
  set(PLATFORM_SPECIFIC_FILES
    ${COCOS_SRC_PATH}/bindings/manual/jsb_platfrom_apple.mm
    ${COCOS_SRC_PATH}/bindings/manual/JavaScriptObjCBridge.h
    ${COCOS_SRC_PATH}/bindings/manual/JavaScriptObjCBridge.mm
  )
  find_library(CORETEXT_LIBRARY CoreText)
  find_library(COREGRAPHICS_LIBRARY CoreGraphics)
  set(PLATFORM_SPECIFIC_LIBS ${CORETEXT_LIBRARY} ${COREGRAPHICS_LIBRARY})
endif()

set(ALL_FILES
    ${HEADER_FILES}
    ${SOURCE_CPP_FILES}
    ${SOURCE_C_FILES}
    ${V8_HEADER_FILES}
    ${V8_SOURCE_CPP_FILES}
    ${V8_SOURCE_C_FILES}
    ${CC_EXTERNAL_SROUCES}
    ${GENERAL_FILES}
    ${PLATFORM_SPECIFIC_FILES}
)

if(COCOS_UNICODE)
    add_definitions("-DUNICODE -D_UNICODE")
endif()

if (COCOS_STATIC)
    add_library(${TARGET_NAME} STATIC ${HEADER_FILES} ${ALL_FILES})
else()
    add_library(${TARGET_NAME} SHARED ${HEADER_FILES} ${ALL_FILES})
endif()

target_link_libraries(${TARGET_NAME} Core ${CC_EXTERNAL_LIBS} ${PLATFORM_SPECIFIC_LIBS})

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME CCScriptEngine)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG CCScriptEngine_d)

message(STATUS "${TARGET_NAME} configuration completed.")

target_compile_definitions(${TARGET_NAME} PUBLIC
    $<$<CONFIG:Debug>:CC_DEBUG=1>
)
