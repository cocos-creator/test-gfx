set(TARGET_NAME GFXVulkan)

set(CURRENT_DIR ${COCOS_SRC_PATH}/renderer/gfx-vulkan)

file(GLOB_RECURSE HEADER_FILES ${CURRENT_DIR}/*.h)
file(GLOB_RECURSE SOURCE_CPP_FILES ${CURRENT_DIR}/*.cpp ${CURRENT_DIR}/*.cc)
file(GLOB_RECURSE SOURCE_C_FILES ${CURRENT_DIR}/*.c)

set(ALL_FILES
  ${HEADER_FILES}
  ${SOURCE_CPP_FILES}
  ${SOURCE_C_FILES}
  )

add_definitions("-DVK_NO_PROTOTYPES")

if(WINDOWS)
	add_definitions("-DVK_USE_PLATFORM_WIN32_KHR")
	add_definitions("-DNOMINMAX")
elseif(NX)
	add_definitions("-DVK_USE_PLATFORM_VI_NN")
elseif(APPLE)
	add_definitions("-DVK_USE_PLATFORM_METAL_EXT")
elseif(ANDROID)
	add_definitions("-DVK_USE_PLATFORM_ANDROID_KHR")
else()
	add_definitions("-DVK_USE_PLATFORM_XLIB_KHR")
endif()

add_definitions("-DBARRIER_DEDUCTION_LEVEL=0")

if (COCOS_STATIC)
	add_library(${TARGET_NAME} STATIC ${HEADER_FILES} ${ALL_FILES})
else()
	add_library(${TARGET_NAME} SHARED ${HEADER_FILES} ${ALL_FILES})
endif()

target_link_libraries(${TARGET_NAME} Core GFXBase ${glslang_libs_name} tbb)
target_compile_definitions(${TARGET_NAME} PRIVATE ${CC_EXTERNAL_PRIVATE_DEFINITIONS})

set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME CCGFXVulkan)
set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG CCGFXVulkan_d)

use_precompiled_header("${CMAKE_CURRENT_SOURCE_DIR}/VKStd.h" "${CMAKE_CURRENT_SOURCE_DIR}/VKStd.cpp" SOURCE_CPP_FILES)

message(STATUS "${TARGET_NAME} configuration completed.")

target_compile_definitions(${TARGET_NAME} PUBLIC
    $<$<CONFIG:Debug>:CC_DEBUG=1>
)
