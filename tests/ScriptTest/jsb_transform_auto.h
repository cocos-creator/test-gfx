#pragma once
#include "base/Config.h"
#include <type_traits>
#include "cocos/bindings/jswrapper/SeApi.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "../gfx-test-case/tests/ScriptTest/Transform.h"

extern se::Object* __jsb_cc_Transform_proto;
extern se::Class* __jsb_cc_Transform_class;

bool js_register_cc_Transform(se::Object* obj);
bool register_all_transform(se::Object* obj);

JSB_REGISTER_OBJECT_TYPE(cc::Transform);
SE_DECLARE_FUNC(js_transform_Transform_setParent);
SE_DECLARE_FUNC(js_transform_Transform_setPosition);
SE_DECLARE_FUNC(js_transform_Transform_setRotation);
SE_DECLARE_FUNC(js_transform_Transform_setScale);
SE_DECLARE_FUNC(js_transform_Transform_Transform);

