set(TARGET_NAME GFXAgent)

set(CURRENT_DIR ${COCOS_SRC_PATH}/renderer/gfx-agent)

file(GLOB_RECURSE HEADER_FILES ${CURRENT_DIR}/*.h)
file(GLOB_RECURSE SOURCE_CPP_FILES ${CURRENT_DIR}/*.cpp ${CURRENT_DIR}/*.cc)
file(GLOB_RECURSE SOURCE_C_FILES ${CURRENT_DIR}/*.c)

set(ALL_FILES
  ${HEADER_FILES}
  ${SOURCE_CPP_FILES}
  ${SOURCE_C_FILES}
  )

if (COCOS_STATIC)
	add_library(${TARGET_NAME} STATIC ${HEADER_FILES} ${ALL_FILES})
else()
	add_library(${TARGET_NAME} SHARED ${HEADER_FILES} ${ALL_FILES})
endif()

target_link_libraries(${TARGET_NAME} Core GFXBase)

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME CCGFXAgent)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG CCGFXAgent_d)

message(STATUS "${TARGET_NAME} configuration completed.")

target_compile_definitions(${TARGET_NAME} PUBLIC
    $<$<CONFIG:Debug>:CC_DEBUG=1>
)
