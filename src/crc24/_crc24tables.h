/*
*********************************************************************************************************
*                              		(c) Copyright 2020-2020, Hexin
*                                           All Rights Reserved
* File    : _crc24tables.h
* Author  : Heyn (heyunhuan@gmail.com)
* Version : V1.3
*
* LICENSING TERMS:
* ---------------
*		New Create at 	2020-04-17 [Heyn] Initialize.
*                       2020-04-27 [Heyn] Optimized code.
*
*********************************************************************************************************
*/

#ifndef __CRC24_TABLES_H__
#define __CRC24_TABLES_H__

#ifndef TRUE
#define                 TRUE                                    1
#endif

#ifndef FALSE
#define                 FALSE                                   0
#endif

#ifndef MAX_TABLE_ARRAY
#define                 MAX_TABLE_ARRAY                         256
#endif

#define                 HEXIN_CRC24_WIDTH                       24

#define                 HEXIN_POLYNOMIAL_IS_HIGH(x)             ( x & 0x800000L )
#define                 HEXIN_REFIN_REFOUT_IS_TRUE(x)           ( ( x->refin == TRUE ) && ( x->refout == TRUE ) )


#define                 CRC24_POLYNOMIAL_00065B                 0x00065B
#define                 CRC24_POLYNOMIAL_5D6DCB                 0x5D6DCB
#define                 CRC24_POLYNOMIAL_864CFB                 0x864CFB
#define                 CRC24_POLYNOMIAL_800063                 0x800063
#define                 CRC24_POLYNOMIAL_328B63                 0x328B63


struct _hexin_crc24 {
    unsigned int  is_initial;
    unsigned int  width;
    unsigned int  poly;
    unsigned int  init;
    unsigned int  refin;
    unsigned int  refout;
    unsigned int  xorout;
    unsigned int  result;
    unsigned int  table[MAX_TABLE_ARRAY];
};

unsigned int hexin_crc24_compute( const unsigned char *pSrc, unsigned int len, struct _hexin_crc24 *param );

#endif //__CRC24_TABLES_H__
