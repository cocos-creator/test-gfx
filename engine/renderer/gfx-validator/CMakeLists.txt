set(TARGET_NAME GFXValidator)

set(CURRENT_DIR ${COCOS_SRC_PATH}/renderer/gfx-validator)

file(GLOB_RECURSE HEADER_FILES ${CURRENT_DIR}/*.h)
file(GLOB_RECURSE SOURCE_CPP_FILES ${CURRENT_DIR}/*.cpp ${CURRENT_DIR}/*.cc)
file(GLOB_RECURSE SOURCE_C_FILES ${CURRENT_DIR}/*.c)

if(COCOS_PLATFORM_IOS OR COCOS_PLATFORM_OSX)
	file(GLOB_RECURSE SOURCE_MM_FILES ${CURRENT_DIR}/*.m ${CURRENT_DIR}/*.mm)
endif()

set(ALL_FILES
  ${HEADER_FILES}
  ${SOURCE_CPP_FILES}
  ${SOURCE_C_FILES}
  )

if(COCOS_PLATFORM_IOS OR COCOS_PLATFORM_OSX)
	set(ALL_FILES ${ALL_FILES} ${SOURCE_MM_FILES})
endif()

if(COCOS_UNICODE)
	add_definitions("-DUNICODE -D_UNICODE")
endif()

if (COCOS_STATIC)
	add_library(${TARGET_NAME} STATIC ${HEADER_FILES} ${ALL_FILES})
else()
	add_library(${TARGET_NAME} SHARED ${HEADER_FILES} ${ALL_FILES})
endif()

target_link_libraries(${TARGET_NAME} Core GFXBase)

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME CCGFXValidator)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG CCGFXValidator_d)

message(STATUS "${TARGET_NAME} configuration completed.")

target_compile_definitions(${TARGET_NAME} PUBLIC
    $<$<CONFIG:Debug>:CC_DEBUG=1>
)
