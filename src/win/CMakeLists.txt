set(TARGET_NAME GFXTestWin)

message(STATUS "Target: ${TARGET_NAME}...")

# ---------------------------------------------------------------------
# set include files

include(${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt)

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

set(ALL_FILES ${HEADER_FILES} ${SOURCE_CPP_FILES} ${SOURCE_C_FILES} ${GFX_TESTCASE_HEADER} ${GFX_TESTCASE_SOURCE})
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

foreach(PVRDLL ${PVRDLLS})
  add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
                     COMMAND ${CMAKE_COMMAND} -E
                         copy ${PVRDLL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
endforeach()

target_include_directories(${TARGET_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src/win ${PROJECT_SOURCE_DIR})
target_link_libraries(${TARGET_NAME} Core GFXGLES2 GFXGLES3)

message(STATUS "${TARGET_NAME} configuration completed.") 
