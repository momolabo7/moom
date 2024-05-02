//
// @todo:
// - Twitch-Bot API
// 

#include "momo.h"
#include <stdio.h>

#if OS_WINDOWS

struct socket_t {
  b32_t is_valid; // Probably should be some error code?
  SOCKET sock;
  operator bool() {
    return is_valid;
  }
};
//
// @note: my god windows why you make me do this.
//
static b32_t
socket_system_begin() {
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2,2), &wsa_data) != 0) {
    return false;
  }
  return true;
}


static socket_t
socket_begin(const char* server, u32_t port)
{
  // 1337
  // 1337 % 10 -> 7 
  
  // @todo: This is absolutely a terrible algorthim lul please redo
  char port_str[6] = {};
  {
    for(u32_t index = 0; 
        index < array_count(port_str)-1; 
        ++index)
    {
      port_str[index] = digit_to_ascii(port % 10);
      port /= 10;
    }
    cstr_reverse(port_str);
  }


  socket_t ret = {};

  addrinfo * addr = NULL;
  addrinfo addr_hints = {};
  //
  // AF_INET supposedly specifies IPv4 family.
  // There are other types: 
  //   AF_IRDA
  //   AF_BLUETOOTH
  //   AF_INET for IPv4
  //   AF_INET6 for IPv6
  //   AF_UNSPEC means either IPv4 or IPv6
  // Should give a good idea on what "address family" means
  //
  addr_hints.ai_family = AF_UNSPEC;  

  // 
  // TCP uses SOCK_STREAM (connection-based)
  // UDP uses DGRAM (datagram-based)
  //
  addr_hints.ai_socktype = SOCK_STREAM;


  // @note: This one is self-explanatory...?
  addr_hints.ai_protocol = IPPROTO_TCP;

  if (getaddrinfo(server, port_str, &addr_hints, &addr) != 0) 
  {
    return ret;
  }
  defer{ freeaddrinfo(addr); };
  
  //
  // With the actual addrinfo, we use that to create our socket
  //
  // @note: addrinfo is a linked list.
  // Perhaps an idea would be to iterate through the linked list of addresses
  // until one works?
  //
  SOCKET sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
  if (sock == INVALID_SOCKET) {
    return ret;
  }
  if (connect(sock, addr->ai_addr, addr->ai_addrlen) == SOCKET_ERROR) {
    return ret;
  }

  ret.is_valid = true;
  ret.sock = sock;
  return ret;
}

static void
socket_send(socket_t s, str_t msg) {
  send(s.sock, (char*)msg.e, msg.size, 0);
}

static void 
socket_end(socket_t s) {
  closesocket(s.sock);
}

static void
socket_system_end() {
  WSACleanup();
}

//
// @note: 
// - Will poll for response
// - Returns a valid str that is up to the received bytes 
//
// @todo: 
// - Maybe it should take in an arena...?
//
static str_t
socket_receive(socket_t s, str_t buffer)
{
  // @note: will poll
  int received_bytes = recv(s.sock, (char*)buffer.e, buffer.size, 0);
  return str_set(buffer.e, received_bytes);
}
static void
os_sleep(u32_t ms_to_sleep) {
  Sleep(ms_to_sleep);
}

#elif OS_LINUX


struct socket_t {
  b32_t is_valid; // Probably should be some error code?
  int sock;
  operator bool() {
    return is_valid;
  }
};

static b32_t 
socket_system_begin() 
{
  return true; 
}

static void  socket_system_end() {}

static socket_t
socket_begin(const char* server, u32_t port)
{
  socket_t ret = {};
  hostent* server_host = gethostbyname(server);
  if(!server_host) {
    return ret;
  }

  int s = socket(AF_INET, SOCK_STREAM, 0);  
  if (s == -1) {
    return ret;
  }

  sockaddr_in address = {};
  address.sin_family = AF_INET;
  memory_copy(&address.sin_addr, server_host->h_addr, server_host->h_length);
  address.sin_port = htons(6667);

  if (connect(s, (sockaddr*)&address, sizeof(address)) == -1)
  {
    return ret;
  }

  ret.is_valid = true;
  ret.sock = s;

  return ret;
}

static void
socket_end(socket_t s) 
{
  close(s.sock);
}

static void
socket_send(socket_t s, str_t msg) 
{
  send(s.sock, (char*)msg.e, msg.size, 0);
}

static str_t
socket_receive(socket_t s, str_t buffer)
{
  // @note: will poll
  ssize_t received_bytes = recv(s.sock, (char*)buffer.e, buffer.size, 0);
  if (received_bytes == -1)
  {
    return str_bad();
  }
  return str_set(buffer.e, received_bytes);
}

static void 
os_sleep(u32_t ms_to_sleep) 
{
  usleep(ms_to_sleep * 1000);
}

#else
# warning "socket library not implemented!"
#endif

#define ms_from_mins(mins) ((mins) * 1000 * 60)
#define ms_from_secs(secs) ((secs) * 1000)

// Twitch Messaging Interface
struct tmi_t
{
  socket_t socket;

  // @note: don't mess with these guys.
  // sb and s will share the same buffer
  u8_t buffer[512];
  stb8_t sender;
  str_t receiver;

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
  socket_t s = socket_begin("irc.chat.twitch.tv", 6667);
  if (!s.is_valid) {
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

  tmi->socket = s;
  return true;
}

static void
tmi_disconnect(tmi_t* tmi) 
{
  socket_end(tmi->socket);
}

static void
tmi_send_message(tmi_t* tmi, str_t message)
{
  stb8_t* sender = &tmi->sender;
  stb8_push_fmt(sender, str_from_lit("PRIVMSG #momolabo7 :%S\r\n"), message); 
  socket_send(tmi->socket, sender->str);
  stb8_clear(sender);
  str_t r = socket_receive(tmi->socket, tmi->receiver);
  _tmi_print_str(r);
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


  tmi_send_message(tmi, str_from_lit("HEHE HOHO IM FEK MOOM"));

#if 0
  while(1)
  {
#if 0
    // test reminders
#define REMINDER "PRIVMSG #momolabo7 :Remember to stretch! :)\r\n"
    socket_send(s, str_from_lit(REMINDER));
    socket_receive(s, buffer);
#endif

    
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

//    os_sleep(ms_from_mins(5));
  }
#endif

  return 0;

}
