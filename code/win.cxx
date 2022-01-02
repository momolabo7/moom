
#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "win_gfx.h"

#include "game_os.h"


//- NOTE(Momo): Global variables
static B32 g_is_running;

static inline LONG RECT_Width(RECT r) { return r.right - r.left; }
static inline LONG RECT_Height(RECT r) { return r.bottom - r.top; }


static inline V2U32
Win_GetWindowDims(HWND window) {
	RECT rect;
	GetWindowRect(window, &rect);
  
  V2U32 ret;
  ret.w = U32(rect.right - rect.left);
  ret.h = U32(rect.bottom - rect.top);
  
  return ret;
	
}

static V2U32
Win_GetClientDims(HWND window) {
	RECT rect;
	GetClientRect(window, &rect);
  
  V2U32 ret;
  ret.w = U32(rect.right - rect.left);
  ret.h = U32(rect.bottom - rect.top);
  
  return ret;
	
}

static Rect2U32 
Win_GetRenderRegion(U32 window_w, 
                    U32 window_h, 
                    U32 render_w, 
                    U32 render_h) 
{
	Assert(render_w > 0 && render_h > 0 && window_w > 0 && window_h > 0);
  
	Rect2U32 ret;
	
	F32 optimal_window_w = (F32)window_h * ((F32)render_w / (F32)render_h);
	F32 optimal_window_h = (F32)window_w * ((F32)render_h / (F32)render_w);
	
	if (optimal_window_w > (F32)window_w) {
		// NOTE(Momo): width has priority - top and bottom bars
		ret.min.x = 0;
		ret.max.x = window_w;
		
		F32 empty_height = (F32)window_h - optimal_window_h;
		
		ret.min.y = (U32)(empty_height * 0.5f);
		ret.max.y = ret.min.y + (U32)optimal_window_h;
	}
	else {
		// NOTE(Momo): height has priority - left and right bars
		ret.min.y = 0;
		ret.max.y = window_h;
		
		
		F32 empty_width = (F32)window_w - optimal_window_w;
		
		ret.min.x = (U32)(empty_width * 0.5f);
		ret.max.x = ret.min.x + (U32)optimal_window_w;
	}
	
	return ret;
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
  Win_Gfx_Fns gfx_fns;
  {
    HMODULE gfx_dll = LoadLibraryA("gfx.dll");
    if (gfx_dll) {
      gfx_fns.init = (Win_Gfx_InitFn*)GetProcAddress(gfx_dll, "Gfx_Init");
      if(!gfx_fns.init) return 1;
      
      gfx_fns.free = (Win_Gfx_FreeFn*)GetProcAddress(gfx_dll, "Gfx_Free");
      if(!gfx_fns.free) return 1;
      
      gfx_fns.render = (Win_Gfx_RenderFn*)GetProcAddress(gfx_dll, "Gfx_Render");
      if(!gfx_fns.render) return 1;
      
    }
    else {
      return 1;
    }
  }
  
  // NOTE(Momo): Load game functions
  Game_Fns game_fns; 
  {
    HMODULE game_dll = LoadLibraryA("game.dll");
    if (game_dll) {
      game_fns.update = (Game_UpdateFn*)GetProcAddress(game_dll, "Game_Update");
      if(!game_fns.update) return 1;
      
      game_fns.get_info = (Game_GetInfoFn*)GetProcAddress(game_dll, "Game_GetInfo");
      if(!game_fns.get_info) return 1;
      
    }
    else {
      return 1;
    }
  }
  
  //- NOTE(Momo): Init gfx
  Gfx* gfx = gfx_fns.init(window, MB(256), 8, 4096);
  if (!gfx) {
    return 1;
  }
  
  // TODO(Momo): Testing texture. Remove after use.
  U8 test_texture[4][4] {
    { 0, 0, 0, 255 },
    { 255, 255, 255, 255 },
    { 255, 255, 255, 255 },
    { 0, 0, 0, 255 },
  };
  
  // Gfx_Texture texture = gfx_fns.add_texture(gfx, 2, 2, (void*)&test_texture);
  
  
  //- NOTE(Momo): Begin game loop
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
      // TODO(Momo): Do we need to do anything about missing a frame?
    }
    
    // TODO(Momo): Test gfx
    
    
    
    // NOTE(Momo): Resize if needed. 
    // TODO(Momo): Maybe we only do this once and then 
    // only when window size changes after?
    V2U32 render_wh = Win_GetClientDims(window);
    
    
    // TODO(Momo): Should probably make a "GameInfo" struct that 
    // contains information like these
    const U32 game_design_width = 800;
    const U32 game_design_height = 800;
    Rect2U32 render_region = Win_GetRenderRegion(render_wh.w,
                                                 render_wh.h,
                                                 game_design_width,
                                                 game_design_height);
    
    
    Gfx_SetTexture(gfx, 0, 2, 2, (U8*)&test_texture);
    Gfx_ClearTextures(gfx);
    {
      RGBAF32 colors;
      colors.r = colors.g = colors.b  = colors.a = 0.3f;
      Gfx_Clear(gfx, colors);
    }
    
    {
      V3F32 position = {0};
      Rect3F32 frustum;
      frustum.min.x = frustum.min.y = frustum.min.z = 0;
      frustum.max.x = 1600;
      frustum.max.y = 900;
      frustum.max.z = 500;
      Gfx_SetOrthoCamera(gfx, position, frustum);
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
      Gfx_DrawSprite(gfx, colors, t, 0);
    }
    
    gfx_fns.render(gfx, render_wh, render_region);
    
    last_count = Win_QueryPerformanceCounter();
    
    
    //- NOTE(Momo): Swap buffers
    {
      HDC dc = GetDC(window);
      SwapBuffers(dc);
      ReleaseDC(window, dc);
    }
    
    
  }
  
  gfx_fns.free(gfx);
  
  
  
  return 0;  
  
  
}