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

#include "momo_common.h"

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

//////////////////////////////////////////////////////
// IMPLEMENTATION
//~ NOTE(Momo): 'Slow' series
// These are for quick and easy 
static U32
calc_crc32_slow(U8* data, U32 data_size, U32 start_register, U32 polynomial) {
	U32 r = start_register;
	for (U32 i = 0; i < data_size; ++i ){
		r ^= data[i] <<  24;
		for (U32 j = 0; j < 8; ++j) {
			if((r & 0x80000000) != 0) {
				r = (U32)((r << 1) ^ polynomial);
			}
			else {
				r <<= 1;
			}
		}
	}
	return r;
}


static U32
calc_crc16_slow(U8* data, U32 data_size, U16 start_register, U16 polynomial) {
	U32 r = start_register;
	for (U32 i = 0; i < data_size; ++i ){
		r ^= data[i] << 8;
		for (U32 j = 0; j < 8; ++j) {
			if((r & 0x8000) != 0) {
				r = (U32)((r << 1) ^ polynomial);
			}
			else {
				r <<= 1;
			}
		}
	}
	return r;
}

static U32
calc_crc8_slow(U8* data, U32 data_size, U16 start_register, U16 polynomial) {
	U32 r = start_register;
	for (U32 i = 0; i < data_size; ++i ){
		r ^= data[i];
		for (U32 j = 0; j < 8; ++j) {
			if((r & 0x80) != 0) {
				r = (U32)((r << 1) ^ polynomial);
			}
			else {
				r <<= 1;
			}
		}
	}
	return r;
}

//~ NOTE(Momo): Table generation 

static CRC32_Table
gen_crc32_table(U32 polynomial) {
	CRC32_Table table = {};
	for (U32 divident = 0; divident < 256; ++divident) {
		U32 remainder = divident <<  24;
		for (U32 j = 0; j < 8; ++j) {
			if((remainder & 0x80000000) != 0) {
				remainder = (U32)((remainder << 1) ^ polynomial);
			}
			else {
				remainder <<= 1;
			}
		}
		table.remainders[divident] = remainder;
	}
	return table;
}

static CRC16_Table
gen_crc16_table(U16 polynomial) {
	CRC16_Table table = {};
	for (U32 divident = 0; divident < 256; ++divident) {
		U32 remainder = divident << 8;
		for (U32 j = 0; j < 8; ++j) {
			if((remainder & 0x8000) != 0) {
				remainder = (U16)((remainder << 1) ^ polynomial);
			}
			else {
				remainder <<= 1;
			}
		}
		table.remainders[divident] = (U16)remainder;
	}
	return table;
}

static CRC8_Table
gen_crc8_table(U8 polynomial) {
	CRC8_Table table = {};
	for (U32 divident = 0; divident < 256; ++divident) {
		U32 remainder = divident;
		for (U32 j = 0; j < 8; ++j) {
			if((remainder & 0x80) != 0) {
				remainder = (U8)((remainder << 1) ^ polynomial);
			}
			else {
				remainder <<= 1;
			}
		}
		table.remainders[divident] = (U8)remainder;
	}
	return table;
}

//~ NOTE(Momo): Fast series. Uses a table
static U32
calc_crc32(U8* data, U32 data_size, U16 start_register, CRC32_Table table) {
	U32 crc = start_register;
	for (U32 i = 0; i < data_size; ++i) {
		U32 divident = (U32)((crc ^ (data[i] << 24)) >> 24);
		crc = (U32)((crc << 8) ^ (U32)(table.remainders[divident]));
	}
	return crc;
}

static U32
calc_crc16(U8* data, U32 data_size, U16 start_register, CRC16_Table table) {
	U16 crc = start_register;
	for (U32 i = 0; i < data_size; ++i) {
		U16 divident = (U16)((crc ^ (data[i] << 8)) >> 8);
		crc = (U16)((crc << 8) ^ (U16)(table.remainders[divident]));
	}
	return crc;
}

static U32
calc_crc8(U8* data, U32 data_size, U8 start_register, CRC8_Table table) {
	U8 crc = start_register;
	for (U32 i = 0; i < data_size; ++i) {
		U8 divident = (U8)(crc ^ data[i]);
		crc = table.remainders[divident];
	}
	return crc;
}

#endif //MOMO_CRC_H
