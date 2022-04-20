#include "tests/ScriptTest/jsb_boids_auto.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "cocos/bindings/manual/jsb_global.h"
#include "Boids.h"

#ifndef JSB_ALLOC
#define JSB_ALLOC(kls, ...) new (std::nothrow) kls(__VA_ARGS__)
#endif

#ifndef JSB_FREE
#define JSB_FREE(ptr) delete ptr
#endif
se::Object* __jsb_application_BoidsOptions_proto = nullptr;
se::Class* __jsb_application_BoidsOptions_class = nullptr;

static bool js_boids_BoidsOptions_get_boidCount(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_get_boidCount : Invalid Native Object");

    CC_UNUSED bool ok = true;
    se::Value jsret;
    ok &= nativevalue_to_se(cobj->boidCount, jsret, s.thisObject() /*ctx*/);
    s.rval() = jsret;
    SE_HOLD_RETURN_VALUE(cobj->boidCount, s.thisObject(), s.rval());
    return true;
}
SE_BIND_PROP_GET(js_boids_BoidsOptions_get_boidCount)

static bool js_boids_BoidsOptions_set_boidCount(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    const auto& args = s.args();
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_set_boidCount : Invalid Native Object");

    CC_UNUSED bool ok = true;
    ok &= sevalue_to_native(args[0], &cobj->boidCount, s.thisObject());
    SE_PRECONDITION2(ok, false, "js_boids_BoidsOptions_set_boidCount : Error processing new value");
    return true;
}
SE_BIND_PROP_SET(js_boids_BoidsOptions_set_boidCount)

static bool js_boids_BoidsOptions_get_maxVelocity(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_get_maxVelocity : Invalid Native Object");

    CC_UNUSED bool ok = true;
    se::Value jsret;
    ok &= nativevalue_to_se(cobj->maxVelocity, jsret, s.thisObject() /*ctx*/);
    s.rval() = jsret;
    SE_HOLD_RETURN_VALUE(cobj->maxVelocity, s.thisObject(), s.rval());
    return true;
}
SE_BIND_PROP_GET(js_boids_BoidsOptions_get_maxVelocity)

static bool js_boids_BoidsOptions_set_maxVelocity(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    const auto& args = s.args();
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_set_maxVelocity : Invalid Native Object");

    CC_UNUSED bool ok = true;
    ok &= sevalue_to_native(args[0], &cobj->maxVelocity, s.thisObject());
    SE_PRECONDITION2(ok, false, "js_boids_BoidsOptions_set_maxVelocity : Error processing new value");
    return true;
}
SE_BIND_PROP_SET(js_boids_BoidsOptions_set_maxVelocity)

static bool js_boids_BoidsOptions_get_alignmentForce(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_get_alignmentForce : Invalid Native Object");

    CC_UNUSED bool ok = true;
    se::Value jsret;
    ok &= nativevalue_to_se(cobj->alignmentForce, jsret, s.thisObject() /*ctx*/);
    s.rval() = jsret;
    SE_HOLD_RETURN_VALUE(cobj->alignmentForce, s.thisObject(), s.rval());
    return true;
}
SE_BIND_PROP_GET(js_boids_BoidsOptions_get_alignmentForce)

static bool js_boids_BoidsOptions_set_alignmentForce(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    const auto& args = s.args();
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_set_alignmentForce : Invalid Native Object");

    CC_UNUSED bool ok = true;
    ok &= sevalue_to_native(args[0], &cobj->alignmentForce, s.thisObject());
    SE_PRECONDITION2(ok, false, "js_boids_BoidsOptions_set_alignmentForce : Error processing new value");
    return true;
}
SE_BIND_PROP_SET(js_boids_BoidsOptions_set_alignmentForce)

static bool js_boids_BoidsOptions_get_cohesionForce(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_get_cohesionForce : Invalid Native Object");

    CC_UNUSED bool ok = true;
    se::Value jsret;
    ok &= nativevalue_to_se(cobj->cohesionForce, jsret, s.thisObject() /*ctx*/);
    s.rval() = jsret;
    SE_HOLD_RETURN_VALUE(cobj->cohesionForce, s.thisObject(), s.rval());
    return true;
}
SE_BIND_PROP_GET(js_boids_BoidsOptions_get_cohesionForce)

static bool js_boids_BoidsOptions_set_cohesionForce(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    const auto& args = s.args();
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_set_cohesionForce : Invalid Native Object");

    CC_UNUSED bool ok = true;
    ok &= sevalue_to_native(args[0], &cobj->cohesionForce, s.thisObject());
    SE_PRECONDITION2(ok, false, "js_boids_BoidsOptions_set_cohesionForce : Error processing new value");
    return true;
}
SE_BIND_PROP_SET(js_boids_BoidsOptions_set_cohesionForce)

