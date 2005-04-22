/* SysZLib.h -- interface of the 'SysZLib' shared library
 *
 *  Original version by Tom Zerucha (tz@execpc.com) 2000
 *  Revisions and ARMlet support by Justin Clark (jclark@copera.com) 2003
 */

#ifndef __ZLIB_H__
#define __ZLIB_H__

#define ZLIB_VERSION "1.1.4"

/* Sections taken from zconf.h needed for the interface */

#define FAR
typedef unsigned int  uInt;  /* 16 bits or more */
typedef unsigned long uLong; /* 32 bits or more */
typedef unsigned char Bytef;
typedef char  FAR charf;
typedef int   FAR intf;
typedef uInt  FAR uIntf;
typedef uLong FAR uLongf;
typedef void *voidpf;
typedef void *voidp;
#define OF(args) args


/* constants */
#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1 /* will be removed, use Z_SYNC_FLUSH instead */
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4
/* Allowed flush values; see deflate() below for details */
#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
/* Return codes for the compression/decompression functions. Negative
 * values are errors, positive values are used for special but normal events.
 */

#define Z_NO_COMPRESSION         0
#define Z_BEST_SPEED             1
#define Z_BEST_COMPRESSION       9
#define Z_DEFAULT_COMPRESSION  (-1)
/* compression levels */

#define Z_FILTERED            1
#define Z_HUFFMAN_ONLY        2
#define Z_DEFAULT_STRATEGY    0
/* compression strategy; see deflateInit2() below for details */

#define Z_BINARY   0
#define Z_ASCII    1
#define Z_UNKNOWN  2
/* Possible values of the data_type field */

#define Z_DEFLATED   8
/* The deflate compression method (the only one supported in this version) */

#define Z_NULL  0  /* for initializing zalloc, zfree, opaque */

typedef voidpf (*alloc_func) OF((voidpf opaque, uInt items, uInt size));
typedef void   (*free_func)  OF((voidpf opaque, voidpf address));

struct internal_state;

typedef struct z_stream_s {
    Bytef    *next_in;  /* next input byte */
    uInt     avail_in;  /* number of bytes available at next_in */
    uLong    total_in;  /* total nb of input bytes read so far */

    Bytef    *next_out; /* next output byte should be put there */
    uInt     avail_out; /* remaining free space at next_out */
    uLong    total_out; /* total nb of bytes output so far */

    char     *msg;      /* last error message, NULL if no error */
    struct internal_state FAR *state; /* not visible by applications */

    alloc_func zalloc;  /* used to allocate the internal state */
    free_func  zfree;   /* used to free the internal state */
    voidpf     opaque;  /* private data object passed to zalloc and zfree */

    int     data_type;  /* best guess about the data type: ascii or binary */
    uLong   adler;      /* adler32 value of the uncompressed data */
    uLong   reserved;   /* reserved for future use */
} z_stream;

typedef z_stream FAR *z_streamp;

    /* Function declarations */
Err ZLibOpen(UInt16 refNum) SYS_TRAP(sysLibTrapOpen);
Err ZLibClose(UInt16 refNum, UInt16 * numappsP) SYS_TRAP(sysLibTrapClose);
Err ZLibSleep(UInt16 refNum) SYS_TRAP(sysLibTrapSleep);
Err ZLibWake(UInt16 refNum) SYS_TRAP(sysLibTrapWake);

Err ZLibdeflateinit2(UInt16 refnum, z_streamp strm, int level, int method,
                      int windowBits, int memLevel,
                      int strategy, const char *version, int stream_size)
        SYS_TRAP(sysLibTrapCustom);

Err ZLibdeflate(UInt16 refnum, z_streamp strm, int flush)
        SYS_TRAP(sysLibTrapCustom+1);

Err ZLibdeflateend(UInt16 refnum, z_streamp strm)
        SYS_TRAP(sysLibTrapCustom+2);

Err ZLibinflateinit2(UInt16 refnum, z_streamp strm,
                      int windowBits, const char *version, int stream_size)
        SYS_TRAP(sysLibTrapCustom+3);

Err ZLibinflate(UInt16 refnum, z_streamp strm, int flush)
        SYS_TRAP(sysLibTrapCustom+4);

Err ZLibinflateend(UInt16 refnum, z_streamp strm)
        SYS_TRAP(sysLibTrapCustom+5);

