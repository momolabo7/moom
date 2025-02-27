
struct eden_console_command_t {
  buf_t key;
  void* ctx;
  void (*func)(void*);
};

struct eden_console_t {
  u32_t command_cap;
  u32_t command_count;
  eden_console_command_t* commands;
  
  bufio_t* info_lines; 
  u32_t info_line_count;

  bufio_t input_line;

};
