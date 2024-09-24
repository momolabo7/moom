//
// @todo:
// - Twitch-Bot API
// 

#include "momo.h"
#include <stdio.h>


struct momolabot_reminder_t {
  str_t message;
  f32_t interval;
  u64_t start_time;
};


struct momolabot_t
{
  socket_t socket;

  // @note: don't mess with these guys.
  // sb and s will share the same buffer
  u8_t buffer[512];
  str_builder_t sender;
  str_t receiver;

  // reminder system
  momolabot_reminder_t reminders[8];
  u32_t reminder_count;
};

static void
momolabot_print_str(str_t str)
{
  if (!str) return;
  for_cnt(i, str.size) 
    printf("%c", str.e[i]); 
  printf("\n");
}

static b32_t 
momolabot_connect(momolabot_t* m, str_t password, str_t channel, str_t nick, arena_t* arena) 
{
  make(socket_t, s);
  if (!socket_begin(s, "irc.chat.twitch.tv", 6667))
  {
    return false;
  }

  socket_set_receive_timeout(s, ms_from_secs(1));

  // @note: this is more of initialization then 
  // 'connecting' but whatever i guess
  str_builder_t* sender = &m->sender;
  m->receiver = str_set(m->buffer, sizeof(m->buffer));
  str_builder_init(sender, m->receiver);

  // @note: we will be abusing and reusing this buffer

  // Tell twitch what kind of information we want
  socket_send(s, str_from_lit("CAP REQ :twitch.tv/membership twitch.tv/tags twitch.tv/commands\r\n"));

  // Send the password
  socket_send(s, password);

  // Inform twitch our nickname
  {
    str_builder_push_fmt(sender, str_from_lit("NICK %S\r\n"), nick); 
    socket_send(s, sender->str);
    str_builder_clear(sender);
    str_t r = socket_receive(s, m->receiver);
    momolabot_print_str(r);
  }
  //printf("%s\n", buffer.e);


  // Tell twitch which channel to join.
  {
    str_builder_push_fmt(sender, str_from_lit("JOIN #%S\r\n"), channel); 
    socket_send(s, sender->str);
    str_builder_clear(sender);
    str_t r = socket_receive(s, m->receiver);
    momolabot_print_str(r);
  }

  m->socket = dref(s);
  return true;
}

static void
momolabot_disconnect(momolabot_t* m) 
{
  socket_end(&m->socket);
}

static void
momolabot_send_message(momolabot_t* m, str_t message)
{
  str_builder_t* sender = &m->sender;
  str_builder_push_fmt(sender, str_from_lit("PRIVMSG #momolabo7 :%S\r\n"), message); 
  socket_send(&m->socket, sender->str);
  str_builder_clear(sender);
  str_t r = socket_receive(&m->socket, m->receiver);
  momolabot_print_str(r);
}

static str_t
momolabot_receive(momolabot_t* m)
{
  return socket_receive(&m->socket, m->receiver);
}

static void
momolabot_process_commands(
    momolabot_t* m, 
    str_t cmd, 
    str_t from, 
//    str_arr_t args, 
    arena_t* arena)
{
  if (!str_match(cmd, str_from_lit("!ping")))
  {
    momolabot_send_message(m, str_from_lit("pong!"));
  }
  // More commands here
}

static void
momolabot_update(momolabot_t* m, arena_t* arena) 
{
  // reminders
  for_cnt(reminder_index, m->reminder_count)
  {
    auto* reminder = m->reminders + reminder_index;
    u64_t end_time = clock_time();
    if (clock_secs_elapsed(reminder->start_time, end_time) >= reminder->interval)
    {
      momolabot_send_message(m, reminder->message);
      reminder->start_time = end_time;
    }
  }

  // commands
  str_t r = momolabot_receive(m);
  momolabot_print_str(r);
  {
    arena_set_revert_point(arena);
    str_arr_t rr = str_split(r, ' ', arena);
    if (rr.size > 3) 
    {
      str_t message_type = rr.e[2];
      if (str_match(message_type, str_from_lit("PRIVMSG")))
      {
        str_t user = rr.e[3];
        user.e += 1; // get rid of the '#' at the start

        str_t cmd = rr.e[4]; 
        cmd.e += 1; // get rid of the ':' at the start
        
        momolabot_process_commands(m, cmd, user, arena);

      }
    }
#if 0
    for_cnt(i, msgs.size) {
      momolabot_print_str(msgs.e[i]);
      printf("\n");
    }
#endif
  }
#if 0

  {
    str_arr_t msgs = str_split(r, ' ', arena);
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
          str_builder_make(stb, 256);
          str_t who = msgs.e[5];
          who.size -=2; // @todo: HELP LA (removes \r\n)
          str_builder_push_fmt(stb, str_from_lit("PRIVMSG #momolabo7 :Check out my BRO %S at twitch.tv/%S\r\n"), who, who);
          momolabot_send_message(m, stb->str);

        }
        printf("Message received: ");
        for_cnt(i , msg.size) 
          printf("%c", msg.e[i]);
        printf("\n");
      }
    }

  }
#endif

  doze(ms_from_secs(1));

}

static void
momolabot_add_reminder(momolabot_t* m, str_t message, f32_t interval_in_secs)
{
  assert(m->reminder_count < array_count(m->reminders));
  auto* r = m->reminders + m->reminder_count++;
  r->message = message;
  r->interval = interval_in_secs;
  r->start_time = clock_time();
}


DWORD WINAPI 
momolabot_test(void* data) {
  printf("Hello\n");
  return 0; 
}

int main() 
{
  DWORD thread_id = 0;
  HANDLE handle = CreateThread(0, 0, momolabot_test, 0, 0, &thread_id);
  if (handle == NULL) {
    printf("Failed to create thread\n");
    return 1;
  }
  SetThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);
  WaitForSingleObject(handle, INFINITE);

  make(arena_t, arena);
  arena_alloc(arena, gigabytes(1), false);
  defer { arena_free(arena); };

  momolabot_t* m = arena_push(momolabot_t, arena);

  str_t pw = file_read_into_str("momolabot_pass", arena);

  b32_t success = socket_system_begin();
  if (!success) {
    printf("Cannot start socket system\n");
    return 1;
  }
  defer { socket_system_end(); };

  if (!momolabot_connect(
        m, 
        pw, 
        str_from_lit("momolabo7"), 
        str_from_lit("momolabot"),
        arena))
  {
    printf("Cannot create socket\n");
    return 1;
  }
  defer { momolabot_disconnect(m); };
  printf("Great success!\n");

  momolabot_send_message(m, str_from_lit("HEHE HOHO IM FEK MOOM AND IM HERE"));
  momolabot_add_reminder(m, str_from_lit("WATER BREAK"), 60.f * 10.f);

  //u64_t reminder_start = clock_time();

  while(1)
  {
    momolabot_update(m, arena);
  }

  return 0;

}
