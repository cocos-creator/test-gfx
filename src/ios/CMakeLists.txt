macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY}
                 ${XCODE_VALUE})
endmacro (set_xcode_property)

message(STATUS "App: ${APP_NAME}...")

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
    )

set(ALL_FILES 
    ${HEADER_FILES} 
    ${SOURCE_CPP_FILES} 
    ${SOURCE_C_FILES} 
    ${SOURCE_MM_FILES} 
    ${GFX_TESTCASE_HEADER} 
    ${GFX_TESTCASE_SOURCE} 
    ${APP_UI_RES}
    )

if(COCOS_UNICODE)
	add_definitions("-DUNICODE -D_UNICODE")
endif()

add_executable(${APP_NAME} ${ALL_FILES})

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
target_link_libraries(${APP_NAME} ${PLATFORM_SPECIFIC_LIBS} Core GFXGLES3 GFXGLES2)
target_include_directories(${APP_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

message(STATUS "${APP_NAME} configuration completed.") 
