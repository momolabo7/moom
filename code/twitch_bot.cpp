//
// @todo:
// - Twitch-Bot API
// 

#include "momo.h"
#include <stdio.h>

// Twitch Messaging Interface
struct tmi_t
{
  socket_t socket;

  // @note: don't mess with these guys.
  // sb and s will share the same buffer
  u8_t buffer[512];
  stb8_t sender;
  str_t receiver;

  // reminder system
  struct {
    str_t message;
    f32_t interval;
  } reminders[8];
  u32_t reminder_count;
  u64_t reminder_timer;

};

static void
_tmi_print_str(str_t str)
{
  for_cnt(i, str.size) 
    printf("%c", str.e[i]); 
  printf("\n");
}

static b32_t 
tmi_connect(tmi_t* tmi, str_t password, str_t channel, str_t nick, arena_t* arena) 
{
  make(socket_t, s);
  if (!socket_begin(s, "irc.chat.twitch.tv", 6667))
  {
    return false;
  }

  // @note: this is more of initialization then 
  // 'connecting' but whatever i guess
  stb8_t* sender = &tmi->sender;
  tmi->receiver = str_set(tmi->buffer, sizeof(tmi->buffer));
  stb8_init_from_str(sender, tmi->receiver);

  // @note: we will be abusing and reusing this buffer

  // Tell twitch what kind of information we want
  {
    socket_send(s, str_from_lit("CAP REQ :twitch.tv/membership twitch.tv/tags twitch.tv/commands\r\n"));
  }

  // Send the password
  {
    socket_send(s, password);
  }

  // Inform twitch our nickname
  {
    stb8_push_fmt(sender, str_from_lit("NICK %S\r\n"), nick); 
    socket_send(s, sender->str);
    stb8_clear(sender);
    str_t r = socket_receive(s, tmi->receiver);
    _tmi_print_str(r);
  }
    //printf("%s\n", buffer.e);

  // Tell twitch which channel to join.
  {
    stb8_push_fmt(sender, str_from_lit("JOIN #%S\r\n"), channel); 
    socket_send(s, sender->str);
    stb8_clear(sender);
    str_t r = socket_receive(s, tmi->receiver);
    _tmi_print_str(r);
  }

  tmi->socket = dref(s);
  tmi->reminder_timer = clock_time();
  return true;
}

static void
tmi_disconnect(tmi_t* tmi) 
{
  socket_end(&tmi->socket);
}

static void
tmi_send_message(tmi_t* tmi, str_t message)
{
  stb8_t* sender = &tmi->sender;
  stb8_push_fmt(sender, str_from_lit("PRIVMSG #momolabo7 :%S\r\n"), message); 
  socket_send(&tmi->socket, sender->str);
  stb8_clear(sender);
  str_t r = socket_receive(&tmi->socket, tmi->receiver);
  _tmi_print_str(r);
}


static void
tmi_update(tmi_t* tmi) 
{
  // reminders
  for_cnt(reminder_index, tmi->reminder_count)
  {
    auto* reminder = tmi->reminders + reminder_index;
    if (clock_secs_elapsed(reminder_start, reminder_end) >= (60.f * 5))
    {
      tmi_send_message(tmi, str_from_lit("Remember To Stretch!"));
      reminder_start = reminder_end;
    }
  }

}

static void
tmi_add_reminder(tmi_t* tmi, str_t message, f32_t interval)
{
  assert(tmi->reminder_count < array_count(tmi->reminders));
  auto* r = tmi->reminders + tmi->reminder_count++;
  r->message = messsage;
  r->interval = interval;
}

static f32_t 
clock_secs_elapsed(u64_t start, u64_t end) 
{
  return (f32_t)(end - start)/clock_resolution();
}


int main() 
{
  make(arena_t, arena);

  arena_alloc(arena, gigabytes(1), false);
  defer { arena_free(arena); };

  tmi_t* tmi = arena_push(tmi_t, arena);

  str_t pw = file_read_into_str("moom_bot_pass", arena);

  b32_t success = socket_system_begin();
  if (!success) {
    printf("Cannot start socket system\n");
    return 1;
  }
  defer { socket_system_end(); };

  if (!tmi_connect(
        tmi, 
        pw, 
        str_from_lit("momolabo7"), 
        str_from_lit("moom_bot"),
        arena))
  {
    printf("Cannot create socket\n");
    return 1;
  }
  defer { tmi_disconnect(tmi); };
  printf("Great success!\n");

  tmi_send_message(tmi, str_from_lit("HEHE HOHO IM FEK MOOM AND IM HERE"));
  tmi_add_reminder(...);

  //u64_t reminder_start = clock_time();

  while(1)
  {
    //u64_t reminder_end = clock_time();
    //printf("%f\n",clock_secs_elapsed(reminder_start, clock_time()));
#if 0
    if (clock_secs_elapsed(reminder_start, reminder_end) >= (60.f * 5))
    {
      tmi_send_message(tmi, str_from_lit("Remember To Stretch!"));
      reminder_start = reminder_end;
    }
#endif
    tmi_update(tmi);

#if 0 
    str_t result = socket_receive(s, buffer);
    {
      arena_set_revert_point(arena);
      str_arr_t msgs = str_split(result, ' ', arena);
      for_cnt(i, r.size) printf("%c", r.e[i]); printf("\n");
      if (msgs.size > 3) 
      {
        str_t type = msgs.e[2];
        if (!str_compare_lexographically(type, str_from_lit("PRIVMSG")))
        {
          // This is the first 'argument' of a command
          str_t cmd = msgs.e[4]; 
          if (!str_compare_lexographically(cmd, str_from_lit(":!wtf")))
          {
            stb8_make(stb, 256);
            str_t who = msgs.e[5];
            who.size -=2; // @todo: HELP LA (removes \r\n)
            stb8_push_fmt(stb, str_from_lit("PRIVMSG #momolabo7 :Check out my BRO %S at twitch.tv/%S\r\n"), who, who);
            socket_send(s, stb->str);

          }
#if 0
          printf("Message received: ");
          for_cnt(i , msg.size) 
            printf("%c", msg.e[i]);
          printf("\n");
#endif
        }
      }
      
    }
#endif

    snooze(ms_from_secs(1));
  }

  return 0;

}
