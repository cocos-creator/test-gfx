#pragma once
#include "base/Config.h"
#include <type_traits>
#include "cocos/bindings/jswrapper/SeApi.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "../gfx-test-case/tests/ScriptTest/Chassis.h"

extern se::Object* __jsb_cc_Transform_proto;
extern se::Class* __jsb_cc_Transform_class;

bool js_register_cc_Transform(se::Object* obj);
bool register_all_chassis(se::Object* obj);

JSB_REGISTER_OBJECT_TYPE(cc::Transform);
SE_DECLARE_FUNC(js_chassis_Transform_setParent);
SE_DECLARE_FUNC(js_chassis_Transform_setPosition);
SE_DECLARE_FUNC(js_chassis_Transform_setRotation);
SE_DECLARE_FUNC(js_chassis_Transform_setRotationFromEuler);
SE_DECLARE_FUNC(js_chassis_Transform_setScale);
SE_DECLARE_FUNC(js_chassis_Transform_Transform);

extern se::Object* __jsb_cc_Model_proto;
extern se::Class* __jsb_cc_Model_class;

bool js_register_cc_Model(se::Object* obj);
bool register_all_chassis(se::Object* obj);

JSB_REGISTER_OBJECT_TYPE(cc::Model);
SE_DECLARE_FUNC(js_chassis_Model_setColor);
SE_DECLARE_FUNC(js_chassis_Model_setEnabled);
SE_DECLARE_FUNC(js_chassis_Model_setTransform);
SE_DECLARE_FUNC(js_chassis_Model_Model);

extern se::Object* __jsb_cc_Root_proto;
extern se::Class* __jsb_cc_Root_class;

bool js_register_cc_Root(se::Object* obj);
bool register_all_chassis(se::Object* obj);

JSB_REGISTER_OBJECT_TYPE(cc::Root);
SE_DECLARE_FUNC(js_chassis_Root_createModel);
SE_DECLARE_FUNC(js_chassis_Root_createTransform);
SE_DECLARE_FUNC(js_chassis_Root_destroy);
SE_DECLARE_FUNC(js_chassis_Root_destroyModel);
SE_DECLARE_FUNC(js_chassis_Root_getModel);
SE_DECLARE_FUNC(js_chassis_Root_initialize);
SE_DECLARE_FUNC(js_chassis_Root_render);
SE_DECLARE_FUNC(js_chassis_Root_getInstance);

extern se::Object* __jsb_cc_RootManager_proto;
extern se::Class* __jsb_cc_RootManager_class;

bool js_register_cc_RootManager(se::Object* obj);
bool register_all_chassis(se::Object* obj);

JSB_REGISTER_OBJECT_TYPE(cc::RootManager);
SE_DECLARE_FUNC(js_chassis_RootManager_destroy);
SE_DECLARE_FUNC(js_chassis_RootManager_create);

