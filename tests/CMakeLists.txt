
file(GLOB_RECURSE GFX_TESTCASE_HEADER
    ${COCOS_ROOT_PATH}/tests/*.h
)

file(GLOB GFX_TESTCASE_SOURCE
    ${COCOS_ROOT_PATH}/tests/*.cc
    ${COCOS_ROOT_PATH}/tests/*.cpp
    ${COCOS_ROOT_PATH}/tests/utils/*.cpp
    ${COCOS_ROOT_PATH}/tests/utils/*.cc
)

list(APPEND GFX_TESTCASE_HEADER
    ${COCOS_SRC_PATH}/renderer/GFXDeviceManager.h
)
