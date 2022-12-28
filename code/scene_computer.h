#ifndef MOE_MODE_COMPUTER
#define MOE_MODE_COMPUTER


struct Computer_Selection{
  str8_t text;
  Moe_Mode_Type mode_type;
};

struct Computer_Selection_List{
  u32_t count;
  Computer_Selection e[10];
};

struct Computer{
  u32_t selected_id;
  Computer_Selection_List selection_list;
};

static void computer_tick(moe_t* moe);

#endif // MOE_MODE_COMPUTER
