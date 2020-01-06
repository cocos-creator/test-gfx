set(GFX_TESTCASE_HEADER
    ${COCOS_ROOT_PATH}/tests/TestBase.h
    ${COCOS_ROOT_PATH}/tests/ClearScreenTest.h
    ${COCOS_ROOT_PATH}/tests/BasicTriangleTest.h
    ${COCOS_ROOT_PATH}/tests/BasicTextureTest.h
    ${COCOS_ROOT_PATH}/tests/DepthTest.h
    ${COCOS_ROOT_PATH}/tests/BunnyData.h
)

set(GFX_TESTCASE_SOURCE
    ${COCOS_ROOT_PATH}/tests/TestBase.cpp
    ${COCOS_ROOT_PATH}/tests/ClearScreenTest.cc
    ${COCOS_ROOT_PATH}/tests/BasicTriangleTest.cc
    ${COCOS_ROOT_PATH}/tests/BasicTextureTest.cc
    ${COCOS_ROOT_PATH}/tests/DepthTest.cc
)

if(APPLE AND NOT IOS)
list(REMOVE_ITEM GFX_TESTCASE_HEADER ${COCOS_ROOT_PATH}/tests/BasicTextureTest.h)
list(REMOVE_ITEM GFX_TESTCASE_SOURCE ${COCOS_ROOT_PATH}/tests/BasicTextureTest.cc)
endif()