static bool js_boids_BoidsOptions_get_separationForce(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_get_separationForce : Invalid Native Object");

    CC_UNUSED bool ok = true;
    se::Value jsret;
    ok &= nativevalue_to_se(cobj->separationForce, jsret, s.thisObject() /*ctx*/);
    s.rval() = jsret;
    SE_HOLD_RETURN_VALUE(cobj->separationForce, s.thisObject(), s.rval());
    return true;
}
SE_BIND_PROP_GET(js_boids_BoidsOptions_get_separationForce)

static bool js_boids_BoidsOptions_set_separationForce(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    const auto& args = s.args();
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_set_separationForce : Invalid Native Object");

    CC_UNUSED bool ok = true;
    ok &= sevalue_to_native(args[0], &cobj->separationForce, s.thisObject());
    SE_PRECONDITION2(ok, false, "js_boids_BoidsOptions_set_separationForce : Error processing new value");
    return true;
}
SE_BIND_PROP_SET(js_boids_BoidsOptions_set_separationForce)

static bool js_boids_BoidsOptions_get_separationDistance(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_get_separationDistance : Invalid Native Object");

    CC_UNUSED bool ok = true;
    se::Value jsret;
    ok &= nativevalue_to_se(cobj->separationDistance, jsret, s.thisObject() /*ctx*/);
    s.rval() = jsret;
    SE_HOLD_RETURN_VALUE(cobj->separationDistance, s.thisObject(), s.rval());
    return true;
}
SE_BIND_PROP_GET(js_boids_BoidsOptions_get_separationDistance)

static bool js_boids_BoidsOptions_set_separationDistance(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    const auto& args = s.args();
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_set_separationDistance : Invalid Native Object");

    CC_UNUSED bool ok = true;
    ok &= sevalue_to_native(args[0], &cobj->separationDistance, s.thisObject());
    SE_PRECONDITION2(ok, false, "js_boids_BoidsOptions_set_separationDistance : Error processing new value");
    return true;
}
SE_BIND_PROP_SET(js_boids_BoidsOptions_set_separationDistance)

static bool js_boids_BoidsOptions_get_flockmateRadius(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_get_flockmateRadius : Invalid Native Object");

    CC_UNUSED bool ok = true;
    se::Value jsret;
    ok &= nativevalue_to_se(cobj->flockmateRadius, jsret, s.thisObject() /*ctx*/);
    s.rval() = jsret;
    SE_HOLD_RETURN_VALUE(cobj->flockmateRadius, s.thisObject(), s.rval());
    return true;
}
SE_BIND_PROP_GET(js_boids_BoidsOptions_get_flockmateRadius)

static bool js_boids_BoidsOptions_set_flockmateRadius(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    const auto& args = s.args();
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_set_flockmateRadius : Invalid Native Object");

    CC_UNUSED bool ok = true;
    ok &= sevalue_to_native(args[0], &cobj->flockmateRadius, s.thisObject());
    SE_PRECONDITION2(ok, false, "js_boids_BoidsOptions_set_flockmateRadius : Error processing new value");
    return true;
}
SE_BIND_PROP_SET(js_boids_BoidsOptions_set_flockmateRadius)

static bool js_boids_BoidsOptions_get_startingDelay(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_get_startingDelay : Invalid Native Object");

    CC_UNUSED bool ok = true;
    se::Value jsret;
    ok &= nativevalue_to_se(cobj->startingDelay, jsret, s.thisObject() /*ctx*/);
    s.rval() = jsret;
    SE_HOLD_RETURN_VALUE(cobj->startingDelay, s.thisObject(), s.rval());
    return true;
}
SE_BIND_PROP_GET(js_boids_BoidsOptions_get_startingDelay)

static bool js_boids_BoidsOptions_set_startingDelay(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    const auto& args = s.args();
    auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
    SE_PRECONDITION2(cobj, false, "js_boids_BoidsOptions_set_startingDelay : Invalid Native Object");

    CC_UNUSED bool ok = true;
    ok &= sevalue_to_native(args[0], &cobj->startingDelay, s.thisObject());
    SE_PRECONDITION2(ok, false, "js_boids_BoidsOptions_set_startingDelay : Error processing new value");
    return true;
}
SE_BIND_PROP_SET(js_boids_BoidsOptions_set_startingDelay)


