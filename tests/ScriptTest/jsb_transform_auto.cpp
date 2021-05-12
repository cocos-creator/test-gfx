#include "../gfx-test-case/tests/ScriptTest/jsb_transform_auto.h"
#include "cocos/bindings/manual/jsb_conversions.h"
#include "cocos/bindings/manual/jsb_global.h"
#include "Transform.h"

#ifndef JSB_ALLOC
#define JSB_ALLOC(kls, ...) new (std::nothrow) kls(__VA_ARGS__)
#endif

#ifndef JSB_FREE
#define JSB_FREE(ptr) delete ptr
#endif
se::Object* __jsb_cc_Transform_proto = nullptr;
se::Class* __jsb_cc_Transform_class = nullptr;

static bool js_transform_Transform_setParent(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Transform>(s);
    SE_PRECONDITION2(cobj, false, "js_transform_Transform_setParent : Invalid Native Object");
    const auto& args = s.args();
    size_t argc = args.size();
    CC_UNUSED bool ok = true;
    if (argc == 1) {
        HolderType<cc::Transform*, false> arg0 = {};
        ok &= sevalue_to_native(args[0], &arg0, s.thisObject());
        SE_PRECONDITION2(ok, false, "js_transform_Transform_setParent : Error processing arguments");
        cobj->setParent(arg0.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 1);
    return false;
}
SE_BIND_FUNC(js_transform_Transform_setParent)

static bool js_transform_Transform_setPosition(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Transform>(s);
    SE_PRECONDITION2(cobj, false, "js_transform_Transform_setPosition : Invalid Native Object");
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
        SE_PRECONDITION2(ok, false, "js_transform_Transform_setPosition : Error processing arguments");
        cobj->setPosition(arg0.value(), arg1.value(), arg2.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
    return false;
}
SE_BIND_FUNC(js_transform_Transform_setPosition)

static bool js_transform_Transform_setRotation(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Transform>(s);
    SE_PRECONDITION2(cobj, false, "js_transform_Transform_setRotation : Invalid Native Object");
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
        SE_PRECONDITION2(ok, false, "js_transform_Transform_setRotation : Error processing arguments");
        cobj->setRotation(arg0.value(), arg1.value(), arg2.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
    return false;
}
SE_BIND_FUNC(js_transform_Transform_setRotation)

static bool js_transform_Transform_setScale(se::State& s)
{
    auto* cobj = SE_THIS_OBJECT<cc::Transform>(s);
    SE_PRECONDITION2(cobj, false, "js_transform_Transform_setScale : Invalid Native Object");
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
        SE_PRECONDITION2(ok, false, "js_transform_Transform_setScale : Error processing arguments");
        cobj->setScale(arg0.value(), arg1.value(), arg2.value());
        return true;
    }
    SE_REPORT_ERROR("wrong number of arguments: %d, was expecting %d", (int)argc, 3);
    return false;
}
SE_BIND_FUNC(js_transform_Transform_setScale)

SE_DECLARE_FINALIZE_FUNC(js_cc_Transform_finalize)

static bool js_transform_Transform_constructor(se::State& s) // constructor.c
{
    cc::Transform* cobj = JSB_ALLOC(cc::Transform);
    s.thisObject()->setPrivateData(cobj);
    se::NonRefNativePtrCreatedByCtorMap::emplace(cobj);
    return true;
}
SE_BIND_CTOR(js_transform_Transform_constructor, __jsb_cc_Transform_class, js_cc_Transform_finalize)




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

bool js_register_transform_Transform(se::Object* obj)
{
    auto* cls = se::Class::create("Transform", obj, nullptr, _SE(js_transform_Transform_constructor));

    cls->defineFunction("setParent", _SE(js_transform_Transform_setParent));
    cls->defineFunction("setPosition", _SE(js_transform_Transform_setPosition));
    cls->defineFunction("setRotation", _SE(js_transform_Transform_setRotation));
    cls->defineFunction("setScale", _SE(js_transform_Transform_setScale));
    cls->defineFinalizeFunction(_SE(js_cc_Transform_finalize));
    cls->install();
    JSBClassType::registerClass<cc::Transform>(cls);

    __jsb_cc_Transform_proto = cls->getProto();
    __jsb_cc_Transform_class = cls;

    se::ScriptEngine::getInstance()->clearException();
    return true;
}
bool register_all_transform(se::Object* obj)
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

    js_register_transform_Transform(ns);
    return true;
}

