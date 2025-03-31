#include "momo.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

#include <stdio.h>

#define GETUNA_WINDOW_WIDTH (200)
#define GETUNA_WINDOW_HEIGHT (30)

#define GETUNA_SS_BUTTON_COMMAND_ID (1)
#define GETUNA_SS_BUTTON_TEXT       "get tuna"
#define GETUNA_SS_BUTTON_X          (0)
#define GETUNA_SS_BUTTON_Y          (0)
#define GETUNA_SS_BUTTON_W          (200)
#define GETUNA_SS_BUTTON_H          (30)

struct getuna_bitmap_t
{
  HBITMAP hbitmap;
  u32_t width, height;
};

struct getuna_ss_t
{
  HWND window;
  HBITMAP bmp;
  
  // @todo: maybe store pixels too?
  u32_t pixels;
};

struct getuna_t
{
  arena_t arena;
  HWND main_window;
  HINSTANCE instance;
  b32_t is_running;

  HBITMAP debug_bitmap;
//  HBITMAP selection_screenshot;

  getuna_bitmap_t selection_screenshot;
  getuna_bitmap_t selection_overlay;



  u32_t ss_count;
  getuna_ss_t sss[32];
  

};
static getuna_t* getuna;


static inline LONG w32_rect_width(RECT r) { return r.right - r.left; }
static inline LONG w32_rect_height(RECT r) { return r.bottom - r.top; }



static void
getuna_spawn_ss_window()
{
  HDC screen_dc = GetDC(0);
  HDC temp_dc = CreateCompatibleDC(screen_dc);
  HBITMAP bmp = CreateCompatibleBitmap(screen_dc, 300,200);
  HBITMAP old_bmp = (HBITMAP)SelectObject(temp_dc, bmp);
  BitBlt(temp_dc, 0, 0, 300, 200, screen_dc, 100, 100, SRCCOPY);
  bmp = (HBITMAP)SelectObject(temp_dc, old_bmp);
  DeleteDC(temp_dc);
  ReleaseDC(0, screen_dc);

  // @todo: temporary code
  getuna->debug_bitmap = bmp;

  HWND window = CreateWindowEx(
      WS_EX_TOOLWINDOW,
      "Screenshot",                   // Class name
      0,
      WS_POPUP | WS_VISIBLE | WS_BORDER,     // Styles
      50, 100,                   // x, y position
      300, 200,                   // width, height
      getuna->main_window,                      // Parent window
      NULL,                      // No menu or control ID needed
      getuna->instance,
      NULL
      );

  if (!window)
  {
    // @todo: error |handling
  }

#if 0
  getuna_ss_t* ss = getuna->sss + getuna->ss_count;
  ss->window = window;
  ss->bmp = bmp;
#endif

}


static b32_t
getuna_spawn_selection_window()
{
  int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
  int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
  int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
  int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
#if 0


  HDC screen_dc = GetDC(0);
  HDC temp_dc = CreateCompatibleDC(screen_dc);
  HBITMAP bmp = CreateCompatibleBitmap(screen_dc, w, h);
  HBITMAP old_bmp = (HBITMAP)SelectObject(temp_dc, bmp);
  BitBlt(temp_dc, 0, 0, w, h, screen_dc, 0, 0, SRCCOPY);
  bmp = (HBITMAP)SelectObject(temp_dc, old_bmp);
  DeleteDC(temp_dc);
  ReleaseDC(0, screen_dc);
#endif
  HDC screen_dc = GetDC(0);
  HDC temp_dc = CreateCompatibleDC(screen_dc);
  HBITMAP bmp = CreateCompatibleBitmap(screen_dc, w,h);
  HBITMAP old_bmp = (HBITMAP)SelectObject(temp_dc, bmp);
  BitBlt(temp_dc, 0, 0, w, h, screen_dc, x, y, SRCCOPY);
  bmp = (HBITMAP)SelectObject(temp_dc, old_bmp);
  DeleteDC(temp_dc);
  ReleaseDC(0, screen_dc);

  getuna->selection_screenshot.hbitmap = bmp;
  getuna->selection_screenshot.width = w;
  getuna->selection_screenshot.height = h;

  HWND window = CreateWindowEx(
      WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
      "Selection",                 
      0,
      WS_POPUP | WS_VISIBLE | WS_BORDER,
      x, y,                    
      w, h,                   
      getuna->main_window,                    
      NULL,
      getuna->instance,
      NULL);

  if (!window)
  {
    // @todo: error handling
    return false;
  }

  return true;


}

LRESULT CALLBACK
w32_getuna_ss_window_callback(
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
      // tricks windows into thinking that the user clicked the title bar
      SendMessage(window, WM_NCLBUTTONDOWN, HTCAPTION, 0);
    } break;
    case WM_CLOSE:  
    case WM_QUIT:
    case WM_DESTROY: 
    {
      PostQuitMessage(0);
    } break;
    case WM_PAINT: 
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(window, &ps);

      // Draw the bitmap
      HDC temp_dc = CreateCompatibleDC(hdc);
      HBITMAP old_bmp = (HBITMAP)SelectObject(temp_dc, getuna->debug_bitmap);
      BITMAP bmp;
      GetObject(getuna->debug_bitmap, sizeof(BITMAP), &bmp);
      BitBlt(hdc, 0, 0, bmp.bmWidth, bmp.bmHeight, temp_dc, 0, 0, SRCCOPY);
      SelectObject(temp_dc, old_bmp);
      DeleteDC(temp_dc);

      EndPaint(window, &ps);
    } break;
    default: {
      result = DefWindowProcA(window, message, w_param, l_param);
    };   
  }
  return result;
}

