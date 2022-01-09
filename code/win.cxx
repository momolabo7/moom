
#include <windows.h>
#undef near
#undef far

#include "momo.h"
#include "win_gfx.h"

#include "game_pf.h"


//-NOTE(Momo): Global variables
typedef struct {
  B32 is_running;
  B32 is_hot_reloading;
} Win_State;
static Win_State Win_global_state;

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
      Win_global_state.is_running = false;
    } break;
    default: {
      result = DefWindowProcA(window, message, w_param, l_param);
    };   
  }
  return result;
}

//~ For Platform API
static void 
Win_HotReload() {
  Win_global_state.is_hot_reloading = true;
}

static void 
Win_Shutdown() {
  Win_global_state.is_running = false;
}


static void*
Win_AllocateMemory(UMI memory_size) {
  return VirtualAllocEx(GetCurrentProcess(),
                        0, 
                        memory_size,
                        MEM_RESERVE | MEM_COMMIT, 
                        PAGE_READWRITE);
}


static void
Win_FreeMemory(void* memory) {
  VirtualFreeEx(GetCurrentProcess(), 
                memory,    
                0, 
                MEM_RELEASE); 
}

static PF
Win_CreatePF()
{
  PF pf_api;
  pf_api.hot_reload = Win_HotReload;
  pf_api.alloc = Win_AllocateMemory;
  pf_api.free = Win_FreeMemory;
  pf_api.shutdown = Win_Shutdown;
  return pf_api;
}


//~ Main functions
int CALLBACK
WinMain(HINSTANCE instance, 
        HINSTANCE prev_instance, 
        LPSTR command_line, 
        int show_code) 
{
  
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  ImmDisableIME((DWORD)-1);
  
  //-NOTE(Momo): Create window in the middle of the screen
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
  
  //-NOTE(Momo): Load Platform API for game
  PF pf_api = Win_CreatePF();
  
  //-NOTE(Momo): Load Gfx functions
  WinGfx_API gfx_api;
  HMODULE gfx_dll;
  {
    gfx_dll =  LoadLibraryA("gfx.dll");
    if (gfx_dll) {
      gfx_api.init = (WinGfx_InitFn*)GetProcAddress(gfx_dll, "WinGfx_Init");
      if(!gfx_api.init) return 1;
      
      gfx_api.free = (WinGfx_FreeFn*)GetProcAddress(gfx_dll, "WinGfx_Free");
      if(!gfx_api.free) return 1;
      
      gfx_api.render = (WinGfx_RenderFn*)GetProcAddress(gfx_dll, "WinGfx_Render");
      if(!gfx_api.render) return 1;
      
    }
    else {
      return 1;
    }
  }
  defer { FreeLibrary(gfx_dll); };
  
  
  //-NOTE(Momo): Init gfx
  Gfx* gfx = gfx_api.init(window);
  if (!gfx) {
    return 1;
  }
  defer { gfx_api.free(gfx); };
  
  
  //- NOTE(Momo): Init input
  Input input = {0};
  
  
  // TODO(Momo): Testing texture. Remove after use.
  U8 test_texture[4][4] {
    { 0, 0, 0, 255 },
    { 255, 255, 255, 255 },
    { 255, 255, 255, 255 },
    { 0, 0, 0, 255 },
  };
  
  //-NOTE(Momo): Begin game loop
  Game game = {0};
  Win_global_state.is_running = true;
  Win_global_state.is_hot_reloading = true;
  
  B32 is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  LARGE_INTEGER performance_frequency;
  QueryPerformanceFrequency(&performance_frequency);
  
  LARGE_INTEGER last_count = Win_QueryPerformanceCounter();
  
  Game_API game_api = {0}; 
  HMODULE game_dll = NULL; 
  
  while (Win_global_state.is_running) {
    //-NOTE(Momo): Hot reload game.dll functions
    if (Win_global_state.is_hot_reloading){
      static constexpr char* running_game_dll = "running_game.dll";
      static constexpr char* compiled_game_dll = "game.dll";
      
      // Release the current game dll
      if (game_dll) {
        FreeLibrary(game_dll);
        game_dll = NULL;
      }
      
      // Copy the compiled game dll
      if(!CopyFile(compiled_game_dll, running_game_dll, false)) {
        return 1;
      }
      
      game_dll = LoadLibraryA(running_game_dll);
      if (game_dll) {
        game_api.update = (Game_UpdateFn*)GetProcAddress(game_dll, "Game_Update");
        if(!game_api.update) {
          FreeLibrary(game_dll);
          return 1;
        }          
      }
      else {
        return 1;
      }
      Win_global_state.is_hot_reloading = false;
    }
    
    Update(&input);
    
    //-NOTE(Momo): Process messages and input
    {
      MSG msg = {};
      while(PeekMessage(&msg, window, 0, 0, PM_REMOVE)) {
        switch(msg.message) {
          case WM_QUIT:
          case WM_DESTROY:
          case WM_CLOSE: {
            Win_global_state.is_running = false;
          } break;
          case WM_KEYUP:
          case WM_KEYDOWN:
          case WM_SYSKEYDOWN:
          case WM_SYSKEYUP:
          {
            U32 code = (U32)msg.wParam;
            B32 is_down = msg.message == WM_KEYDOWN;
            switch(code) {
              case 0x57: /* W  */ 
              {
                input.button_up.now = is_down;
              } break;
            }
            
          } break;
          
          default: {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
          }
        }
        
      }
    }
    
    //-NOTE(Momo) Game logic here 
    // TODO(Momo): figure out target secs per frame
    const F64 game_dt = 1/60.0;
    
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
    
    game_api.update(&game, &pf_api, &input, gfx, (F32)(game_dt));
    
    
    gfx_api.render(gfx, render_wh, render_region);
    //-NOTE(Momo): Frame-rate control
    // 1. Calculate how much time has passed since the last frame
    // 2. If the time elapsed is greater than the target time elapsed,
    //    sleep/spin-lock until then.    
    F64 secs_elapsed = 
      Win_GetSecsElapsed(last_count,
                         Win_QueryPerformanceCounter(),
                         performance_frequency);
    
    
    
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
    
    
    last_count = Win_QueryPerformanceCounter();
    
    
    //-NOTE(Momo): Swap buffers
    {
      HDC dc = GetDC(window);
      SwapBuffers(dc);
      ReleaseDC(window, dc);
    }
    
    
  }
  
  
  
  
  return 0;  
  
  
}