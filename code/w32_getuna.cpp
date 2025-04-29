#include "momo.h"
#include <stdio.h>

#define GETUNA_WINDOW_WIDTH (200)
#define GETUNA_WINDOW_HEIGHT (30)

#define GETUNA_SS_BUTTON_COMMAND_ID (1)
#define GETUNA_SS_BUTTON_TEXT       "get tuna"
#define GETUNA_SS_BUTTON_X          (0)
#define GETUNA_SS_BUTTON_Y          (0)
#define GETUNA_SS_BUTTON_W          (200)
#define GETUNA_SS_BUTTON_H          (30)

#define GETUNA_WM_SS (WM_USER+1)

enum {
  GETUNA_SS_CONTEXT_EXIT,
  GETUNA_SS_CONTEXT_ALPHA_100,
  GETUNA_SS_CONTEXT_ALPHA_75,
  GETUNA_SS_CONTEXT_ALPHA_50,
};


struct getuna_t
{
  arena_t arena;
  HWND main_window;
  HINSTANCE instance;
  b32_t is_running;
  
  //
  // Selection Overlay Drawing 
  // 
  b32_t is_dragging;

  // @note: In client coordinates
  POINT drag_start;
  POINT drag_end;
  POINT drag_end_prev; // previous frame's drag_end

  // @note: In virtual monitor coordinates
  POINT drag_start_raw;
  POINT drag_end_raw;

  //@note: this is for reference; the original screenshot of the whole desktop
  w32_dib_t selection_screenshot; 
  
  // @note: this is used to present to the screen as a complete frame
  w32_dib_t selection_frame; 


};
static getuna_t* getuna;

