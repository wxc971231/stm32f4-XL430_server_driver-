#ifndef __CRC_H
#define __CRC_H

#include "sys.h" 

u16 update_crc(u16 crc_accum,u8 *data_blk_ptr, u16 data_blk_size);
void setCrc(u8 *buf,u16 size);

#endif
