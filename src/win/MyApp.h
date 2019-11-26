#ifndef __MYAPP_H__
#define __MYAPP_H__

#include "GameApp.h"

NS_CC_BEGIN

class MyApp: public GameApp
{
 public:
	MyApp();
	~MyApp();

 public:
  bool Initialize();
  void Destroy();
  void FrameMove(float deltaTime);
  void Render();

 private:
  GFXCommandBuffer* cmd_buff_;
  GFXShader* _shader = nullptr;
  GFXInputAssembler* _inputAssembler = nullptr;
  GFXBindingLayout* _layout = nullptr;
  GFXFramebuffer* fbo_;
  GFXBuffer* _vertexBuffer = nullptr;
  GFXPipelineState* _pipeline = nullptr;
  GFXPipelineLayout* _pipelineLayout = nullptr;
};

NS_CC_END

#endif