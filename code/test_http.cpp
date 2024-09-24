#include <stdio.h>
#include "momo.h"

struct https_t
{
  HINTERNET session;
  HINTERNET connection;
  HINTERNET request;
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


static str_t
https_request(
    https_t* https, 
    const wchar_t* request_type,
    const wchar_t* endpoint,
    arena_t* arena )
{
  arena_marker_t mark = arena_mark(arena);
  DWORD bytes_read;
  DWORD size_to_read;
  DWORD size_so_far = 0;
  str_t ret = str_bad();

  https->request = WinHttpOpenRequest(
    https->connection,
    request_type,
    endpoint,
    NULL,
    WINHTTP_NO_REFERER,
    WINHTTP_DEFAULT_ACCEPT_TYPES,
    WINHTTP_FLAG_SECURE);

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
       ret = arena_push_str(arena, size_so_far, 16);
       if (!ret) 
       {
         goto bad_end;
       }
     }

     else if (size_so_far > ret.size)
     {
       if (!arena_grow_str(arena, &ret, size_so_far))
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
   return str_bad();

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
  str_t res = https_request(&https, L"GET", L"/", &arena);
  if (res)
  {
    for(int i = 0; i < res.size; ++i)
      printf("%c", res.e[i]);
    printf("\n");
  }
  https_end(&https);
}

