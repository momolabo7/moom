// NOTE(Momo): Renderer code for Win w/ Openg


#include "win_renderer.h"
#include "game_renderer_opengl.h"


//~WGL stuff

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
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001

typedef BOOL WINAPI 
wglChoosePixelFormatARBFn(HDC hdc,
                          const int* piAttribIList,
                          const FLOAT* pfAttribFList,
                          UINT nMaxFormats,
                          int* piFormats,
                          UINT* nNumFormats);

typedef BOOL WINAPI 
wglSwapIntervalEXTFn(int interval);

typedef HGLRC WINAPI 
wglCreateContextAttribsARBFn(HDC hdc, 
                             HGLRC hShareContext,
                             const int* attribList);

typedef const char* WINAPI 
wglGetExtensionsStringEXTFn(void);

static wglChoosePixelFormatARBFn* wglChoosePixelFormatARB;
static wglSwapIntervalEXTFn* wglSwapIntervalEXT;
static wglCreateContextAttribsARBFn* wglCreateContextAttribsARB;
static wglGetExtensionsStringEXTFn* wglGetExtensionsStringEXT;



static void* 
win_try_get_wgl_function(const char* name, HMODULE fallback_module)
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
win_set_pixel_format(HDC dc) {
  S32 suggested_pixel_format_index = 0;
  U32 extended_pick = 0;
  
  if (wglChoosePixelFormatARB) {
    S32 attrib_list[] = {
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
static B32
win_load_wgl_extentions() {
  WNDCLASSA window_class = {};
  // Er yeah...we have to create a 'fake' Opengl context 
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
    win_set_pixel_format(dc);
    HGLRC opengl_context = wglCreateContext(dc);
    
    B32 success = true;
    
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
    DWORD test = GetLastError();
    return false;
  }
}

//~API implementation
exported void
win_unload_renderer(Renderer* r) {
  // Unused?
}

exported Renderer*
win_load_renderer(HWND window, 
                  UMI command_queue_size,
                  UMI texture_queue_size, 
                  Arena* arena) 
{
  HDC dc = GetDC(window); 
  if (!dc) {
    return nullptr;
  }
  //defer { ReleaseDC(window, dc); };
  
  
  Opengl* opengl = push<Opengl>(arena);
  
  // Allocate memory for render commands
  init_command_queue(&opengl->command_queue, 
                     push_block(arena, command_queue_size), 
                     command_queue_size);
  
  // Allocate memory for texture transfer queue
  init_texture_queue(&opengl->texture_queue, 
                     push_block(arena, texture_queue_size),
                     texture_queue_size);
  
  
  if (!opengl) {
    return nullptr;
  }
  
  if (!win_load_wgl_extentions()) {
    return nullptr;
  }
  
  win_set_pixel_format(dc);
  
  S32 opengl_attribs[] {
    WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
    WGL_CONTEXT_MINOR_VERSION_ARB, 5,
    WGL_CONTEXT_FLAG_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
#if 0
    | WGL_CONTEXT_DEBUG_BIT_ARB
#endif
    ,
    WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0,
  };
  HGLRC opengl_ctx = wglCreateContextAttribsARB(dc, 0, 
                                                opengl_attribs); 
  
  if (!opengl_ctx) {
    return nullptr;
  }
  
  
  if(wglMakeCurrent(dc, opengl_ctx)) {
    HMODULE module = LoadLibraryA("opengl32.dll");
#define WGL_SetOpenglFunction(name) \
opengl->name = (GL_##name*)win_try_get_wgl_function(#name, module); \
if (!opengl->name) { return nullptr; } 
    
    WGL_SetOpenglFunction(glEnable);
    WGL_SetOpenglFunction(glDisable); 
    WGL_SetOpenglFunction(glViewport);
    WGL_SetOpenglFunction(glScissor);
    WGL_SetOpenglFunction(glCreateShader);
    WGL_SetOpenglFunction(glCompileShader);
    WGL_SetOpenglFunction(glShaderSource);
    WGL_SetOpenglFunction(glAttachShader);
    WGL_SetOpenglFunction(glDeleteShader);
    WGL_SetOpenglFunction(glClear);
    WGL_SetOpenglFunction(glClearColor);
    WGL_SetOpenglFunction(glCreateBuffers);
    WGL_SetOpenglFunction(glNamedBufferStorage);
    WGL_SetOpenglFunction(glCreateVertexArrays);
    WGL_SetOpenglFunction(glVertexArrayVertexBuffer);
    WGL_SetOpenglFunction(glEnableVertexArrayAttrib);
    WGL_SetOpenglFunction(glVertexArrayAttribFormat);
    WGL_SetOpenglFunction(glVertexArrayAttribBinding);
    WGL_SetOpenglFunction(glVertexArrayBindingDivisor);
    WGL_SetOpenglFunction(glBlendFunc);
    WGL_SetOpenglFunction(glBlendFuncSeparate);
    WGL_SetOpenglFunction(glCreateProgram);
    WGL_SetOpenglFunction(glLinkProgram);
    WGL_SetOpenglFunction(glGetProgramiv);
    WGL_SetOpenglFunction(glGetProgramInfoLog);
    WGL_SetOpenglFunction(glVertexArrayElementBuffer);
    WGL_SetOpenglFunction(glCreateTextures);
    WGL_SetOpenglFunction(glTextureStorage2D);
    WGL_SetOpenglFunction(glTextureSubImage2D);
    WGL_SetOpenglFunction(glBindTexture);
    WGL_SetOpenglFunction(glTexParameteri);
    WGL_SetOpenglFunction(glBindVertexArray);
    WGL_SetOpenglFunction(glDrawElementsInstancedBaseInstance);
    WGL_SetOpenglFunction(glGetUniformLocation);
    WGL_SetOpenglFunction(glNamedBufferSubData);
    WGL_SetOpenglFunction(glProgramUniform4fv);
    WGL_SetOpenglFunction(glProgramUniformMatrix4fv);
    WGL_SetOpenglFunction(glUseProgram);
    WGL_SetOpenglFunction(glDeleteTextures);
    WGL_SetOpenglFunction(glDebugMessageCallbackARB);
    WGL_SetOpenglFunction(glDrawArrays);
  }
#undef WGL_SetOpenglFunction
  
  if (!opengl_init(opengl)) {
    return nullptr;
  }
  
  // TODO(Momo): Figure out how to get callback?
#if 0
  opengl->glEnable(GL_DEBUG_OUTPUT);
  opengl->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  //    g_opengl.glDebugMessageCallbackARB(Win_Opengl_DebugCallback, nullptr);
#endif
  
  
#if 0
  // VSync. 
  if (!wglSwapIntervalEXT) {
    wglSwapIntervalEXT = (wglSwapIntervalEXTFn*)wglGetProcAddress("wglSwapIntervalEXT");
  }
  if (wglSwapIntervalEXT) {
    wglSwapIntervalEXT(1);
  }
#endif
  
  return (Renderer*)opengl;
  
}


exported void
win_begin_renderer_frame(Renderer* renderer,  V2U render_wh, Rect2U region) {
  return opengl_begin_frame((Opengl*)renderer, render_wh, region);
}

exported void
win_end_renderer_frame(Renderer* renderer) {
  opengl_end_frame((Opengl*)renderer);
  SwapBuffers(wglGetCurrentDC());
}


