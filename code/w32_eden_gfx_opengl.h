#ifndef __W32_EDEN_GFX_OPENGL_H__
#define __W32_EDEN_GFX_OPENGL_H__

#include "w32_eden_gfx.h"

#pragma comment(lib, "opengl32.lib")

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_PIXEL_TYPE_ARB                      0x2013
#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_FULL_ACCELERATION_ARB               0x2027
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB        0x20A9
#define WGL_CONTEXT_FLAG_ARB                    0x2094
#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002
#define WGL_CONTEXT_MOMO_PROFILE_BIT_ARB        0x00000001

typedef BOOL WINAPI wglChoosePixelFormatARBFn(HDC hdc, const int* piAttribIList, const FLOAT* pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);

typedef BOOL WINAPI wglSwapIntervalEXTFn(int interval);

typedef HGLRC WINAPI wglCreateContextAttribsARBFn(HDC hdc, HGLRC hShareContext, const int* attribList);

typedef const char* WINAPI wglGetExtensionsStringEXTFn(void);

static wglChoosePixelFormatARBFn* wglChoosePixelFormatARB;
static wglSwapIntervalEXTFn* wglSwapIntervalEXT;
static wglCreateContextAttribsARBFn* wglCreateContextAttribsARB;
//static wglGetExtensionsStringEXTFn* wglGetExtensionsStringEXT;


static void* 
_w32_try_get_wgl_function(const char* name, HMODULE fallback_module)
{
  void* p = (void*)wglGetProcAddress(name);
  if ((p == 0) || 
      (p == (void*)0x1) || 
      (p == (void*)0x2) || 
      (p == (void*)0x3) || 
      (p == (void*)-1))
  {
    p = (void*)GetProcAddress(fallback_module, name);
  }
  return p;
  
}

static void
_w32_set_pixel_format(HDC dc) {
  s32_t suggested_pixel_format_index = 0;
  u32_t extended_pick = 0;
  
  if (wglChoosePixelFormatARB) {
    s32_t attrib_list[] = {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
      WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
      WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,
      0,
    };
    
    wglChoosePixelFormatARB(dc, attrib_list, 0, 1,
                            &suggested_pixel_format_index, &extended_pick);
    
  }
  
  if (!extended_pick) {
    PIXELFORMATDESCRIPTOR desired_pixel_format = {};
    desired_pixel_format.nSize = sizeof(desired_pixel_format);
    desired_pixel_format.nVersion = 1;
    desired_pixel_format.iPixelType = PFD_TYPE_RGBA;
    desired_pixel_format.dwFlags = 
      PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER; 
    desired_pixel_format.cColorBits = 32;
    desired_pixel_format.cAlphaBits = 8;
    desired_pixel_format.iLayerType = PFD_MAIN_PLANE;
    
    // Here, we ask windows to find the best supported pixel 
    // format based on our desired format.
    suggested_pixel_format_index = 
      ChoosePixelFormat(dc, &desired_pixel_format);
  }
  PIXELFORMATDESCRIPTOR suggested_pixel_format = {};
  
  DescribePixelFormat(dc, suggested_pixel_format_index, 
                      sizeof(suggested_pixel_format), 
                      &suggested_pixel_format);
  SetPixelFormat(dc, 
                 suggested_pixel_format_index, 
                 &suggested_pixel_format);
}
static b32_t
_w32_load_wgl_extentions() {
  WNDCLASSA window_class = {};
  // Er yeah...we have to create a 'fake' opengl_t context 
  // to load the extensions lol.
  window_class.lpfnWndProc = DefWindowProcA;
  window_class.hInstance = GetModuleHandle(0);
  window_class.lpszClassName = "WGLLoader2";
  
  if (RegisterClassA(&window_class)) {
    HWND window = CreateWindowExA( 
                                  0,
                                  window_class.lpszClassName,
                                  "wgl Loader2",
                                  0,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  0,
                                  0,
                                  window_class.hInstance,
                                  0);
    
    HDC dc = GetDC(window);
    _w32_set_pixel_format(dc);
    HGLRC opengl_context = wglCreateContext(dc);
    
    b32_t success = true;
    
    if (wglMakeCurrent(dc, opengl_context)) {
      wglChoosePixelFormatARB = (wglChoosePixelFormatARBFn*)wglGetProcAddress("wglChoosePixelFormatARB");
      wglCreateContextAttribsARB = (wglCreateContextAttribsARBFn*)wglGetProcAddress("wglCreateContextAttribsARB");
      
      if (!wglChoosePixelFormatARB || !wglCreateContextAttribsARB) {
        success = false;
      }
      
      DestroyWindow(window);
      ReleaseDC(window, dc);
      wglDeleteContext(opengl_context);
      wglMakeCurrent(0, 0);
      return success;
    }
    else {
      return false;
    }
    
  }
  else {
    //DWORD test = GetLastError();
    return false;
  }
}



