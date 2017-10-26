/*
 * File:   MatchCode.h
 * Author: liyilin
 *
 * Created on August 29, 2016, 9:42 AM
 */

#ifndef MATCHCODE_H
#define	MATCHCODE_H
#include "BitStream.h"

static CBitCode BC_DYNA_TIMETDIFF[] =
{
	{0x00,	2,	'C',	0,		3},			// 00				= 0
	{0x04,	3,	'C',	0,		6},			// 100				= 1
	{0x05,	3,	'C',	0,		2},			// 101				= 2
	{0x01,	2,	'D',	4,		0},			// 01	+4Bit		= 4Bit
	{0x06,	3,	'D',	6,		16},		// 110	+6Bit		= 6Bit+16
	{0x0E,	4,	'D',	10,		80},		// 1110	+10Bit		= 10Bit+64+16
	{0x0F,	4,	'O',	32,		0},			// 1111	+24Bit		= 24Bit Org
};
static int BCNUM_DYNA_TIMETDIFF = sizeof(BC_DYNA_TIMETDIFF)/sizeof(CBitCode);

static CBitCode BC_DYNA_TIMEDIFF[] =
{
	{0x00,	1,	'C',	0,		3},			// 0				= 3s
	{0x06,	3,	'C',	0,		6},			// 110				= 6s
	{0x02,	2,	'D',	4,		0},			// 10	+4Bit		= 4Bit
	{0x07,	3,	'O',	18,		0},			// 111	+18Bit		= 18Bit Org
};
static int BCNUM_DYNA_TIMEDIFF = sizeof(BC_DYNA_TIMEDIFF)/sizeof(CBitCode);

static CBitCode BC_DYNA_PRICE_DIFFS[] =
{
	{0x00,	1,	'I',	4,		0},			// 0	+4Bit		= 4Bit
	{0x02,	2,	'I',	6,		8},			// 10	+6Bit		= 6Bit+8
	{0x06,	3,	'I',	8,		40},		// 110	+8Bit		= 8Bit+32+8
	{0x0E,	4,	'I',	16,		168},		// 1110	+16Bit		= 16Bit+128+32+8
	{0x0F,	4,	'O',	32,		0},			// 1111 +32Bit		= 32Bit Org
};
static int BCNUM_DYNA_PRICE_DIFFS = sizeof(BC_DYNA_PRICE_DIFFS)/sizeof(CBitCode);

static CBitCode BC_DYNA_PRICE_DIFF[] =
{
	{0x00,	1,	'D',	4,		0},			// 0	+4Bit		= 4Bit
	{0x02,	2,	'D',	6,		16},		// 10	+6Bit		= 6Bit+16
	{0x06,	3,	'D',	8,		80},		// 110	+8Bit		= 8Bit+64+16
	{0x0E,	4,	'D',	16,		336},		// 1110+16Bit		= 16Bit+256+64+16
	{0x0F,	4,	'O',	32,		0},			// 1111+32Bit		= 32Bit Org
};
static int BCNUM_DYNA_PRICE_DIFF = sizeof(BC_DYNA_PRICE_DIFF)/sizeof(CBitCode);

static CBitCode BC_DYNA_VOL[] =
{
	{0x06,	3,	'D',	12,		0},			// 110	+12Bit		= 12Bit
	{0x02,	2,	'D',	16,		4096},		// 10	+16Bit		= 16Bit+4096
	{0x00,	1,	'D',	24,		69632},		// 0	+24Bit		= 24Bit+65536+4096
	{0x07,	3,	'O',	32,		0},			// 111  +32Bit		= 32Bit Org
};
static int BCNUM_DYNA_VOL = sizeof(BC_DYNA_VOL)/sizeof(CBitCode);

static CBitCode BC_DYNA_VOL_DIFF[] =
{
	{0x02,	2,	'H',	4,		0},			// 10	+4Bit		= (4Bit)*100
	{0x00,	1,	'H',	8,		16},		// 0	+8Bit		= (8Bit+16)*100
	{0x06,	3,	'H',	12,		272},		// 110	+12Bit		= (12Bit+256+16)*100
	{0x1C,	5,	'D',	12,		0},			// 11100+12Bit		= 12Bit
	{0x1D,	5,	'D',	16,		4096},		// 11101+16Bit		= 16Bit+4096
	{0x1E,	5,	'D',	24,		69632},		// 11110+24Bit		= 24Bit+65536+4096
	{0x1F,	5,	'O',	32,		0},			// 11111+32Bit		= 32Bit Org
};
static int BCNUM_DYNA_VOL_DIFF = sizeof(BC_DYNA_VOL_DIFF)/sizeof(CBitCode);