template<>
bool sevalue_to_native(const se::Value &from, application::BoidsOptions * to, se::Object *ctx)
{
    assert(from.isObject());
    se::Object *json = from.toObject();
    auto* data = reinterpret_cast<application::BoidsOptions*>(json->getPrivateData());
    if (data) {
        *to = *data;
        return true;
    }
    se::Value field;
    bool ok = true;
    json->getProperty("boidCount", &field);
    if(!field.isNullOrUndefined()) {
        ok &= sevalue_to_native(field, &(to->boidCount), ctx);
    }
    json->getProperty("maxVelocity", &field);
    if(!field.isNullOrUndefined()) {
        ok &= sevalue_to_native(field, &(to->maxVelocity), ctx);
    }
    json->getProperty("alignmentForce", &field);
    if(!field.isNullOrUndefined()) {
        ok &= sevalue_to_native(field, &(to->alignmentForce), ctx);
    }
    json->getProperty("cohesionForce", &field);
    if(!field.isNullOrUndefined()) {
        ok &= sevalue_to_native(field, &(to->cohesionForce), ctx);
    }
    json->getProperty("separationForce", &field);
    if(!field.isNullOrUndefined()) {
        ok &= sevalue_to_native(field, &(to->separationForce), ctx);
    }
    json->getProperty("separationDistance", &field);
    if(!field.isNullOrUndefined()) {
        ok &= sevalue_to_native(field, &(to->separationDistance), ctx);
    }
    json->getProperty("flockmateRadius", &field);
    if(!field.isNullOrUndefined()) {
        ok &= sevalue_to_native(field, &(to->flockmateRadius), ctx);
    }
    json->getProperty("startingDelay", &field);
    if(!field.isNullOrUndefined()) {
        ok &= sevalue_to_native(field, &(to->startingDelay), ctx);
    }
    return ok;
}

SE_DECLARE_FINALIZE_FUNC(js_application_BoidsOptions_finalize)

static bool js_boids_BoidsOptions_constructor(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    CC_UNUSED bool ok = true;
    const auto& args = s.args();
    size_t argc = args.size();

    if(argc == 0)
    {
        application::BoidsOptions* cobj = JSB_ALLOC(application::BoidsOptions);
        s.thisObject()->setPrivateData(cobj);
        se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
        return true;
    }

    if(argc == 1 && args[0].isObject())
    {
        se::Object *json = args[0].toObject();
        se::Value field;

        application::BoidsOptions* cobj = JSB_ALLOC(application::BoidsOptions);
        ok &= sevalue_to_native(args[0], cobj, s.thisObject());
        if(!ok) {
            JSB_FREE(cobj);
            SE_REPORT_ERROR("argument convertion error");
            return false;
        }

        s.thisObject()->setPrivateData(cobj);
        se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
        return true;
    }

    application::BoidsOptions* cobj = JSB_ALLOC(application::BoidsOptions);
    if (argc > 0 && !args[0].isUndefined()) {
        ok &= sevalue_to_native(args[0], &(cobj->boidCount), nullptr);
    }
    if (argc > 1 && !args[1].isUndefined()) {
        ok &= sevalue_to_native(args[1], &(cobj->maxVelocity), nullptr);
    }
    if (argc > 2 && !args[2].isUndefined()) {
        ok &= sevalue_to_native(args[2], &(cobj->alignmentForce), nullptr);
    }
    if (argc > 3 && !args[3].isUndefined()) {
        ok &= sevalue_to_native(args[3], &(cobj->cohesionForce), nullptr);
    }
    if (argc > 4 && !args[4].isUndefined()) {
        ok &= sevalue_to_native(args[4], &(cobj->separationForce), nullptr);
    }
    if (argc > 5 && !args[5].isUndefined()) {
        ok &= sevalue_to_native(args[5], &(cobj->separationDistance), nullptr);
    }
    if (argc > 6 && !args[6].isUndefined()) {
        ok &= sevalue_to_native(args[6], &(cobj->flockmateRadius), nullptr);
    }
    if (argc > 7 && !args[7].isUndefined()) {
        ok &= sevalue_to_native(args[7], &(cobj->startingDelay), nullptr);
    }

    if(!ok) {
        JSB_FREE(cobj);
        SE_REPORT_ERROR("Argument convertion error");
        return false;
    }

    s.thisObject()->setPrivateData(cobj);
    se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
    return true;
}
SE_BIND_CTOR(js_boids_BoidsOptions_constructor, __jsb_application_BoidsOptions_class, js_application_BoidsOptions_finalize)



static bool js_application_BoidsOptions_finalize(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto iter = se::NonRefNativePtrCreatedByCtorMap::find(SE_THIS_OBJECT<application::BoidsOptions>(s));
    if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
    {
        se::NonRefNativePtrCreatedByCtorMap::erase(iter);
        auto* cobj = SE_THIS_OBJECT<application::BoidsOptions>(s);
        JSB_FREE(cobj);
    }
    return true;
}
SE_BIND_FINALIZE_FUNC(js_application_BoidsOptions_finalize)

