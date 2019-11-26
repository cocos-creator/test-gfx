set(TARGET_NAME GFXTestIOS)

message(STATUS "Target: ${TARGET_NAME}...")

# ---------------------------------------------------------------------
# set include files

set(INCLUDE_FILES
  ${COCOS_EXTERNAL_PATH}/khronos
  ${COCOS_SRC_PATH}/Core
  ${COCOS_SRC_PATH}/GFXGLES3
  ./GFXTest
)

include_directories( ${INCLUDE_FILES} )

# ---------------------------------------------------------------------

file(GLOB_RECURSE HEADER_FILES *.h)
file(GLOB_RECURSE SOURCE_CPP_FILES *.cpp *.cc)
file(GLOB_RECURSE SOURCE_C_FILES *.c)

if(COCOS_PLATFORM_IOS OR COCOS_PLATFORM_OSX)
	file(GLOB_RECURSE SOURCE_MM_FILES *.m *.mm)
endif()

set(ALL_FILES ${HEADER_FILES} ${SOURCE_CPP_FILES} ${SOURCE_C_FILES})

if(COCOS_PLATFORM_IOS OR COCOS_PLATFORM_OSX)
	set(ALL_FILES ${ALL_FILES} ${SOURCE_MM_FILES})
endif()

if(COCOS_UNICODE)
	add_definitions("-DUNICODE -D_UNICODE")
endif()

add_executable(${TARGET_NAME} MACOSX_BUNDLE ${ALL_FILES})

target_link_libraries(${TARGET_NAME} Core GFXGLES3)

message(STATUS "${TARGET_NAME} configuration completed.") 
