#pragma once
#include "base/Config.h"
#include <type_traits>
#include "cocos/bindings/jswrapper/SeApi.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "tests/ScriptTest/Boids.h"

extern se::Object* __jsb_application_BoidsOptions_proto;
extern se::Class* __jsb_application_BoidsOptions_class;

bool js_register_application_BoidsOptions(se::Object* obj);
bool register_all_boids(se::Object* obj);

template<>
bool sevalue_to_native(const se::Value &, application::BoidsOptions *, se::Object *ctx);
JSB_REGISTER_OBJECT_TYPE(application::BoidsOptions);

extern se::Object* __jsb_application_BoidsManager_proto;
extern se::Class* __jsb_application_BoidsManager_class;

bool js_register_application_BoidsManager(se::Object* obj);
bool register_all_boids(se::Object* obj);

JSB_REGISTER_OBJECT_TYPE(application::BoidsManager);
SE_DECLARE_FUNC(js_boids_BoidsManager_destroy);
SE_DECLARE_FUNC(js_boids_BoidsManager_init);
SE_DECLARE_FUNC(js_boids_BoidsManager_tick);

