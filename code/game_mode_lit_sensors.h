#define LIT_SENSOR_PARTICLE_CD 0.1f
#define LIT_SENSOR_COLOR_MASK 0xFFFFFF00
#define LIT_SENSOR_RADIUS 8.f

struct Lit_Sensor {
  V2 pos;
  U32 target_color;
  U32 current_color;
  
  F32 particle_cd;
  
};

struct Lit_Sensor_List {
  U32 count;
  Lit_Sensor e[32];
  U32 activated;
};

static void 
lit_push_sensor(Lit_Sensor_List* sensors, V2 pos, U32 target_color) 
{
  assert(!al_is_full(sensors));
  Lit_Sensor* s = al_append(sensors);
  s->pos = pos;
  s->target_color = target_color;
  s->current_color = 0;
}

static void 
lit_update_sensors(Lit_Sensor_List* sensors, 
                   Lit_Particle_Pool* particles,
                   Lit_Light_List* lights, 
                   RNG* rng, 
                   F32 dt) 
{
  U32 activated = 0;
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
        if (t2_is_point_within(tri, sensor->pos)) 
        {
          // TODO(Momo): Probably not the right way do sensor
          current_color += light->color >> 8 << 8; // ignore alpha
          break; // ignore the rest of the triangles
        }
      }
    
    }

    // Sensor color check
    sensor->current_color = current_color;
    if ((sensor->current_color & LIT_SENSOR_COLOR_MASK) == 
        (sensor->target_color & LIT_SENSOR_COLOR_MASK)) 
    {
      ++activated;
    }
    sensors->activated = activated;

    // Particle emission check
    sensor->particle_cd -= dt;
    if (sensor->particle_cd <= 0.f) {
      sensor->particle_cd = LIT_SENSOR_PARTICLE_CD;
      V2 rand_dir = rng_unit_circle(rng);
      V2 particle_vel = v2_scale(rand_dir, 100.f); 
      lit_spawn_particle(particles, 
                         1.f,
                         sensor->pos,
                         particle_vel,
                         rgba(1.f, 1.f, 1.f, 1.f),
                         rgba(1.f, 1.f, 1.f, 0.f));

    }
  }

}


static void 
lit_render_sensors(Lit_Sensor_List* sensors, Painter* painter) {
  al_foreach(sensor_index, sensors)
  {
    Lit_Sensor* sensor = al_at(sensors, sensor_index);
    Circ2 circ = { LIT_SENSOR_RADIUS, sensor->pos };
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

