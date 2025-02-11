#include "momo.h"

static b32_t 
buf_to_vars(buf_t stream, buf_t fmt, ...)
{
  // @note: this is the reckless kind of implementation which
  // assumes that the user's format is correct.
  // By right, we should terminate when 'at' is bad.
  va_list args;
  va_start(args, fmt);
  defer { va_end(args); };

  u32_t fmt_at = 0, stream_at = 0;
  while (fmt_at < fmt.size && stream_at < stream.size)
  {

    if (fmt.e[fmt_at] == '%')
    {
      ++fmt_at;

      if (fmt.e[fmt_at] == 'd')
      {
        s32_t* out = va_arg(args, s32_t*);
        u32_t ope = stream_at;
        while(true)
        {
          if (!u8_is_digit(stream.e[ope]))
          {
            break;
          }
          ++ope;
        }
        buf_t range_to_convert = buf_slice(stream, stream_at, ope);
        if (!buf_to_s32(range_to_convert, out))
        {
          return false;
        }
        stream_at = ope;
        ++fmt_at;
      }
      else if (fmt.e[fmt_at] == 'u')
      {
        u32_t* out = va_arg(args, u32_t*);
        u32_t ope = stream_at;
        while(true)
        {
          if (!u8_is_digit(stream.e[ope]))
          {
            break;
          }
          ++ope;
        }
        buf_t range_to_convert = buf_slice(stream, stream_at, ope);
        if (!buf_to_u32(range_to_convert, out))
        {
          return false;
        }
        stream_at = ope;
        ++fmt_at;
      }
      else if (fmt.e[fmt_at] == 'f')
      {
        f32_t* out = va_arg(args, f32_t*);
        u32_t ope = stream_at;
        while(true)
        {
          if (u8_is_digit(stream.e[ope]) || stream.e[ope] == '.')
          {
            ++ope;
          }
          else
          {
            break;
          }
        }
        buf_t range_to_convert = buf_slice(stream, stream_at, ope);
        if (!buf_to_f32(range_to_convert, out))
        {
          return false;
        }
        stream_at = ope;
        ++fmt_at;
      }
      else 
      {
        ++stream_at;
        ++fmt_at;
      }
    }

    else 
    {
      ++stream_at;
      ++fmt_at;
    }
  }

  return true;
}

#include <stdio.h>

int main() 
{
  buf_t a = buf_from_lit("wtf 123 123 1.23");
  buf_t b = buf_from_lit("wtf %d %d %f");

  s32_t c, d;
  f32_t e;
  buf_to_vars(a, b, &c, &d, &e);
  printf("%d %d %f\n", c, d, e);

}
