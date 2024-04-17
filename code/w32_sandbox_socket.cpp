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
#define CHANNEL "JOIN #momohoudai\r\n"
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
#elif OS_LINUX
#warning "socket library not implemented!"
#elif
#warning "socket library not implemented!"
#endif

int main() 
{
  str_t pw = foolish_read_file_into_buffer("moom_bot_pass");
  str_t refresh_token = foolish_read_file_into_buffer("moom_bot_refresh");
  defer {
    foolish_free_buffer(pw);
    foolish_free_buffer(refresh_token);
  };

  printf("Hello World\n");
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


  char buffer[512] = {};

  send(s.sock, CAPREQ, sizeof(CAPREQ), 0);
  os_socket_send(s, pw);
  send(s.sock, NICK, sizeof(NICK), 0);
  {
    int received_bytes = recv(s.sock, buffer, sizeof(buffer), 0);
    buffer[received_bytes] = 0;
    printf("%s\n", buffer);
  }

#if 0
  send(s.sock, CHANNEL, sizeof(CHANNEL), 0);
  {
    int received_bytes = recv(s.sock, buffer, sizeof(buffer), 0);
    buffer[received_bytes] = 0;
    printf("%s\n", buffer);
  }
  printf("Joined\n");
#define TESTMSG "PRIVMSG #momohoudai :HEHE HOHO IM FEK MOOM\r\n"
  send(s.sock, TESTMSG, sizeof(TESTMSG), 0);
  {
    int received_bytes = recv(s.sock, buffer, sizeof(buffer), 0);
    buffer[received_bytes] = 0;
    printf("%s\n", buffer);
  }
  while(1){

  }

#endif
  printf("Great success!\n");


  return 0;

}
