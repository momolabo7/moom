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
  

  b32_t is_a_string = false; 

  // count cols
  for(u8_t* itr = buffer.data; ; ++itr) {
    if (itr[0] == '"') {
      is_a_string = !is_a_string;
    }
    else if (itr[0] == '\n')  {
      if (!is_a_string)  {
        csv->columns++;
        break;
      }
    }
    else if (itr[0] == ',')
      if (!is_a_string)  {
        csv->columns++;
      }
      
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
  is_a_string = false; 
  for_cnt(cell_index, cell_count)
  {
    st8_t token = {};
    token.e = buffer.data + at;
    while((is_a_string || buffer.data[at] != ',') &&
          (is_a_string || buffer.data[at] != '\n') && 
          buffer.data[at] != 0) 
    {
      if (buffer.data[at] == '"') {
        is_a_string = !is_a_string;
      }
      ++at;
      ++token.count;
    }
    csv->tokens[cell_index] = token;

    // End of file
    if (buffer.data[at] == 0)  {
      break;
    }

    ++at; // Need to skip past the \n or ','
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

  return ret;
}


static b32_t ffxiv_test() {
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
  foolish_write_buffer_to_file("ffxiv.csv", output);
}

static b32_t ffxiv_generate_item_data() {
  buffer_t item_csv = foolish_read_file_into_buffer("Item.csv", true);
  if (!item_csv) {
    printf("Cannot open Item.csv\n");
    return false;
  }

  assert(item_csv);
  arena_t arena = foolish_allocate_arena(megabytes(256));

  make(csv_t, csv);
  if (!csv_read(csv, item_csv, &arena)) {
    return false;
  }

  // Test write
  usz_t in_row_itr = 4;
  usz_t interested_cols[] = {0, 10, 26};
  usz_t out_rows = csv->rows - in_row_itr + 1; // +1 for header
  usz_t out_cols = array_count(interested_cols);
  usz_t out_cells = out_rows * out_cols;

  st8_t* out = arena_push_arr(st8_t, &arena, out_cells);
  usz_t cell_index = 0;
  out[cell_index++] = st8_from_lit("item_id");
  out[cell_index++] = st8_from_lit("item_name");
  out[cell_index++] = st8_from_lit("npc_price");

  while(cell_index < out_cells) {
    for_cnt(interested_col_index, out_cols) {
      out[cell_index++] = csv_get_cell(csv, in_row_itr, interested_cols[interested_col_index]);
    }
    in_row_itr++;
  }


  buffer_t output = csv_write(out, out_rows, out_cols, &arena);
  foolish_write_buffer_to_file("item_data.csv", output);


  return true;
}

static b32_t ffxiv_generate_recipe_data() {
  buffer_t item_csv = foolish_read_file_into_buffer("Recipe.csv", true);
  if (!item_csv) {
    printf("Cannot open Recipe.csv\n");
    return false;
  }

  assert(item_csv);
  arena_t arena = foolish_allocate_arena(megabytes(256));

  make(csv_t, csv);
  if (!csv_read(csv, item_csv, &arena)) {
    return false;
  }

  // Test write
  usz_t in_row_itr = 4;
  usz_t interested_cols[] = { 
    0, 4, 5, 
    6,7, // ingredient #0
    8,9,
    10,11,
    12,13,
    14,15,
    16,17,
    18,19,
    20,21,
    22,23,
    24,25,
  };
  usz_t interested_cols_itr = 0;
  usz_t out_rows = csv->rows - in_row_itr + 1; // +1 for header
  usz_t out_cols = array_count(interested_cols);
  usz_t out_cells = out_rows * out_cols;

  st8_t* out = arena_push_arr(st8_t, &arena, out_cells);
  if (!out) return false;
  usz_t cell_index = 0;
  out[cell_index++] = st8_from_lit("recipe_id");
  out[cell_index++] = st8_from_lit("item_id");
  out[cell_index++] = st8_from_lit("output_amt");
  //out[cell_index++] = st8_from_lit("class");
  out[cell_index++] = st8_from_lit("ingredient_id_0");
  out[cell_index++] = st8_from_lit("ingredient_amt_0");
  out[cell_index++] = st8_from_lit("ingredient_id_1");
  out[cell_index++] = st8_from_lit("ingredient_amt_1");
  out[cell_index++] = st8_from_lit("ingredient_id_2");
  out[cell_index++] = st8_from_lit("ingredient_amt_2");
  out[cell_index++] = st8_from_lit("ingredient_id_3");
  out[cell_index++] = st8_from_lit("ingredient_amt_3");
  out[cell_index++] = st8_from_lit("ingredient_id_4");
  out[cell_index++] = st8_from_lit("ingredient_amt_4");
  out[cell_index++] = st8_from_lit("ingredient_id_5");
  out[cell_index++] = st8_from_lit("ingredient_amt_5");
  out[cell_index++] = st8_from_lit("ingredient_id_6");
  out[cell_index++] = st8_from_lit("ingredient_amt_6");
  out[cell_index++] = st8_from_lit("ingredient_id_7");
  out[cell_index++] = st8_from_lit("ingredient_amt_7");
  out[cell_index++] = st8_from_lit("ingredient_id_8");
  out[cell_index++] = st8_from_lit("ingredient_amt_8");
  out[cell_index++] = st8_from_lit("ingredient_id_9");
  out[cell_index++] = st8_from_lit("ingredient_amt_9");

  while(cell_index < out_cells) {
    for_cnt(interested_col_index, out_cols) {
      out[cell_index++] = csv_get_cell(csv, in_row_itr, interested_cols[interested_col_index]);
    }
    in_row_itr++;
  }

#if 0
  while(i < out_cells) {
    out[cell_index++] = csv_get_cell(csv, in_row_itr, interested_cols[interested_cols_itr++]);
    out[cell_index++] = csv_get_cell(csv, in_row_itr, interested_cols[interested_cols_itr++]);
    out[cell_index++] = csv_get_cell(csv, in_row_itr, interested_cols[interested_cols_itr++]);
    in_row_itr++;
    interested_cols_itr = 0;
  }
#endif



  buffer_t output = csv_write(out, out_rows, out_cols, &arena);
  foolish_write_buffer_to_file("recipe_data.csv", output);


  return true;
}

int main() {
  //ffxiv_generate_item_data();
  ffxiv_generate_recipe_data();

  
  
  
  return 0;


}
