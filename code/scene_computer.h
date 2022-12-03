#ifndef MOE_MODE_COMPUTER
#define MOE_MODE_COMPUTER


struct Computer_Selection{
  String8 text;
  Moe_Mode_Type mode_type;
};

struct Computer_Selection_List{
  U32 count;
  Computer_Selection e[10];
};

struct Computer{
  U32 selected_id;
  Computer_Selection_List selection_list;
};

static void computer_tick(Moe* moe);

#endif // MOE_MODE_COMPUTER
