System.register(["./application.js"], function (_export, _context) {
  "use strict";

  var createApplication;

  function findCanvas() {
    var frame = document.querySelector('#GameDiv');
    var container = document.querySelector('#Cocos3dGameContainer');
    var canvas = document.querySelector('#GameCanvas');
    canvas.setAttribute('tabindex', '99');
    return {
      frame: frame,
      container: container,
      canvas: canvas
    };
  }

  function addClass(element, name) {
    var hasClass = (' ' + element.className + ' ').indexOf(' ' + name + ' ') > -1;

    if (!hasClass) {
      if (element.className) {
        element.className += ' ';
      }

      element.className += name;
    }
  }

  return {
    setters: [function (_applicationJs) {
      createApplication = _applicationJs.createApplication;
    }],
    execute: function () {
      createApplication({}).then(function (application) {
        return application.start({
          findCanvas: findCanvas
        });
      })["catch"](function (err) {
        console.error(err);
      });
    }
  };
});