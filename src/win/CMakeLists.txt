set(TARGET_NAME ${APP_NAME})

message(STATUS "Target: ${TARGET_NAME}...")

# ---------------------------------------------------------------------
# set include files

# include(${PROJECT_SOURCE_DIR}/tests/CMakeLists.txt)

set(INCLUDE_FILES
  ${GFX_EXTERNAL_PATH}/khronos
  ${GFX_EXTERNAL_PATH}/boost
  ${GFX_EXTERNAL_PATH}/concurrentqueue
  ${GFX_EXTERNAL_PATH}/tbb/include
  ${GFX_EXTERNAL_PATH}/taskflow
  ${GFX_EXTERNAL_PATH}/acl/includes
  ${GFX_EXTERNAL_PATH}/rtm/includes
  ${COCOS_SRC_PATH}/renderer/core
  ${COCOS_SRC_PATH}/renderer/gfx-gles2
  ${COCOS_SRC_PATH}/renderer/gfx-gles3
  ${COCOS_SRC_PATH}/renderer/gfx-vulkan
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
    ${CC_EXTERNAL_SOURCES}
)

if(COCOS_UNICODE)
	add_definitions("-DUNICODE -D_UNICODE")
endif()

add_definitions("-DCC_USE_VULKAN")
add_definitions("-DCC_USE_GLES3")
add_definitions("-DCC_USE_GLES2")

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

list(APPEND CC_EXTERNAL_LIBS
  ws2_32 userenv psapi winmm Iphlpapi
)

target_link_libraries(${TARGET_NAME}
  ${ENGINE_BASE_LIBS}
  GFXGLESCommon
  GFXGLES2
  GFXGLES3
  GFXVulkan
  ${CC_EXTERNAL_LIBS}
)

if(LINUX OR WINDOWS)
  set(bin_dir ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})
  add_custom_target(copy_resource ALL
      COMMAND ${CMAKE_COMMAND} -E echo "Copying resources to ${bin_dir}"
      COMMAND ${CMAKE_COMMAND} -E make_directory ${bin_dir}/Resources
      COMMAND robocopy "${GAME_RES_FOLDER}/" "${bin_dir}/Resources/" /MIR || (exit 0)
      COMMAND ${CMAKE_COMMAND} -E echo "Copying resources done!"
  )
  add_dependencies(${TARGET_NAME} copy_resource)
  set_target_properties(copy_resource PROPERTIES FOLDER Utils)
endif()

if(MSVC)
    file(GLOB_RECURSE WIN32_DLLS
        ${COCOS_EXTERNAL_PATH}/win32/libs/*.dll
        ${COCOS_EXTERNAL_PATH}/win32/libs/Debug/*.dll
    )

    foreach(item ${WIN32_DLLS})
        get_filename_component(filename ${item} NAME)
        get_filename_component(abs ${item} ABSOLUTE)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${abs} $<TARGET_FILE_DIR:${TARGET_NAME}>/${filename}
        )
    endforeach()
    target_link_options(${TARGET_NAME} PRIVATE /SUBSYSTEM:WINDOWS)
endif()

source_group(TREE ${COCOS_SRC_PATH}/base PREFIX "Header Files/base" FILES ${CC_BASE_HEADERS})
source_group(TREE ${COCOS_SRC_PATH}/base PREFIX "Source Files/base" FILES ${CC_BASE_SOURCES})
source_group(TREE ${COCOS_SRC_PATH}/math PREFIX "Header Files/math" FILES ${CC_MATH_HEADERS})
source_group(TREE ${COCOS_SRC_PATH}/math PREFIX "Source Files/math" FILES ${CC_MATH_SOURCES})
source_group(TREE ${COCOS_EXTERNAL_PATH}/sources PREFIX "Header Files/external" FILES ${CC_EXTERNAL_SOURCES})

message(STATUS "${TARGET_NAME} configuration completed.")
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