static CBitCode BC_DYNA_VOL_DIFFS[] =
{
	{0x00,	1,	'h',	6,		0},			// 0	+6Bit		= (6Bit)*100
	{0x02,	2,	'h',	8,		32},		// 10	+8Bit		= (8Bit+32)*100
	{0x06,	3,	'h',	12,		160},		// 110	+12Bit		= (12Bit+128+32)*100
	{0x1C,	5,	'I',	12,		0},			// 11100+12Bit		= 12Bit
	{0x1D,	5,	'I',	16,		2048},		// 11101+16Bit		= 16Bit+2048
	{0x1E,	5,	'I',	24,		34816},		// 11110+24Bit		= 24Bit+32768+2048
	{0x1F,	5,	'O',	32,		0},			// 11111+32Bit		= 32Bit Org
};
static int BCNUM_DYNA_VOL_DIFFS = sizeof(BC_DYNA_VOL_DIFFS)/sizeof(CBitCode);

static CBitCode BC_DYNA_AMNT[] =
{
	{0x00,	2,	'I',	16,		0},			// 00	+16Bit		= 16Bit
	{0x01,	2,	'I',	24,		32768},		// 01	+24Bit		= 24Bit+32768
	{0x02,	2,	'I',	28,		8421376},	// 10	+28Bit		= 28Bit+8388608+32768
	{0x03,	2,	'O',	32,		0},			// 11	+32Bit		= 32Bit Org
};
static int BCNUM_DYNA_AMNT = sizeof(BC_DYNA_AMNT)/sizeof(CBitCode);


static CBitCode BC_DYNA_STRONG[] =
{
	{0x00,	2,	'I',	8,		0},			// 00	+8Bit		= 8Bit
	{0x01,	2,	'I',	11,		128},		// 01	+11Bit		= 11Bit+128
	{0x02,	2,	'I',	13,		1152},		// 10	+13Bit		= 13Bit+1024+128
	{0x03,	2,	'O',	32,		0},			// 11	+32Bit		= 32Bit Org
};
static int BCNUM_DYNA_STRONG = sizeof(BC_DYNA_STRONG)/sizeof(CBitCode);

static CBitCode BC_DYNA_STRONG_DIFF[] =
{
	{0x00,	1,	'I',	3,		0},			// 0	+3Bit		= 3Bit
	{0x02,	2,	'I',	6,		4},			// 10	+6Bit		= 6Bit+4
	{0x06,	3,	'I',	10,		36},		// 110	+10Bit		= 10Bit+32+4
	{0x0E,	4,	'I',	14,		548},		// 1110	+14Bit		= 14Bit+512+32+4
	{0x0F,	4,	'O',	32,		0},			// 1111	+32Bit		= 32Bit Org
};
static int BCNUM_DYNA_STRONG_DIFF = sizeof(BC_DYNA_STRONG_DIFF)/sizeof(CBitCode);

static CBitCode BC_STOCKNUM[] =
{
	{0x00,	1,	'D',	6,		0},			// 0	+6Bit		= 6Bit        (63)
	{0x02,	2,	'D',	9,		64},		// 10	+9Bit		= 9Bit+64     (575)
	{0x06,	3,	'D',	12,		576},		// 110	+12Bit		= 12Bit+512+64(4671)
	{0x07,	3,	'O',	32,		0},			// 111	+32Bit		= 32Bit Org
};
static int BCNUM_STOCKNUM = sizeof(BC_STOCKNUM)/sizeof(CBitCode);

static CBitCode BC_STOCKNUM_DIFF[] =
{
	{0x00,	1,	'C',	0,		0},			// 0				= 0
	{0x02,	2,	'I',	3,		0},			// 10	+3Bit		= 3Bit       (3)
	{0x06,	3,	'I',	6,		4},			// 110	+6Bit		= 6Bit+4     (35)
	{0x0E,	4,	'I',	12,		36},		// 1110	+12Bit		= 12Bit+32+4 (2083)
	{0x0F,	4,	'O',	32,		0},			// 1111	+32Bit		= 32Bit Org
};
static int BCNUM_STOCKNUM_DIFF = sizeof(BC_STOCKNUM_DIFF)/sizeof(CBitCode);

static CBitCode BC_DYNA_AMNT_DIFF[] =
{
	{0x00,	2,	'I',	4,		0},			// 00	+4Bit		= 4Bit
	{0x01,	2,	'I',	8,		8},			// 01	+8Bit		= 8Bit+8
	{0x04,	3,	'I',	12,		136},		// 100	+12Bit		= 12Bit+128+8
	{0x05,	3,	'I',	16,		2184},		// 101	+16Bit		= 16Bit+2048+128+8
	{0x06,	3,	'I',	24,		34952},		// 110	+24Bit		= 24Bit+32768+2048+128+8
	{0x07,	3,	'O',	32,		0},			// 111	+32Bit		= 32Bit Org
};
static int BCNUM_DYNA_AMNT_DIFF = sizeof(BC_DYNA_AMNT_DIFF)/sizeof(CBitCode);

