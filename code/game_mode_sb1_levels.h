
static void
lit_level_0(Lit* m) {
  lit_push_point(m, {0.f, 0.f});     // 0
  lit_push_point(m, {1600.f, 0.f});  // 1
  lit_push_point(m, {1600.f, 900.f});// 2
  lit_push_point(m, {0.f, 900.f});   // 3
  
  lit_push_edge(m, 0, 1);
  lit_push_edge(m, 1, 2);
  lit_push_edge(m, 2, 3);
  lit_push_edge(m, 3, 0);

#if 1 
  lit_push_point(m, {100.f, 100.f});  //4
  lit_push_point(m, {1500.f, 100.f}); //5
  lit_push_point(m, {1500.f, 800.f}); //6
  lit_push_point(m, {100.f, 800.f});  //7
  lit_push_edge(m, 4, 5);
  lit_push_edge(m, 5, 6);
  lit_push_edge(m, 6, 7);
  lit_push_edge(m, 7, 4);
#endif
  
  
  // lights
  lit_push_light(m, {250.f, 600.f}, 0x880000FF, 90.f, 0.f);
  lit_push_light(m, {450.f, 600.f}, 0x008800FF, 270.f, 0.f);
  lit_push_light(m, {650.f, 600.f}, 0x000088FF, 360.f, 0.f);
  
  // initialize player
  Lit_Player* p = &m->player;
  lit_init_player(p, 400.f, 400.f);
  // Test sensor
#if 1
  {
    lit_push_point(m, {400.f, 400.f}); // 8
    lit_push_point(m, {450.f, 400.f}); // 9 
    lit_push_point(m, {450.f, 500.f}); // 10
    lit_push_point(m, {400.f, 500.f}); // 11
                                       
    lit_push_edge(m, 8, 9);
    lit_push_edge(m, 9, 10);
    lit_push_edge(m, 10, 11);

    lit_push_sensor(m, {400.f, 600.f}, 0x888800FF); 
  }
#endif
  lit_set_win_point(m, {800.f, 400.f});
}

static void
lit_level_1(Lit* m) {
  lit_push_point(m, {0.f, 0.f});     // 0
  lit_push_point(m, {1600.f, 0.f});  // 1
  lit_push_point(m, {1600.f, 900.f});// 2
  lit_push_point(m, {0.f, 900.f});   // 3
  
  lit_push_edge(m, 0, 1);
  lit_push_edge(m, 1, 2);
  lit_push_edge(m, 2, 3);
  lit_push_edge(m, 3, 0);

  
  // lights
  lit_push_light(m, {250.f, 600.f}, 0x880000FF, 90.f, 0.f);
  
  // initialize player
  Lit_Player* p = &m->player;
  lit_init_player(p, 400.f, 400.f);

  lit_set_win_point(m, {800.f, 400.f});
}

typedef void (*Lit_Level)(Lit* mode); 
static Lit_Level lit_levels[] = {
  lit_level_0,
  lit_level_1,
};


static void
lit_load_level(Lit* m, U32 level_id) {
  al_clear(&m->sensors);
  al_clear(&m->lights);
  al_clear(&m->edges);

  lit_levels[level_id](m);

  m->is_win_reached = false;
}

static void
lit_load_next_level(Lit* m){
  m->current_level_id = (m->current_level_id + 1)%array_count(lit_levels);
  lit_load_level(m, m->current_level_id);  
}

