//
// @todo:
// - Linux implementation (DO THIS FIRST MOMO)
// - Twitch-Bot API
// 


#define FOOLISH 

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#undef near
#undef far

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Winmm.lib")

#include "momo.h"

#define SERVER "irc.chat.twitch.tv"
#define PORT "6667"
#define NICK "NICK moom_bot\r\n"
#define CHANNEL "JOIN #momolabo7\r\n"
#define CAPREQ "CAP REQ :twitch.tv/membership twitch.tv/tags twitch.tv/commands\r\n"


struct os_socket_t {
  b32_t is_valid; // Probably should be some error code?
#if OS_WINDOWS
  SOCKET sock;
#else 
  // Linux here?
#endif
};

#if OS_WINDOWS
// @note: my god windows why you make me do this.
static b32_t
os_socket_system_begin() {
  WSADATA wsa_data;
  if (WSAStartup(MAKEWORD(2,2), &wsa_data) != 0) {
    return false;
  }
  return true;
}

// @todo: parameters for socket
static os_socket_t
os_socket_begin(const char* server, const char* port)
{
  os_socket_t ret = {};

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


  if (getaddrinfo(server, port, &addr_hints, &addr) != 0) {
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
os_socket_send(os_socket_t s, str_t msg) {
  send(s.sock, (char*)msg.e, msg.size, 0);
}

static void 
os_socket_end(os_socket_t s) {
  closesocket(s.sock);
}

static void
os_socket_system_end() {
  WSACleanup();
}

//
// @note: 
// - Will null terminate the last byte of the buffer
// - Will poll for response
// - Returns a valid str that is up to the received bytes 
//
// @todo: maybe we shouldn't null terminate?
//
static str_t
os_socket_receive(os_socket_t s, str_t buffer)
{
  // @note: will poll
  int received_bytes = recv(s.sock, (char*)buffer.e, buffer.size, 0);
  return str_set(buffer.e, received_bytes);
}
static void
os_sleep(u32_t ms_to_sleep) {
  Sleep(ms_to_sleep);
}

#define ms_from_mins(mins) ((mins) * 1000 * 60)
#define ms_from_secs(secs) ((secs) * 1000)

#elif OS_LINUX
#warning "socket library not implemented!"
#elif
#warning "socket library not implemented!"
#endif

int main() 
{
  make(arena_t, arena);
  arena_alloc(arena, gigabytes(1), false);

  str_t pw = foolish_read_file_into_buffer("moom_bot_pass");
  str_t refresh_token = foolish_read_file_into_buffer("moom_bot_refresh");
  defer {
    foolish_free_buffer(pw);
    foolish_free_buffer(refresh_token);
  };

  b32_t success = os_socket_system_begin();
  if (!success) {
    printf("Cannot start socket system\n");
    return 1;
  }
  defer { os_socket_system_end(); };

  os_socket_t s = os_socket_begin(SERVER, PORT);
  if (!s.is_valid) {
    printf("Cannot create socket\n");
    return 1;
  }
  defer { os_socket_end(s); };



  str_t buffer = arena_push_str(arena, 512, 16);
  os_socket_send(s, str_from_lit(CAPREQ));
  os_socket_send(s, pw);
  os_socket_send(s, str_from_lit(NICK));
  str_t r = os_socket_receive(s, buffer);
  for_cnt(i, r.size) printf("%c", r.e[i]); printf("\n");
  //printf("%s\n", buffer.e);

  os_socket_send(s, str_from_lit(CHANNEL));
  r = os_socket_receive(s, buffer);
  for_cnt(i, r.size) printf("%c", r.e[i]); printf("\n");
  //printf("%s\n", buffer.e);
  printf("Joined\n");

#if 0
#define TESTMSG "PRIVMSG #momolabo7 :HEHE HOHO IM FEK MOOM\r\n"
  os_socket_send(s, str_from_lit(TESTMSG));
  r = os_socket_receive(s, buffer);
  for_cnt(i, r.size) printf("%c", r.e[i]); printf("\n");
#endif

  // @todo: output
  while(1)
  {
#if 0
    // test reminders
#define REMINDER "PRIVMSG #momolabo7 :Remember to stretch! :)\r\n"
    os_socket_send(s, str_from_lit(REMINDER));
    os_socket_receive(s, buffer);
#endif

    str_t result = os_socket_receive(s, buffer);
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
            os_socket_send(s, stb->str);

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

  printf("Great success!\n");
  return 0;

}
