/* date = november 19th 2021 3:20 pm */

#ifndef MOMO_CRC_H
#define MOMO_CRC_H

struct CRC32Table {
	U32 remainders[256];
};

struct CRC16Table {
	U16 remainders[256];
};

struct CRC8Table {
	U8 remainders[256];
};

// We should be able to constexpr these
static CRC32Table GenerateCRC32Table(U32 polynomial);
static CRC16Table GenerateCRC32Table(U16 polynomial);
static CRC8Table  GenerateCRC8Table(U8 polynomial); 

// These require the CRCTables
static U32 CRC32(U8* data, U32 data_size, U16 start_register, CRC32Table table);
static U32 CRC16(U8* data, U32 data_size, U16 start_register, CRC16Table table);
static U32 CRC8(U8* data, U32 data_size, U8 start_register, CRC8Table table);

static U32 SlowCRC32(U8* data, U32 data_size, U32 start_register, U32 polynomial);
static U32 SlowCRC16(U8* data, U32 data_size, U16 start_register, U16 polynomial);
static U32 SlowCRC8(U8* data, U32 data_size, U16 start_register, U16 polynomial);

#include "momo_crc.cpp"

#endif //MOMO_CRC_H
