System.register('internal:constants', [], function (_export, _context) {
    const _global = typeof window === 'undefined' ? global : window;

    function defined (name) {
        return typeof _global[name] === 'object';
    }

    function tryDefineGlobal (name, value) {
        if (typeof _global[name] === 'undefined') {
            return (_global[name] = value);
        } else {
            return _global[name];
        }
    }

    // No export to global required since we have already done here.
    const EXPORT_TO_GLOBAL = false;
    const BUILD = tryDefineGlobal('CC_BUILD', false);
    const TEST = tryDefineGlobal('CC_TEST', defined('tap') || defined('QUnit'));
    const EDITOR = tryDefineGlobal('CC_EDITOR', defined('Editor') && defined('process') && ('electron' in process.versions));
    const PREVIEW = tryDefineGlobal('CC_PREVIEW', !EDITOR);
    const DEV = tryDefineGlobal('CC_DEV', true); // (CC_EDITOR && !CC_BUILD) || CC_PREVIEW || CC_TEST
    const DEBUG = tryDefineGlobal('CC_DEBUG', true); // CC_DEV || Debug Build
    const JSB = tryDefineGlobal('CC_JSB', defined('jsb'));
    const NATIVE = JSB;
    const HTML5 = false;
    // @ts-expect-error
    const WECHAT = tryDefineGlobal('CC_WECHAT', !!(defined('wx') && (wx.getSystemInfoSync || wx.getSharedCanvas)));
    const MINIGAME = tryDefineGlobal('CC_MINIGAME', false);
    const RUNTIME_BASED = tryDefineGlobal('CC_RUNTIME_BASED', false);
    const ALIPAY = tryDefineGlobal('CC_ALIPAY', false);
    const XIAOMI = tryDefineGlobal('CC_XIAOMI', false);
    const BYTEDANCE = tryDefineGlobal('CC_BYTEDANCE', false);
    const BAIDU = tryDefineGlobal('CC_BAIDU', false);
    const COCOSPLAY = tryDefineGlobal('CC_COCOSPLAY', false);
    const HUAWEI = tryDefineGlobal('CC_HUAWEI', false);
    const OPPO = tryDefineGlobal('CC_OPPO', false);
    const VIVO = tryDefineGlobal('CC_VIVO', false);
    // @ts-expect-error
    const SUPPORT_JIT = tryDefineGlobal('CC_SUPPORT_JIT', ('function' === typeof loadRuntime));
    const SERVER_MODE = false;
    _export('EXPORT_TO_GLOBAL', EXPORT_TO_GLOBAL);
    _export('BUILD', BUILD);
    _export('TEST', TEST);
    _export('EDITOR', EDITOR);
    _export('PREVIEW', PREVIEW);
    _export('DEV', DEV);
    _export('DEBUG', DEBUG);
    _export('JSB', JSB);
    _export('NATIVE', NATIVE);
    _export('HTML5', HTML5);
    _export('WECHAT', WECHAT);
    _export('MINIGAME', MINIGAME);
    _export('RUNTIME_BASED', RUNTIME_BASED);
    _export('ALIPAY', ALIPAY);
    _export('XIAOMI', XIAOMI);
    _export('BYTEDANCE', BYTEDANCE);
    _export('BAIDU', BAIDU);
    _export('COCOSPLAY', COCOSPLAY);
    _export('HUAWEI', HUAWEI);
    _export('OPPO', OPPO);
    _export('VIVO', VIVO);
    _export('SUPPORT_JIT', SUPPORT_JIT);
    _export('SERVER_MODE', SERVER_MODE);
    return {
        setters: [],
        execute: function () {}
    };
});