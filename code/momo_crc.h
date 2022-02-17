// Authors: Gerald Wong, momodevelop
// 
// This file contains CRC calculation functions.
//
// Notes:
// - calc_crcXX() functions require you to generate CRC tables
//   first with gen_crc_table() series of functions.
// 
// Todo:
// - 
// 

#ifndef MOMO_CRC_H
#define MOMO_CRC_H

struct CRC32_Table {
	U32 remainders[256];
};

struct CRC16_Table {
	U16 remainders[256];
};

struct CRC8_Table {
	U8 remainders[256];
};

// We should be able to constexpr these
static CRC32_Table gen_crc32_table(U32 polynomial);
static CRC16_Table gen_crc16_table(U16 polynomial);
static CRC8_Table  gen_crc8_table(U8 polynomial); 

// These require the CRCTables
static U32 calc_crc32(U8* data, U32 data_size, U16 start_register, CRC32_Table table);
static U32 calc_crc16(U8* data, U32 data_size, U16 start_register, CRC16_Table table);
static U32 calc_crc8(U8* data, U32 data_size, U8 start_register, CRC8_Table table);

static U32 calc_crc32_slow(U8* data, U32 data_size, U32 start_register, U32 polynomial);
static U32 calc_crc16_slow(U8* data, U32 data_size, U16 start_register, U16 polynomial);
static U32 calc_crc8_slow(U8* data, U32 data_size, U16 start_register, U16 polynomial);

#include "momo_crc.cpp"

#endif //MOMO_CRC_H
