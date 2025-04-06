#include "momo.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")

// https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
#define CHIP8_DISPLAY_WIDTH  (64)
#define CHIP8_DISPLAY_HEIGHT (32)
#define CHIP8_WINDOW_SCALE   (10)
#define CHIP8_WINDOW_WIDTH   (CHIP8_DISPLAY_WIDTH*CHIP8_WINDOW_SCALE)
#define CHIP8_WINDOW_HEIGHT  (CHIP8_DISPLAY_HEIGHT*CHIP8_WINDOW_SCALE) 
#define CHIP8_FRAME_RATE     (60)

struct chip8_t
{
  u8_t memory[4096];
  u32_t memory_usage;

  u8_t display[CHIP8_DISPLAY_WIDTH * CHIP8_DISPLAY_HEIGHT];

  u16_t program_counter;

  u16_t index_register;

  u8_t stack_pointer;
  u16_t stack[256];

  u8_t delay_timer;
  u8_t sound_timer; 

  u8_t registers[16];
};

static b32_t
chip8_init(chip8_t* chip8, buf_t instructions)
{
  if (instructions.size <= sizeof(chip8->memory) - 0x200)
  {
    memory_copy(chip8->memory + 0x200, instructions.e, instructions.size);
    chip8->memory_usage = instructions.size;
    chip8->program_counter = 0x200;
    return true;
  }

  return false;
}

static b32_t
chip8_init_from_file(chip8_t* chip8, const char* filename, arena_t* arena)
{
  arena_set_revert_point(arena);
  buf_t instructions = file_read_into_buffer(filename, arena);
  return chip8_init(chip8, instructions);
}



static void
chip8_update(chip8_t* chip8)
{
  // Fetch
  //
  // Read the instruction that PC is current pointing at from
  // memory. An instruction is 2 bytes.
  u16_t instruction = dref((u16_t*)(chip8->memory + chip8->program_counter));
  chip8->program_counter += 2;
  instruction = u16_endian_swap(instruction);

  
  // Decode + Execute
  u8_t type = (instruction & 0xF000) >> 12;
  switch(type) 
  {
    case 0x0:
    {
      if (instruction == 0x0E0) 
      {
        memory_zero(chip8->display, sizeof(chip8->display));
      }
      else if (instruction == 0x0EE)
      {
        // @todo
      }
    }break;
    case 0x1:
    {
      // 1NNN: jump PC to NNN
      chip8->program_counter = (instruction & 0x0FFF);
    }break;
    case 0x2:
    {
    }break;
    case 0x3:
    {
    }break;
    case 0x4:
    {
    }break;
    case 0x5:
    {
    }break;
    case 0x6:
    {
      // 6XNN: set register X to NN
      u8_t x = (instruction & 0x0F00) >> 8; 
      chip8->registers[x] = (instruction & 0x00FF);
    }break;
    case 0x7:
    {
      // 7XNN: add NN to register X
      u8_t x = (instruction & 0x0F00) >> 8; 
      chip8->registers[x] += (instruction & 0x00FF);
    }break;
    case 0x8:
    {
    }break;
    case 0x9:
    {
    }break;
    case 0xA:
    {
      // ANNN: set index register to NNN
      chip8->index_register = (instruction & 0x0FFF);
    }break;
    case 0xB:
    {
    }break;
    case 0xC:
    {
    }break;
    case 0xD:
    {
      // DXYN: draw 
      u8_t x = (instruction & 0x0F00) >> 8; 
      u8_t y = (instruction & 0x00F0) >> 4; 
      u8_t n = instruction & 0x000F; 

      chip8->registers[0xF] = 0;
      v2u_t coords = v2u_set(
          chip8->registers[x] % CHIP8_DISPLAY_WIDTH, 
          chip8->registers[y] % CHIP8_DISPLAY_HEIGHT);
      // set VF to zero?
      for (u8_t r = 0; r < n; ++r)
      {
        if ((coords.y + r) >= CHIP8_DISPLAY_HEIGHT)
          break;

        u8_t sprite_byte = chip8->memory[chip8->index_register + r];
        // sprites are always 8-pixels wide
        for (u8_t c = 0; c < 8; ++c)
        {
          if ((coords.x + c) >= CHIP8_DISPLAY_WIDTH)
            break;

          u8_t sprite_pixel = (sprite_byte >> (7 - c)) & 0x1;
          u8_t* screen_pixel = &chip8->display[(coords.y + r) * CHIP8_DISPLAY_WIDTH + (coords.x + c)];
          if (sprite_pixel)
          {
            if (*screen_pixel == 1)
            {
              chip8->registers[0xF] = 1;
            }

            *screen_pixel ^= 1;
          }
        }
      }
      
    }break;
    case 0xE:
    {
    }break;
    case 0xF:
    {
    }break;

  }

#if 0
  switch(instruction & 0xF000)
  {
    case 0x0000: {
      switch(instruction & 0x0FFF)
      {
        case 0x00E0: {
          // 00E0: clear screen
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
#endif


}

static void
chip8_exit(chip8_t* chip8)
{
}




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
    
    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU;

    // disable maximize and resizing
    style  &= ~(WS_MAXIMIZEBOX | WS_SIZEBOX);
    
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

  w32_dib_init(&g_frame_dib, CHIP8_WINDOW_WIDTH, CHIP8_WINDOW_HEIGHT);
  defer { w32_dib_free(&g_frame_dib); };  

  chip8_t* chip8 = arena_push(chip8_t, &arena);
  if (!chip8_init_from_file(chip8, "ibm.ch8", &arena))
  {
    return false;
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

    chip8_update(chip8);

    // rendering
    for(s32_t y = 0; y < CHIP8_DISPLAY_HEIGHT; ++y)
    {
      for (s32_t x = 0; x < CHIP8_DISPLAY_WIDTH; ++x) 
      {
        u8_t pixel = chip8->display[y * CHIP8_DISPLAY_WIDTH + x];

        s32_t start_x = x * CHIP8_WINDOW_SCALE;
        s32_t start_y = y * CHIP8_WINDOW_SCALE;
        s32_t end_x = start_x + CHIP8_WINDOW_SCALE;
        s32_t end_y = start_y + CHIP8_WINDOW_SCALE;

        for (s32_t dy = start_y; dy < end_y; ++dy)
        {
          for (s32_t dx = start_x; dx < end_x; ++dx)
          {
            if (pixel)
            {
              w32_dib_pixel_xy(&g_frame_dib, dx, dy)[0] = 0xffffffff;
            }
            else
            {
              w32_dib_pixel_xy(&g_frame_dib, dx, dy)[0] = 0;
            }
          }
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
