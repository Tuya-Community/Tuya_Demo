#include "tl_common.h"
#include "drivers.h"
#include "stack/ble/ble.h"
///#include "spp.h"
//#include "battery_check.h"
#include "vendor/common/blt_common.h"
#include "tuya_log.h"
#include "nv.h"

//#if(MODULE_WATCHDOG_ENABLE)
//	#include "../../proj/mcu/watchdog_i.h"
//#endif

#define MODULE_FLASH_ENABLE 1

#if (MODULE_FLASH_ENABLE)

#define ENABLE_CHECK_SUM        0

extern const u8 protect_flash_cmd;


void nv_flashWrite(u32 startAddr, u16 len, u8 *buf) {
	u16 writeLen = (len > PAGE_AVALIABLE_SIZE(startAddr)) ? PAGE_AVALIABLE_SIZE(startAddr) : len;
#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
	//tuya_log_d("nv_flashWrite1-%08x,%d",startAddr,len);
	flash_write_page(startAddr, writeLen, buf);

	if(len > PAGE_AVALIABLE_SIZE(startAddr)) {
		u32 dstAddr = startAddr + writeLen;
		u16 dataOffset = writeLen;
		len -= writeLen;

		while(len > 0) {
			writeLen = (len > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : len;
			flash_write_page(dstAddr, writeLen, buf + dataOffset);

			if(len > FLASH_PAGE_SIZE) {
				len -= FLASH_PAGE_SIZE;
				dataOffset += FLASH_PAGE_SIZE;
				dstAddr += FLASH_PAGE_SIZE;
			}
			else {
				len = 0;
			}
		}
	}
	// tuya_log_d("nv_flashWrite2");
}

nv_sts_t nv_init(u8 rst) {
	if(rst) {
		/* if reset is true, erase all flash for NV */
		u8 i;

		for(i = 0; i < NV_MAX_MOULDS; i++) {
#if(MODULE_WATCHDOG_ENABLE)
			wd_clear();
#endif
			flash_erase_sector(MOUDLES_START_ADDR(i));
		}
	}
}

u8 nv_calculateCheckSum(u8 *buf, u16 len) {
	u8 checkSum = *buf;
	u16 i;

	for(i = 1; i < len; i++) {
		checkSum ^= buf[i];
	}

	return checkSum;
}

/* find the item address in nv */
/*************************************
 * @brief	find stored items
 *
 * @param	modules - search index
 * 			id - item id
 * 			itemmInfo - strut used to store find item infor
 * 			verify - verify data
 * 	@return	find result
 */
nv_sts_t nv_findItem(u8 modules, u8 id, item_info_t *itemInfo, u8 verify) {
	u8 j;
	u16 usedSize = 0;
	u16 verifySize = 0;
	u8 idExist = 0;
	u8 buf[NV_HEADER_SIZE];
	nv_header_t *nvHdrPtr;
	u32 readAddr;
	flash_read_page(MOUDLES_START_ADDR(modules), sizeof(u16), (u8 *)&usedSize);
	itemInfo->usedSize = usedSize;

	/* find the id page which contain the required id */
	for(j = 0; j < NV_HEADER_TABLE_SIZE; j++) {
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
		readAddr = MOUDLES_START_ADDR(modules) + sizeof(u16) + NV_HEADER_SIZE * j;
		flash_read_page(readAddr, NV_HEADER_SIZE, buf);
		nvHdrPtr = (nv_header_t *)buf;
		itemInfo->nvOffset = sizeof(u16) + j * NV_HEADER_SIZE;

		/* if the id is found, return it */
		if(nvHdrPtr->id == id) {
			memcpy(&(itemInfo->hdrInfo), nvHdrPtr, sizeof(nv_header_t));

			if(!verify) {
				return SUCCESS;
			}
			else {
				idExist = 1;
			}
		}
		else if(nvHdrPtr->id == INVALID_NV_VALUE) {//no more items followed
			if(!verify) {
				itemInfo->hdrInfo.id = INVALID_NV_VALUE;
				return SUCCESS;
			}
			else {
				if(idExist == 0) {
					itemInfo->hdrInfo.id = INVALID_NV_VALUE;
				}

				if(verifySize == usedSize || usedSize == 0xffff) {
					return SUCCESS;
				}
				else {
					return NV_ITEM_LEN_NOT_MATCH;
				}
			}
		}

		if(verify) {
			verifySize = nvHdrPtr->offset + nvHdrPtr->len;
		}
	}

	return SUCCESS;
}

void nv_xferData(u32 srcAddr, u32 dstAddr, u16 size, u8 eraseFlag) {
	u32 srcA = srcAddr;
	u32 dstA = dstAddr;
	u16 readSize;

	u8 tmp[READ_BYTES_PER_TIME];
	u16 xferLen;
	u32 xferSrc;
	u32 xferDst;
	u8 xferSize;

	if(eraseFlag == 1) {
		/* erase the dest sector */
		flash_erase_sector(dstAddr);
	}

	/* get the avaliable page size */
	readSize = PAGE_AVALIABLE_SIZE(srcAddr);
	readSize = (readSize > size) ? size : readSize;
#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif
	xferLen = readSize;
	xferSrc = srcAddr;
	xferDst = dstAddr;

	while(xferLen > 0) {
		xferSize = (xferLen > READ_BYTES_PER_TIME) ? READ_BYTES_PER_TIME : xferLen;
		flash_read_page(xferSrc, xferSize, tmp);
		flash_write_page(xferDst, xferSize, tmp);
		xferSrc += xferSize;
		xferDst += xferSize;

		if(xferLen > READ_BYTES_PER_TIME) {
			xferLen -= READ_BYTES_PER_TIME;
		}
		else {
			xferLen = 0;
		}

#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
	}

	if(size > PAGE_AVALIABLE_SIZE(srcAddr)) {
		size -= readSize;
		srcA += readSize;
		dstA += readSize;

		while(size > 0) {
			readSize = (size > FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : size;
			xferLen = readSize;
			xferSrc = srcA;
			xferDst = dstA;

			while(xferLen > 0) {
				xferSize = (xferLen > READ_BYTES_PER_TIME) ? READ_BYTES_PER_TIME : xferLen;
				flash_read_page(xferSrc, xferSize, tmp);
#if(MODULE_WATCHDOG_ENABLE)
				wd_clear();
#endif
				flash_write_page(xferDst, xferSize, tmp);
				xferSrc += xferSize;
				xferDst += xferSize;

				if(xferLen > READ_BYTES_PER_TIME) {
					xferLen -= READ_BYTES_PER_TIME;
				}
				else {
					xferLen = 0;
				}
			}

#if(MODULE_WATCHDOG_ENABLE)
			wd_clear();
#endif
			srcA += readSize;
			dstA += readSize;

			if(size > FLASH_PAGE_SIZE) {
				size -= FLASH_PAGE_SIZE;
			}
			else {
				size = 0;
			}
		}
	}
}

nv_sts_t nv_write(u8 modules, u8 id, u16 len, u8 *buf) {
	item_info_t itemInfo;

    tuya_log_d("nv_write:modules=0x%0x,,id=0x%0x,len=0x%0x",modules,id,len);

	if(id == INVALID_NV_VALUE) {
		return NV_INVALID_ID;
	}

	if(modules >= NV_MAX_MOULDS) {
		return NV_INVALID_MODULS;
	}

	u8 r = irq_disable();

	if(SUCCESS != nv_findItem(modules, id, &itemInfo, 1)) {
		irq_restore(r);
		return NV_ITEM_LEN_NOT_MATCH;
	}


	nv_header_t *nvHdr = &(itemInfo.hdrInfo);

	/* if no space left in flash for NV */
	if((itemInfo.hdrInfo.id == INVALID_NV_VALUE) && (itemInfo.usedSize != 0xffff)
	        && (MOUDLES_AVALIABLE_SIZE(itemInfo.usedSize) < NV_ALIGN_LENTH(len + 1))) {
		irq_restore(r);
		return NV_NOT_ENOUGH_SAPCE;
	}

	/* if the NV item is already in flash */
	if(nvHdr->id == id) {
		/* check the length */
		if(nvHdr->len != NV_ALIGN_LENTH(len + 1)) {
			irq_restore(r);
			return NV_ITEM_LEN_NOT_MATCH;
		}
		/* back up the flash data */
		nv_xferData(MOUDLES_START_ADDR(modules), MOUDLES_START_ADDR(NV_MAX_MOULDS), itemInfo.usedSize, 1);
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif

		/* write back the restored data before this item */
		nv_xferData(MOUDLES_START_ADDR(NV_MAX_MOULDS), MOUDLES_START_ADDR(modules), itemInfo.hdrInfo.offset, 1);
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif

		/* if there are still some data after the item */
		if(itemInfo.hdrInfo.offset + itemInfo.hdrInfo.len < itemInfo.usedSize) {
			/* write bakc the restored data after this item */
			nv_xferData(MOUDLES_START_ADDR(NV_MAX_MOULDS) + itemInfo.hdrInfo.offset + itemInfo.hdrInfo.len,
			            MOUDLES_START_ADDR(modules) + itemInfo.hdrInfo.offset + itemInfo.hdrInfo.len,
			            itemInfo.usedSize - itemInfo.hdrInfo.offset - itemInfo.hdrInfo.len, 0);
		}


#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
		/* update the NV item */
		nv_flashWrite(MOUDLES_START_ADDR(modules) + itemInfo.hdrInfo.offset, len, buf);

#if ENABLE_CHECK_SUM
		u8 checksum = nv_calculateCheckSum(buf, len);
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
		/* write the fcs */
		nv_flashWrite(MOUDLES_START_ADDR(NV_MAX_MOULDS) + itemInfo.hdrInfo.offset + len, 1, &checksum);
#endif
		irq_restore(r);
		return SUCCESS;
	}
	/* it is a new NV item in flash */
	else {
		u16 usedSize;
		/* update the nv header info */
		nvHdr->id = id;
		nvHdr->len = NV_ALIGN_LENTH(len + 1);

		if(itemInfo.usedSize == 0xffff) {
			nvHdr->offset = PAGE_HEADER_SISE;
			usedSize = nvHdr->len + PAGE_HEADER_SISE;
			itemInfo.usedSize = PAGE_HEADER_SISE;
		}
		else {
			nvHdr->offset = itemInfo.usedSize;
			usedSize = itemInfo.usedSize + nvHdr->len;
		}

		/* back up the flash data */
		nv_xferData(MOUDLES_START_ADDR(modules), MOUDLES_START_ADDR(NV_MAX_MOULDS), itemInfo.usedSize, 1);
		/* erase the dest sector */
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
		flash_erase_sector(MOUDLES_START_ADDR(modules));
		nv_xferData(MOUDLES_START_ADDR(NV_MAX_MOULDS) + sizeof(u16), MOUDLES_START_ADDR(modules) + sizeof(u16), itemInfo.nvOffset, 0);

#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif

		/* save page header info */
		flash_write_page(MOUDLES_START_ADDR(modules), sizeof(u16), (u8 *)&usedSize);

		flash_write_page(MOUDLES_START_ADDR(modules) + itemInfo.nvOffset, sizeof(nv_header_t), (u8 *)nvHdr);

#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
		nv_xferData(MOUDLES_START_ADDR(NV_MAX_MOULDS) + itemInfo.nvOffset + sizeof(nv_header_t),
		            MOUDLES_START_ADDR(modules) + itemInfo.nvOffset + sizeof(nv_header_t),
		            itemInfo.usedSize - itemInfo.nvOffset - sizeof(nv_header_t), 0);


		/* save to Flash */
		nv_flashWrite(MOUDLES_START_ADDR(modules) + nvHdr->offset, len, buf);
#if ENABLE_CHECK_SUM
		u8 checksum = nv_calculateCheckSum(buf, len);
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
		nv_flashWrite(MOUDLES_START_ADDR(modules) + nvHdr->offset + len, 1, &checksum);
#endif
		//flash_read_page(itemInfo.hdrAddr, sizeof(nv_header_t), (u8*)(&t_itemInfo.hdrInfo));
		irq_restore(r);

		/* protect the flash */
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
		return SUCCESS;
	}
}

nv_sts_t nv_read(u8 modules, u8 id, u16 len, u8 *buf) {
	item_info_t itemInfo;
	u32 readAddr;
	nv_header_t *nvHdr = &(itemInfo.hdrInfo);

	/* id should be bigger than 0 */
	if(id == INVALID_NV_VALUE) {
		return NV_INVALID_ID;
	}

	u8 r = irq_disable();
	nv_findItem(modules, id, &itemInfo, 0);

#if(MODULE_WATCHDOG_ENABLE)
	wd_clear();
#endif

	if(nvHdr->id == INVALID_NV_VALUE) {
		irq_restore(r);
		return NV_ITEM_NOT_FOUND;
	}

	/* read the nv item from flash */
	if(NV_ALIGN_LENTH(len + 1) == nvHdr->len) {
		/* read from Flash */
		readAddr = MOUDLES_START_ADDR(modules) + nvHdr->offset;
        flash_read_page(readAddr,len,buf);
		/*foreach(i, len) {
			buf[i] = read_reg8(readAddr + i);
            ty_read_buffer[i] = read_reg8(readAddr + i);
		}*///error for read;

#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
#if ENABLE_CHECK_SUM
		u8 checksum = read_reg8((MOUDLES_START_ADDR(modules) + nvHdr->offset + len));

		if(checksum != nv_calculateCheckSum(buf, len)) {
			irq_restore(r);
			return NV_CHECK_SUM_ERROR;
		}

#endif
#if(MODULE_WATCHDOG_ENABLE)
		wd_clear();
#endif
		irq_restore(r);

		return SUCCESS;
	}
	else {
		irq_restore(r);
		return NV_ITEM_LEN_NOT_MATCH;
	}
}

nv_sts_t nv_resetModule(u8 modules) {
	flash_erase_sector(MOUDLES_START_ADDR(modules));
	return SUCCESS;
}

nv_sts_t nv_resetAll(void) {
	foreach(i, NV_MAX_MOULDS) {
		nv_resetModule(i);
	}

	return SUCCESS;
}


#endif
