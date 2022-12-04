
#ifndef MOE_CONSOLE_H
#define MOE_CONSOLE_H


struct Console_Command {
  String8 key;
  void* ctx;
  void (*func)(void*);
};

struct Console_Line {
  U8 buffer[256];
  String8 str;
};

struct Console {
  U32 command_count;
  Console_Command commands[10];
  
  String8_Builder info_lines[9];
  String8_Builder input_line;
};


#endif //MOE_CONSOLE_H