uLong ZLibcrc32(UInt16 refnum, uLong crc, const Bytef * buf, uInt len)
        SYS_TRAP(sysLibTrapCustom+6);

uLong ZLibadler32(UInt16 refnum, uLong adler, const Bytef * buf, uInt len)
        SYS_TRAP(sysLibTrapCustom+7);

Err ZLibcompress2(UInt16 refnum, Bytef * dest, uLongf * destLen,
                   const Bytef * source, uLong sourceLen, int level)
        SYS_TRAP(sysLibTrapCustom+8);

Err ZLibuncompress(UInt16 refnum, Bytef * dest, uLongf * destLen,
                    const Bytef * source, uLong sourceLen)
        SYS_TRAP(sysLibTrapCustom+9);

#ifndef NOZLIBDEFS
UInt16 ZLibRef = 0;
#else
extern UInt16 ZLibRef;
#endif


/* Some useful functions for opening and closing the library */

extern __inline Err SysZLib_OpenLibrary (UInt16 *refNumP)
{
  Err error;
  Boolean loaded = false;
  
  /* First try to find the library. */
  error = SysLibFind("Z.lib", refNumP);
  
  /* If not found, load the library instead. */
  if (error == sysErrLibNotFound)
  {
    error = SysLibLoad('libr', 'ZLib', refNumP);
    loaded = true;
  }
  
  if (error == errNone)
  {
    error = ZLibOpen (*refNumP);
    if (error != errNone)
    {
      if (loaded)
      {
        SysLibRemove(*refNumP);
      }
      
      *refNumP = sysInvalidRefNum;
    }
  }
  
  return error;
}

extern __inline Err SysZLib_CloseLibrary (UInt16 refNum)
{
  Err error;
  UInt16 zltmp;

  if (refNum == sysInvalidRefNum)
  {
    return sysErrParamErr;
  }
  
  error = ZLibClose (refNum, &zltmp);
  
  if (zltmp == 0)
  {
    /* no users left, so unload library */
    SysLibRemove(refNum);
  } 

  return error;
}

#define ZLSetup SysZLib_OpenLibrary(&ZLibRef)
#define ZLTeardown SysZLib_CloseLibrary(ZLibRef)


#define deflateInit2(strm, level, method, windowBits, memLevel, strategy) \
        ZLibdeflateinit2((ZLibRef),(strm),(level),(method),(windowBits), \
        (memLevel),(strategy), ZLIB_VERSION, sizeof(z_stream))
#define inflateInit2(strm, windowBits) \
        ZLibinflateinit2((ZLibRef), (strm), (windowBits), ZLIB_VERSION, \
        sizeof(z_stream))

/* 13 bits requires 32k, Memlevel 6 requires 32k.  Each increment doubles 
 * (for each) 
*/
#define deflateInit(a,b) ZLibdeflateinit2((ZLibRef), a, b, Z_DEFLATED, \
  13, 6, Z_DEFAULT_STRATEGY, ZLIB_VERSION, sizeof(z_stream))
#define deflateInit_(a,b,c,d) ZLibdeflateinit2((ZLibRef), a, b, Z_DEFLATED, \
  13, 6, Z_DEFAULT_STRATEGY, c, d)

#define inflateInit(a) ZLibinflateinit2((ZLibRef), a, 15, ZLIB_VERSION, \
  sizeof(z_stream))
#define inflateInit_(a,b,c) ZLibinflateinit2((ZLibRef), a, 15, b, c)

#define inflate(x,y) ZLibinflate(ZLibRef,x,y)
#define deflate(x,y) ZLibdeflate(ZLibRef,x,y)
#define inflateEnd(x) ZLibinflateend(ZLibRef,x)
#define deflateEnd(x) ZLibdeflateend(ZLibRef,x)
#define crc32(a,b,c) ZLibcrc32(ZLibRef,a,b,c)
#define adler32(a,b,c) ZLibadler32(ZLibRef,a,b,c)
#define compress(a,b,c,d,e) ZLibcompress2(ZLibRef,a,b,c,d,e)
#define uncompress(a,b,c,d) ZLibuncompress(ZLibRef,a,b,c,d)

/* Error codes returned by zlib_open */
#define SysZLibErrParam       (appErrorClass | 1)                
#define SysZLibErrNotOpen     (appErrorClass | 2)                
#define SysZLibErrStillOpen   (appErrorClass | 3)                
#define SysZLibErrOutOfMemory (appErrorClass | 4)


#endif /* __ZLIB_H__ */
