
//~DLL loading
struct Win_Loaded_Code {
  // Need to fill these up
  U32 function_count;
  const char** function_names;
  const char* module_path;
  void** functions;
  
#if INTERNAL
  LARGE_INTEGER module_write_time;
  const char* tmp_path;
#endif  
  
  B32 is_valid;
  HMODULE dll; 
};

static void
win_unload_code(Win_Loaded_Code* code) {
  if(code->dll) {
    FreeLibrary(code->dll);
    code->dll = 0;
  }
  code->is_valid = false;
  zero_range(code->functions, code->function_count);
}

static void
win_load_code(Win_Loaded_Code* code) {
  code->is_valid = false;
  
#if INTERNAL
  B32 copy_success = false;
  for (U32 attempt = 0; attempt < 100; ++attempt) {
    if(CopyFile(code->module_path, code->tmp_path, false)) {
      copy_success = true;
      break;
    }
    Sleep(100);
  }
  code->dll = LoadLibraryA(code->tmp_path);
#else //INTERNAL
  code->dll = LoadLibraryA(code->module_path);
#endif //INTERNAL
  
  
  
  if (code->dll) {
    code->is_valid = true;
    for (U32 function_index = 0; 
         function_index < code->function_count; 
         ++function_index) 
    {
      void* function = GetProcAddress(code->dll, code->function_names[function_index]);
      if (!function) {
        code->is_valid = false;
        break;
      }
      code->functions[function_index] = function;
    }
    
  }
  
  if(!code->is_valid) {
    win_unload_code(code);
  }
  
  
  
}

#if INTERNAL
static B32
win_reload_code_if_outdated(Win_Loaded_Code* code) {
  B32 reloaded = false;
  // Check last modified date
  LARGE_INTEGER last_write_time = win_get_file_last_write_time(code->module_path);
  if(last_write_time.QuadPart > code->module_write_time.QuadPart) { 
    win_unload_code(code); 
    for (U32 i = 0; i < 100; ++i ){
      win_load_code(code);
      if (code->is_valid) {
        win_log("[%s] reloaded successfully\n", code->module_path);
        code->module_write_time = win_get_file_last_write_time(code->module_path);
        reloaded = true;
        break;
      }
      Sleep(100);
    }
  }
  
  
  return reloaded;
}

#endif


