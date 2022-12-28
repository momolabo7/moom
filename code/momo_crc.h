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


struct crc32_table_t {
  u32_t remainders[256];
};

struct crc16_table_t {
  u16_t remainders[256];
};

struct crc8_table_t {
  u8_t remainders[256];
};

static void crc32_init_table(crc32_table_t* table, u32_t polynomial);
static void crc16_init_table(crc16_table_t* table, u16_t polynomial);
static void crc8_init_table(crc8_table_t* table, u8_t polynomial); 

// These require the CRCXX_Tables
static u32_t crc32(u8_t* data, u32_t data_size, u16_t start_register, crc32_table_t* table);
static u32_t crc16(u8_t* data, u32_t data_size, u16_t start_register, crc16_table_t* table);
static u32_t crc8(u8_t* data, u32_t data_size, u8_t start_register, crc8_table_t* table);

static u32_t crc32_slow(u8_t* data, u32_t data_size, u32_t start_register, u32_t polynomial);
static u32_t crc16_slow(u8_t* data, u32_t data_size, u16_t start_register, u16_t polynomial);
static u32_t crc8_slow(u8_t* data, u32_t data_size, u16_t start_register, u16_t polynomial);

//////////////////////////////////////////////////////
// IMPLEMENTATION
//~ NOTE(Momo): 'Slow' series
// These are for quick and easy 
static u32_t
crc32_slow(u8_t* data, u32_t data_size, u32_t start_register, u32_t polynomial) {
  u32_t r = start_register;
  for (u32_t i = 0; i < data_size; ++i ){
    r ^= data[i] <<  24;
    for (u32_t j = 0; j < 8; ++j) {
      if((r & 0x80000000) != 0) {
        r = (u32_t)((r << 1) ^ polynomial);
      }
      else {
        r <<= 1;
      }
    }
  }
  return r;
}


static u32_t
crc16_slow(u8_t* data, u32_t data_size, u16_t start_register, u16_t polynomial) {
  u32_t r = start_register;
  for (u32_t i = 0; i < data_size; ++i ){
    r ^= data[i] << 8;
    for (u32_t j = 0; j < 8; ++j) {
      if((r & 0x8000) != 0) {
        r = (u32_t)((r << 1) ^ polynomial);
      }
      else {
        r <<= 1;
      }
    }
  }
  return r;
}

static u32_t
crc8_slow(u8_t* data, u32_t data_size, u16_t start_register, u16_t polynomial) {
  u32_t r = start_register;
  for (u32_t i = 0; i < data_size; ++i ){
    r ^= data[i];
    for (u32_t j = 0; j < 8; ++j) {
      if((r & 0x80) != 0) {
        r = (u32_t)((r << 1) ^ polynomial);
      }
      else {
        r <<= 1;
      }
    }
  }
  return r;
}

//~ NOTE(Momo): Table generation 
static void 
crc32_init_table(crc32_table_t* table,u32_t polynomial) {
  for (u32_t divident = 0; divident < 256; ++divident) {
    u32_t remainder = divident <<  24;
    for (u32_t j = 0; j < 8; ++j) {
      if((remainder & 0x80000000) != 0) {
        remainder = (u32_t)((remainder << 1) ^ polynomial);
      }
      else {
        remainder <<= 1;
      }
    }
    table->remainders[divident] = remainder;
  }
}

static void
crc16_init_table(crc16_table_t* table, u16_t polynomial) {
  for (u32_t divident = 0; divident < 256; ++divident) {
    u32_t remainder = divident << 8;
    for (u32_t j = 0; j < 8; ++j) {
      if((remainder & 0x8000) != 0) {
        remainder = (u16_t)((remainder << 1) ^ polynomial);
      }
      else {
        remainder <<= 1;
      }
    }
    table->remainders[divident] = (u16_t)remainder;
  }
}

static void 
crc8_init_table(crc8_table_t* table, u8_t polynomial) {
  for (u32_t divident = 0; divident < 256; ++divident) {
    u32_t remainder = divident;
    for (u32_t j = 0; j < 8; ++j) {
      if((remainder & 0x80) != 0) {
        remainder = (u8_t)((remainder << 1) ^ polynomial);
      }
      else {
        remainder <<= 1;
      }
    }
    table->remainders[divident] = (u8_t)remainder;
  }
}

//~ NOTE(Momo): Fast series. Uses a table
static u32_t
crc32(u8_t* data, u32_t data_size, u16_t start_register, crc32_table_t* table) {
  u32_t crc = start_register;
  for (u32_t i = 0; i < data_size; ++i) {
    u32_t divident = (u32_t)((crc ^ (data[i] << 24)) >> 24);
    crc = (u32_t)((crc << 8) ^ (u32_t)(table->remainders[divident]));
  }
  return crc;
}

static u32_t
crc16(u8_t* data, u32_t data_size, u16_t start_register, crc16_table_t* table) {
  u16_t crc = start_register;
  for (u32_t i = 0; i < data_size; ++i) {
    u16_t divident = (u16_t)((crc ^ (data[i] << 8)) >> 8);
    crc = (u16_t)((crc << 8) ^ (u16_t)(table->remainders[divident]));
  }
  return crc;
}

static u32_t
crc8(u8_t* data, u32_t data_size, u8_t start_register, crc8_table_t* table) {
  u8_t crc = start_register;
  for (u32_t i = 0; i < data_size; ++i) {
    u8_t divident = (u8_t)(crc ^ data[i]);
    crc = table->remainders[divident];
  }
  return crc;
}

#endif //MOMO_CRC_H
