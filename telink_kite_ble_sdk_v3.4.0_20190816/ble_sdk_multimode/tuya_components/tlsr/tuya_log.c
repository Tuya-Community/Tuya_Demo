/*
 * This file is part of the TUYA BLE SDK.
 *
 * Copyright (c) tuya corporation
 * Author: lsy
 * Created on: 2019-03-22
 * note: tuya log½Ó¿Ú
 */
#include "tuya_ble_type.h"
#include <stdarg.h>

#include "elog.h"



void tuya_log_dumpHex(const char *name, uint8_t width, uint8_t *buf, uint16_t size)
{
	elog_hexdump(name, width, buf, size);
}	
void tuya_log_init()
{
	/* initialize EasyLogger */
    elog_init();
    /* set EasyLogger log format */
    elog_set_fmt(ELOG_LVL_ASSERT,  ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME|ELOG_FMT_FUNC|ELOG_FMT_LINE);
    elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME|ELOG_FMT_FUNC|ELOG_FMT_LINE);
    elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME|ELOG_FMT_FUNC|ELOG_FMT_LINE);
    elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_TIME);
    //elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_LVL);
    //elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_LVL);
    /* start EasyLogger */
    elog_start();
    
    /* dynamic set enable or disable for output logs (true or false) */
//    elog_set_output_enabled(false);
    /* dynamic set output logs's level (from ELOG_LVL_ASSERT to ELOG_LVL_VERBOSE) */
//    elog_set_filter_lvl(ELOG_LVL_WARN);
    /* dynamic set output logs's filter for tag */
//    elog_set_filter_tag("main");
    /* dynamic set output logs's filter for keyword */
//    elog_set_filter_kw("Hello");
}
