
#ifndef MOMO_CONSOLE_H
#define MOMO_CONSOLE_H


struct console_command_t {
  str_t key;
  void* ctx;
  void (*func)(void*);
};

struct console_t {
  u32_t command_cap;
  u32_t command_count;
  console_command_t* commands;
  
  stb8_t info_lines[9]; // @todo: this should be allocated as well
  stb8_t input_line;
};

static void
console_init(console_t* console, arena_t* allocator, u32_t characters_per_line, u32_t max_commands) 
{
  console->command_count = 0;
  console->command_cap = max_commands;
  console->commands = arena_push_arr(console_command_t, allocator, max_commands);

  u32_t line_size = characters_per_line;
  stb8_init(&console->input_line,
           arena_push_arr(u8_t, allocator, line_size),
           line_size);
  
  for (u32_t info_line_index = 0;
       info_line_index < array_count(console->info_lines);
       ++info_line_index) 
  {    
    stb8_t* info_line = console->info_lines + info_line_index;
    stb8_init(info_line,
             arena_push_arr(u8_t, allocator, line_size),
             line_size);
  }
}

static void
console_add_command(console_t* console, str_t key, void* ctx, void(*func)(void*)) 
{
  // simulate adding commands
  assert(console->command_count < console->command_cap);
  console_command_t* cmd = console->commands + console->command_count++;
  cmd->key = key;
  cmd->ctx = ctx;
  cmd->func = func;
}

static void
console_push_info(console_t* console, str_t str) {
  // @note: There's probably a better to do with via some
  // crazy indexing scheme, but this is debug so we don't care for now
  
  // Copy everything from i + 1 from i
  for (u32_t i = 0; 
       i < array_count(console->info_lines) - 1;
       ++i)
  {
    u32_t line_index = array_count(console->info_lines) - 1 - i;
    stb8_t* line_to = console->info_lines + line_index;
    stb8_t* line_from = console->info_lines + line_index - 1;
    stb8_clear(line_to);
    stb8_push_str(line_to, line_from->str);
  } 
  stb8_clear(console->info_lines + 0);
  stb8_push_str(console->info_lines + 0, str);
}

static void
console_execute(console_t* console) {
  for(u32_t command_index = 0; 
      command_index < console->command_count; 
      ++command_index) 
  {
    console_command_t* cmd = console->commands + command_index;
    if (str_match(cmd->key, console->input_line.str)) {
      cmd->func(cmd->ctx);
    }
  }
  
  console_push_info(console, console->input_line.str);
  stb8_clear(&console->input_line);
}


#endif //MOMO_CONSOLE_H