static CBitCode BC_TRADE_TRADENO[] =
{
	{0x00,	1,	'D',	4,		0},			// 0				= 4Bit
	{0x02,	2,	'D',	8,		16},		// 10				= 8Bit+16
	{0x03,	2,	'O',	16,		0},			// 11	+16Bit		= 16Bit Org
};
static int BCNUM_TRADE_TRADENO = sizeof(BC_TRADE_TRADENO)/sizeof(CBitCode);

static CBitCode BC_TRADE_PRICE[] =
{
	{0x02,	2,	'D',	8,		0},			//10	+8Bit		= 8Bit
	{0x00,	1,	'D',   16,		256},		//0		+16Bit		= 16Bit+256
	{0x06,	3,	'D',   24,		65792},		//110	+24Bit		= 24Bit+65536+256
	{0x07,	3,	'O',   32,		0},			//111	+32Bit		= 32Bit Org
};
static int BCNUM_TRADE_PRICE = sizeof(BC_TRADE_PRICE)/sizeof(CBitCode);

static CBitCode BC_TRANSACTION_TIMEDIFF[] =
{
	{0x00,	1,	'C',	0,		3},			// 0				= 3s
	{0x06,	3,	'C',	0,		6},			// 110				= 6s
	{0x02,	2,	'I',	4,		0},			// 10	+4Bit		= 4Bit
	{0x0E,	4,	'I',	8,		16},		// 1110	+8Bit		= 8Bit+16
	{0x0F,	4,	'O',	18,		0},			// 1111	+18Bit		= 18Bit Org
};
static int BCNUM_TRANSACTION_TIMEDIFF = sizeof(BC_TRANSACTION_TIMEDIFF)/sizeof(CBitCode);

static CBitCode BC_TRADE_PRICE_DIFFS[] =
{
	{0x00,	1,	'C',	0,		0},			// 0				= 0
	{0x02,	2,	'I',	2,		0},			// 10	+2Bit		= 2Bit
	{0x06,	3,	'I',	4,		2},			// 110	+4Bit		= 4Bit+2
	{0x0E,	4,	'I',	8,		10},		// 1110	+8Bit		= 8Bit+8+2
	{0x1E,	5,	'I',	16,		138},		// 11110+16Bit		= 16Bit+128+8+2
	{0x1F,	5,	'O',	32,		0},			// 11111+32Bit		= 32Bit Org
};
static int BCNUM_TRADE_PRICE_DIFFS = sizeof(BC_TRADE_PRICE_DIFFS)/sizeof(CBitCode);

static CBitCode BC_TRADE_VOL[] =
{
	{0x00,	1,	'H',	4,		1},			// 0	+4Bit		= (4Bit+1)*100
	{0x02,	2,	'H',	7,		17},		// 10	+7Bit		= (7Bit+16+1)*100
	{0x0C,	4,	'H',	10,		145},		// 1100	+10Bit		= (10Bit+128+16+1)*100
	{0x0D,	4,	'D',	8,		0},			// 1101	+8Bit		= 8Bit
	{0x0E,	4,	'D',	12,		256},		// 1110	+12Bit		= 12Bit+256
	{0x1E,	5,	'D',	16,		4352},		// 11110+16Bit		= 16Bit+4096+256
	{0x1F,	5,	'O',	32,		0},			// 11111+32Bit		= 32Bit Org
};
static int BCNUM_TRADE_VOL = sizeof(BC_TRADE_VOL)/sizeof(CBitCode);

static CBitCode BC_TRADE_NUMBER[] =
{
	{0x00,	1,	'C',	0,		1},					//0				= 1
	{0x02,	2,	'D',	7,		2},					//10	+7Bit	= 7Bit+2
	{0x06,	3,	'D',	12,		130},				//110	+12Bit	= 12Bit+128+2
	{0x0E,	4,	'D',	16,		4226},				//1110	+16Bit	= 16Bit+4096+128+2
	{0x0F,	4,	'O',	32,		0},					//1111	+32Bit	= 32Bit Org
};
static int BCNUM_TRADE_NUMBER = sizeof(BC_TRADE_NUMBER)/sizeof(CBitCode);

