
#ifndef MOE_CONSOLE_H
#define MOE_CONSOLE_H


struct moe_console_command_t {
  str8_t key;
  void* ctx;
  void (*func)(void*);
};

struct moe_console_line_t {
  u8_t buffer[256];
  str8_t str;
};

struct moe_console_t {
  u32_t command_count;
  moe_console_command_t commands[10];
  
  sb8_t info_lines[9];
  sb8_t input_line;
};


#endif //MOE_CONSOLE_H
