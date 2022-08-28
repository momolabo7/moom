
struct Lit_Sensor {
  V2 pos;
  U32 target_color;
  U32 current_color;
};

struct Lit_Sensor_List {
  U32 count;
  Lit_Sensor e[32];
};

static void 
lit_update_sensors(Lit_Sensor_List* sensors, Lit_Light_List* lights) {
  al_foreach(sensor_index, sensors)
  {
    Lit_Sensor* sensor = al_at(sensors, sensor_index);
    U32 current_color = 0x0000000;
    
    // For each light, for each triangle, add light
    al_foreach(light_index, lights)
    {
      Lit_Light* light = al_at(lights, light_index);
      
      al_foreach(tri_index, &light->triangles)
      {
        Tri2 tri = *al_at(&light->triangles, tri_index);
        if (t2_is_point_within(tri,
                               sensor->pos)) 
        {
          // TODO(Momo): Probably not the right way do sensor
          current_color += light->color >> 8 << 8; // ignore alpha
          break; // ignore the rest of the triangles
        }
      }
    }
    sensor->current_color = current_color;

    // TODO: Goodbye CPU. We should do some kind of
    // OnEnter/OnExit kind of algo
# if 0
    if (sensor->current_color == sensor->target_color) {
      sensor->edge->is_disabled = true; 
    }
    else {
      sensor->edge->is_disabled = false; 
    }
# endif
  }

}


static void 
lit_render_sensors(Lit_Sensor_List* sensors, Painter* painter) {
  al_foreach(sensor_index, sensors)
  {
    Lit_Sensor* sensor = al_at(sensors, sensor_index);
    Circ2 circ = { 32.f, sensor->pos };
    paint_filled_circle(painter, circ, 8,  hex_to_rgba(sensor->target_color)); 

    // only for debugging
#if 0
    sb8_make(sb, 128);
    sb8_push_fmt(sb, str8_from_lit("[%X]"), sensor->current_color);
    paint_text(painter,
               FONT_DEFAULT, 
               sb->str,
               hex_to_rgba(0xFFFFFFFF),
               sensor->pos.x - 100.f,
               sensor->pos.y + 10.f,
               32.f);
#endif

    advance_depth(painter);
  }

  
}