static CBitCode BC_BARGAIN_TIMEDIFF[] =
{
	{0x00,	1,	'C',	0,		3},			// 0				= 3s
	{0x06,	3,	'C',	0,		6},			// 110				= 6s
	{0x02,	2,	'D',	4,		0},			// 10	+4Bit		= 4Bit
	{0x0E,	4,	'D',	8,		16},		// 1110	+8Bit		= 8Bit+16
	{0x0F,	4,	'O',	18,		0},			// 1111	+18Bit		= 18Bit Org
};
static int BCNUM_BARGAIN_TIMEDIFF = sizeof(BC_BARGAIN_TIMEDIFF)/sizeof(CBitCode);

static CBitCode GENERAL_UNSIGNED_DATA_MATCH[] = {
    { 0x02, 2, 'D', 8, 0 },             // 10	+8Bit		= 8Bit
    { 0x00, 1, 'D', 12, 256 },          // 0    +12Bit		= 12Bit+256
    { 0x06, 3, 'D', 16, 4352 },         // 110	+16Bit		= 16Bit+4096+256
    { 0x0E, 4, 'D', 24, 69888 },        // 1110	+24Bit		= 24Bit+65536+4096+256
    { 0x0F, 4, 'O', 32, 0 },            // 1111	+32Bit		= 32Bit Org
};
static int GENERAL_UNSIGNED_DATA_MATCH_NUMBER = sizeof (GENERAL_UNSIGNED_DATA_MATCH) / sizeof (CBitCode);

static CBitCode GENERAL_UNSIGNED_DATA_DIFF[] = {
    { 0x00, 2, 'I', 4, 0 },             // 00	+4Bit		= 4Bit
    { 0x01, 2, 'I', 8, 8 },             // 01	+8Bit		= 8Bit+8
    { 0x04, 3, 'I', 12, 136 },          // 100	+12Bit		= 12Bit+128+8
    { 0x05, 3, 'I', 16, 2184 },         // 101	+16Bit		= 16Bit+2048+128+8
    { 0x06, 3, 'I', 24, 34952 },        // 110	+24Bit		= 24Bit+32768+2048+128+8
    { 0x07, 3, 'O', 32, 0 },            // 111	+32Bit		= 32Bit Org
};
static int GENERAL_UNSIGNED_DATA_DIFF_NUMBER = sizeof (GENERAL_UNSIGNED_DATA_DIFF) / sizeof (CBitCode);

static CBitCode GENERAL_UNSIGNED_SHORT_MATCH[] =
{
	{0x00,	1,	'D',	4,		0},			// 0	+4Bit		= 4Bit
	{0x02,	2,	'D',	6,		16},		// 10	+6Bit		= 6Bit+16
	{0x06,	3,	'D',	8,		80},		// 110	+8Bit		= 8Bit+64+16
	{0x0E,	4,	'D',	16,		336},		// 1110+16Bit		= 16Bit+256+64+16
	{0x0F,	4,	'O',	32,		0},			// 1111+32Bit		= 32Bit Org
};
static int GENERAL_UNSIGNED_SHORT_MATCH_NUMBER = sizeof(GENERAL_UNSIGNED_SHORT_MATCH) / sizeof(CBitCode);

/*
OrderKind:
{0x00,	'0'}
{0x01,	'2'}
{0x02,	'K'}
{0x03,	'V'}
{0x04,	'W'}
{0x05,	'X'}
{0x06,	'Y'}
*/
static int KEY_ORDER_KIND_BIT = 4;

/*
FunctionCode:
{0x00,	'B'}
{0x01,	'S'}
{0x02,	'C'}
*/
static int KEY_ORDER_FUNC_BIT = 2;

static CBitCode BC_DAY_NUMBER[] =
{
	{0x00,	1,	'C',	0,		1},					//0				= 1
	{0x02,	2,	'D',	7,		2},					//10	+7Bit	= 7Bit+2
	{0x06,	3,	'D',	12,		130},				//110	+12Bit	= 12Bit+128+2
	{0x0E,	4,	'D',	16,		4226},				//1110	+16Bit	= 16Bit+4096+128+2
	{0x0F,	4,	'O',	32,		0},					//1111	+32Bit	= 32Bit Org
};
static int BCNUM_DAY_NUMBER = sizeof(BC_DAY_NUMBER)/sizeof(CBitCode);

static CBitCode BC_DAY_TIME_MIND[] =
{
	{0x00,	1,	'C',	0,		0},					//0				= 0天
	{0x02,	2,	'C',	0,		1},					//10			= 1天
	{0x06,	3,	'C',	0,		3},					//110			= 3天
	{0x1C,	5,	'D',	4,		0},					//11100	+4Bit	= 4Bit天
	{0x1D,	5,	'D',	8,		16},				//11101	+8Bit	= 8Bit+16天
	{0x1E,	5,	'O',	16,		0},					//11110	+16Bit	= 16Bit天
};
static int BCNUM_DAY_TIME_MIND = sizeof(BC_DAY_TIME_MIND)/sizeof(CBitCode);

