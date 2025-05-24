#include "momo.h"


#include <stdio.h>

#define STABLE_WINDOW_WIDTH (200)
#define STABLE_WINDOW_HEIGHT (30)



struct stable_t
{
  arena_t arena;
  b32_t is_running;
  HWND main_window;
  HINSTANCE instance;

};
static stable_t* stable;



static b32_t
stable_spawn_overlay_window()
{
#if 0
  int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
  int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

  w32_dib_t* ss_dib = &stable->selection_screenshot;
  w32_dib_t* frame_dib = &stable->selection_frame;
  w32_dib_init(ss_dib, w, h);
  w32_dib_init(frame_dib, w, h);

  HDC screen_dc = GetDC(0);
  w32_dib_blit_from_dc(ss_dib, screen_dc, x, y);
  w32_dib_blit_from_dc(frame_dib, screen_dc, x, y);
  ReleaseDC(0, screen_dc);

  stable->drag_start = stable->drag_end = stable->drag_end_prev = {0};

  for (int i = 0; i < w * h; ++i) {
    u32_t* pixel = w32_dib_pixel_index(frame_dib, i); 
    pixel[0] = (pixel[0] & 0xFF000000) | ((pixel[0] & 0x00FEFEFE) >> 1);
  }

  HWND window = CreateWindowEx(
      WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
      "Overlay",                 
      0,
      WS_POPUP | WS_VISIBLE | WS_BORDER,
      x, y,                    
      w, h,                   
      stable->main_window,                    
      NULL,
      stable->instance,
      NULL);

  if (!window)
  {
    // @todo: error handling
    return false;
  }

#endif
  return true;
}

LRESULT CALLBACK
w32_stable_overlay_window_callback(
    HWND window, 
    UINT message, 
    WPARAM w_param,
    LPARAM l_param) 
{
  LRESULT result = 0;
  switch(message) 
  {
    case WM_CREATE: 
    {
    } break;
    case WM_LBUTTONDOWN:
    {
    } break;
    case WM_LBUTTONUP:
    {
    } break;
    case WM_MOUSEMOVE:
    {
    } break;
    case WM_CLOSE:  
    case WM_QUIT:
    case WM_DESTROY: 
    {
    } break;
    case WM_KEYDOWN:
    {
    } break;
    case WM_PAINT: 
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(window, &ps);
      EndPaint(window, &ps);

    } break;
    default: {
      result = DefWindowProcA(window, message, w_param, l_param);
    };   
  }
  return result;
}

LRESULT CALLBACK
w32_stable_main_window_callback(
    HWND window, 
    UINT message, 
    WPARAM w_param,
    LPARAM l_param) 
{

  LRESULT result = 0;
  switch(message) 
  {
    case WM_CREATE: 
    {
    } break;
    case WM_CLOSE:  
    case WM_QUIT:
    case WM_DESTROY: 
    {
      stable->is_running = false;
      PostQuitMessage(0);
    } break;
    case WM_COMMAND:
    {
    } break;
    default: {
      result = DefWindowProcA(window, message, w_param, l_param);
    };   
  }
  return result;
}

int APIENTRY CALLBACK
WinMain(
    HINSTANCE instance, 
    HINSTANCE prev_instance, 
    LPSTR argv, 
    int argc) 
{
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
  stable = arena_alloc_bootstrap_zero(stable_t, arena, gigabytes(1)); 
  stable->instance = instance;

  // create window
  {
    const char* title = "no vomit";
    const char* icon_path = "window.ico";
    const s32_t icon_w = 256;
    const s32_t icon_h = 256;

    WNDCLASSA w32_class = {};
    w32_class.style = CS_HREDRAW | CS_VREDRAW;
    w32_class.lpfnWndProc = w32_stable_main_window_callback;
    w32_class.hInstance = instance;
    w32_class.hCursor = LoadCursor(0, IDC_ARROW);
    w32_class.lpszClassName = "MainWindowClass";
    w32_class.hIcon = (HICON)LoadImageA(NULL, 
                                        icon_path,
                                        IMAGE_ICON, 
                                        icon_w, 
                                        icon_h,
                                        LR_LOADFROMFILE);
    
    if(!RegisterClassA(&w32_class)) {
      return 1;
    }

    // for overlay window
    WNDCLASS selection_class = {};
    //w32_class.style = CS_HREDRAW | CS_VREDRAW;
    selection_class.lpfnWndProc = w32_stable_overlay_window_callback;
    selection_class.hInstance = instance;
    selection_class.lpszClassName = "Overlay";
    selection_class.hCursor = LoadCursor(NULL, IDC_CROSS);
    
    if(!RegisterClass(&selection_class)) {
      return 1;
    }

    
    RECT window_rect = {0};
    {
      // Monitor dimensions
      HMONITOR monitor = MonitorFromWindow(0, MONITOR_DEFAULTTONEAREST);
      MONITORINFOEX monitor_info;
      monitor_info.cbSize = sizeof(monitor_info);
      GetMonitorInfo(monitor, &monitor_info); 
      
      LONG monitor_w = w32_rect_width(monitor_info.rcMonitor);
      LONG monitor_h = w32_rect_height(monitor_info.rcMonitor);
      
      window_rect.left = monitor_w/2 - STABLE_WINDOW_WIDTH/2;
      window_rect.right = monitor_w/2 + STABLE_WINDOW_WIDTH/2;
      window_rect.top = monitor_h/2 - STABLE_WINDOW_HEIGHT/2;
      window_rect.bottom = monitor_h/2 + STABLE_WINDOW_HEIGHT/2;

    }
    
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU;

    // disable maximize and resizing
    style  &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
    
    AdjustWindowRectEx(&window_rect,
                       style,
                       FALSE,
                       0);    
    
    stable->main_window = CreateWindowExA(0,
                             w32_class.lpszClassName,
                             title,
                             style,
                             window_rect.left,
                             window_rect.top,
                             w32_rect_width(window_rect),
                             w32_rect_height(window_rect),
                             0,
                             0,
                             instance,
                             0);

    if (!stable->main_window) 
    {
      return 1;
    }


    // Create the push button
    CreateWindow(
        "BUTTON",               // Predefined class; Unicode use L"BUTTON"
        STABLE_SS_BUTTON_TEXT,             // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
        STABLE_SS_BUTTON_X,                     
        STABLE_SS_BUTTON_Y,                     
        STABLE_SS_BUTTON_W,                    
        STABLE_SS_BUTTON_H,                     
        stable->main_window,                 
        (HMENU)STABLE_SS_BUTTON_COMMAND_ID,               
        0,              // Instance handle
        NULL            // Additional application data
        );
  }

  stable->is_running = true;
  while(stable->is_running)
  {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return 0;
}
