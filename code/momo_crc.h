/* date = November 19th 2021 3:20 pm */

#ifndef MOMO_CRC_H
#define MOMO_CRC_H

typedef struct CRC32_Table {
	U32 remainders[256];
} CRC32_Table ;

typedef struct CRC16_Table {
	U16 remainders[256];
} CRC16_Table;

typedef struct CRC8_Table {
	U8 remainders[256];
} CRC8_Table;

static U32 CRC32_Slow(U8* data, U32 data_size, U32 start_register, U32 polynomial);
static U32 CRC16_Slow(U8* data, U32 data_size, U16 start_register, U16 polynomial);
static U32 CRC8_Slow(U8* data, U32 data_size, U16 start_register, U16 polynomial);

static CRC32_Table CRC32_GenerateTable(U32 polynomial);
static CRC16_Table CRC16_GenerateTable(U16 polynomial);
static CRC8_Table CRC8_GenerateTable(U8 polynomial); 

static U32 CRC32(U8* data, U32 data_size, U16 start_register, CRC32_Table table);
static U32 CRC16(U8* data, U32 data_size, U16 start_register, CRC16_Table table);
static U32 CRC8(U8* data, U32 data_size, U8 start_register, CRC8_Table table);

#endif //MOMO_CRC_H
