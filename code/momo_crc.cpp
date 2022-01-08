
//~ NOTE(Momo): 'Slow' series
// These are for quick and easy 
static U32
SlowCRC32(U8* data, U32 data_size, U32 start_register, U32 polynomial) {
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
SlowCRC16(U8* data, U32 data_size, U16 start_register, U16 polynomial) {
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
SlowCRC8(U8* data, U32 data_size, U16 start_register, U16 polynomial) {
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

static CRC32Table
GenerateCRC32Table(U32 polynomial) {
	CRC32Table table = {};
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

static CRC16Table
GenerateCRC16Table(U16 polynomial) {
	CRC16Table table = {};
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

static CRC8Table
GenerateCRC8Table(U8 polynomial) {
	CRC8Table table = {};
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
CRC32(U8* data, U32 data_size, U16 start_register, CRC32Table table) {
	U32 crc = start_register;
	for (U32 i = 0; i < data_size; ++i) {
		U32 divident = (U32)((crc ^ (data[i] << 24)) >> 24);
		crc = (U32)((crc << 8) ^ (U32)(table.remainders[divident]));
	}
	return crc;
}

static U32
CRC16(U8* data, U32 data_size, U16 start_register, CRC16Table table) {
	U16 crc = start_register;
	for (U32 i = 0; i < data_size; ++i) {
		U16 divident = (U16)((crc ^ (data[i] << 8)) >> 8);
		crc = (U16)((crc << 8) ^ (U16)(table.remainders[divident]));
	}
	return crc;
}

static U32
CRC8(U8* data, U32 data_size, U8 start_register, CRC8Table table) {
	U8 crc = start_register;
	for (U32 i = 0; i < data_size; ++i) {
		U8 divident = (U8)(crc ^ data[i]);
		crc = table.remainders[divident];
	}
	return crc;
}
