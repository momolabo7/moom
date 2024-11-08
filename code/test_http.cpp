#include <stdio.h>
#include "momo.h"



struct https_t
{
  HINTERNET session;
  HINTERNET connection;
  HINTERNET request;
};

enum https_request_type_t
{
  HTTPS_REQUEST_TYPE_GET,
  HTTPS_REQUEST_TYPE_POST,
  HTTPS_REQUEST_TYPE_PUT,
};

static void 
https_begin(https_t* https)
{
  https->session = WinHttpOpen(
    L"todo_some_name",
    WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
    WINHTTP_NO_PROXY_NAME,
    WINHTTP_NO_PROXY_BYPASS,
    0);
}

static void
https_connect(https_t* https, const wchar_t* url)
{
  https->connection = WinHttpConnect(
    https->session,
    url,
    INTERNET_DEFAULT_HTTPS_PORT,
    0);
}


static buffer_t
https_request(
    https_t* https, 
    https_request_type_t request_type,
    buffer_t endpoint,
    arena_t* arena)
{
  const wchar_t* request_type_str = L"GET";
  if(request_type == HTTPS_REQUEST_TYPE_POST) request_type_str = L"POST";
  if(request_type == HTTPS_REQUEST_TYPE_PUT) request_type_str = L"PUT";

  arena_marker_t mark = arena_mark(arena);
  DWORD bytes_read;
  DWORD size_to_read;
  DWORD size_so_far = 0;
  buffer_t ret = buffer_bad();


  s32_t wchar_len = MultiByteToWideChar(CP_UTF8, 0, (char*)endpoint.e, -1, NULL, 0);
  LPWSTR wchar_endpoint = (LPWSTR)arena_push_size(arena, sizeof(wchar_t)*wchar_len, 16);
  MultiByteToWideChar(CP_UTF8, 0, (char*)endpoint.e, -1, wchar_endpoint, wchar_len);

  https->request = WinHttpOpenRequest(
    https->connection,
    request_type_str,
    wchar_endpoint,
    NULL,
    WINHTTP_NO_REFERER,
    WINHTTP_DEFAULT_ACCEPT_TYPES,
    WINHTTP_FLAG_SECURE);

   arena_revert(mark);

   if (!WinHttpSendRequest(https->request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
   {
     goto bad_end;
   }

   if (!WinHttpReceiveResponse(https->request, NULL))
   {
     goto bad_end;
   }

   while (WinHttpQueryDataAvailable(https->request, &size_to_read) && size_to_read > 0)
   {
     usz_t old_size = ret.size;
     size_so_far += size_to_read;


     if (!ret) 
     {
       ret = arena_push_buffer(arena, size_so_far, 16);
       if (!ret) 
       {
         goto bad_end;
       }
     }

     else if (size_so_far > ret.size)
     {
       if (!arena_grow_buffer(arena, &ret, size_so_far))
       {
         goto bad_end;
       }
     }

     if (!WinHttpReadData(https->request, ret.e + old_size, size_to_read, &bytes_read))
     {
       goto bad_end;
     }

   }

   return ret;

bad_end:
   arena_revert(mark);
   return buffer_bad();

}


static void
https_end(https_t* https)
{
  WinHttpCloseHandle(https->request);
  WinHttpCloseHandle(https->connection);
  WinHttpCloseHandle(https->session);
}


int main() 
{
  arena_t arena;
  https_t https;

  arena_alloc(&arena, gigabytes(1), false);

  https_begin(&https);
  https_connect(&https, L"google.com");
  buffer_t res = https_request(&https, HTTPS_REQUEST_TYPE_GET, buffer_from_lit(L"/"), &arena);
  if (res)
  {
    for(int i = 0; i < res.size; ++i)
      printf("%c", res.e[i]);
    printf("\n");
  }
  https_end(&https);
}