static CBitCode BC_MINDAY_PRICE[] =
{
	{0x00,	1,	'D',	8,		0},			//0	+8Bit		= 8Bit
	{0x02,	2,	'D',   16,		256},		//10		+16Bit		= 16Bit+256
	{0x06,	3,	'D',   24,		65792},		//110	+24Bit		= 24Bit+65536+256
	{0x07,	3,	'O',   32,		0},			//111	+32Bit		= 32Bit Org
};
static int BCNUM_MINDAY_PRICE = sizeof(BC_MINDAY_PRICE)/sizeof(CBitCode);

static CBitCode BC_MINDAY_PRICE_DIFFS[] =
{
	{0x00,	1,	'I',	4,		0},			// 0	+4Bit		= 4Bit
	{0x02,	2,	'I',	6,		8},			// 10	+6Bit		= 6Bit+8
	{0x06,	3,	'I',	8,		40},		// 110	+8Bit		= 8Bit+32+8
	{0x0E,	4,	'I',	16,		168},		// 1110	+16Bit		= 16Bit+128+32+8
	{0x0F,	4,	'O',	32,		0},			// 1111 +32Bit		= 32Bit Org
};
static int BCNUM_MINDAY_PRICE_DIFFS = sizeof(BC_MINDAY_PRICE_DIFFS)/sizeof(CBitCode);

static CBitCode BC_DAY_VOL[] =
{
	{0x06,	3,	'D',	12,		0},			// 110	+12Bit		= 12Bit
	{0x02,	2,	'D',	16,		4096},		// 10	+16Bit		= 16Bit+4096
	{0x00,	1,	'D',	24,		69632},		// 0	+24Bit		= 24Bit+65536+4096
	{0x07,	3,	'O',	32,		0},			// 111  +32Bit		= 32Bit Org
};
static int BCNUM_DAY_VOL = sizeof(BC_DAY_VOL)/sizeof(CBitCode);

static CBitCode BC_MINDAY_PRICE_DIFF[] =
{
	{0x00,	1,	'D',	4,		0},			// 0	+4Bit		= 4Bit
	{0x02,	2,	'D',	6,		16},		// 10	+6Bit		= 6Bit+16
	{0x06,	3,	'D',	8,		80},		// 110	+8Bit		= 8Bit+64+16
	{0x0E,	4,	'D',	16,		336},		// 1110+16Bit		= 16Bit+256+64+16
	{0x0F,	4,	'O',	32,		0},			// 1111+32Bit		= 32Bit Org
};
static int BCNUM_MINDAY_PRICE_DIFF = sizeof(BC_MINDAY_PRICE_DIFF)/sizeof(CBitCode);

static CBitCode BC_MINDAY_AMNT[] =
{
	{0x00,	2,	'I',	16,		0},			// 00	+16Bit		= 16Bit
	{0x01,	2,	'I',	24,		32768},		// 01	+24Bit		= 24Bit+32768
	{0x02,	2,	'I',	28,		8421376},	// 10	+28Bit		= 28Bit+8388608+32768
	{0x03,	2,	'O',	32,		0},			// 11	+32Bit		= 32Bit Org
};
static int BCNUM_MINDAY_AMNT = sizeof(BC_MINDAY_AMNT)/sizeof(CBitCode);

static CBitCode BC_DAY_TIME[] =
{
	{0x00,	1,	'C',	0,		1},					//0				= 1天
	{0x02,	2,	'C',	0,		3},					//10			= 3天
	{0x0C,	4,	'C',	0,		2},					//1100			= 2天
	{0x0D,	4,	'D',	4,		4},					//1101	+4Bit	= 4Bit+4天
	{0x0E,	4,	'D',	8,		20},				//1110	+8Bit	= 8Bit+20天
	{0x0F,	4,	'O',	16,		0},					//1111	+16Bit	= 16Bit天
};
static int BCNUM_DAY_TIME = sizeof(BC_DAY_TIME)/sizeof(CBitCode);

static CBitCode BC_MIN_TIME[] =
{
	{0x00,	1,	'C',	0,		1},					//0				= 1时间单位
	{0x02,	2,	'D',	5,		2},					//10	+5Bit	= 5Bit+2时间单位
	{0x06,	3,	'D',	8,		34},				//110	+8Bit	= 8Bit+32+2时间单位
	{0x07,	3,	'O',	16,		0},					//111	+16Bit	= 16Bit时间单位
};
static int BCNUM_MIN_TIME = sizeof(BC_MIN_TIME)/sizeof(CBitCode);

