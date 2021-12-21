
#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "game_gfx.h"
#include "win_gfx.h"

#include "game_gfx.cpp"

//- NOTE(Momo): Global variables

static B32 g_is_running;


static LONG
RECT_Width(RECT r) {
  return r.right - r.left;
}

static LONG
RECT_Height(RECT r) {
  return r.bottom - r.top;
}

static LARGE_INTEGER
Win_QueryPerformanceCounter(void) {
  LARGE_INTEGER result;
  QueryPerformanceCounter(&result);
  return result;
}

static F64
Win_GetSecsElapsed(LARGE_INTEGER start,
                   LARGE_INTEGER end,
                   LARGE_INTEGER performance_frequency) 
{
  
  return (F64(end.QuadPart - start.QuadPart)) / performance_frequency.QuadPart;
}

LRESULT CALLBACK
Win_WindowCallback(HWND window, 
                   UINT message, 
                   WPARAM w_param,
                   LPARAM l_param) 
{
  LRESULT result = 0;
  switch(message) {
    case WM_CLOSE:  
    case WM_QUIT:
    case WM_DESTROY: {
      g_is_running = false;
    } break;
    default: {
      result = DefWindowProcA(window, message, w_param, l_param);
    };   
  }
  return result;
}

int CALLBACK
WinMain(HINSTANCE instance, 
        HINSTANCE prev_instance, 
        LPSTR command_line, 
        int show_code) 
{
  
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  ImmDisableIME((DWORD)-1);
  
  
  
  
  
  //- NOTE(Momo): Create window in the middle of the screen
  HWND window;
  {
    // TODO(Momo): Maybe this can be defined elsewhere...?
    // Who decides this anyway?
    // Perhaps we let the game decide and just let these be
    // default value?
    const int win_w = 1600;
    const int win_h = 900;
    const char* title = "Momodevelop: TXT";
    const char* icon_path = "window.ico";
    const int icon_w = 256;
    const int icon_h = 256;
    
    
    WNDCLASSA win_class = {};
    win_class.style = CS_HREDRAW | CS_VREDRAW;
    win_class.lpfnWndProc = Win_WindowCallback;
    win_class.hInstance = instance;
    win_class.hCursor = LoadCursor(0, IDC_ARROW);
    win_class.lpszClassName = "MainWindowClass";
    
    win_class.hIcon = (HICON)LoadImageA(NULL, 
                                        icon_path,
                                        IMAGE_ICON, 
                                        icon_w, 
                                        icon_h,
                                        LR_LOADFROMFILE);
    
    if(!RegisterClassA(&win_class)) {
      return 1;
    }
    
    RECT win_rect = {0};
    {
      // NOTE(Momo): Monitor dimensions
      HMONITOR monitor = MonitorFromWindow(0, MONITOR_DEFAULTTONEAREST);
      MONITORINFOEX monitor_info;
      monitor_info.cbSize = sizeof(monitor_info);
      GetMonitorInfo(monitor, &monitor_info); 
      
      LONG monitor_w = RECT_Width(monitor_info.rcMonitor);
      LONG monitor_h = RECT_Height(monitor_info.rcMonitor);
      
      win_rect.left = monitor_w/2 - win_w/2;
      win_rect.right = monitor_w/2 + win_w/2;
      win_rect.top = monitor_h/2 - win_h/2;
      win_rect.bottom = monitor_h/2 + win_h/2;
    }
    
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    
    AdjustWindowRectEx(&win_rect,
                       style,
                       FALSE,
                       0);    
    
    window = CreateWindowExA(0,
                             win_class.lpszClassName,
                             title,
                             style,
                             win_rect.left,
                             win_rect.top,
                             RECT_Width(win_rect),
                             RECT_Height(win_rect),
                             0,
                             0,
                             instance,
                             0);
    
    if (!window) {
      return 1;
    }
    
    
    
    
  }
  
  //- NOTE(Momo): Load Gfx functions
  Win_Gfx_Functions gfx_fns = {0};
  {
    HMODULE gfx_dll = LoadLibraryA("gfx.dll");
    if (gfx_dll) {
      gfx_fns.init = (Win_Gfx_Init*)GetProcAddress(gfx_dll, "Gfx_Init");
      if(!gfx_fns.init) return 1;
      
      gfx_fns.free = (Win_Gfx_Free*)GetProcAddress(gfx_dll, "Gfx_Free");
      if(!gfx_fns.free) return 1;
      
      gfx_fns.add_texture = (Win_Gfx_AddTexture*)GetProcAddress(gfx_dll, "Gfx_AddTexture");
      if(!gfx_fns.add_texture) return 1;
      
      gfx_fns.clear_textures = (Win_Gfx_ClearTextures*)GetProcAddress(gfx_dll, "Gfx_ClearTextures");
      if(!gfx_fns.clear_textures) return 1;
      
      gfx_fns.begin_frame = (Win_Gfx_BeginFrame*)GetProcAddress(gfx_dll, "Gfx_BeginFrame");
      if(!gfx_fns.begin_frame) return 1;
      
      gfx_fns.end_frame = (Win_Gfx_EndFrame*)GetProcAddress(gfx_dll, "Gfx_EndFrame");
      if(!gfx_fns.end_frame) return 1;
    }
    else {
      return 1;
    }
  }
  
  
  //- NOTE(Momo): Init gfx
  Gfx* gfx = gfx_fns.init(window, MB(10), 8, 4096);
  if (!gfx) {
    return 1;
  }
  
  
  
  g_is_running = true;
  B32 is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  
  
  
  
  LARGE_INTEGER performance_frequency;
  QueryPerformanceFrequency(&performance_frequency);
  
  LARGE_INTEGER last_count = Win_QueryPerformanceCounter();
  
  F32 tmp_delta = 0.f;
  B32 tmp_increase = true;
  F32 tmp_rot = 0.f;
  
  while (g_is_running) {
    
    //- NOTE(Momo): Process messages
    {
      MSG msg = {};
      while(PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
        switch(msg.message) {
          case WM_QUIT:
          case WM_DESTROY:
          case WM_CLOSE: {
            g_is_running = false;
          } break;
          
          default: {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
          }
        }
        
      }
    }
    
    //- NOTE(Momo): We control the frame rate here
    // 1. Calculate how much time has passed since the last frame
    // 2. If the time elapsed is greater than the target time elapsed,
    //    sleep/spin-lock until then.    
    F64 secs_elapsed = 
      Win_GetSecsElapsed(last_count,
                         Win_QueryPerformanceCounter(),
                         performance_frequency);
    
    
    // TODO(Momo): figure out target secs per frame
    const F64 game_dt = 1/60.0;
    
    if(game_dt > secs_elapsed) {
      if (is_sleep_granular) {
        DWORD ms_to_sleep 
          = (DWORD)(1000 * (game_dt - secs_elapsed));
        
        // NOTE(Momo): Return control to OS
        if (ms_to_sleep > 1) {
          Sleep(ms_to_sleep - 1);
        }
        
        // NOTE(Momo): Spin lock
        while(game_dt > secs_elapsed) {
          secs_elapsed = 
            Win_GetSecsElapsed(last_count,
                               Win_QueryPerformanceCounter(),
                               performance_frequency);
        }
        
      }
    }
    else {
      // NOTE(Momo): At this point, we basically missed a frame :(
    }
    
    V2U32 render_wh;
    render_wh.w = 1600;
    render_wh.h = 900;
    
    Rect2U32 render_region;
    render_region.min.x = render_region.min.y = 0;
    render_region.max.x = 1600;
    render_region.max.y = 900;
    
    Gfx_Cmds* cmds = gfx_fns.begin_frame(gfx, render_wh, render_region);
    
    {
      RGBAF32 colors;
      colors.r = colors.g = colors.b  = colors.a = 0.3f;
      Gfx_Clear(cmds, colors);
    }
    
    {
      V3F32 position = {0};
      Rect3F32 frustum;
      frustum.min.x = frustum.min.y = frustum.min.z = 0;
      frustum.max.x = 1600;
      frustum.max.y = 900;
      frustum.max.z = 500;
      Gfx_SetOrthoCamera(cmds, position, frustum);
    }
    
    {
      if (tmp_increase)
        tmp_delta += (F32)game_dt; 
      else
        tmp_delta -= (F32)game_dt;
      
      if (tmp_delta >= 1.f ){
        tmp_delta = 1.f;
        tmp_increase = false;
      }
      
      if (tmp_delta <= 0.f) {
        tmp_delta = 0.f;
        tmp_increase = true;
      }
      
      
      RGBAF32 colors = RGBAF32_Create(0.f, 0.f, 0.f, 1.f);
      HSLF32 hsl = HSLF32_Create(tmp_delta, 1.f, 0.5f);
      colors.rgb = HSLF32_ToRGBF32(hsl);
      
      M44F32 scale = M44F32_Scale(600.f, 600.f, 10.f);
      M44F32 rot = M44F32_RotationZ(tmp_rot += (F32)game_dt);
      M44F32 trans = M44F32_Translation(800.f, 450.f, 300.f);
      M44F32 t = M44F32_Concat(trans, M44F32_Concat(scale, rot));
      Gfx_DrawRect(cmds, colors, t);
    }
    
    gfx_fns.end_frame(gfx, cmds);
    
    last_count = Win_QueryPerformanceCounter();
    
    
    //- NOTE(Momo): Swap buffers
    {
      HDC dc = GetDC(window);
      SwapBuffers(dc);
      ReleaseDC(window, dc);
    }
    
    
  }
  
  
  
  
  
  
  
  return 0;  
  
  
}