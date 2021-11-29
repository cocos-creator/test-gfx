macro (set_xcode_property TARGET XCODE_PROPERTY XCODE_VALUE)
    set_property (TARGET ${TARGET} PROPERTY XCODE_ATTRIBUTE_${XCODE_PROPERTY}
                 ${XCODE_VALUE})
endmacro (set_xcode_property)

macro (use_molten_vk)
    set(USE_METAL OFF)
    set(USE_VULKAN ON)
endmacro (use_molten_vk)

message(STATUS "App: ${APP_NAME}...")

option(USE_METAL "USE METAL" ON)
option(USE_VULKAN "USE METAL" OFF)

# use_molten_vk()

# ---------------------------------------------------------------------

file(GLOB_RECURSE HEADER_FILES *.h)
file(GLOB_RECURSE SOURCE_CPP_FILES *.cpp *.cc)
file(GLOB_RECURSE SOURCE_C_FILES *.c)
file(GLOB_RECURSE SOURCE_MM_FILES *.m *.mm)

set(ALL_FILES
    ${HEADER_FILES}
    ${SOURCE_CPP_FILES}
    ${SOURCE_C_FILES}
    ${SOURCE_MM_FILES}
    ${GFX_TESTCASE_HEADER}
    ${GFX_TESTCASE_SOURCE}
    ${APP_UI_RES}
    ${CC_EXTERNAL_SOURCES}
    ${cc_common_res}
    )

if (USE_VULKAN)
    add_definitions("-DCC_USE_VULKAN")
    set(LIB_MOLTEN_VK "${GFX_EXTERNAL_PATH}/MoltenVK/ios/release/libMoltenVK.dylib")
    list(APPEND ALL_FILES ${LIB_MOLTEN_VK})
elseif (USE_METAL)
    add_definitions("-DCC_USE_METAL")
endif()

add_executable(${APP_NAME} ${ALL_FILES})

set_target_properties(${APP_NAME} PROPERTIES RESOURCE "${APP_UI_RES}")
set_xcode_property(${APP_NAME} INFOPLIST_FILE ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist)
set_xcode_property(${APP_NAME} PRODUCT_BUNDLE_IDENTIFIER "com.cocos.device-test")

# Locate system libraries on iOS
find_library(UIKIT_LIBRARY UIKit)
find_library(QUARTZCORE_LIBRARY QuartzCore)
find_library(MPS_LIBRARY MetalPerformanceShaders)
list(APPEND PLATFORM_SPECIFIC_LIBS
    ${UIKIT_LIBRARY}
    ${QUARTZCORE_LIBRARY}
    ${MPS_LIBRARY}
)

if (USE_VULKAN)
    find_library(METAL_LIBRARY Metal)
    find_library(METALKIT_LIBRARY MetalKit)
    list(APPEND PLATFORM_SPECIFIC_LIBS ${METAL_LIBRARY} ${METALKIT_LIBRARY})
    set(GRAPHIC_LIBS GFXVulkan)
    set_source_files_properties(${LIB_MOLTEN_VK} PROPERTIES MACOSX_PACKAGE_LOCATION Frameworks)
    set_source_files_properties(${LIB_MOLTEN_VK} PROPERTIES XCODE_FILE_ATTRIBUTES "CodeSignOnCopy")
    set_target_properties(${APP_NAME} PROPERTIES FRAMEWORK true)
    set_xcode_property(${APP_NAME} LD_RUNPATH_SEARCH_PATHS @executable_path/Frameworks)
elseif (USE_METAL)
    find_library(METAL_LIBRARY Metal)
    find_library(METALKIT_LIBRARY MetalKit)
    list(APPEND PLATFORM_SPECIFIC_LIBS ${METAL_LIBRARY} ${METALKIT_LIBRARY})
    set(GRAPHIC_LIBS GFXMetal)
endif()

target_link_libraries(${APP_NAME}
    ${PLATFORM_SPECIFIC_LIBS}
    ${ENGINE_BASE_LIBS}
    ${GRAPHIC_LIBS}
    ${CC_EXTERNAL_LIBS}
    z
)

target_include_directories(${APP_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${COCOS_SRC_PATH}/platform
    ${COCOS_EXTERNAL_PATH}/sources
    ${CC_EXTERNAL_INCLUDES}
    ${CC_EXTERNAL_INCLUDES}/v8
)

if(MACOSX)
    target_include_directories(${APP_NAME} PRIVATE
        ##FIXME: bad dependency
        ${COCOS_SRC_PATH}/platform/ios
    )
endif()

source_group(TREE ${COCOS_SRC_PATH}/base PREFIX "Header Files/base" FILES ${CC_BASE_HEADERS})
source_group(TREE ${COCOS_SRC_PATH}/base PREFIX "Source Files/base" FILES ${CC_BASE_SOURCES})
source_group(TREE ${COCOS_SRC_PATH}/math PREFIX "Header Files/math" FILES ${CC_MATH_HEADERS})
source_group(TREE ${COCOS_SRC_PATH}/math PREFIX "Source Files/math" FILES ${CC_MATH_SOURCES})
source_group(TREE ${COCOS_EXTERNAL_PATH}/sources PREFIX "Header Files/external" FILES ${CC_EXTERNAL_SOURCES})

# setup default flags
target_compile_definitions(${APP_NAME} PUBLIC
    $<$<NOT:$<BOOL:${USE_VIDEO}>>:USE_VIDEO=0>
    $<$<NOT:$<BOOL:${USE_WEB_VIEW}>>:USE_WEB_VIEW=0>
    $<$<NOT:$<BOOL:${USE_AUDIO}>>:USE_AUDIO=0>
    $<$<NOT:$<BOOL:${USE_SOCKET}>>:USE_SOCKET=0>
    $<$<NOT:$<BOOL:${USE_MIDDLEWARE}>>:USE_MIDDLEWARE=0>
    $<$<NOT:$<BOOL:${USE_SPINE}>>:USE_SPINE=0>
    $<$<NOT:$<BOOL:${USE_DRAGONBONES}>>:USE_DRAGONBONES=0>
    $<$<NOT:$<BOOL:${USE_TIFF}>>:CC_USE_TIFF=0>
    $<$<NOT:$<BOOL:${USE_JPEG}>>:CC_USE_JPEG=0>
    $<$<BOOL:${USE_GFX_RENDERER}>:USE_GFX_RENDERER=1>
    $<$<NOT:$<BOOL:${USE_V8_DEBUGGER}>>:USE_V8_DEBUGGER=0>
)

message(STATUS "${APP_NAME} configuration completed.")
