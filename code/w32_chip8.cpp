#include "momo.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
#define CHIP8_DISPLAY_WIDTH  (64)
#define CHIP8_DISPLAY_HEIGHT (32)
#define CHIP8_WINDOW_WIDTH   (400)
#define CHIP8_WINDOW_HEIGHT  (400) 
#define CHIP8_FRAME_RATE     (60)

struct chip8_t
{
  u8_t ram[4096];
  u32_t ram_usage;

  u8_t display[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT];

  u16_t program_counter;

  u16_t stack_pointer; // used to point at locations in memory
  u16_t stack[256];

  u8_t delay_timer;
  u8_t sound_timer; 

  u8_t registers[16];
};

static b32_t
chip8_init(chip8_t* chip8, buffer_t instructions)
{
  if (instructions.size <= sizeof(chip8->ram))
  {
    memory_copy(chip8->ram, instructions.e, instructions.size);
    chip8->ram_usage = instructions.size;
    return true;
  }

  return false;
}

static b32_t
chip8_init_from_file(chip8_t* chip8, const char* filename, arena_t* arena)
{
  arena_set_revert_point(arena);
  buffer_t instructions = file_read_into_buffer("test.ch8", arena);
  return chip8_init(chip8, instructions);
}



static void
chip8_update(chip8_t* chip8)
{
  // Fetch
  u16_t instruction;
  {
    // Read the instruction that PC is current pointing at from
    // memory. An instruction is 2 bytes.
    instruction = dref((u16_t*)(chip8->ram + chip8->program_counter));
    chip8->program_counter += 2;
  }
  
  // Decode + Execute
  u8_t nibbles[4];
  nibbles[0] = (instruction & 0xF000) >> 24;
  nibbles[1] = (instruction & 0x0F00) >> 16;
  nibbles[2] = (instruction & 0x00F0) >> 8;
  nibbles[3] = (instruction & 0x000F);


  switch(instruction & 0xF000)
  {
    case 0x0000: {
      switch(instruction & 0x0FFF)
      {
        case 0x00E0: {
          // 00E0: clear screen
          memory_zero(chip8->display, sizeof(chip8->display));
        } break;
        case 0x0EE: {
          // 00E0: call subroutine
          // @todo
        } break;
      }
    } break;
    case 0x1000: {
      // 1NNN: jump
      // @note: set PC to NNN
      chip8->program_counter = (instruction & 0x0FFF);
    } break;
    case 0x2000: {
      // 2NNN: call
      chip8->stack[chip8->stack_pointer++] = chip8->program_counter;
      chip8->program_counter = (instruction & 0x0FFF);
      
    }; 


  }


}

static void
chip8_exit(chip8_t* chip8)
{
}



// @note: this is basicall a bitmap
struct w32_dib_t 
{
  BITMAPINFO bitmap_info;
  HBITMAP bitmap;

  u32_t width, height;
  u32_t buffer_size;

  u32_t* pixel_buffer;
};

struct w32_frc_t
{
  f32_t target_secs_per_frame;
  u32_t target_frame_rate;
  u64_t last_frame_count;
  b32_t is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  u64_t performance_frequency;
};




static void 
w32_frc_init(w32_frc_t* frc, u32_t target_frame_rate)
{
  frc->target_frame_rate = target_frame_rate;
  frc->target_secs_per_frame = 1.f/target_frame_rate;
  frc->is_sleep_granular = timeBeginPeriod(1) == TIMERR_NOERROR;
  frc->performance_frequency = clock_resolution();
}

static void 
w32_frc_begin(w32_frc_t* frc)
{
  frc->last_frame_count = clock_time();
}

static void 
w32_frc_end(w32_frc_t* frc)
{
  // Sleep
  f32_t secs_elapsed_after_update = (clock_time() - frc->last_frame_count)/frc->performance_frequency;

  if (frc->target_secs_per_frame > secs_elapsed_after_update)
  {
    if (frc->is_sleep_granular)
    {

      u32_t ms_to_sleep = (u32_t)(1000 * (frc->target_secs_per_frame - secs_elapsed_after_update));
      if (ms_to_sleep > 0)
      {
        doze(ms_to_sleep);
      }
    }
  }

  // spin lock the remaining time
  f32_t secs_elapsed_after_sleep = (clock_time() - frc->last_frame_count)/frc->performance_frequency; 
  if (frc->target_secs_per_frame > secs_elapsed_after_sleep)
  {
    secs_elapsed_after_sleep = (clock_time() - frc->last_frame_count)/frc->performance_frequency;
  }

  frc->last_frame_count = clock_time();
}

//
// globals
//
// @todo: clean 
//
b32_t g_is_running = true;
w32_dib_t g_frame_dib;