static 
w32_gfx_load_sig(w32_gfx_load)
{
  HDC dc = GetDC(window); 
  if (!dc) return 0;
  if (!_w32_load_wgl_extentions()) return 0;

  _w32_set_pixel_format(dc);
  
  s32_t opengl_attribs[] {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
    WGL_CONTEXT_MINOR_VERSION_ARB, 5,
    WGL_CONTEXT_FLAG_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#if 0
    | WGL_CONTEXT_DEBUG_BIT_ARB
#endif
    ,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_MOMO_PROFILE_BIT_ARB,
    0,
  };
  HGLRC opengl_ctx = wglCreateContextAttribsARB(dc, 0, opengl_attribs); 
  
  if (!opengl_ctx) {
    return false;
  }

  auto* opengl = arena_alloc_bootstrap(eden_opengl_t, arena, gigabytes(1));
  eden->gfx.platform_data = opengl;

  if (!opengl) {
    return false;
  }
  
  
  if(wglMakeCurrent(dc, opengl_ctx)) {
    HMODULE module = LoadLibraryA("opengl32.dll");
#define wgl_set_opengl_function(name) \
opengl->name = (eden_opengl_##name*)_w32_try_get_wgl_function(#name, module); \
if (!opengl->name) { return false; } 
    wgl_set_opengl_function(glEnable);
    wgl_set_opengl_function(glDisable); 
    wgl_set_opengl_function(glViewport);
    wgl_set_opengl_function(glScissor);
    wgl_set_opengl_function(glCreateShader);
    wgl_set_opengl_function(glCompileShader);
    wgl_set_opengl_function(glShaderSource);
    wgl_set_opengl_function(glAttachShader);
    wgl_set_opengl_function(glDeleteShader);
    wgl_set_opengl_function(glClear);
    wgl_set_opengl_function(glClearColor);
    wgl_set_opengl_function(glCreateBuffers);
    wgl_set_opengl_function(glNamedBufferStorage);
    wgl_set_opengl_function(glCreateVertexArrays);
    wgl_set_opengl_function(glVertexArrayVertexBuffer);
    wgl_set_opengl_function(glEnableVertexArrayAttrib);
    wgl_set_opengl_function(glVertexArrayAttribFormat);
    wgl_set_opengl_function(glVertexArrayAttribBinding);
    wgl_set_opengl_function(glVertexArrayBindingDivisor);
    wgl_set_opengl_function(glBlendFunc);
    wgl_set_opengl_function(glBlendFuncSeparate);
    wgl_set_opengl_function(glCreateProgram);
    wgl_set_opengl_function(glLinkProgram);
    wgl_set_opengl_function(glGetProgramiv);
    wgl_set_opengl_function(glGetProgramInfoLog);
    wgl_set_opengl_function(glVertexArrayElementBuffer);
    wgl_set_opengl_function(glCreateTextures);
    wgl_set_opengl_function(glTextureStorage2D);
    wgl_set_opengl_function(glTextureSubImage2D);
    wgl_set_opengl_function(glBindTexture);
    wgl_set_opengl_function(glTexParameteri);
    wgl_set_opengl_function(glDrawElementsInstancedBaseInstance);
    wgl_set_opengl_function(glGetUniformLocation);
    wgl_set_opengl_function(glNamedBufferSubData);
    wgl_set_opengl_function(glProgramUniform4fv);
    wgl_set_opengl_function(glProgramUniformMatrix4fv);
    wgl_set_opengl_function(glUseProgram);
    wgl_set_opengl_function(glDeleteTextures);
    wgl_set_opengl_function(glDrawArrays);
    wgl_set_opengl_function(glGetError);
    wgl_set_opengl_function(glGenVertexArrays);
    wgl_set_opengl_function(glGenBuffers);
    wgl_set_opengl_function(glBindBuffer);
    wgl_set_opengl_function(glBufferData);
    wgl_set_opengl_function(glEnableVertexAttribArray);
    wgl_set_opengl_function(glVertexAttribPointer);
    wgl_set_opengl_function(glVertexAttribDivisor);
    wgl_set_opengl_function(glBindVertexArray);
    wgl_set_opengl_function(glDrawElements);
    wgl_set_opengl_function(glBufferSubData);
    wgl_set_opengl_function(glClearDepth);
  }
#undef wgl_set_opengl_function
  
  if (!eden_opengl_init(
        eden,
        texture_queue_size,
        max_commands,
        max_textures,
        max_payloads,
        max_elements)) 
  {
    return false;
  }
  
#if 0
  opengl->glEnable(GL_DEBUG_OUTPUT);
  opengl->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  opengl->glDebugMessageCallbackARB(Win_opengl_t_DebugCallback, nullptr);
#endif
  
  
  // VSync. 
  if (!wglSwapIntervalEXT) {
    wglSwapIntervalEXT = (wglSwapIntervalEXTFn*)wglGetProcAddress("wglSwapIntervalEXT");
  }
  if (wglSwapIntervalEXT) {
    wglSwapIntervalEXT(1);
  }

  return true;
}



static 
w32_gfx_begin_frame_sig(w32_gfx_begin_frame)
{
  eden_opengl_begin_frame(gfx, render_wh, region_x0, region_y0, region_x1, region_y1);
}

static
w32_gfx_end_frame_sig(w32_gfx_end_frame) {
  eden_opengl_end_frame(gfx);
  SwapBuffers(wglGetCurrentDC());
}

#endif
