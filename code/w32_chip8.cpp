
#include "momo.h"

static inline LONG w32_rect_width(RECT r) { return r.right - r.left; }
static inline LONG w32_rect_height(RECT r) { return r.bottom - r.top; }

LRESULT CALLBACK
w32_window_callback(HWND window, 
                    UINT message, 
                    WPARAM w_param,
                    LPARAM l_param) 
{
  LRESULT result = 0;
  switch(message) {
    case WM_CLOSE:  
    case WM_QUIT:
    case WM_DESTROY: {
      //w32_state.is_running = false;
    } break;
    default: {
      result = DefWindowProcA(window, message, w_param, l_param);
    };   
  }
  return result;
}

int APIENTRY CALLBACK
WinMain(HINSTANCE instance, 
    HINSTANCE prev_instance, 
    LPSTR argv, 
    int argc) 
{
  HANDLE stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
  if (!stdout_handle) {
    AttachConsole(ATTACH_PARENT_PROCESS);
  }

  
  HWND window;
  {
    const int window_w = (int)100;
    const int window_h = (int)100;
    const char* title = "chip8 emulator";
    const char* icon_path = "window.ico";
    const int icon_w = 256;
    const int icon_h = 256;
    WNDCLASSA w32_class = {};
    w32_class.style = CS_HREDRAW | CS_VREDRAW;
    w32_class.lpfnWndProc = w32_window_callback;
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
    
    RECT window_rect = {0};
    {
      // Monitor dimensions
      HMONITOR monitor = MonitorFromWindow(0, MONITOR_DEFAULTTONEAREST);
      MONITORINFOEX monitor_info;
      monitor_info.cbSize = sizeof(monitor_info);
      GetMonitorInfo(monitor, &monitor_info); 
      
      LONG monitor_w = w32_rect_width(monitor_info.rcMonitor);
      LONG monitor_h = w32_rect_height(monitor_info.rcMonitor);
      
      window_rect.left = monitor_w/2 - window_w/2;
      window_rect.right = monitor_w/2 + window_w/2;
      window_rect.top = monitor_h/2 - window_h/2;
      window_rect.bottom = monitor_h/2 + window_h/2;
    }
    
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    
    AdjustWindowRectEx(&window_rect,
                       style,
                       FALSE,
                       0);    
    
    window = CreateWindowExA(0,
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
    
    if (!window) {
      return 1;
    }
    
    
  }

  
  b32_t is_running = true;
  while(is_running)
  {
  }

  return 0;

}