LRESULT CALLBACK
w32_getuna_overlay_window_callback(
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
    case WM_CLOSE:  
    case WM_QUIT:
    case WM_DESTROY: 
    {
      PostQuitMessage(0);
    } break;
    case WM_KEYDOWN:
    {
      if (w_param == VK_ESCAPE) {
        PostQuitMessage(0);
      }
    } break;
    case WM_PAINT: 
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(window, &ps);


      getuna_bitmap_t* bmp = &getuna->selection_screenshot;
      {
        HDC temp_dc = CreateCompatibleDC(hdc);
        HBITMAP old_bmp = (HBITMAP)SelectObject(temp_dc, bmp->hbitmap);
        BitBlt(hdc, 0, 0, bmp->width, bmp->height, temp_dc, 0, 0, SRCCOPY);
        SelectObject(temp_dc, old_bmp);
        DeleteDC(temp_dc);
      }


      // dimming starts here
      {
        BITMAPINFO bi = { 0 };
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = bmp->width;
        bi.bmiHeader.biHeight = -bmp->height; // Negative = top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32; // <-- Important!
        bi.bmiHeader.biCompression = BI_RGB;

        void* bits = 0;
        HDC temp_dc = CreateCompatibleDC(hdc);
        HBITMAP dim_bmp = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS,&bits, 0, 0);
        HBITMAP old_bmp = (HBITMAP)SelectObject(temp_dc, dim_bmp);

        DWORD* pixels = (DWORD*)bits;
        for (int i = 0; i < bmp->width * bmp->height; ++i) {
          pixels[i] = 0x77000000; // AARRGGBB: semi-transparent black (0x77 alpha)
        }

        // Blend onto the screen
        BLENDFUNCTION blend = { AC_SRC_OVER, 0, 120, 0 }; // 120/255 alpha
        AlphaBlend(hdc, 0, 0, bmp->width, bmp->height, temp_dc, 0, 0, bmp->width, bmp->height, blend);
        //BitBlt(hdc, 0, 0, 300, 300, temp_dc, 0, 0, SRCCOPY);

        SelectObject(temp_dc, old_bmp);
        DeleteObject(dim_bmp);
        DeleteDC(temp_dc);
      }


      // TODO: draw the rect box
      EndPaint(window, &ps);

    } break;
    default: {
      result = DefWindowProcA(window, message, w_param, l_param);
    };   
  }
  return result;
}

LRESULT CALLBACK
w32_getuna_main_window_callback(
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
      getuna->is_running = false;
      PostQuitMessage(0);
    } break;
    case WM_COMMAND:
    {
      if (LOWORD(w_param) == GETUNA_SS_BUTTON_COMMAND_ID )
      {
        //getuna_spawn_ss_window();
        getuna_spawn_selection_window();
      }
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
  getuna = arena_alloc_bootstrap(getuna_t, arena, gigabytes(1)); 
  getuna->instance = instance;

  // create window
  {
    const char* title = "get tuna";
    const char* icon_path = "window.ico";
    const s32_t icon_w = 256;
    const s32_t icon_h = 256;

    WNDCLASSA w32_class = {};
    w32_class.style = CS_HREDRAW | CS_VREDRAW;
    w32_class.lpfnWndProc = w32_getuna_main_window_callback;
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

    // for screenshot window
    WNDCLASS ss_class = {};
    //w32_class.style = CS_HREDRAW | CS_VREDRAW;
    ss_class.lpfnWndProc = w32_getuna_ss_window_callback;
    ss_class.hInstance = instance;
    ss_class.lpszClassName = "Screenshot";

    if(!RegisterClass(&ss_class)) {
      return 1;
    }

    // for overlay window
    WNDCLASS selection_class = {};
    //w32_class.style = CS_HREDRAW | CS_VREDRAW;
    selection_class.lpfnWndProc = w32_getuna_overlay_window_callback;
    selection_class.hInstance = instance;
    selection_class.lpszClassName = "Selection";
    
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
      
      window_rect.left = monitor_w/2 - GETUNA_WINDOW_WIDTH/2;
      window_rect.right = monitor_w/2 + GETUNA_WINDOW_WIDTH/2;
      window_rect.top = monitor_h/2 - GETUNA_WINDOW_HEIGHT/2;
      window_rect.bottom = monitor_h/2 + GETUNA_WINDOW_HEIGHT/2;

    }
    
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU;

    // disable maximize and resizing
    style  &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
    
    AdjustWindowRectEx(&window_rect,
                       style,
                       FALSE,
                       0);    
    
    getuna->main_window = CreateWindowExA(0,
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

    if (!getuna->main_window) 
    {
      return 1;
    }


    // Create the push button
    CreateWindow(
        "BUTTON",               // Predefined class; Unicode use L"BUTTON"
        GETUNA_SS_BUTTON_TEXT,             // Button text
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
        GETUNA_SS_BUTTON_X,                     
        GETUNA_SS_BUTTON_Y,                     
        GETUNA_SS_BUTTON_W,                    
        GETUNA_SS_BUTTON_H,                     
        getuna->main_window,                 
        (HMENU)GETUNA_SS_BUTTON_COMMAND_ID,               
        0,              // Instance handle
        NULL            // Additional application data
        );
  }

  getuna->is_running = true;
  while(getuna->is_running)
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
