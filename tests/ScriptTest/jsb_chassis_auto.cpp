#include "../gfx-test-case/tests/ScriptTest/jsb_chassis_auto.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "cocos/bindings/manual/jsb_global.h"
#include "Chassis.h"

#ifndef JSB_ALLOC
#define JSB_ALLOC(kls, ...) new (std::nothrow) kls(__VA_ARGS__)
#endif

#ifndef JSB_FREE
#define JSB_FREE(ptr) delete ptr
#endif
se::Object* __jsb_cc_Transform_proto = nullptr;
se::Class* __jsb_cc_Transform_class = nullptr;

static bool js_chassis_Transform_setParent(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Transform>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Transform_setParent : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        HolderType<cc::Transform*, false> arg0 = {};
        ok &= sevalue_to_native(args[0], &arg0, s.thisObject());
        SE_PRECONDITION2(ok, false, "js_chassis_Transform_setParent : Error processing arguments");
        cobj->setParent(arg0.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_chassis_Transform_setParent)

static bool js_chassis_Transform_setPosition(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Transform>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Transform_setPosition : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 3) {
        HolderType<float, false> arg0 = {};
        HolderType<float, false> arg1 = {};
        HolderType<float, false> arg2 = {};
        ok &= sevalue_to_native(args[0], &arg0, s.thisObject());
        ok &= sevalue_to_native(args[1], &arg1, s.thisObject());
        ok &= sevalue_to_native(args[2], &arg2, s.thisObject());
        SE_PRECONDITION2(ok, false, "js_chassis_Transform_setPosition : Error processing arguments");
        cobj->setPosition(arg0.value(), arg1.value(), arg2.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
    return false;
}
SE_BIND_FUNC(js_chassis_Transform_setPosition)

static bool js_chassis_Transform_setRotation(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Transform>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Transform_setRotation : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        HolderType<const float*, false> arg0 = {};
        ok &= sevalue_to_native(args[0], &arg0, s.thisObject());
        SE_PRECONDITION2(ok, false, "js_chassis_Transform_setRotation : Error processing arguments");
        cobj->setRotation(arg0.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_chassis_Transform_setRotation)

static bool js_chassis_Transform_setRotationFromEuler(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Transform>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Transform_setRotationFromEuler : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 3) {
        HolderType<float, false> arg0 = {};
        HolderType<float, false> arg1 = {};
        HolderType<float, false> arg2 = {};
        ok &= sevalue_to_native(args[0], &arg0, s.thisObject());
        ok &= sevalue_to_native(args[1], &arg1, s.thisObject());
        ok &= sevalue_to_native(args[2], &arg2, s.thisObject());
        SE_PRECONDITION2(ok, false, "js_chassis_Transform_setRotationFromEuler : Error processing arguments");
        cobj->setRotationFromEuler(arg0.value(), arg1.value(), arg2.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
    return false;
}
SE_BIND_FUNC(js_chassis_Transform_setRotationFromEuler)

static bool js_chassis_Transform_setScale(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Transform>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Transform_setScale : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 3) {
        HolderType<float, false> arg0 = {};
        HolderType<float, false> arg1 = {};
        HolderType<float, false> arg2 = {};
        ok &= sevalue_to_native(args[0], &arg0, s.thisObject());
        ok &= sevalue_to_native(args[1], &arg1, s.thisObject());
        ok &= sevalue_to_native(args[2], &arg2, s.thisObject());
        SE_PRECONDITION2(ok, false, "js_chassis_Transform_setScale : Error processing arguments");
        cobj->setScale(arg0.value(), arg1.value(), arg2.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
    return false;
}
SE_BIND_FUNC(js_chassis_Transform_setScale)

SE_DECLARE_FINALIZE_FUNC(js_cc_Transform_finalize)

static bool js_chassis_Transform_constructor(se::State& s) // constructor.c
{
    cc::Transform* cobj = JSB_ALLOC(cc::Transform);
    s.thisObject()->setPrivateData(cobj);
    se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
    return true;
}
SE_BIND_CTOR(js_chassis_Transform_constructor, __jsb_cc_Transform_class, js_cc_Transform_finalize)




static bool js_cc_Transform_finalize(se::State& s)
{
    auto iter = se::NonRefNativePtrCreatedByCtorMap::find(SE_THIS_OBJECT<cc::Transform>(s));
    if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
    {
        se::NonRefNativePtrCreatedByCtorMap::erase(iter);
        auto* cobj = SE_THIS_OBJECT<cc::Transform>(s);
        JSB_FREE(cobj);
    }
    return true;
}
SE_BIND_FINALIZE_FUNC(js_cc_Transform_finalize)

bool js_register_chassis_Transform(se::Object* obj)
{
    auto* cls = se::Class::create("Transform", obj, nullptr, _SE(js_chassis_Transform_constructor));

    cls->defineFunction("setParent", _SE(js_chassis_Transform_setParent));
    cls->defineFunction("setPosition", _SE(js_chassis_Transform_setPosition));
    cls->defineFunction("setRotation", _SE(js_chassis_Transform_setRotation));
    cls->defineFunction("setRotationFromEuler", _SE(js_chassis_Transform_setRotationFromEuler));
    cls->defineFunction("setScale", _SE(js_chassis_Transform_setScale));
    cls->defineFinalizeFunction(_SE(js_cc_Transform_finalize));
    cls->install();
    JSBClassType::registerClass<cc::Transform>(cls);

    __jsb_cc_Transform_proto = cls->getProto();
    __jsb_cc_Transform_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}
se::Object* __jsb_cc_Model_proto = nullptr;
se::Class* __jsb_cc_Model_class = nullptr;

static bool js_chassis_Model_setColor(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Model>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Model_setColor : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 4) {
        HolderType<float, false> arg0 = {};
        HolderType<float, false> arg1 = {};
        HolderType<float, false> arg2 = {};
        HolderType<float, false> arg3 = {};
        ok &= sevalue_to_native(args[0], &arg0, s.thisObject());
        ok &= sevalue_to_native(args[1], &arg1, s.thisObject());
        ok &= sevalue_to_native(args[2], &arg2, s.thisObject());
        ok &= sevalue_to_native(args[3], &arg3, s.thisObject());
        SE_PRECONDITION2(ok, false, "js_chassis_Model_setColor : Error processing arguments");
        cobj->setColor(arg0.value(), arg1.value(), arg2.value(), arg3.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 4);
    return false;
}
SE_BIND_FUNC(js_chassis_Model_setColor)

static bool js_chassis_Model_setEnabled(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Model>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Model_setEnabled : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        HolderType<bool, false> arg0 = {};
        ok &= sevalue_to_native(args[0], &arg0, s.thisObject());
        SE_PRECONDITION2(ok, false, "js_chassis_Model_setEnabled : Error processing arguments");
        cobj->setEnabled(arg0.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_chassis_Model_setEnabled)

static bool js_chassis_Model_setTransform(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Model>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Model_setTransform : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        HolderType<const cc::Transform*, false> arg0 = {};
        ok &= sevalue_to_native(args[0], &arg0, s.thisObject());
        SE_PRECONDITION2(ok, false, "js_chassis_Model_setTransform : Error processing arguments");
        cobj->setTransform(arg0.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_chassis_Model_setTransform)

SE_DECLARE_FINALIZE_FUNC(js_cc_Model_finalize)

static bool js_chassis_Model_constructor(se::State& s)  // constructor_overloaded.c
{
    CC_UNUSED bool ok = true;
    const auto& args = s.args();
    size_t argc = args.size();
    do {
        if (argc == 1) {
            HolderType<unsigned int, false> arg0 = {};
            ok &= sevalue_to_native(args[0], &arg0, s.thisObject());
            if (!ok) { ok = true; break; }
            cc::Model* cobj = JSB_ALLOC(cc::Model, arg0.value());
            s.thisObject()->setPrivateData(cobj);
            se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
            return true;
        }
    } while(false);
    do {
        if (argc == 0) {
            cc::Model* cobj = JSB_ALLOC(cc::Model);
            s.thisObject()->setPrivateData(cobj);
            se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
            return true;
        }
    } while(false);
    SE_REPORT_ERROR("wrong number of arguments: %d", (int)argc);
    return false;
}
SE_BIND_CTOR(js_chassis_Model_constructor, __jsb_cc_Model_class, js_cc_Model_finalize)




static bool js_cc_Model_finalize(se::State& s)
{
    auto iter = se::NonRefNativePtrCreatedByCtorMap::find(SE_THIS_OBJECT<cc::Model>(s));
    if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
    {
        se::NonRefNativePtrCreatedByCtorMap::erase(iter);
        auto* cobj = SE_THIS_OBJECT<cc::Model>(s);
        JSB_FREE(cobj);
    }
    return true;
}
SE_BIND_FINALIZE_FUNC(js_cc_Model_finalize)

bool js_register_chassis_Model(se::Object* obj)
{
    auto* cls = se::Class::create("Model", obj, nullptr, _SE(js_chassis_Model_constructor));

    cls->defineFunction("setColor", _SE(js_chassis_Model_setColor));
    cls->defineFunction("setEnabled", _SE(js_chassis_Model_setEnabled));
    cls->defineFunction("setTransform", _SE(js_chassis_Model_setTransform));
    cls->defineFinalizeFunction(_SE(js_cc_Model_finalize));
    cls->install();
    JSBClassType::registerClass<cc::Model>(cls);

    __jsb_cc_Model_proto = cls->getProto();
    __jsb_cc_Model_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}
se::Object* __jsb_cc_Root_proto = nullptr;
se::Class* __jsb_cc_Root_class = nullptr;

static bool js_chassis_Root_createModel(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Root>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Root_createModel : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        cc::Model* result = cobj->createModel();
        ok &= nativevalue_to_se(result, s.rval(), nullptr /*ctx*/);
        SE_PRECONDITION2(ok, false, "js_chassis_Root_createModel : Error processing arguments");
        se::NonRefNativePtrCreatedByCtorMap::emplace(result);
        SE_HOLD_RETURN_VALUE(result, s.thisObject(), s.rval());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_chassis_Root_createModel)

static bool js_chassis_Root_createTransform(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Root>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Root_createTransform : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        cc::Transform* result = cobj->createTransform();
        ok &= nativevalue_to_se(result, s.rval(), nullptr /*ctx*/);
        SE_PRECONDITION2(ok, false, "js_chassis_Root_createTransform : Error processing arguments");
        se::NonRefNativePtrCreatedByCtorMap::emplace(result);
        SE_HOLD_RETURN_VALUE(result, s.thisObject(), s.rval());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_chassis_Root_createTransform)

static bool js_chassis_Root_destroy(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Root>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Root_destroy : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        cobj->destroy();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_chassis_Root_destroy)

static bool js_chassis_Root_initialize(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Root>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Root_initialize : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        cobj->initialize();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_chassis_Root_initialize)

static bool js_chassis_Root_render(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Root>(s);
    SE_PRECONDITION2(cobj, false, "js_chassis_Root_render : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        cobj->render();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_chassis_Root_render)

static bool js_chassis_Root_getInstance(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        cc::Root* result = cc::Root::getInstance();
        ok &= nativevalue_to_se(result, s.rval(), nullptr /*ctx*/);
        SE_PRECONDITION2(ok, false, "js_chassis_Root_getInstance : Error processing arguments");
        SE_HOLD_RETURN_VALUE(result, s.thisObject(), s.rval());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_chassis_Root_getInstance)



static bool js_cc_Root_finalize(se::State& s)
{
    auto iter = se::NonRefNativePtrCreatedByCtorMap::find(SE_THIS_OBJECT<cc::Root>(s));
    if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
    {
        se::NonRefNativePtrCreatedByCtorMap::erase(iter);
        auto* cobj = SE_THIS_OBJECT<cc::Root>(s);
        JSB_FREE(cobj);
    }
    return true;
}
SE_BIND_FINALIZE_FUNC(js_cc_Root_finalize)

bool js_register_chassis_Root(se::Object* obj)
{
    auto* cls = se::Class::create("Root", obj, nullptr, nullptr);

    cls->defineFunction("createModel", _SE(js_chassis_Root_createModel));
    cls->defineFunction("createTransform", _SE(js_chassis_Root_createTransform));
    cls->defineFunction("destroy", _SE(js_chassis_Root_destroy));
    cls->defineFunction("initialize", _SE(js_chassis_Root_initialize));
    cls->defineFunction("render", _SE(js_chassis_Root_render));
    cls->defineStaticFunction("getInstance", _SE(js_chassis_Root_getInstance));
    cls->defineFinalizeFunction(_SE(js_cc_Root_finalize));
    cls->install();
    JSBClassType::registerClass<cc::Root>(cls);

    __jsb_cc_Root_proto = cls->getProto();
    __jsb_cc_Root_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}
se::Object* __jsb_cc_RootManager_proto = nullptr;
se::Class* __jsb_cc_RootManager_class = nullptr;

static bool js_chassis_RootManager_destroy(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    if (argc == 0) {
        cc::RootManager::destroy();
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_chassis_RootManager_destroy)

static bool js_chassis_RootManager_create(se::State& s)
{
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 0) {
        cc::Root* result = cc::RootManager::create();
        ok &= nativevalue_to_se(result, s.rval(), nullptr /*ctx*/);
        SE_PRECONDITION2(ok, false, "js_chassis_RootManager_create : Error processing arguments");
        SE_HOLD_RETURN_VALUE(result, s.thisObject(), s.rval());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 0);
    return false;
}
SE_BIND_FUNC(js_chassis_RootManager_create)



static bool js_cc_RootManager_finalize(se::State& s)
{
    auto iter = se::NonRefNativePtrCreatedByCtorMap::find(SE_THIS_OBJECT<cc::RootManager>(s));
    if (iter != se::NonRefNativePtrCreatedByCtorMap::end())
    {
        se::NonRefNativePtrCreatedByCtorMap::erase(iter);
        auto* cobj = SE_THIS_OBJECT<cc::RootManager>(s);
        JSB_FREE(cobj);
    }
    return true;
}
SE_BIND_FINALIZE_FUNC(js_cc_RootManager_finalize)

bool js_register_chassis_RootManager(se::Object* obj)
{
    auto* cls = se::Class::create("RootManager", obj, nullptr, nullptr);

    cls->defineStaticFunction("destroy", _SE(js_chassis_RootManager_destroy));
    cls->defineStaticFunction("create", _SE(js_chassis_RootManager_create));
    cls->defineFinalizeFunction(_SE(js_cc_RootManager_finalize));
    cls->install();
    JSBClassType::registerClass<cc::RootManager>(cls);

    __jsb_cc_RootManager_proto = cls->getProto();
    __jsb_cc_RootManager_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}
bool register_all_chassis(se::Object* obj)
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

    js_register_chassis_Model(ns);
    js_register_chassis_Root(ns);
    js_register_chassis_Transform(ns);
    js_register_chassis_RootManager(ns);
    return true;
}

