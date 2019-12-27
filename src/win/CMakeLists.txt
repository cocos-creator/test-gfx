set(TARGET_NAME GFXTestWin)

message(STATUS "Target: ${TARGET_NAME}...")

# ---------------------------------------------------------------------
# set include files

# include(${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt)

set(INCLUDE_FILES
  ${GFX_EXTERNAL_PATH}/khronos
  ${COCOS_SRC_PATH}/renderer/core
  ${COCOS_SRC_PATH}/renderer/gfx-gles2
  ${COCOS_SRC_PATH}/renderer/gfx-gles3
  ./GFXTest
)

# ---------------------------------------------------------------------

file(GLOB_RECURSE HEADER_FILES *.h)
file(GLOB_RECURSE SOURCE_CPP_FILES *.cpp *.cc)
file(GLOB_RECURSE SOURCE_C_FILES *.c)

set(ALL_FILES 
    ${HEADER_FILES} 
    ${SOURCE_CPP_FILES} 
    ${SOURCE_C_FILES} 
    ${GFX_TESTCASE_HEADER} 
    ${GFX_TESTCASE_SOURCE}
    ${CC_PLATFORM_SOURCES}
    ${CC_EXTERNAL_SROUCES}
    ${CC_BASE_HEADERS}
    ${CC_BASE_SOURCES}
    ${CC_MATH_HEADERS}
    ${CC_MATH_SOURCES}
    ${cc_common_res}
    ${COCOS_SRC_PATH}/cocos2d.h
    ${COCOS_SRC_PATH}/cocos2d.cpp
)

if(COCOS_UNICODE)
	add_definitions("-DUNICODE -D_UNICODE")
endif()

add_executable(${TARGET_NAME} WIN32 ${ALL_FILES})

if(COCOS_CPU_X64)
  set(PVRSDK_LIB_DIR ${GFX_EXTERNAL_PATH}/pvrsdk/lib/Windows_x86_64)
else()
  set(PVRSDK_LIB_DIR ${GFX_EXTERNAL_PATH}/pvrsdk/lib/Windows_x86_32)
endif()

# Gather list of all .dll files
file(GLOB PVRDLLS ${PVRSDK_LIB_DIR}/*.dll)
if(COCOS_PLATFORM_WIN32)
  list(APPEND PVRDLLS ${COCOS_EXTERNAL_PATH}/win32/libs/zlib1.dll)
endif()

foreach(PVRDLL ${PVRDLLS})
  add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                         copy ${PVRDLL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
endforeach()

target_include_directories(${TARGET_NAME} PUBLIC 
    ${PROJECT_SOURCE_DIR}/src/win 
    ${PROJECT_SOURCE_DIR}
    ${INCLUDE_FILES}
    ${COCOS_EXTERNAL_PATH}/sources
    ${CC_EXTERNAL_INCLUDES}
    ${COCOS_SRC_PATH}/platform
)

target_link_libraries(${TARGET_NAME} 
  Core 
  GFXGLES2 
  GFXGLES3
  ${CC_EXTERNAL_LIBS}
) 

include(CocosBuildHelpers)
if(LINUX OR WINDOWS)
    set(COCOS2DX_ROOT_PATH ${COCOS_EXTERNAL_PATH})
    cocos_def_copy_resource_target(${TARGET_NAME})
    cocos_copy_target_res(${TARGET_NAME} LINK_TO "${COCOS_BUILD_PATH}/${CMAKE_CFG_INTDIR}/Resources" FOLDERS ${GAME_RES_FOLDER})
endif()

source_group(TREE ${COCOS_SRC_PATH}/platform PREFIX "Header Files/platform" FILES ${CC_PLATFORM_SOURCES})
source_group(TREE ${COCOS_SRC_PATH}/base PREFIX "Header Files/base" FILES ${CC_BASE_HEADERS})
source_group(TREE ${COCOS_SRC_PATH}/base PREFIX "Source Files/base" FILES ${CC_BASE_SOURCES})
source_group(TREE ${COCOS_SRC_PATH}/math PREFIX "Header Files/math" FILES ${CC_MATH_HEADERS})
source_group(TREE ${COCOS_SRC_PATH}/math PREFIX "Source Files/math" FILES ${CC_MATH_SOURCES})
source_group(TREE ${COCOS_EXTERNAL_PATH}/sources PREFIX "Header Files/external" FILES ${CC_EXTERNAL_SROUCES})

message(STATUS "${TARGET_NAME} configuration completed.") 
