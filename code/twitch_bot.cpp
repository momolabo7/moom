//
// @todo:
// - Twitch-Bot API
// 

#include "momo.h"
#include <stdio.h>

static f32_t 
clock_secs_elapsed(u64_t start, u64_t end) 
{
  return (f32_t)(end - start)/clock_resolution();
}

struct karu_bot_reminder_t {
  str_t message;
  f32_t interval;
  u64_t start_time;
};


struct karu_bot_t
{
  socket_t socket;

  // @note: don't mess with these guys.
  // sb and s will share the same buffer
  u8_t buffer[512];
  strb_t sender;
  str_t receiver;

  // reminder system
  karu_bot_reminder_t reminders[8];
  u32_t reminder_count;
};

static void
karu_print_str(str_t str)
{
  if (!str) return;
  for_cnt(i, str.size) 
    printf("%c", str.e[i]); 
  printf("\n");
}

static b32_t 
karu_connect(karu_bot_t* karu, str_t password, str_t channel, str_t nick, arena_t* arena) 
{
  make(socket_t, s);
  if (!socket_begin(s, "irc.chat.twitch.tv", 6667))
  {
    return false;
  }

  socket_set_receive_timeout(s, ms_from_secs(1));

  // @note: this is more of initialization then 
  // 'connecting' but whatever i guess
  strb_t* sender = &karu->sender;
  karu->receiver = str_set(karu->buffer, sizeof(karu->buffer));
  strb_init_from_str(sender, karu->receiver);

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
    strb_push_fmt(sender, str_from_lit("NICK %S\r\n"), nick); 
    socket_send(s, sender->str);
    strb_clear(sender);
    str_t r = socket_receive(s, karu->receiver);
    karu_print_str(r);
  }
  //printf("%s\n", buffer.e);


  // Tell twitch which channel to join.
  {
    strb_push_fmt(sender, str_from_lit("JOIN #%S\r\n"), channel); 
    socket_send(s, sender->str);
    strb_clear(sender);
    str_t r = socket_receive(s, karu->receiver);
    karu_print_str(r);
  }

  karu->socket = dref(s);
  return true;
}

static void
karu_disconnect(karu_bot_t* karu) 
{
  socket_end(&karu->socket);
}

static void
karu_send_message(karu_bot_t* karu, str_t message)
{
  strb_t* sender = &karu->sender;
  strb_push_fmt(sender, str_from_lit("PRIVMSG #momolabo7 :%S\r\n"), message); 
  socket_send(&karu->socket, sender->str);
  strb_clear(sender);
  str_t r = socket_receive(&karu->socket, karu->receiver);
  karu_print_str(r);
}

static str_t
karu_receive(karu_bot_t* karu)
{
  return socket_receive(&karu->socket, karu->receiver);
}

static void
karu_process_commands(
    karu_bot_t* karu, 
    str_t cmd, 
    str_t from, 
    str_arr_t args, 
    arena_t* arena)
{
  if (!str_match(cmd, str_from_lit("!ping")))
  {
    karu_send_message(karu, str_from_lit("pong!"));
  }
  // More commands here
}

static void
karu_update(karu_bot_t* karu, arena_t* arena) 
{
  // reminders
  for_cnt(reminder_index, karu->reminder_count)
  {
    auto* reminder = karu->reminders + reminder_index;
    u64_t end_time = clock_time();
    if (clock_secs_elapsed(reminder->start_time, end_time) >= reminder->interval)
    {
      karu_send_message(karu, reminder->message);
      reminder->start_time = end_time;
    }
  }

  // commands
  str_t r = karu_receive(karu);
  karu_print_str(r);
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
        
        karu_process_commands(karu, cmd, user, arena);

      }
    }
#if 0
    for_cnt(i, msgs.size) {
      karu_print_str(msgs.e[i]);
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
          strb_make(stb, 256);
          str_t who = msgs.e[5];
          who.size -=2; // @todo: HELP LA (removes \r\n)
          strb_push_fmt(stb, str_from_lit("PRIVMSG #momolabo7 :Check out my BRO %S at twitch.tv/%S\r\n"), who, who);
          karu_send_message(karu, stb->str);

        }
        printf("Message received: ");
        for_cnt(i , msg.size) 
          printf("%c", msg.e[i]);
        printf("\n");
      }
    }

  }
#endif

  printf("Ping!\n");
  doze(ms_from_secs(1));

}

static void
karu_add_reminder(karu_bot_t* karu, str_t message, f32_t interval_in_secs)
{
  assert(karu->reminder_count < array_count(karu->reminders));
  auto* r = karu->reminders + karu->reminder_count++;
  r->message = message;
  r->interval = interval_in_secs;
  r->start_time = clock_time();
}


DWORD WINAPI 
karu_test(void* data) {
  printf("Hello\n");
  return 0; 
}

int main() 
{
  DWORD thread_id = 0;
  HANDLE handle = CreateThread(0, 0, karu_test, 0, 0, &thread_id);
  if (handle == NULL) {
    printf("Failed to create thread\n");
    return 1;
  }
  SetThreadPriority(handle, THREAD_PRIORITY_TIME_CRITICAL);
  WaitForSingleObject(handle, INFINITE);

  make(arena_t, arena);
  arena_alloc(arena, gigabytes(1), false);
  defer { arena_free(arena); };

  karu_bot_t* karu = arena_push(karu_bot_t, arena);

  str_t pw = file_read_into_str("moom_bot_pass", arena);

  b32_t success = socket_system_begin();
  if (!success) {
    printf("Cannot start socket system\n");
    return 1;
  }
  defer { socket_system_end(); };

  if (!karu_connect(
        karu, 
        pw, 
        str_from_lit("momolabo7"), 
        str_from_lit("moom_bot"),
        arena))
  {
    printf("Cannot create socket\n");
    return 1;
  }
  defer { karu_disconnect(karu); };
  printf("Great success!\n");

  karu_send_message(karu, str_from_lit("HEHE HOHO IM FEK MOOM AND IM HERE"));
  karu_add_reminder(karu, str_from_lit("WATER BREAK"), 60.f * 10.f);

  //u64_t reminder_start = clock_time();

  while(1)
  {
    //u64_t reminder_end = clock_time();
    //printf("%f\n",clock_secs_elapsed(reminder_start, clock_time()));
#if 0
    if (clock_secs_elapsed(reminder_start, reminder_end) >= (60.f * 5))
    {
      karu_send_message(karu, str_from_lit("Remember To Stretch!"));
      reminder_start = reminder_end;
    }
#endif
    karu_update(karu, arena);

  }

  return 0;

}
