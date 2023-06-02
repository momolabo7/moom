// NOTE(Momo): gfx_t code for Win w/ opengl_t


#include "w32_gfx.h"
#include "game_gfx_opengl.h"

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
#define WGL_CONTEXT_MOMO_PROFILE_BIT_ARB        0x00000001

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
    DWORD test = GetLastError();
    return false;
  }
}

//~API implementation
static void
w32_gfx_unload(gfx_t* gfx) {
#if 0
  opengl_t* ogl = (opengl_t*)gfx;
  w32_free_memory((pf_memory_t*)ogl->user_data);
#endif
}

static gfx_t*
w32_gfx_load(HWND window, 
             usz_t command_queue_size,
             usz_t texture_queue_size,
             arena_t* arena) 
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
  HGLRC opengl_ctx = wglCreateContextAttribsARB(dc, 0, 
                                                opengl_attribs); 
  
  if (!opengl_ctx) {
    return nullptr;
  }


  opengl_t* opengl = arena_push(opengl_t, arena);
  void* command_queue_block = arena_push_size(arena, command_queue_size, 16); 
  void* texture_queue_block = arena_push_size(arena, texture_queue_size, 16); 


  if (!opengl || !command_queue_block || !texture_queue_block) {
    return nullptr;
  }

  //opengl->huser_data = opengl;
  
  
  if(wglMakeCurrent(dc, opengl_ctx)) {
    HMODULE module = LoadLibraryA("opengl32.dll");
#define WGL_Setopengl_tFunction(name) \
opengl->name = (OPENGL_##name*)_w32_try_get_wgl_function(#name, module); \
if (!opengl->name) { return nullptr; } 
    
    WGL_Setopengl_tFunction(glEnable);
    WGL_Setopengl_tFunction(glDisable); 
    WGL_Setopengl_tFunction(glViewport);
    WGL_Setopengl_tFunction(glScissor);
    WGL_Setopengl_tFunction(glCreateShader);
    WGL_Setopengl_tFunction(glCompileShader);
    WGL_Setopengl_tFunction(glShaderSource);
    WGL_Setopengl_tFunction(glAttachShader);
    WGL_Setopengl_tFunction(glDeleteShader);
    WGL_Setopengl_tFunction(glClear);
    WGL_Setopengl_tFunction(glClearColor);
    WGL_Setopengl_tFunction(glCreateBuffers);
    WGL_Setopengl_tFunction(glNamedBufferStorage);
    WGL_Setopengl_tFunction(glCreateVertexArrays);
    WGL_Setopengl_tFunction(glVertexArrayVertexBuffer);
    WGL_Setopengl_tFunction(glEnableVertexArrayAttrib);
    WGL_Setopengl_tFunction(glVertexArrayAttribFormat);
    WGL_Setopengl_tFunction(glVertexArrayAttribBinding);
    WGL_Setopengl_tFunction(glVertexArrayBindingDivisor);
    WGL_Setopengl_tFunction(glBlendFunc);
    WGL_Setopengl_tFunction(glBlendFuncSeparate);
    WGL_Setopengl_tFunction(glCreateProgram);
    WGL_Setopengl_tFunction(glLinkProgram);
    WGL_Setopengl_tFunction(glGetProgramiv);
    WGL_Setopengl_tFunction(glGetProgramInfoLog);
    WGL_Setopengl_tFunction(glVertexArrayElementBuffer);
    WGL_Setopengl_tFunction(glCreateTextures);
    WGL_Setopengl_tFunction(glTextureStorage2D);
    WGL_Setopengl_tFunction(glTextureSubImage2D);
    WGL_Setopengl_tFunction(glBindTexture);
    WGL_Setopengl_tFunction(glTexParameteri);
    WGL_Setopengl_tFunction(glBindVertexArray);
    WGL_Setopengl_tFunction(glDrawElementsInstancedBaseInstance);
    WGL_Setopengl_tFunction(glGetUniformLocation);
    WGL_Setopengl_tFunction(glNamedBufferSubData);
    WGL_Setopengl_tFunction(glProgramUniform4fv);
    WGL_Setopengl_tFunction(glProgramUniformMatrix4fv);
    WGL_Setopengl_tFunction(glUseProgram);
    WGL_Setopengl_tFunction(glDeleteTextures);
    WGL_Setopengl_tFunction(glDebugMessageCallbackARB);
    WGL_Setopengl_tFunction(glDrawArrays);
  }
#undef WGL_Setopengl_tFunction
  
  if (!opengl_init(opengl, 
                command_queue_block, 
                command_queue_size,
                texture_queue_block,
                texture_queue_size)) 
  {
    return 0;
  }
  
#if 0
  opengl->glEnable(GL_DEBUG_OUTPUT);
  opengl->glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  //    g_opengl.glDebugMessageCallbackARB(Win_opengl_t_DebugCallback, nullptr);
#endif
  
  
#if 1
  // VSync. 
  if (!wglSwapIntervalEXT) {
    wglSwapIntervalEXT = (wglSwapIntervalEXTFn*)wglGetProcAddress("wglSwapIntervalEXT");
  }
  if (wglSwapIntervalEXT) {
    wglSwapIntervalEXT(1);
  }
#endif
  return &opengl->gfx;
}


static void
w32_gfx_begin_frame(gfx_t* renderer,  v2u_t render_wh, u32_t region_x0, u32_t region_y0, u32_t region_x1, u32_t region_y1) 
{
  return opengl_begin_frame((opengl_t*)renderer, render_wh, region_x0, region_y0, region_x1, region_y1);
}

static void
w32_gfx_end_frame(gfx_t* renderer) {
  opengl_end_frame((opengl_t*)renderer);
  SwapBuffers(wglGetCurrentDC());
}



