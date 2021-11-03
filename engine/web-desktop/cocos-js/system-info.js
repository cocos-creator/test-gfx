System.register('pal/system-info', ["engine/pal/system-info/enum-type/index"], function (_export, _context) {
    let OS, BrowserType, Platform;

    const systemInfo = {};
    
    _export('systemInfo', systemInfo);

    return {
        setters: [
            function (_global) {
                OS = _global.OS;
                BrowserType = _global.BrowserType;
                Platform = _global.Platform;
            }
        ],
        execute: function () {
            const nav = window.navigator;
            const ua = nav.userAgent.toLowerCase();
            // init os, osVersion and osMainVersion
            let isAndroid = false; let iOS = false; let osVersion = ''; let osMajorVersion = 0;
            let uaResult = /android\s*(\d+(?:\.\d+)*)/i.exec(ua) || /android\s*(\d+(?:\.\d+)*)/i.exec(nav.platform);
            if (uaResult) {
                isAndroid = true;
                osVersion = uaResult[1] || '';
                osMajorVersion = parseInt(osVersion) || 0;
            }
            uaResult = /(iPad|iPhone|iPod).*OS ((\d+_?){2,3})/i.exec(ua);
            if (uaResult) {
                iOS = true;
                osVersion = uaResult[2] || '';
                osMajorVersion = parseInt(osVersion) || 0;
                // refer to https://github.com/cocos-creator/engine/pull/5542 , thanks for contribition from @krapnikkk
                // ipad OS 13 safari identifies itself as "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15) AppleWebKit/605.1.15 (KHTML, like Gecko)"
                // so use maxTouchPoints to check whether it's desktop safari or not.
                // reference: https://stackoverflow.com/questions/58019463/how-to-detect-device-name-in-safari-on-ios-13-while-it-doesnt-show-the-correct
                // FIXME: should remove it when touch-enabled mac are available
                // TODO: due to compatibility issues, it is still determined to be ios, and a new operating system type ipados may be added later？
            } else if (/(iPhone|iPad|iPod)/.exec(nav.platform) || (nav.platform === 'MacIntel' && nav.maxTouchPoints && nav.maxTouchPoints > 1)) {
                iOS = true;
                osVersion = '';
                osMajorVersion = 0;
            }

            let osName = OS.UNKNOWN;
            if (nav.appVersion.indexOf('Win') !== -1) {
                osName = OS.WINDOWS;
            } else if (iOS) {
                osName = OS.IOS;
            } else if (nav.appVersion.indexOf('Mac') !== -1) {
                osName = OS.OSX;
            } else if (nav.appVersion.indexOf('X11') !== -1 && nav.appVersion.indexOf('Linux') === -1) {
                osName = OS.LINUX;
            } else if (isAndroid) {
                osName = OS.ANDROID;
            } else if (nav.appVersion.indexOf('Linux') !== -1 || ua.indexOf('ubuntu') !== -1) {
                osName = OS.LINUX;
            }

            systemInfo.os = osName;
            systemInfo.osVersion = osVersion;
            systemInfo.osMainVersion = osMajorVersion;

            // @ts-expect-error getBattery is not totally supported
            nav.getBattery?.().then((battery) => {
                systemInfo._battery = battery;
            });

            systemInfo.isNative = false;
            systemInfo.isBrowser = true;

            systemInfo.isMobile = /mobile|android|iphone|ipad/.test(ua);
            systemInfo.platform = systemInfo.isMobile ? Platform.MOBILE_BROWSER : Platform.DESKTOP_BROWSER;


            // TODO: use dack-type to determine the browserType
            // init browserType and browserVersion
            systemInfo.browserType = BrowserType.UNKNOWN;
            const typeReg0 = /wechat|weixin|micromessenger/i;
            const typeReg1 = /mqqbrowser|micromessenger|qqbrowser|sogou|qzone|liebao|maxthon|ucbs|360 aphone|360browser|baiduboxapp|baidubrowser|maxthon|mxbrowser|miuibrowser/i;
            const typeReg2 = /qq|qqbrowser|ucbrowser|ubrowser|edge|HuaweiBrowser/i;
            const typeReg3 = /chrome|safari|firefox|trident|opera|opr\/|oupeng/i;
            const browserTypes = typeReg0.exec(ua) || typeReg1.exec(ua) || typeReg2.exec(ua) || typeReg3.exec(ua);

            let browserType = browserTypes ? browserTypes[0].toLowerCase() : OS.UNKNOWN;
            if (browserType === 'safari' && isAndroid) {
                browserType = BrowserType.ANDROID;
            } else if (browserType === 'qq' && /android.*applewebkit/i.test(ua)) {
                browserType = BrowserType.ANDROID;
            }
            const typeMap = {
                micromessenger: BrowserType.WECHAT,
                wechat: BrowserType.WECHAT,
                weixin: BrowserType.WECHAT,
                trident: BrowserType.IE,
                edge: BrowserType.EDGE,
                '360 aphone': BrowserType.BROWSER_360,
                mxbrowser: BrowserType.MAXTHON,
                'opr/': BrowserType.OPERA,
                ubrowser: BrowserType.UC,
                huaweibrowser: BrowserType.HUAWEI,
            };

            systemInfo.browserType = typeMap[browserType] || browserType;

            // init browserVersion
            systemInfo.browserVersion = '';
            const versionReg1 = /(mqqbrowser|micromessenger|qqbrowser|sogou|qzone|liebao|maxthon|uc|ucbs|360 aphone|360|baiduboxapp|baidu|maxthon|mxbrowser|miui(?:.hybrid)?)(mobile)?(browser)?\/?([\d.]+)/i;
            const versionReg2 = /(qq|chrome|safari|firefox|trident|opera|opr\/|oupeng)(mobile)?(browser)?\/?([\d.]+)/i;
            let tmp = versionReg1.exec(ua);
            if (!tmp) {
                tmp = versionReg2.exec(ua);
            }
            systemInfo.browserVersion = tmp ? tmp[4] : '';
        }
    };
});