bool js_register_boids_BoidsOptions(se::Object* obj) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto* cls = se::Class::create("BoidsOptions", obj, nullptr, _SE(js_boids_BoidsOptions_constructor));

    cls->defineProperty("boidCount", _SE(js_boids_BoidsOptions_get_boidCount), _SE(js_boids_BoidsOptions_set_boidCount));
    cls->defineProperty("maxVelocity", _SE(js_boids_BoidsOptions_get_maxVelocity), _SE(js_boids_BoidsOptions_set_maxVelocity));
    cls->defineProperty("alignmentForce", _SE(js_boids_BoidsOptions_get_alignmentForce), _SE(js_boids_BoidsOptions_set_alignmentForce));
    cls->defineProperty("cohesionForce", _SE(js_boids_BoidsOptions_get_cohesionForce), _SE(js_boids_BoidsOptions_set_cohesionForce));
    cls->defineProperty("separationForce", _SE(js_boids_BoidsOptions_get_separationForce), _SE(js_boids_BoidsOptions_set_separationForce));
    cls->defineProperty("separationDistance", _SE(js_boids_BoidsOptions_get_separationDistance), _SE(js_boids_BoidsOptions_set_separationDistance));
    cls->defineProperty("flockmateRadius", _SE(js_boids_BoidsOptions_get_flockmateRadius), _SE(js_boids_BoidsOptions_set_flockmateRadius));
    cls->defineProperty("startingDelay", _SE(js_boids_BoidsOptions_get_startingDelay), _SE(js_boids_BoidsOptions_set_startingDelay));
    cls->defineFinalizeFunction(_SE(js_application_BoidsOptions_finalize));
    cls->install();
    JSBClassType::registerClass<application::BoidsOptions>(cls);

    __jsb_application_BoidsOptions_proto = cls->getProto();
    __jsb_application_BoidsOptions_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}
se::Object* __jsb_application_BoidsManager_proto = nullptr;
se::Class* __jsb_application_BoidsManager_class = nullptr;

static bool js_boids_BoidsManager_destroy(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        application::BoidsManager::destroy();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_boids_BoidsManager_destroy)

static bool js_boids_BoidsManager_init(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        HolderType<application::BoidsOptions, true> arg0 = {};
        ok &= sevalue_to_native(args[0], &arg0, nullptr);
        SE_PRECONDITION2(ok, false, "js_boids_BoidsManager_init : Error processing arguments");
        application::BoidsManager::init(arg0.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_boids_BoidsManager_init)

static bool js_boids_BoidsManager_tick(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        HolderType<float, false> arg0 = {};
        ok &= sevalue_to_native(args[0], &arg0, nullptr);
        SE_PRECONDITION2(ok, false, "js_boids_BoidsManager_tick : Error processing arguments");
        application::BoidsManager::tick(arg0.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_boids_BoidsManager_tick)


static bool js_application_BoidsManager_finalize(se::State& s) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto iter = se::NonRefNativePtrCreatedByCtorMap::find(SE_THIS_OBJECT<application::BoidsManager>(s));
    if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
    {
        se::NonRefNativePtrCreatedByCtorMap::erase(iter);
        auto* cobj = SE_THIS_OBJECT<application::BoidsManager>(s);
        JSB_FREE(cobj);
    }
    return true;
}
SE_BIND_FINALIZE_FUNC(js_application_BoidsManager_finalize)

bool js_register_boids_BoidsManager(se::Object* obj) // NOLINT(readability-identifier-naming, google-runtime-references)
{
    auto* cls = se::Class::create("BoidsManager", obj, nullptr, nullptr);

    cls->defineStaticFunction("destroy", _SE(js_boids_BoidsManager_destroy));
    cls->defineStaticFunction("init", _SE(js_boids_BoidsManager_init));
    cls->defineStaticFunction("tick", _SE(js_boids_BoidsManager_tick));
    cls->defineFinalizeFunction(_SE(js_application_BoidsManager_finalize));
    cls->install();
    JSBClassType::registerClass<application::BoidsManager>(cls);

    __jsb_application_BoidsManager_proto = cls->getProto();
    __jsb_application_BoidsManager_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}
bool register_all_boids(se::Object* obj)
{
    // Get the ns
    se::Value nsVal;
    if (!obj->getProperty("jsb", &nsVal))
    {
        se::HandleObject jsobj(se::Object::createPlainObject());
        nsVal.setObject(jsobj);
        obj->setProperty("jsb", nsVal);
    }
    se::Object* ns = nsVal.toObject();

    js_register_boids_BoidsOptions(ns);
    js_register_boids_BoidsManager(ns);
    return true;
}

