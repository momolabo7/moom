/* date = April 7th 2022 11:52 am */

#ifndef WIN_H
#define WIN_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#define NOMINMAX
#define CINTERFACE
#define COBJMACROS
#define INITGUID
#define COBJMACROS
#define CONST_VTABLE
#include <windows.h>
#include <timeapi.h>
#include <imm.h>
#include <initguid.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#undef near
#undef far

#include "momo.h"
#include "game_platform.h"
#include "game_profiler.h"


#include "w32_common.h"
#include "w32_gfx_opengl.h"
#include "w32_audio.h"
#include "w32_loaded_code.h"

#endif //WIN_H
