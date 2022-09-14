set(TARGET_NAME ${APP_NAME})

message(STATUS "Target: ${TARGET_NAME}...")

# ---------------------------------------------------------------------

file(GLOB_RECURSE HEADER_FILES *.h)
file(GLOB_RECURSE SOURCE_CPP_FILES *.cpp)
file(GLOB COMMON_CPP_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

set(ALL_FILES
  ${HEADER_FILES}
  ${COMMON_CPP_FILES}
  ${SOURCE_CPP_FILES}
  ${SOURCE_C_FILES}
  ${GFX_TESTCASE_HEADER}
  ${GFX_TESTCASE_SOURCE}
)

add_definitions("-DCC_USE_VULKAN")
add_definitions("-DCC_USE_GLES3")
add_definitions("-DCC_USE_GLES2")

add_executable(${TARGET_NAME} ${ALL_FILES})

target_include_directories(${TARGET_NAME} PUBLIC
  ${PROJECT_SOURCE_DIR}/src/win
  ${PROJECT_SOURCE_DIR}
  ${COCOS_EXTERNAL_PATH}/sources
  ${CC_EXTERNAL_INCLUDES}
  ${COCOS_SRC_PATH}/platform
)

list(APPEND CC_EXTERNAL_LIBS
  ws2_32 userenv psapi winmm Iphlpapi
)

target_link_libraries(${TARGET_NAME}
  ${ENGINE_BASE_LIBS}
  ${CC_EXTERNAL_LIBS}
)

cc_win32_definations(${TARGET_NAME})

set(bin_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR})
add_custom_target(copy_resource ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Copying resources to ${bin_dir}"
    COMMAND ${CMAKE_COMMAND} -E make_directory ${bin_dir}/Resources
    COMMAND robocopy "${GAME_RES_FOLDER}/" "${bin_dir}/Resources/" /MIR || (exit 0)
    COMMAND ${CMAKE_COMMAND} -E echo "Copying resources done!"
)
add_dependencies(${TARGET_NAME} copy_resource)
set_target_properties(copy_resource PROPERTIES FOLDER Utils)

if(MSVC)
  if(${CMAKE_SIZEOF_VOID_P} STREQUAL "4")
    file(GLOB_RECURSE WIN_DLLS ${COCOS_EXTERNAL_PATH}/win32/libs/*.dll)
  else()
    file(GLOB_RECURSE WIN_DLLS ${COCOS_EXTERNAL_PATH}/win64/libs/*.dll)
  endif()

  list(APPEND WIN_DLLS ${GFX_DEPENDENT_DLLS})

  foreach(item ${WIN_DLLS})
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

message(STATUS "${TARGET_NAME} configuration completed.")
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${TARGET_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
