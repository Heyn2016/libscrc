/*
 ********************************************************************************************************
*                              		(c) Copyright 2017-2020, Hexin
 *                                           All Rights Reserved
 * File    : _crc32module.c
 * Author  : Heyn (heyunhuan@gmail.com)
 * Version : V1.3
 *
 * LICENSING TERMS:
 * ---------------
 *		New Create at 	2017-08-09 16:39PM
 *                      2017-08-17 [Heyn] Optimized Code.
 *                      2017-08-21 [Heyn] Optimization code for the C99 standard.
 *                                        for ( unsigned int i=0; i<256; i++ ) -> for ( i=0; i<256; i++ )
 *                      2017-09-22 [Heyn] Optimized Code. New add table32() function.
 *                      2020-03-20 [Heyn] New add adler32 and fletcher32 functions.
 *                      2020-04-17 [Heyn] Issues #1
 *                      2020-04-26 [Heyn] Optimized Code
 * 
 * Web : https://en.wikipedia.org/wiki/Polynomial_representations_of_cyclic_redundancy_checks
 *
 ********************************************************************************************************
 */

#include <Python.h>
#include "_crc32tables.h"

static unsigned char hexin_PyArg_ParseTuple( PyObject *self, PyObject *args,
                                             unsigned int init,
                                             unsigned int (*function)( const unsigned char *,
                                                                       unsigned int,
                                                                       unsigned int ),
                                             unsigned int *result )
{
    Py_buffer data = { NULL, NULL };

#if PY_MAJOR_VERSION >= 3
    if ( !PyArg_ParseTuple( args, "y*|I", &data, &init ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return FALSE;
    }
#else
    if ( !PyArg_ParseTuple( args, "s*|I", &data, &init ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return FALSE;
    }
#endif /* PY_MAJOR_VERSION */

    *result = (* function)( (const unsigned char *)data.buf, (unsigned int)data.len, init );

    if ( data.obj )
       PyBuffer_Release( &data );

    return TRUE;
}

static unsigned char hexin_PyArg_ParseTuple_Paramete( PyObject *self, PyObject *args, struct _hexin_crc32 *param )
{
    Py_buffer data = { NULL, NULL };

#if PY_MAJOR_VERSION >= 3
    if ( !PyArg_ParseTuple( args, "y*", &data ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return FALSE;
    }
#else
    if ( !PyArg_ParseTuple( args, "s*", &data ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return FALSE;
    }
#endif /* PY_MAJOR_VERSION */

    param->result = hexin_crc32_compute( (const unsigned char *)data.buf, (unsigned int)data.len, param  );

    if ( data.obj )
       PyBuffer_Release( &data );

    return TRUE;
}

static PyObject * _crc32_mpeg_2( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc32_param_mpeg2 = { .is_initial=FALSE,
                                                     .width  = HEXIN_CRC32_WIDTH,
                                                     .poly   = CRC32_POLYNOMIAL_04C11DB7,
                                                     .init   = 0xFFFFFFFFL,
                                                     .refin  = FALSE,
                                                     .refout = FALSE,
                                                     .xorout = 0x00000000L,
                                                     .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc32_param_mpeg2 ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc32_param_mpeg2.result );
}

static PyObject * _crc32_crc32( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc32_param_crc32 = { .is_initial=FALSE,
                                                     .width  = HEXIN_CRC32_WIDTH,
                                                     .poly   = CRC32_POLYNOMIAL_04C11DB7,
                                                     .init   = 0xFFFFFFFFL,
                                                     .refin  = TRUE,
                                                     .refout = TRUE,
                                                     .xorout = 0xFFFFFFFFL,
                                                     .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc32_param_crc32 ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc32_param_crc32.result );
}

static PyObject * _crc32_table( PyObject *self, PyObject *args )
{
    unsigned int i = 0x00000000L;
    unsigned int poly = CRC32_POLYNOMIAL_04C11DB7, ref = 0;
    unsigned int table[MAX_TABLE_ARRAY] = { 0x00000000L };
    PyObject* plist = PyList_New( MAX_TABLE_ARRAY );

#if PY_MAJOR_VERSION >= 3
    if ( !PyArg_ParseTuple( args, "I|p", &poly, &ref ) )
        return NULL;
#else
    if ( !PyArg_ParseTuple( args, "I|p", &poly, &ref ) )
        return NULL;
#endif /* PY_MAJOR_VERSION */

    if ( ref == FALSE ) {
        hexin_crc32_init_table_poly_is_low ( poly, table );
    } else {
        hexin_crc32_init_table_poly_is_high( poly, table );
    }

    for ( i=0; i<MAX_TABLE_ARRAY; i++ ) {
        PyList_SetItem( plist, i, Py_BuildValue( "I", table[i] ) );
    }

    return plist;
}

static PyObject * _crc32_hacker( PyObject *self, PyObject *args, PyObject* kws )
{
    Py_buffer data = { NULL, NULL };
    struct _hexin_crc32 crc32_param_hacker = { .is_initial=FALSE,
                                               .width  = HEXIN_CRC32_WIDTH,
                                               .poly   = CRC32_POLYNOMIAL_04C11DB7,
                                               .init   = 0xFFFFFFFFL,
                                               .refin  = TRUE,
                                               .refout = TRUE,
                                               .xorout = 0xFFFFFFFFL,
                                               .result = 0 };

    static char* kwlist[]={ "data", "poly", "init", "xorout", "refin", "refout", NULL };

#if PY_MAJOR_VERSION >= 3
    if ( !PyArg_ParseTupleAndKeywords( args, kws, "y*|IIIpp", kwlist, &data,
                                                                      &crc32_param_hacker.poly,
                                                                      &crc32_param_hacker.init,
                                                                      &crc32_param_hacker.xorout,
                                                                      &crc32_param_hacker.refin,
                                                                      &crc32_param_hacker.refout ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return NULL;        
    }
#else
    if ( !PyArg_ParseTupleAndKeywords( args, kws, "s*|IIIpp", kwlist, &data,
                                                                      &crc32_param_hacker.poly,
                                                                      &crc32_param_hacker.init,
                                                                      &crc32_param_hacker.xorout,
                                                                      &crc32_param_hacker.refin,
                                                                      &crc32_param_hacker.refout ) ) {
        if ( data.obj ) {
            PyBuffer_Release( &data );
        }
        return NULL;  
    }
#endif /* PY_MAJOR_VERSION */

    crc32_param_hacker.result = hexin_crc32_compute( (const unsigned char *)data.buf, (unsigned int)data.len, &crc32_param_hacker );

    if ( data.obj )
       PyBuffer_Release( &data );

    return Py_BuildValue( "I", crc32_param_hacker.result );
}


static PyObject * _crc32_adler32( PyObject *self, PyObject *args )
{
    unsigned int result   = 0x00000000L;
    unsigned int reserved = 0x00000000L;
 
    if ( !hexin_PyArg_ParseTuple( self, args, reserved, hexin_calc_crc32_adler, ( unsigned int * )&result ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", result );
}

static PyObject * _crc32_fletcher32( PyObject *self, PyObject *args )
{
    unsigned int result   = 0x00000000L;
    unsigned int reserved = 0x00000000L;
 
    if ( !hexin_PyArg_ParseTuple( self, args, reserved, hexin_calc_crc32_fletcher, ( unsigned int * )&result ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", result );
}

static PyObject * _crc32_posix( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc32_param_posix = { .is_initial=FALSE,
                                                     .width  = HEXIN_CRC32_WIDTH,
                                                     .poly   = CRC32_POLYNOMIAL_04C11DB7,
                                                     .init   = 0x00000000L,
                                                     .refin  = FALSE,
                                                     .refout = FALSE,
                                                     .xorout = 0xFFFFFFFFL,
                                                     .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc32_param_posix ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc32_param_posix.result );
}

/*
* Alias: CRC-32/AAL5, CRC-32/DECT-B, B-CRC-32
*/
static PyObject * _crc32_bzip2( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc32_param_bzip2 = { .is_initial=FALSE,
                                                     .width  = HEXIN_CRC32_WIDTH,
                                                     .poly   = CRC32_POLYNOMIAL_04C11DB7,
                                                     .init   = 0xFFFFFFFFL,
                                                     .refin  = FALSE,
                                                     .refout = FALSE,
                                                     .xorout = 0xFFFFFFFFL,
                                                     .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc32_param_bzip2 ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc32_param_bzip2.result );
}

static PyObject * _crc32_jamcrc( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc32_param_jamcrc = { .is_initial=FALSE,
                                                      .width  = HEXIN_CRC32_WIDTH,
                                                      .poly   = 0x04C11DB7L,
                                                      .init   = 0xFFFFFFFFL,
                                                      .refin  = TRUE,
                                                      .refout = TRUE,
                                                      .xorout = 0x00000000L,
                                                      .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc32_param_jamcrc ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc32_param_jamcrc.result );
}

static PyObject * _crc32_autosar( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc32_param_autosar = { .is_initial=FALSE,
                                                       .width  = HEXIN_CRC32_WIDTH,
                                                       .poly   = 0xF4ACFB13L,
                                                       .init   = 0xFFFFFFFFL,
                                                       .refin  = TRUE,
                                                       .refout = TRUE,
                                                       .xorout = 0xFFFFFFFFL,
                                                       .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc32_param_autosar ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc32_param_autosar.result );
}

static PyObject * _crc32_crc32_c( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc32_iscsi = { .is_initial=FALSE,
                                               .width  = HEXIN_CRC32_WIDTH,
                                               .poly   = 0x1EDC6F41L,
                                               .init   = 0xFFFFFFFFL,
                                               .refin  = TRUE,
                                               .refout = TRUE,
                                               .xorout = 0xFFFFFFFFL,
                                               .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc32_iscsi ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc32_iscsi.result );
}

static PyObject * _crc32_crc32_d( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc32_base91 = { .is_initial=FALSE,
                                                .width  = HEXIN_CRC32_WIDTH,
                                                .poly   = 0xA833982BL,
                                                .init   = 0xFFFFFFFFL,
                                                .refin  = TRUE,
                                                .refout = TRUE,
                                                .xorout = 0xFFFFFFFFL,
                                                .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc32_base91 ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc32_base91.result );
}

static PyObject * _crc32_crc32_q( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc32_aixm = { .is_initial=FALSE,
                                              .width  = HEXIN_CRC32_WIDTH,
                                              .poly   = 0x814141ABL,
                                              .init   = 0,
                                              .refin  = FALSE,
                                              .refout = FALSE,
                                              .xorout = 0,
                                              .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc32_aixm ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc32_aixm.result );
}

static PyObject * _crc32_xfer( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc32_param_xfer = { .is_initial=FALSE,
                                                    .width  = HEXIN_CRC32_WIDTH,
                                                    .poly   = 0x000000AFL,
                                                    .init   = 0x00000000L,
                                                    .refin  = FALSE,
                                                    .refout = FALSE,
                                                    .xorout = 0x00000000L,
                                                    .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc32_param_xfer ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc32_param_xfer.result );
}

static PyObject * _crc30_cdma( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc30_cdma_param = { .is_initial=FALSE,
                                                    .width  = 30,
                                                    .poly   = CRC30_POLYNOMIAL_2030B9C7,
                                                    .init   = 0x3FFFFFFFL,
                                                    .refin  = FALSE,
                                                    .refout = FALSE,
                                                    .xorout = 0x3FFFFFFFL,
                                                    .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc30_cdma_param ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc30_cdma_param.result );
}

static PyObject * _crc31_philips( PyObject *self, PyObject *args )
{
    static struct _hexin_crc32 crc31_philips_param = { .is_initial=FALSE,
                                                       .width  = 31,
                                                       .poly   = CRC31_POLYNOMIAL_04C11DB7,
                                                       .init   = 0x7FFFFFFFL,
                                                       .refin  = FALSE,
                                                       .refout = FALSE,
                                                       .xorout = 0x7FFFFFFFL,
                                                       .result = 0 };

    if ( !hexin_PyArg_ParseTuple_Paramete( self, args, &crc31_philips_param ) ) {
        return NULL;
    }

    return Py_BuildValue( "I", crc31_philips_param.result );
}

/* method table */
static PyMethodDef _crc32Methods[] = {
    { "mpeg2",       (PyCFunction)_crc32_mpeg_2,     METH_VARARGS,   "Calculate CRC (MPEG2) of CRC32 [Poly=0x04C11DB7, Init=0xFFFFFFFF, Xorout=0x00000000 Refin=False Refout=False]"},
    { "fsc",         (PyCFunction)_crc32_mpeg_2,     METH_VARARGS,   "Calculate CRC (Ethernt's FSC) of CRC32 [Poly=0x04C11DB7, Init=0xFFFFFFFF, Xorout=0x00000000 Refin=False Refout=False]"},
    { "crc32",       (PyCFunction)_crc32_crc32,      METH_VARARGS,   "Calculate CRC (WinRAR, File) of CRC32  [Poly=0xEDB88320, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "iso_hdlc32",  (PyCFunction)_crc32_crc32,      METH_VARARGS,   "Calculate ISO-HDLC of CRC32  [Poly=0xEDB88320, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "xz32",        (PyCFunction)_crc32_crc32,      METH_VARARGS,   "Calculate XZ of CRC32  [Poly=0xEDB88320, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "pkzip",       (PyCFunction)_crc32_crc32,      METH_VARARGS,   "Calculate PKZIP of CRC32  [Poly=0xEDB88320, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "adccp",       (PyCFunction)_crc32_crc32,      METH_VARARGS,   "Calculate ADCCP of CRC32  [Poly=0xEDB88320, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "v_42",        (PyCFunction)_crc32_crc32,      METH_VARARGS,   "Calculate V-42 of CRC32  [Poly=0xEDB88320, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "table32",     (PyCFunction)_crc32_table,      METH_VARARGS,   "Print CRC32 table to list. libscrc.table32( polynomial )" },
    { "hacker32",    (PyCFunction)_crc32_hacker,     METH_KEYWORDS|METH_VARARGS, "User calculation CRC32\n"
                                                                                 "@data   : bytes\n"
                                                                                 "@poly   : default=0xEDB88320\n"
                                                                                 "@init   : default=0xFFFFFFFF\n"
                                                                                 "@xorout : default=0x00000000\n"
                                                                                 "@ref    : default=False" },
    { "adler32",    (PyCFunction)_crc32_adler32,     METH_VARARGS,   "Calculate adler32 (MOD=65521)" },
    { "fletcher32", (PyCFunction)_crc32_fletcher32,  METH_VARARGS,   "Calculate fletcher32" },
    { "posix",      (PyCFunction)_crc32_posix,       METH_VARARGS,   "Calculate CRC (POSIX) of CRC32 [Poly=0x04C11DB7, Init=0x00000000, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "cksum",      (PyCFunction)_crc32_posix,       METH_VARARGS,   "Calculate CRC (CKSUM) of CRC32 [Poly=0x04C11DB7, Init=0x00000000, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "bzip2",      (PyCFunction)_crc32_bzip2,       METH_VARARGS,   "Calculate CRC (BZIP2) of CRC32 [Poly=0x04C11DB7, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "aal5",       (PyCFunction)_crc32_bzip2,       METH_VARARGS,   "Calculate CRC (AAL5) of CRC32 [Poly=0x04C11DB7, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "jamcrc",     (PyCFunction)_crc32_jamcrc,      METH_VARARGS,   "Calculate CRC (JAMCRC) of CRC32 [Poly=0x04C11DB7, Init=0xFFFFFFFF, Xorout=0x00000000 Refin=True Refout=True]"},
    { "autosar",    (PyCFunction)_crc32_autosar,     METH_VARARGS,   "Calculate CRC (AUTOSAR) of CRC32 [Poly=0xF4ACFB13, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "crc32_c",    (PyCFunction)_crc32_crc32_c,     METH_VARARGS,   "Calculate CRC (CRC32-C) of CRC32 [Poly=0x1EDC6F41, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "iscsi",      (PyCFunction)_crc32_crc32_c,     METH_VARARGS,   "Calculate CRC (ISCSI) of CRC32 [Poly=0x1EDC6F41, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "base91_c",   (PyCFunction)_crc32_crc32_c,     METH_VARARGS,   "Calculate CRC (BASE91-C) of CRC32 [Poly=0x1EDC6F41, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "crc32_d",    (PyCFunction)_crc32_crc32_d,     METH_VARARGS,   "Calculate CRC (CRC32-D) of CRC32 [Poly=0xA833982B, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "base91_d",   (PyCFunction)_crc32_crc32_d,     METH_VARARGS,   "Calculate CRC (BASE91-D) of CRC32 [Poly=0xA833982B, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]"},
    { "crc32_q",    (PyCFunction)_crc32_crc32_q,     METH_VARARGS,   "Calculate CRC (CRC32-Q) of CRC32 [Poly=0x814141AB, Init=0x00000000, Xorout=0x00000000 Refin=True Refout=True]"},
    { "aixm",       (PyCFunction)_crc32_crc32_q,     METH_VARARGS,   "Calculate CRC (AIXM) of CRC32 [Poly=0x814141AB, Init=0x00000000, Xorout=0x00000000 Refin=True Refout=True]"},
    { "xfer",       (PyCFunction)_crc32_xfer,        METH_VARARGS,   "Calculate CRC (XFER) of CRC32 [Poly=0x000000AF, Init=0x00000000, Xorout=0x00000000 Refin=True Refout=True]"},
    { "cdma",       (PyCFunction)_crc30_cdma,        METH_VARARGS,   "Calculate CDMA of CRC30 [Poly=0x2030B9C7, Init=0x3FFFFFFF, Xorout=0x3FFFFFFF Refin=False Refout=False]"},
    { "philips",    (PyCFunction)_crc31_philips,     METH_VARARGS,   "Calculate PHILIPS of CRC31 [Poly=0x04C11DB7, Init=0x7FFFFFFF, Xorout=0x7FFFFFFF Refin=False Refout=False]"},
    { NULL, NULL, 0, NULL }        /* Sentinel */
};

/* module documentation */
PyDoc_STRVAR( _crc32_doc,
"Calculation of CRC32 \n"
"libscrc.fsc        -> Calculate CRC for Ethernet frame sequence (FSC) [Poly=0x04C11DB7, Init=0xFFFFFFFF, Xorout=0x00000000 Refin=False Refout=False]\n"
"libscrc.mpeg2      -> Calculate CRC for Media file (MPEG2) [Poly=0x04C11DB7, Init=0xFFFFFFFF, Xorout=0x00000000 Refin=False Refout=False]\n"
"libscrc.crc32      -> Calculate CRC for file [Poly=0x04C11DB7L, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]\n"
"libscrc.iso_hdlc32 -> Calculate ISO-HDLC [Poly=0x04C11DB7L, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]\n"
"libscrc.xz32       -> Calculate XZ [Poly=0x04C11DB7L, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]\n"
"libscrc.pkzip      -> Calculate PKZIP [Poly=0x04C11DB7L, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]\n"
"libscrc.adccp      -> Calculate ADCCP [Poly=0x04C11DB7L, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]\n"
"libscrc.v_42       -> Calculate V-42 [Poly=0x04C11DB7L, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True Refout=True]\n"
"libscrc.table32    -> Print CRC32 table to list\n"
"libscrc.hacker32   -> Free calculation CRC32 (not support python2 series) Xorout=0x00000000 Refin=False Refout=False\n"
"libscrc.adler32    -> Calculate adler32 (MOD=65521)\n"
"libscrc.fletcher32 -> Calculate fletcher32\n"
"libscrc.posix      -> Calculate CRC (POSIX) [Poly=0x04C11DB7, Init=0x00000000, Xorout=0xFFFFFFFF Refin=False Refout=False]\n"
"libscrc.cksum      -> Calculate CRC (CKSUM) [Poly=0x04C11DB7, Init=0x00000000, Xorout=0xFFFFFFFF Refin=False Refout=False]\n"
"libscrc.bzip2      -> Calculate CRC (BZIP2) [Poly=0x04C11DB7, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True  Refout=True]\n"
"libscrc.jamcrc     -> Calculate CRC (JAMCRC) [Poly=0x04C11DB7, Init=0xFFFFFFFF, Xorout=0x00000000 Refin=True  Refout=True]\n"
"libscrc.autosar    -> Calculate CRC (AUTOSAR) [Poly=0xF4ACFB13, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True  Refout=True]\n"
"libscrc.crc32_c    -> Calculate CRC (CRC32-C) [Poly=0x1EDC6F41, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True  Refout=True]\n"
"libscrc.crc32_d    -> Calculate CRC (CRC32-D) [Poly=0xA833982B, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True  Refout=True]\n"
"libscrc.base91_d   -> Calculate CRC (BASE91-D) [Poly=0xA833982B, Init=0xFFFFFFFF, Xorout=0xFFFFFFFF Refin=True  Refout=True]\n"
"libscrc.crc32_q    -> Calculate CRC (CRC32-Q) [Poly=0x814141AB, Init=0x00000000, Xorout=0x00000000 Refin=True  Refout=True]\n"
"libscrc.aixm       -> Calculate CRC (AIXM) [Poly=0x814141AB, Init=0x00000000, Xorout=0x00000000 Refin=True  Refout=True]\n"
"libscrc.xfer       -> Calculate CRC (XFER) [Poly=0x000000AF, Init=0x00000000, Xorout=0x00000000 Refin=True  Refout=True]\n"
"libscrc.cdma       -> Calculate CDMA of CRC30 [Poly=0x2030B9C7, Init=0x3FFFFFFF, Xorout=0x3FFFFFFF Refin=False Refout=False]\n"
"libscrc.philips    -> Calculate PHILIPS of CRC31 Calculate PHILIPS of CRC31 [Poly=0x04C11DB7, Init=0x7FFFFFFF, Xorout=0x7FFFFFFF Refin=False Refout=False]\n"
"\n" );


#if PY_MAJOR_VERSION >= 3

/* module definition structure */
static struct PyModuleDef _crc32module = {
   PyModuleDef_HEAD_INIT,
   "_crc32",                    /* name of module */
   _crc32_doc,                  /* module documentation, may be NULL */
   -1,                          /* size of per-interpreter state of the module */
   _crc32Methods
};

/* initialization function for Python 3 */
PyMODINIT_FUNC
PyInit__crc32( void )
{
    PyObject *m;

    m = PyModule_Create( &_crc32module );
    if ( m == NULL ) {
        return NULL;
    }

    PyModule_AddStringConstant( m, "__version__", "1.3"  );
    PyModule_AddStringConstant( m, "__author__",  "Heyn" );

    return m;
}

#else

/* initialization function for Python 2 */
PyMODINIT_FUNC
init_crc32( void )
{
    ( void ) Py_InitModule3( "_crc32", _crc32Methods, _crc32_doc );
}

#endif /* PY_MAJOR_VERSION */
