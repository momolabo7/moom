
// Test to do threaded audio on wasapi

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#undef near
#undef far

#pragma comment(lib, "Ws2_32.lib")


int main() 
{
  printf("Hello World\n");
  WSADATA wsa_data;
  // Start Winsock DLL, specifying version 2.2
  if (WSAStartup(MAKEWORD(2,2), &wsa_data) != 0) {
    return 1;
  }

#if 0
  addrinfo * result = NULL;
  addrinfo hints;

  ZeroMemory(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;
  hints.ai_flags = AI_PASSIVE;
  getaddrinfo(NULL, 1337, &hints, &result);
#endif


  return 0;

}