static CBitCode BC_MIN_NUMBER[] =
{
	{0x00,	1,	'C',	0,		0},					//0				= 0
	{0x02,	2,	'D',	4,		1},					//10	+4Bit	= 4Bit+1
	{0x06,	3,	'D',	8,		17},				//110	+8Bit	= 8Bit+16+1
	{0x07,	3,	'O',	16,		0},					//111	+16Bit	= 16Bit
};
static int BCNUM_MIN_NUMBER = sizeof(BC_MIN_NUMBER)/sizeof(CBitCode);

static CBitCode BC_MIN_VOL[] =
{
	{0x00,	1,	'D',	12,		0},			// 0	+12Bit		= 12Bit
	{0x02,	2,	'D',	16,		4096},		// 10	+16Bit		= 16Bit+4096
	{0x06,	3,	'D',	24,		69632},		// 110	+24Bit		= 24Bit+65536+4096
	{0x07,	3,	'O',	32,		0},			// 111  +32Bit		= 32Bit Org
};
static int BCNUM_MIN_VOL = sizeof(BC_MIN_VOL)/sizeof(CBitCode);

static CBitCode BC_DYNA_QH_OI_DIFF[] =
{
	{0x00,	1,	'I',	12,		0},			// 0	+12Bit		= 12Bit
	{0x02,	2,	'I',	16,		2048},		// 10	+16Bit		= 16Bit+2048
	{0x06,	3,	'I',	24,		34816},		// 110	+24Bit		= 24Bit+32768+2048
	{0x07,	3,	'O',	32,		0},			// 111  +32Bit		= 32Bit Org
};
static int BCNUM_DYNA_QH_OI_DIFF = sizeof(BC_DYNA_QH_OI_DIFF)/sizeof(CBitCode);

static CBitCode BC_DYNA_TRADENUM[] =
{
	{0x02,	2,	'D',	8,		0},			// 10	+8Bit		= 8Bit
	{0x00,	1,	'D',	12,		256},		// 0	+12Bit		= 12Bit+256
	{0x06,	3,	'D',	16,		4352},		// 110	+16Bit		= 16Bit+4096+256
	{0x0E,	4,	'D',	24,		69888},		// 1110	+24Bit		= 24Bit+65536+4096+256
	{0x0F,	4,	'O',	32,		0},			// 1111	+32Bit		= 32Bit Org
};
static int BCNUM_DYNA_TRADENUM = sizeof(BC_DYNA_TRADENUM)/sizeof(CBitCode);

static CBitCode BC_DAY_TRADENUM[] =
{
	{0x06,	3,	'D',	8,		0},			// 110	+8Bit		= 8Bit
	{0x02,	2,	'D',	12,		256},		// 10	+12Bit		= 12Bit+256
	{0x00,	1,	'D',	16,		4352},		// 0	+16Bit		= 16Bit+4096+256
	{0x0E,	4,	'D',	24,		69888},		// 1110	+24Bit		= 24Bit+65536+4096+256
	{0x0F,	4,	'O',	32,		0},			// 1111	+32Bit		= 32Bit Org
};
static int BCNUM_DAY_TRADENUM = sizeof(BC_DAY_TRADENUM)/sizeof(CBitCode);

static CBitCode BC_DYNA_PRICE[] =
{
	{0x02,	2,	'D',	8,		0},			//10	+8Bit		= 8Bit
	{0x00,	1,	'D',   16,		256},		//0		+16Bit		= 16Bit+256
	{0x06,	3,	'D',   24,		65792},		//110	+24Bit		= 24Bit+65536+256
	{0x07,	3,	'O',   32,		0},			//111	+32Bit		= 32Bit Org
};
static int BCNUM_DYNA_PRICE = sizeof(BC_DYNA_PRICE)/sizeof(CBitCode);

static CBitCode BC_DYNA_PRICE_COUNT[] =
{
	{0x00,	2,	'C',	0,		0},			// 00				= 0
	{0x04,	3,	'C',	0,		1},			// 100				= 1
	{0x05,	3,	'C',	0,		2},			// 101				= 2
	{0x01,	2,	'D',	4,		3},			// 01	+4Bit		= 4Bit+3
	{0x06,	3,	'D',	6,		19},		// 110	+6Bit		= 6Bit+16+3
	{0x0E,	4,	'D',	10,		83},		// 1110	+10Bit		= 10Bit+64+16+3
	{0x0F,	4,	'O',	32,		0},			// 1111	+24Bit		= 24Bit Org
};
static int BCNUM_DYNA_PRICE_COUNT = sizeof(BC_DYNA_PRICE_COUNT)/sizeof(CBitCode);


