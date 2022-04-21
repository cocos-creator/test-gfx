#pragma once
#include "base/Config.h"
#include <type_traits>
#include "cocos/bindings/jswrapper/SeApi.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "tests/ScriptTest/Chassis.h"

extern se::Object* __jsb_cc_experimental_TransformView_proto;
extern se::Class* __jsb_cc_experimental_TransformView_class;

bool js_register_cc_experimental_TransformView(se::Object* obj);
bool register_all_chassis(se::Object* obj);

JSB_REGISTER_OBJECT_TYPE(cc::experimental::TransformView);
SE_DECLARE_FUNC(js_chassis_TransformView_setParent);
SE_DECLARE_FUNC(js_chassis_TransformView_setPosition);
SE_DECLARE_FUNC(js_chassis_TransformView_setRotation);
SE_DECLARE_FUNC(js_chassis_TransformView_setRotationFromEuler);
SE_DECLARE_FUNC(js_chassis_TransformView_setScale);
SE_DECLARE_FUNC(js_chassis_TransformView_updateWorldTransform);
SE_DECLARE_FUNC(js_chassis_TransformView_TransformView);

extern se::Object* __jsb_cc_experimental_ModelView_proto;
extern se::Class* __jsb_cc_experimental_ModelView_class;

bool js_register_cc_experimental_ModelView(se::Object* obj);
bool register_all_chassis(se::Object* obj);

JSB_REGISTER_OBJECT_TYPE(cc::experimental::ModelView);
SE_DECLARE_FUNC(js_chassis_ModelView_setColor);
SE_DECLARE_FUNC(js_chassis_ModelView_setEnabled);
SE_DECLARE_FUNC(js_chassis_ModelView_setTransform);
SE_DECLARE_FUNC(js_chassis_ModelView_ModelView);

extern se::Object* __jsb_cc_experimental_Root_proto;
extern se::Class* __jsb_cc_experimental_Root_class;

bool js_register_cc_experimental_Root(se::Object* obj);
bool register_all_chassis(se::Object* obj);

JSB_REGISTER_OBJECT_TYPE(cc::experimental::Root);
SE_DECLARE_FUNC(js_chassis_Root_createModel);
SE_DECLARE_FUNC(js_chassis_Root_createTransform);
SE_DECLARE_FUNC(js_chassis_Root_destroy);
SE_DECLARE_FUNC(js_chassis_Root_initialize);
SE_DECLARE_FUNC(js_chassis_Root_render);
SE_DECLARE_FUNC(js_chassis_Root_getInstance);

extern se::Object* __jsb_cc_experimental_RootManager_proto;
extern se::Class* __jsb_cc_experimental_RootManager_class;

bool js_register_cc_experimental_RootManager(se::Object* obj);
bool register_all_chassis(se::Object* obj);

JSB_REGISTER_OBJECT_TYPE(cc::experimental::RootManager);
SE_DECLARE_FUNC(js_chassis_RootManager_destroy);
SE_DECLARE_FUNC(js_chassis_RootManager_create);
SE_DECLARE_FUNC(js_chassis_RootManager_getInstance);