static inline LONG w32_rect_width(RECT r) { return r.right - r.left; }
static inline LONG w32_rect_height(RECT r) { return r.bottom - r.top; }

static void
w32_dib_free(w32_dib_t* dib)
{
  if (dib->bitmap)
  {
    DeleteObject(dib->bitmap);
  }
}

static void 
w32_dib_blit_to_dc(w32_dib_t* dib, HDC dc) 
{
  if (dib->bitmap)
  {
    HDC temp_dc = CreateCompatibleDC(0);
    HGDIOBJ old_bitmap = SelectObject(temp_dc, dib->bitmap);
    BitBlt(
        dc, 
        0, 0, 
        dib->width, dib->height,
        temp_dc,
        0, 0,
        SRCCOPY);
    SelectObject(temp_dc, old_bitmap);
    DeleteDC(temp_dc);
  }
}

static void
w32_dib_set_pixel(w32_dib_t* dib, u32_t x, u32_t y, u32_t a, u32_t r, u32_t g, u32_t b) 
{
  assert(x < dib->width);
  assert(y < dib->height);
  u32_t color = (a << 24) + (r << 16) + (g << 8) + (b << 0);
  dib->pixel_buffer[x + y * dib->height] = color;
}

static b32_t
w32_dib_init(w32_dib_t* dib, u32_t width, u32_t height, HWND window) 
{
  assert(width);
  assert(height);
  assert(window);

  dib->bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  dib->bitmap_info.bmiHeader.biPlanes = 1;
  dib->bitmap_info.bmiHeader.biBitCount = 32;
  dib->bitmap_info.bmiHeader.biCompression = BI_RGB;
  dib->bitmap_info.bmiHeader.biSizeImage = 0;
  dib->bitmap_info.bmiHeader.biClrUsed = 0;
  dib->bitmap_info.bmiHeader.biClrImportant = 0;
  dib->bitmap_info.bmiHeader.biWidth = width;
  dib->bitmap_info.bmiHeader.biHeight = height;

  dib->width = width;
  dib->height = height;


  HDC hdc = GetDC(window);
  defer { ReleaseDC(0, hdc); };
  dib->bitmap = CreateDIBSection(
      hdc, 
      &dib->bitmap_info, 
      DIB_RGB_COLORS, 
      (void**)&dib->pixel_buffer,
      0,
      0);
  if (!dib->bitmap)
    return false;

  return true;
}


LRESULT CALLBACK
w32_window_callback(HWND window, 
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
    case WM_PAINT: 
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(window, &ps);
      w32_dib_blit_to_dc(&g_frame_dib, hdc);
      EndPaint(window, &ps);
      
    } break;
    case WM_CLOSE:  
    case WM_QUIT:
    case WM_DESTROY: 
    {
      g_is_running = false;
      PostQuitMessage(0);
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
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  // create window
  HWND window;
  {
    const char* title = "chip8 emulator";
    const char* icon_path = "window.ico";
    const s32_t icon_w = 256;
    const s32_t icon_h = 256;

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
      
      window_rect.left = monitor_w/2 - CHIP8_WINDOW_WIDTH/2;
      window_rect.right = monitor_w/2 + CHIP8_WINDOW_WIDTH/2;
      window_rect.top = monitor_h/2 - CHIP8_WINDOW_HEIGHT/2;
      window_rect.bottom = monitor_h/2 + CHIP8_WINDOW_HEIGHT/2;
    }
    
    DWORD style = WS_OVERLAPPED | WS_VISIBLE | WS_SYSMENU;
    
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
  
  arena_t arena = {};
  arena_alloc(&arena, gigabytes(1), false); 
  defer { arena_free(&arena); };

  w32_dib_init(&g_frame_dib, CHIP8_WINDOW_WIDTH, CHIP8_WINDOW_HEIGHT, window);
  defer { w32_dib_free(&g_frame_dib); };  

  chip8_t* chip8 = arena_push(chip8_t, &arena);
  {
    //chip8_init_from_file()
  }


  make(w32_frc_t, frc);
  w32_frc_init(frc, CHIP8_FRAME_RATE);

  while(g_is_running)
  {
    w32_frc_begin(frc);

    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
   
    {
      // @todo audio
    }

    // rendering
    {
      for (s32_t x = 0; x < g_frame_dib.width; ++x) 
      {
        for(s32_t y = 0; y < g_frame_dib.height; ++y)
        {
          w32_dib_set_pixel(&g_frame_dib, x, y, 255, 255, 255, 0);
        }
      }

      // @note: tells window that the screen is outdated and requires repainting
      InvalidateRect(window, NULL, FALSE);

      // @note: generally speaking, this fires the WM_PAINT message, where we 
      // will blit the DIB representing our frame buffer onto.
      UpdateWindow(window);
    }


    w32_frc_end(frc);
    

  }
  

  return 0;

}