static CBitCode BC_DYNA_PRICE_MMP[] =
{
	{0x00,	2,	'C',	0,	999999},		// 00				= Psell1 = Pnew
	{0x01,	2,	'C',	0,		0},			// 01				= Pbuy1 = Pnew
	{0x02,	2,	'I',	4,		0},			// 10	+4Bit		= 4Bit
	{0x06,	3,	'I',	8,		8},			// 110  +8Bit		= 8Bit+8
	{0x07,	3,	'O',	32,		0},			// 111	+32Bit		= 32Bit Org
};
static int BCNUM_DYNA_PRICE_MMP = sizeof(BC_DYNA_PRICE_MMP)/sizeof(CBitCode);

static CBitCode BC_DYNA_PRICE_GAP[] =
{
	{0x00,	1,	'C',	0,		2},			// 0				= 2
	{0x02,	2,	'D',	3,		3},			// 10	+3Bit		= 3Bit+3
	{0x06,	3,	'D',	8,		11},		// 110  +8Bit		= 8Bit+8+3
	{0x07,	3,	'O',	32,		0},			// 111	+32Bit		= 32Bit Org
};
static int BCNUM_DYNA_PRICE_GAP = sizeof(BC_DYNA_PRICE_GAP)/sizeof(CBitCode);

static CBitCode BC_BARGAIN_VOL[] =
{
	{0x00,	1,	'H',	4,		0},			// 0	+4Bit		= (4Bit)*100
	{0x02,	2,	'H',	8,		16},		// 10	+8Bit		= (8Bit+16)*100
	{0x0C,	4,	'H',	12,		272},		// 1100	+12Bit		= (12Bit+256+16)*100
	{0x0D,	4,	'D',	12,		0},			// 1101+12Bit		= 12Bit
	{0x0E,	4,	'D',	16,		4096},		// 1110+16Bit		= 16Bit+4096
	{0x1E,	5,	'D',	24,		69632},		// 11110+24Bit		= 24Bit+65536+4096
	{0x1F,	5,	'O',	32,		0},			// 11111+32Bit		= 32Bit Org
};
static int BCNUM_BARGAIN_VOL = sizeof(BC_BARGAIN_VOL)/sizeof(CBitCode);

static CBitCode BC_TRADE_TRADENUM[] =
{
	{0x06,	3,	'D',	8,		0},			// 110	+8Bit		= 8Bit
	{0x02,	2,	'D',	12,		256},		// 10	+12Bit		= 12Bit+256
	{0x00,	1,	'D',	16,		4352},		// 0	+16Bit		= 16Bit+4096+256
	{0x0E,	4,	'D',	20,		69888},		// 1110	+20Bit		= 20Bit+65536+4096+256
	{0x1E,	5,	'D',	24,		1118464},	// 11110+24Bit		= 24Bit+1048576+65536+4096+256
	{0x1F,	5,	'O',	32,		0},			// 11111+32Bit		= 32Bit Org
};
static int BCNUM_TRADE_TRADENUM = sizeof(BC_TRADE_TRADENUM)/sizeof(CBitCode);

static CBitCode BC_DYNA_PRICE_INS[] =
{
	{0x0E,	4,	'C',	0,	999999},		// 1110				= 0
	{0x00,	1,	'C',	0,		1},			// 0				= 1
	{0x02,	2,	'D',	4,		2},			// 10	+4Bit		= 4Bit+2
	{0x06,	3,	'D',	16,		18},		// 110  +16Bit		= 16Bit+16+2
	{0x0F,	4,	'O',	32,		0},			// 1111 +32Bit		= 32Bit Org
};
static int BCNUM_DYNA_PRICE_INS = sizeof(BC_DYNA_PRICE_INS)/sizeof(CBitCode);

static const int DATE_YYYYMMDD_BITS = 26;
static const int TIME_HHMMSS_BITS = 18;


