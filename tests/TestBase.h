#pragma once
#include "Core.h"
#include "cocos2d.h"

//#define USE_METAL
//#define USE_VULKAN
//#define USE_GLES2
NS_CC_BEGIN

typedef struct WindowInfo {
  intptr_t windowHandle;
  GFXRect screen;
  int physicalWidth;
  int physicalHeight;
} WindowInfo;

struct Framebuffer {
  GFXRenderPass *renderPass = nullptr;
  GFXTexture *colorTex = nullptr;
  GFXTexture *depthStencilTex = nullptr;
  GFXFramebuffer *framebuffer = nullptr;

  ~Framebuffer() {
    CC_SAFE_DESTROY(framebuffer);
    CC_SAFE_DESTROY(depthStencilTex);
    CC_SAFE_DESTROY(colorTex);
    CC_SAFE_DESTROY(renderPass);
  }
};

#define DEFINE_CREATE_METHOD(className)                                        \
  static TestBaseI *create(const WindowInfo &info) {                           \
    TestBaseI *test = CC_NEW(className(info));                                 \
    if (test->initialize())                                                    \
      return test;                                                             \
    else {                                                                     \
      CC_SAFE_DESTROY(test);                                                   \
      return nullptr;                                                          \
    }                                                                          \
  }

class TestBaseI : public Object {
public:
  TestBaseI(const WindowInfo &info);
  virtual void tick(float dt) = 0;

  virtual bool initialize() { return true; }
  virtual void destroy() {}
  virtual ~TestBaseI() = default;

  static GFXDevice *getDevice() { return _device; }
  static void destroyGlobal();

  static unsigned char *RGB2RGBA(Image *img);
  static void modifyProjectionBasedOnDevice(Mat4 &projection);
  static float getViewportTopBasedOnDevice(float top, float height);
  static uint getMipmapLevelCounts(uint width, uint height);

protected:
  static GFXDevice *_device;
   static GFXFramebuffer* _fbo;
  static std::vector<GFXCommandBuffer *> _commandBuffers;
    
   static GFXRenderPass *_renderPass;
};

NS_CC_END
