#pragma once
#include "base/Config.h"
#include <type_traits>
#include "cocos/bindings/jswrapper/SeApi.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "../gfx-test-case/tests/ScriptTest/Transform.h"

bool js_register_transform_Transform(se::Object* obj);
bool register_all_transform(se::Object* obj);
