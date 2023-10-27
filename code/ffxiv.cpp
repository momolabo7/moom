#include "momo.h"

//
// MARK:(CSV)
//
// CSV assumes perfectly squared CSV.
//

struct csv_t {
  buffer_t buffer;

  usz_t rows;
  usz_t columns;

  st8_t* tokens;
};

static void print_token(st8_t str) {
  for(usz_t i = 0; i < str.count; ++i) {
    printf("%c", str.e[i]);
  }
  printf("\n");
}

struct st8_t 
csv_get_cell(csv_t* csv, usz_t row, usz_t col) {
  if (row < csv->rows && col < csv->columns) {
    return csv->tokens[col + (row * csv->columns)];
  }
  return {};
}

static b32_t
csv_read(csv_t* csv, buffer_t buffer, arena_t* arena) {
  csv->buffer = buffer;
  
  // count cols
  for(u8_t* itr = buffer.data; ; ++itr) {
    if (itr[0] == '\n') 
      break;
    if (itr[0] == ',')
      csv->columns++;
      
  }
  
  // count rows
  for(usz_t i = 0; i < buffer.size; ++i) {
    if (buffer.data[i] == '\n')
      csv->rows++;
  }


  usz_t cell_count = csv->rows * csv->columns; 
  csv->tokens = arena_push_arr(st8_t, arena, cell_count); 
  
  if(!csv->tokens) {
    return false;
  }

  usz_t at = 0;
  for_cnt(cell_index, cell_count)
  {
    st8_t token = {};
    token.e = buffer.data + at;
    while(buffer.data[at] != ',' && 
          buffer.data[at] != '\n' &&
          buffer.data[at] != 0) 
    {
      ++at;
      ++token.count;
    }
    csv->tokens[cell_index] = token;

    // End of file
    if (buffer.data[at] == 0)  {
      break;
    }

    ++at; 
  }
  return true;
}

//
// data is a double array of strings
//
static buffer_t
csv_write(st8_t* data , usz_t rows, usz_t columns, arena_t* arena) {
  // Figure out the buffer size
  usz_t required_size = 0;
  usz_t cell_count = rows * columns;
  for_cnt(cell_index, cell_count) {
    st8_t str = data[cell_index];
    required_size += str.count;
  }

  // Each entry either comes with '\n' or ','
  required_size += cell_count; 
  buffer_t ret = arena_push_buffer(arena, required_size, 16);
  if (!ret) {
    return ret;
  }

  usz_t buffer_at = 0;
  usz_t column_counter = 0;
  for_cnt(cell_index, cell_count) {
    st8_t str = data[cell_index];
    for_cnt(char_index, str.count) {
      ret.data[buffer_at++] = str.e[char_index];
    }
    ++column_counter;

    if (column_counter == columns) {
      ret.data[buffer_at++] = '\n';
      column_counter = 0;
    }
    else {
      ret.data[buffer_at++] = ',';
    }
    
  }
  printf("Total count: %zu\n", required_size);


  return ret;
}

static b32_t
foolish_write_file(const char* filename, buffer_t buffer) {
  FILE *file = fopen(filename, "wb");
  if (!file) return false;
  defer { fclose(file); };
  
  fwrite(buffer.data, 1, buffer.size, file);
  return true;
}

int main() {
  buffer_t item_csv = foolish_read_file_into_buffer("Item.csv", true);
  if (!item_csv) {
    printf("Cannot open Item.csv");
    return 1;
  }

  assert(item_csv);
  arena_t arena = foolish_allocate_arena(megabytes(256));

  make(csv_t, csv);
  if (!csv_read(csv, item_csv, &arena)) {
    return 1;
  }

  // Test write
  st8_t* out = arena_push_arr(st8_t, &arena, 16);
  for_cnt(i, 16) {
    out[i] = st8_from_lit("test");
  }
  buffer_t output = csv_write(out, 4, 4, &arena);

  foolish_write_file("ffxiv.csv", output);

  
  
  return 0;


}