namespace MCM
{
void EncodeDataDiff(const UINT32 dwNew, const UINT32 dwOld, CBitCode* pBitCodes, const int nNumOfCodes, COutputBitStream & stream)
{
	if(dwNew == dwOld)
	{
	    stream.WriteBool(false);
	}
	else
	{
//            ++arrive;
	    stream.WriteBool(true);
	    stream.EncodeData(dwNew, pBitCodes, nNumOfCodes, &dwOld);
	}
}


void EncodeDataSnap(const UINT32 dwNew, const UINT32 dwOld, CBitCode* pBitCodes, const int nNumOfCodes, COutputBitStream & stream)
{
	if(dwNew == dwOld)
	{
	    stream.WriteBool(false);
	}
	else
	{
	    stream.WriteBool(true);
	    stream.EncodeData(dwNew, pBitCodes, nNumOfCodes);
	}
}


void EncodeBigintDiff(const CBigInt xNew, const CBigInt xOld, CBitCode* pBitCodes, const int nNumOfCodes, COutputBitStream & stream)
{
    if (xNew == xOld) {
        stream.WriteBool(false);
    } else {
        stream.WriteBool(true);
        stream.EncodeBigInt(xNew, pBitCodes, nNumOfCodes, &xOld);
    }
}


void EncodeBigintSnap(const CBigInt xNew, const CBigInt xOld, CBitCode* pBitCodes, const int nNumOfCodes, COutputBitStream & stream)
{
	if(xNew == xOld)
	{
	    stream.WriteBool(false);
	}
	else
	{
	    stream.WriteBool(true);
	    stream.EncodeBigInt(xNew, pBitCodes, nNumOfCodes);
	}
}



void DecodeDataDiff(UINT32 & dwNew, const UINT32 dwOld, CBitCode* pBitCodes, const int nNumOfCodes, CBitStream & stream)
{
	if(stream.ReadDWORD(1))
	{
	    stream.DecodeData(dwNew, pBitCodes, nNumOfCodes, &dwOld);
	}
    else
    {
        dwNew = dwOld;
    }
}



void DecodeDataSnap(UINT32 & dwNew, const UINT32 dwOld, CBitCode* pBitCodes, const int nNumOfCodes, CBitStream & stream)
{
	if(stream.ReadDWORD(1))
	{
	    stream.DecodeData(dwNew, pBitCodes, nNumOfCodes);
	}
    else
    {
        dwNew = dwOld;
    }
}



void DecodeBigintDiff(CBigInt & xNew, const CBigInt xOld, CBitCode* pBitCodes, const int nNumOfCodes, CBitStream & stream)
{
	if(stream.ReadDWORD(1))
	{
	    stream.DecodeBigInt(xNew, pBitCodes, nNumOfCodes, &xOld);
	}
    else
    {
        xNew = xOld;
    }
}



void DecodeBigintSnap(CBigInt & xNew, const CBigInt xOld, CBitCode* pBitCodes, const int nNumOfCodes, CBitStream & stream)
{
	if(stream.ReadDWORD(1))
	{
	    stream.DecodeBigInt(xNew, pBitCodes, nNumOfCodes);
	}
    else
    {
        xNew = xOld;
    }
}

inline void EncodeDWORDDiff(COutputBitStream& stream, UINT32 dwNew, UINT32 dwOld, int bits)
{
    if (dwNew == dwOld) {
        stream.WriteBool(false);
    } else {
        stream.WriteBool(true);
        stream.WriteDWORD(dwNew, bits);
    }
}

UINT32 DecodeUINT32(CBitCode* pBitCodes, const int nNumOfCodes, CBitStream & stream)
{
    UINT32 data;

    stream.DecodeData(data, pBitCodes, nNumOfCodes);

    return data;
}

UINT32 DecodeUINT32Diff(
    const UINT32 dwOld,
    CBitCode* pBitCodes,
    const int nNumOfCodes,
    CBitStream & stream)
{
    UINT32 dwNew;
	if(stream.ReadDWORD(1)) {
	    stream.DecodeData(dwNew, pBitCodes, nNumOfCodes, &dwOld);
	} else {
        dwNew = dwOld;
    }

    return dwNew;
}

template <typename T, size_t N>
inline void EncodeArray(COutputBitStream& stream, const T (&array)[N])
{
    unsigned char* raw = (unsigned char*) array;
    for (int i = 0; i < N * sizeof(T); i++) {
        stream.WriteDWORD(raw[i], 8);
    }
}

template <typename T, size_t N>
inline void DecodeArray(CBitStream& stream, T (&array)[N])
{
    unsigned char* raw = (unsigned char*) array;
    for (int i = 0; i < N * sizeof(T); i++) {
        raw[i] = stream.ReadDWORD(8);
    }
}

template <typename T, size_t N>
inline void EncodeArrayDiff(
    COutputBitStream& stream,
    const T (&newarray)[N],
    const T (&oldarray)[N])
{
    if (!(memcmp(newarray, oldarray, N * sizeof(T)))) {
        stream.WriteBool(false);
    } else {
        stream.WriteBool(true);
        EncodeArray(stream, newarray);
    }
}

template <typename T>
inline T ReadDWORDDiff(CBitStream& stream, T old, int nbits)
{
    if (stream.ReadDWORD(1)) {
        return static_cast<T>(stream.ReadDWORD(nbits));
    } else {
        return old;
    }
}

template <typename T, size_t N>
inline void DecodeArrayDiff(CBitStream& stream, const T (&oldarray)[N], T (&newarray)[N])
{
    if (stream.ReadDWORD(1)) {
        DecodeArray(stream, newarray);
    } else {
        memcpy(newarray, oldarray, N * sizeof(T));
    }
}

};
#endif	/* MATCHCODE_H */

