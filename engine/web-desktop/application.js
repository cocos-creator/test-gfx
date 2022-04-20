System.register([], function (_export, _context) {
  "use strict";

  function _defineProperty(obj, key, value) { if (key in obj) { Object.defineProperty(obj, key, { value: value, enumerable: true, configurable: true, writable: true }); } else { obj[key] = value; } return obj; }

  function createApplication(_ref) {
    // NOTE: before here we shall not import any module!
    var promise = Promise.resolve();
    return promise.then(function () {
      return _defineProperty({
        start: start
      }, 'import', topLevelImport);
    });

    function start(_ref4) {
      var findCanvas = _ref4.findCanvas;
      var settings;
      var gfx;
      return Promise.resolve().then(function () {
        return topLevelImport('test-gfx/engine/index');
      }).then(function (cc_gfx) {
        gfx = cc_gfx;
        return loadSettingsJson();
      }).then(function () {
        return topLevelImport('pal/system-info');
      }).then(function () {
        settings = window._CCSettings;
        if (settings.scriptPackages) {
          return loadModulePacks(settings.scriptPackages);
        }
      }).then(function () {
        onGameStarted(gfx, settings, findCanvas);
      });
    }

    function topLevelImport(url) {
      return _context["import"]("".concat(url));
    }

    function loadModulePacks(packs) {
      return Promise.all(packs.map(function (pack) {
        return topLevelImport(pack);
      }));
    }

    function loadSettingsJson() {
      var settings = 'src/settings.json';
      return new Promise(function (resolve, reject) {
        var requestSettings = function requestSettings() {
          var xhr = new XMLHttpRequest();
          xhr.open('GET', settings);
          xhr.responseType = 'text';

          xhr.onload = function () {
            window._CCSettings = JSON.parse(xhr.response);
            resolve();
          };

          xhr.onerror = function () {
            if (retryCount-- > 0) {
              setTimeout(requestSettings, retryInterval);
            } else {
              reject(new Error('request settings failed!'));
            }
          };

          xhr.send(null);
        };

        var retryCount = 3;
        var retryInterval = 2000;
        requestSettings();
      });
    }
  }

  function getLocation(touch, canvas) {
    const box = canvas?.getBoundingClientRect();
    let x = touch.clientX - box.x;
    let y = box.y + box.height - touch.clientY;
    const dpr = window.devicePixelRatio;
    x *= dpr;
    y *= dpr;
    return { x, y };
  }

  function onGameStarted(gfx, settings, findCanvas) {
    const gameOptions = getGameOptions(gfx, settings, findCanvas);
    window._CCSettings = undefined;

    // main entry
    const app = new gfx.Application();
    app.initialize({
      api: 7, // webgl - 6, webgl2 - 7
      canvas: gameOptions.adapter.canvas,
      width: gameOptions.adapter.canvas.width,
      height: gameOptions.adapter.canvas.height,
    });
    gameOptions.adapter.canvas.ontouchend = (event) => {
      const pos = getLocation(event, gameOptions.adapter.canvas);
      app.nextTest(pos.x < gameOptions.adapter.canvas.width / 2);
    };
    gameOptions.adapter.canvas.onmouseup = (event) => {
      const pos = getLocation(event, gameOptions.adapter.canvas);
      app.nextTest(pos.x < gameOptions.adapter.canvas.width / 2);
    };
    const tick = () => {
      if (!app.tick()) return;
      requestAnimationFrame(tick);
    };
    requestAnimationFrame(tick);
  }

  function getGameOptions(cc, settings, findCanvas) {
    var options = {
      showFPS: !false && settings.debug,
      frameRate: 60,
      adapter: findCanvas('GameCanvas'),
      customJointTextureLayouts: settings.customJointTextureLayouts || [],
    };
    return options;
  }

  _export("createApplication", createApplication);

  return {
    setters: [],
    execute: function () { }
  };
});
