#include "MyApp.h"
#include "gfx-gles2/GFXGLES2.h"
#include "gfx-gles3/GFXGLES3.h"

NS_CC_BEGIN

MyApp::MyApp() {
}

MyApp::~MyApp() {
}

bool MyApp::Initialize() {

  // Init device
  RECT rect;
  GetClientRect(window_handle_, &rect);

  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);

#if 0
  device_ = CC_NEW(GLES3Device);
#else
  device_ = CC_NEW(GLES2Device);
#endif

  GFXDeviceInfo dev_info;
  dev_info.window_handle = (intptr_t)window_handle_;
  dev_info.width = rect.right - rect.left;
  dev_info.height = rect.bottom - rect.top;
  dev_info.native_width = screenWidth;
  dev_info.native_height = screenHeight;
  if (!device_->Initialize(dev_info)) {
    return false;
  }

  // Init primary command buffer
  GFXCommandBufferInfo cmd_buff_info;
  cmd_buff_info.allocator = device_->cmd_allocator();
  cmd_buff_info.type = GFXCommandBufferType::PRIMARY;
  cmd_buff_ = device_->CreateGFXCommandBuffer(cmd_buff_info);
  /*
  GFXShaderStageList shaderStageList;
  GFXShaderStage vertexShaderStage;
  vertexShaderStage.type = GFXShaderType::VERTEX;
  vertexShaderStage.source = R"(
        #ifdef GL_ES
        precision highp float;
        #endif
        attribute vec2 a_position;
        void main()
        {
            gl_Position = vec4(a_position, 0, 1);
        }
    )";
  shaderStageList.emplace_back(vertexShaderStage);

  GFXShaderStage fragmentShaderStage;
  fragmentShaderStage.type = GFXShaderType::FRAGMENT;
  fragmentShaderStage.source = R"(
        #ifdef GL_ES
        precision highp float;
        #endif
        uniform vec4 color;
        void main()
        {
            gl_FragColor = color;
        }
    )";
  shaderStageList.emplace_back(fragmentShaderStage);

  GFXShaderInfo shaderInfo;
  shaderInfo.name = "Render Triangle";
  shaderInfo.stages = shaderStageList;
  _shader = device_->CreateGFXShader(shaderInfo);

  GFXInputAssemblerInfo inputAssemblerInfo;
  GFXAttributeList attributeList;
  GFXAttribute position = {"a_position", GFXFormat::RG32F, false, 0, false};

  float vertexData[] = {
      -1.0f, 0.0f,
      0.0f, -1.0f,
      1.0f, 1.0f
  };



  GFXBufferInfo bufferInfo = {
        GFXBufferUsage::VERTEX, 
        GFXMemoryUsage::HOST,
        2*sizeof(float), 
        sizeof(vertexData), 
        GFXBufferFlagBit::NONE };

  _vertexBuffer = device_->CreateGFXBuffer(bufferInfo);
  _vertexBuffer->Update(vertexData, 0, sizeof(vertexData));

  inputAssemblerInfo.vertex_buffers.emplace_back(_vertexBuffer);
  _inputAssembler = device_->CreateGFXInputAssembler(inputAssemblerInfo);
*/
  fbo_ = device_->window()->framebuffer();

  //GFXPipelineLayoutInfo pipelineLayoutInfo;
  ////_pipelineLayout = device_->CreateGFXPipelieLayout(pipelineLayoutInfo);

  //GFXPipelineStateInfo pipelineInfo;
  //pipelineInfo.primitive = GFXPrimitiveMode::TRIANGLE_LIST;
  //pipelineInfo.shader = _shader;
  //pipelineInfo.layout = _pipelineLayout;
  //pipelineInfo.render_pass = device_->window()->render_pass();

  ////_pipeline = device_->CreateGFXPipelineState(pipelineInfo);

  return true;
}

void MyApp::Destroy() {
  CC_SAFE_DESTROY(cmd_buff_);
  CC_SAFE_DESTROY(device_);
  CC_SAFE_DESTROY(_vertexBuffer);
  CC_SAFE_DESTROY(_shader);
  CC_SAFE_DESTROY(_inputAssembler);
  CC_SAFE_DESTROY(_layout);
  CC_SAFE_DESTROY(_pipelineLayout);
  CC_SAFE_DESTROY(_pipeline);
}

void MyApp::FrameMove(float deltaTime) {

}

void MyApp::Render() {

    GFXRect render_area = {0, 0, device_->width(), device_->height() };

    GFXColor clear_color;
    clear_color.r = 0.3f;
    clear_color.g = 0.6f;
    clear_color.b = 0.9f;
    clear_color.a = 1.0f;

    cmd_buff_->Begin();
    //cmd_buff_->BindInputAssembler(_inputAssembler);
    //cmd_buff_->BindPipelineState(_pipeline);
    cmd_buff_->BeginRenderPass(fbo_, render_area, GFXClearFlagBit::ALL, &clear_color, 1, 1.0f, 0);
    //cmd_buff_->Draw(_inputAssembler);
    cmd_buff_->EndRenderPass();
    cmd_buff_->End();

    device_->queue()->submit(&cmd_buff_, 1);
    device_->Present();
}

NS_CC_END