static void
getuna_spawn_ss_window(LONG x, LONG y, LONG w, LONG h)
{
  HDC screen_dc = GetDC(0);
  HDC temp_dc = CreateCompatibleDC(screen_dc);
  HBITMAP bmp = CreateCompatibleBitmap(screen_dc, w, h);
  HBITMAP old_bmp = (HBITMAP)SelectObject(temp_dc, bmp);
  BitBlt(temp_dc, 0, 0, w, h, screen_dc, x, y, SRCCOPY);
  bmp = (HBITMAP)SelectObject(temp_dc, old_bmp);
  DeleteDC(temp_dc);
  ReleaseDC(0, screen_dc);

  // @todo: temporary code
  // getuna->debug_bitmap = bmp;

  HWND window = CreateWindowEx(
      WS_EX_TOOLWINDOW | WS_EX_LAYERED,
      "Screenshot",                   // Class name
      0,
      WS_POPUP | WS_VISIBLE | WS_BORDER,     // Styles
      x, y,                   // x, y position
      w, h,                   // width, height
      getuna->main_window,                      // Parent window
      NULL,                      // No menu or control ID needed
      getuna->instance,
      NULL
      );

  //
  // @note: bro is storing the whole HBITMAP into the window user data pointer thingy.
  // Ideally we want allocate a struct and have the long ptr point to that struct but
  // atm I'm too lazy to deal with memory allocation implications and here :)
  //
  SetWindowLongPtr(window, GWLP_USERDATA, (LONG_PTR)bmp);
  if (!window)
  {
    // @todo: error handling
  }

  //ShowWindow(window);
  SetLayeredWindowAttributes(window, 0, 255, LWA_ALPHA);
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

  w32_dib_t* ss_dib = &getuna->selection_screenshot;
  w32_dib_t* frame_dib = &getuna->selection_frame;
  w32_dib_init(ss_dib, w, h);
  w32_dib_init(frame_dib, w, h);

  HDC screen_dc = GetDC(0);
  w32_dib_blit_from_dc(ss_dib, screen_dc, x, y);
  w32_dib_blit_from_dc(frame_dib, screen_dc, x, y);
  ReleaseDC(0, screen_dc);

  getuna->drag_start = getuna->drag_end = getuna->drag_end_prev = {0};

  for (int i = 0; i < w * h; ++i) {
    u32_t* pixel = w32_dib_pixel_index(frame_dib, i); 
    pixel[0] = (pixel[0] & 0xFF000000) | ((pixel[0] & 0x00FEFEFE) >> 1);
  }

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
    case WM_CONTEXTMENU:
    {
      HMENU menu = CreatePopupMenu();
      int x = GET_X_LPARAM(l_param);
      int y = GET_Y_LPARAM(l_param);
      if (menu)
      {
        AppendMenu(menu, MF_STRING, GETUNA_SS_CONTEXT_ALPHA_100, TEXT("100%"));
        AppendMenu(menu, MF_STRING, GETUNA_SS_CONTEXT_ALPHA_75, TEXT("75%"));
        AppendMenu(menu, MF_STRING, GETUNA_SS_CONTEXT_ALPHA_50, TEXT("50%"));
        AppendMenu(menu, MF_STRING, GETUNA_SS_CONTEXT_EXIT, TEXT("Exit"));

        // @note: this is a blocking call
        TrackPopupMenuEx(
            menu,
            TPM_LEFTALIGN | TPM_TOPALIGN | TPM_RIGHTBUTTON,
            x, y, window, NULL);

        DestroyMenu(menu);
      }
    } break;
    case WM_COMMAND:
    {
      int id = LOWORD(w_param);
      if (id == GETUNA_SS_CONTEXT_EXIT) 
      {
        // @todo: free bitmap resources
        HBITMAP ss_bitmap = (HBITMAP)GetWindowLongPtr(window, GWLP_USERDATA);
        DeleteObject(ss_bitmap);
        DestroyWindow(window); 
      }
      else if (id == GETUNA_SS_CONTEXT_ALPHA_100)
      {
        SetLayeredWindowAttributes(window, 0, 255, LWA_ALPHA);
      }
      else if (id == GETUNA_SS_CONTEXT_ALPHA_75)
      {
        SetLayeredWindowAttributes(window, 0, 255/4*3, LWA_ALPHA);
      }
      else if (id == GETUNA_SS_CONTEXT_ALPHA_50)
      {
        SetLayeredWindowAttributes(window, 0, 255 >> 1, LWA_ALPHA);
      }
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
      HBITMAP ss_bitmap = (HBITMAP)GetWindowLongPtr(window, GWLP_USERDATA);
      HDC temp_dc = CreateCompatibleDC(hdc);
      HBITMAP old_bmp = (HBITMAP)SelectObject(temp_dc, ss_bitmap);
      BITMAP bmp;
      GetObject(ss_bitmap, sizeof(BITMAP), &bmp);
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

static u32_t
w32_getuna_darken_pixel(u32_t pixel)
{
  return (pixel & 0xFF000000) | ((pixel & 0x00FEFEFE) >> 1);
}

LRESULT CALLBACK
w32_getuna_selection_window_callback(
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
      getuna->is_dragging = true;
      GetCursorPos(&getuna->drag_start_raw);
      getuna->drag_start = getuna->drag_start_raw;
      ScreenToClient(window, &getuna->drag_start);
    } break;
    case WM_LBUTTONUP:
    {
      getuna->is_dragging = false;
      PostMessage(window, WM_CLOSE, 0, 0);
      PostMessage(getuna->main_window, GETUNA_WM_SS, 0, 0);
    } break;
    case WM_MOUSEMOVE:
    {
      if (getuna->is_dragging)
      {
        getuna->drag_end_prev = getuna->drag_end;

        GetCursorPos(&getuna->drag_end_raw);
        getuna->drag_end = getuna->drag_end_raw;
        ScreenToClient(window, &getuna->drag_end);
        InvalidateRect(window, NULL, TRUE);
      }
    } break;
    case WM_CLOSE:  
    case WM_QUIT:
    case WM_DESTROY: 
    {
      w32_dib_free(&getuna->selection_frame);
      w32_dib_free(&getuna->selection_screenshot);
      DestroyWindow(window);
    } break;
    case WM_KEYDOWN:
    {
      if (w_param == VK_ESCAPE) 
      {
        PostMessage(window, WM_CLOSE, 0, 0);
      }
    } break;
    case WM_PAINT: 
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(window, &ps);

      w32_dib_t* frame_dib = &getuna->selection_frame;
      w32_dib_t* ss_dib = &getuna->selection_screenshot;

      // Restore border
      {
        LONG min_x, max_x, min_y, max_y;
        minmax_of(getuna->drag_start.x, getuna->drag_end_prev.x, min_x, max_x); 
        minmax_of(getuna->drag_start.y, getuna->drag_end_prev.y, min_y, max_y); 

        for (LONG y = min_y; y <= max_y; ++y)
        {
          dref(w32_dib_pixel_xy(frame_dib, min_x, y)) = w32_getuna_darken_pixel(dref(w32_dib_pixel_xy(ss_dib, min_x, y)));
          dref(w32_dib_pixel_xy(frame_dib, max_x, y)) = w32_getuna_darken_pixel(dref(w32_dib_pixel_xy(ss_dib, max_x, y)));
        }

        for (LONG x = min_x; x <= max_x; ++x)
        {
          dref(w32_dib_pixel_xy(frame_dib, x, min_y)) = w32_getuna_darken_pixel(dref(w32_dib_pixel_xy(ss_dib, x, min_y)));
          dref(w32_dib_pixel_xy(frame_dib, x, max_y)) = w32_getuna_darken_pixel(dref(w32_dib_pixel_xy(ss_dib, x, max_y)));
        }
      }

      // Draw border
      {
        LONG min_x, max_x, min_y, max_y;
        minmax_of(getuna->drag_start.x, getuna->drag_end.x, min_x, max_x); 
        minmax_of(getuna->drag_start.y, getuna->drag_end.y, min_y, max_y); 

        u32_t fill_color = 0xffff0000;
        for (LONG y = min_y; y <= max_y; ++y)
        {
            dref(w32_dib_pixel_xy(frame_dib, min_x, y)) = fill_color;
            dref(w32_dib_pixel_xy(frame_dib, max_x, y)) = fill_color;
        }
        for (LONG x = min_x; x <= max_x; ++x)
        {
            dref(w32_dib_pixel_xy(frame_dib, x, min_y)) = fill_color;
            dref(w32_dib_pixel_xy(frame_dib, x, max_y)) = fill_color;
        }
#if 0
        // "whiten" area within border
        for (LONG y = min_y+1; y <= max_y-1; ++y)
        {
          for (LONG x = min_x+1; x <= max_x-1; ++x)
          {
            dref(w32_dib_pixel_xy(frame_dib, x, y)) = dref(w32_dib_pixel_xy(ss_dib, x, y));
          }
        }
#endif
      }
      w32_dib_blit_to_dc(frame_dib, hdc);
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
        ShowWindow(getuna->main_window, SW_HIDE);
        getuna_spawn_selection_window();

      }
    } break;
    case GETUNA_WM_SS:
    {
      LONG x0, y0, x1, y1;
      minmax_of(getuna->drag_start_raw.x, getuna->drag_end_raw.x, x0, x1);
      minmax_of(getuna->drag_start_raw.y, getuna->drag_end_raw.y, y0, y1);

      getuna_spawn_ss_window(x0, y0, x1-x0, y1-y0);
      ShowWindow(getuna->main_window, SW_SHOW);
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
  getuna = arena_alloc_bootstrap_zero(getuna_t, arena, gigabytes(1)); 
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
    selection_class.lpfnWndProc = w32_getuna_selection_window_callback;
    selection_class.hInstance = instance;
    selection_class.lpszClassName = "Selection";
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
